#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <assert.h>
#include <memory>
#include <iostream>

template <typename T, size_t N, T Default>
struct IndexNode;

template <typename T, size_t N, T Default>
using IndexNodePtr = std::shared_ptr<IndexNode<T, N, Default>>;

template <typename T, size_t N, T Default>
using IndexNodePtrContainer = std::vector<IndexNodePtr<T, N, Default>>;

template <typename T, size_t N, T Default>
using IndexNodePtrContainerPtr = std::shared_ptr<IndexNodePtrContainer<T, N, Default>>;

template <typename T, size_t N, T Default>
using IndexNodeInternalIterator = typename IndexNodePtrContainer<T, N, Default>::iterator;

struct IExtraNodeDeleter
{
    virtual ~IExtraNodeDeleter(){}
    virtual void DeleteChildNode(void* node) = 0;
};

template <typename T, size_t N, T Default>
struct IndexNode : IExtraNodeDeleter
{
    IndexNode(IExtraNodeDeleter* parentExtraNodeDeleter, size_t index)
        : mParentExtraNodeDeleter(parentExtraNodeDeleter)
        , mIndex(index)
        , mChildren(std::make_shared<IndexNodePtrContainer<T, N - 1, Default>>()) {}

    IndexNode(IndexNode& node) = delete;

    IndexNode& operator = (const IndexNode& node) = delete;

    ~IndexNode() {}
    
    const IndexNode<T, N - 1, Default>& operator [] (size_t index) const
    {
        auto it = std::find_if(mChildren->begin(), mChildren->end(),
            [index](auto node)
            {
                return node->mIndex == index;
            });

        if (it != mChildren->end())
            return **it;
        assert(0);
    }

    IndexNode<T, N - 1, Default>& operator [] (size_t index)
    {
        auto it = std::find_if(mChildren->begin(), mChildren->end(),
            [index](auto node)
            {
                return node->mIndex == index;
            });

        if (it != mChildren->end())
            return **it;

        auto node = std::make_shared<IndexNode<T, N - 1, Default>>(this, index);
        mChildren->emplace_back(node);
        return *node;
    }

    void DeleteChildNode(void* node) override
    {
        auto it = std::find_if(mChildren->begin(), mChildren->end(),
            [node](auto child)
            {
                return &*child == static_cast<IndexNode<T, N - 1, Default>*>(node);
            });
        if (it != mChildren->end())
        {
            mChildren->erase(it);
            if (mChildren->size() > 0)
            {
                return;
            }         

            if (mParentExtraNodeDeleter)
            {
                mParentExtraNodeDeleter->DeleteChildNode(this);
                mParentExtraNodeDeleter = nullptr;
            }
        }
    }

    IExtraNodeDeleter* mParentExtraNodeDeleter;
    size_t mIndex;
    IndexNodePtrContainerPtr<T, N - 1, Default> mChildren;
};

template <typename T, T Default>
struct IndexNode<T, 0, Default> : IExtraNodeDeleter
{
    IndexNode(IExtraNodeDeleter* parentExtraNodeDeleter, size_t index)
        : mParentExtraNodeDeleter(parentExtraNodeDeleter)
        , mIndex(index)
        , mValue({Default}) {}

    IndexNode(IndexNode& node)
        : mParentExtraNodeDeleter(nullptr)
        , mIndex(node.mIndex)
        , mValue(node.mValue)
    {
        node.DeleteNodeFromParentIfDefault();
        mParentExtraNodeDeleter = node.mParentExtraNodeDeleter;
    }

    ~IndexNode() {}

    IndexNode& operator = (const IndexNode& node)
    {
        mIndex = node.mIndex;
        mValue = node.mValue;
        node.DeleteNodeFromParentIfDefault();
        mParentExtraNodeDeleter = node.mParentExtraNodeDeleter;
        return *this;
    }

    IndexNode& operator = (const T& value)
    {
        mValue[0] = value;
        DeleteNodeFromParentIfDefault();
        return *this;
    }

    operator T()
    {
        return DeleteNodeFromParentIfDefault() ? Default : mValue[0];
    }

    operator const T() const
    {
        return DeleteNodeFromParentIfDefault() ? Default : mValue[0];
    }

    void DeleteChildNode(void*) override
    {
    }

    bool IsDefault() const
    {
        return mValue[0] == Default;
    }

    bool DeleteNodeFromParentIfDefault() const
    {
        if (IsDefault() && mParentExtraNodeDeleter)
        {
            mParentExtraNodeDeleter->DeleteChildNode((void*)this);
            mParentExtraNodeDeleter = nullptr;
            return true;
        }
        return false;
    }

    mutable IExtraNodeDeleter* mParentExtraNodeDeleter;
    size_t mIndex;
    std::vector<T> mValue;

    static T DefaultValue;
};

template <typename T, T Default>
T IndexNode<T, 0, Default>::DefaultValue = Default;

template <typename T, T Default>
std::ostream& operator << (std::ostream& stream, IndexNode<T, 0, Default>& node)
{
    if (node.DeleteNodeFromParentIfDefault())
    {
        return stream << IndexNode<T, 0, Default>::DefaultValue;
    }
    return stream << node.mValue[0];
}

template <typename T, T Default>
std::ostream& operator << (std::ostream& stream, const IndexNode<T, 0, Default>& node)
{
    if (node.DeleteNodeFromParentIfDefault())
    {
        return stream << IndexNode<T, 0, Default>::DefaultValue;
    }
    return stream << node.mValue[0];
}

template <typename T, size_t N, T Default>
class IndexNodeIterator
{
public:
    IndexNodeIterator(IndexNodePtr<T, N, Default> rootContainer, bool isBegin)
        : mInternalIterator(rootContainer->mChildren->begin())
        , mInternalIteratorEnd(rootContainer->mChildren->end())
        , mNextIterator(*mInternalIterator, true)
        , mNextIteratorEnd(*std::prev(mInternalIteratorEnd), false)
    {
        if (!isBegin)
        {
            mInternalIterator = mInternalIteratorEnd;
            mNextIterator = mNextIteratorEnd;
        }
    }

    IndexNodeIterator(const IndexNodeIterator& other)
        : mInternalIterator(other.mInternalIterator)
        , mInternalIteratorEnd(other.mInternalIteratorEnd)
        , mNextIterator(other.mNextIterator)
        , mNextIteratorEnd(other.mNextIteratorEnd)
    {
    }

    IndexNodeIterator& operator = (const IndexNodeIterator& other)
    {
        mInternalIterator = other.mInternalIterator;
        mInternalIteratorEnd = other.mInternalIteratorEnd;
        mNextIterator = other.mNextIterator;
        mNextIteratorEnd = other.mNextIteratorEnd;
        return *this;
    }

    IndexNodeIterator& operator ++ ()
    {
        assert (mInternalIterator != mInternalIteratorEnd);

        ++mNextIterator;

        if (mNextIterator.mInternalIterator == mNextIterator.mInternalIteratorEnd)
        {
            ++mInternalIterator;
            if (mInternalIterator != mInternalIteratorEnd)
            {
                mNextIterator = IndexNodeIterator<T, N - 1, Default>(*mInternalIterator, true);
            }
        }
        return *this;
    }

    bool operator == (IndexNodeIterator it) const
    {
        return mInternalIterator == it.mInternalIterator;
    }

    bool operator != (IndexNodeIterator it) const
    {
        return !(*this == it);
    }

    std::pair<std::vector<size_t>, T> operator * () const
    {
        assert (mInternalIterator != mInternalIteratorEnd);
        assert (mNextIterator != mNextIteratorEnd);

        size_t index = (*mInternalIterator)->mIndex;
        auto indicesNext = *mNextIterator;
        indicesNext.first.emplace(indicesNext.first.begin(), index);
        return indicesNext;
    }

public:
    IndexNodeInternalIterator<T, N - 1, Default> mInternalIterator;
    IndexNodeInternalIterator<T, N - 1, Default> mInternalIteratorEnd;
    IndexNodeIterator<T, N - 1, Default> mNextIterator;
    IndexNodeIterator<T, N - 1, Default> mNextIteratorEnd;
};

template <typename T, T Default>
class IndexNodeIterator<T, 0, Default>
{
public:
    IndexNodeIterator(IndexNodePtr<T, 0, Default>& rootContainer, bool isBegin)
        : mInternalIterator(isBegin ? rootContainer->mValue.begin() : rootContainer->mValue.end())
        , mInternalIteratorEnd(rootContainer->mValue.end()) {}

    IndexNodeIterator(const IndexNodeIterator& other)
        : mInternalIterator(other.mInternalIterator)
        , mInternalIteratorEnd(other.mInternalIteratorEnd)
    {
    }

    IndexNodeIterator& operator = (const IndexNodeIterator& other)
    {
        mInternalIterator = other.mInternalIterator;
        mInternalIteratorEnd = other.mInternalIteratorEnd;
        return *this;
    }

    IndexNodeIterator& operator ++ ()
    {
        assert (mInternalIterator != mInternalIteratorEnd);
        ++mInternalIterator;
        return *this;
    }

    bool operator == (IndexNodeIterator it) const
    {
        return mInternalIterator == it.mInternalIterator;
    }

    bool operator != (IndexNodeIterator it) const
    {
        return !(*this == it);
    }

    std::pair<std::vector<size_t>, T> operator * () const
    {
        std::vector<size_t> indices = {};
        return std::make_pair(indices, *mInternalIterator);
    }

    typename std::vector<T>::iterator mInternalIterator;
    typename std::vector<T>::iterator mInternalIteratorEnd;
};

template <typename T, size_t N, T Default>
class Matrix : public IExtraNodeDeleter
{
public:
    Matrix()
        : mRoot(nullptr)
    {
    }

    Matrix(IndexNodePtr<T, N, Default> root)
        : mRoot(root)
    {
    }

    size_t size() const
    {
        size_t s = 0;
        if (!mRoot)
            return 0;
        for (auto child : *mRoot->mChildren)
        {
            s += Matrix<T, N - 1, Default>(child).size();
        }
        return s;
    }

    const IndexNode<T, N - 1, Default>& operator [] (size_t index) const
    {
        assert (mRoot);
        return (*mRoot)[index];
    }

    IndexNode<T, N - 1, Default>& operator [] (size_t index)
    {
        if (!mRoot)
            mRoot = std::make_shared<IndexNode<T, N, Default>>(this, index);
        return (*mRoot)[index];
    }

    IndexNodeIterator<T, N, Default> begin()
    {
        return IndexNodeIterator<T, N, Default>(mRoot, true);
    }

    IndexNodeIterator<T, N, Default> end()
    {
        return IndexNodeIterator<T, N, Default>(mRoot, false);
    }

    void DeleteChildNode(void* node) override {
#ifdef DEBUG_PRINT
        std::cout << "DeleteChildNode, N==" << N << ", node=" << node << ", this=" << this << std::endl;
        mRoot = nullptr;
#endif
    }
private:
    IndexNodePtr<T, N, Default> mRoot;
};

template <typename T, T Default>
class Matrix<T, 0, Default>
{
public:
    Matrix()
        : mRoot(nullptr)
    {
    }

    Matrix(IndexNodePtr<T, 0, Default> root)
        : mRoot(root)
    {
    }

    size_t size() const
    {
         return 1;
    }

    IndexNodeIterator<T, 0, Default> begin()
    {
        return IndexNodeIterator<T, 0, Default>(*mRoot, true);
    }

    IndexNodeIterator<T, 0, Default> end()
    {
        return IndexNodeIterator<T, 0, Default>(*mRoot, false);
    }

private:
    IndexNodePtr<T, 0, Default> mRoot;
};

int version();
