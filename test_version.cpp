#define BOOST_TEST_MODULE test_version

#include "lib.h"

#include <boost/test/unit_test.hpp>
//#include <../../Boost/boost_1_83_0/boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(matrix_test)

BOOST_AUTO_TEST_CASE(test_empty)
{
    Matrix<int, 2, -1> matrix;

    BOOST_CHECK_EQUAL(matrix.size(), 0);
}

BOOST_AUTO_TEST_CASE(test_get_element)
{
    Matrix<int, 2, -1> matrix;

    auto a = matrix[0][0];
    BOOST_CHECK_EQUAL(static_cast<int>(a), -1);
    BOOST_CHECK(matrix[0][0] == -1);
    auto Lambda = [](const auto& elem)
    {
        BOOST_CHECK(static_cast<const int>(elem) == -1);
    };
    Lambda(matrix[0][0]);

    BOOST_CHECK_EQUAL(matrix.size(), 0);
    matrix[100][100] = 314;
    BOOST_CHECK(matrix[100][100] == 314);
    BOOST_CHECK_EQUAL(matrix.size(), 1);
}

BOOST_AUTO_TEST_CASE(test_iterator)
{
    Matrix<int, 2, -1> matrix;

    matrix[100][101] = 314;
    matrix[102][103] = 315;

    int loops = 0;
    for (auto c: matrix)
    {
        if (loops == 0)
        {
            std::vector<size_t> l{100, 101};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 314);
        }
        else if (loops == 1)
        {
            std::vector<size_t> l{102, 103};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 315);
        }
        ++loops;
    }
    BOOST_CHECK_EQUAL(loops, 2);
}

BOOST_AUTO_TEST_CASE(test_1_dimensional_matrix)
{
    Matrix<int, 1, 0> matrix;
    matrix[0] = 1000;
    matrix[1] = 1001;
    matrix[2] = 0;

    BOOST_CHECK_EQUAL(matrix.size(), 2);

    int loops = 0;
    for (auto occupied : matrix)
    {
        if (loops == 0)
            BOOST_CHECK_EQUAL(occupied.second, 1000);
        else if (loops == 1)
            BOOST_CHECK_EQUAL(occupied.second, 1001);
        ++loops;
    }
    BOOST_CHECK_EQUAL(loops, 2);
}

BOOST_AUTO_TEST_CASE(test_3_dimensional_matrix)
{
    Matrix<int, 3, -1> matrix;

    matrix[100][101][102] = 314;
    matrix[103][104][105] = 315;

    int loops = 0;
    for (auto c: matrix)
    {
        if (loops == 0)
        {
            std::vector<size_t> l{100, 101, 102};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 314);
        }
        else if (loops == 1)
        {
            std::vector<size_t> l{103, 104, 105};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 315);
        }
        ++loops;
    }
    BOOST_CHECK_EQUAL(loops, 2);
}

BOOST_AUTO_TEST_CASE(test_default_value)
{
    Matrix<int, 2, -1> matrix;

    matrix[100][101] = 314;
    matrix[102][103] = 315;
    matrix[104][105] = -1;

    BOOST_CHECK(matrix[104][105] == -1);
    BOOST_CHECK_EQUAL(matrix.size(), 2);
}

BOOST_AUTO_TEST_CASE(test_diagonals)
{
    Matrix<int, 2, 0> matrix;

    for (size_t i = 0; i < 10; ++i)
    {
        matrix[i][i] = i;
        matrix[i][9 - i] = 9 - i;
    }

    for (size_t i = 0; i < 10; ++i)
    {
        BOOST_CHECK(matrix[i][i] == i);
        BOOST_CHECK(matrix[i][9 - i] == 9 - i);
    }

    BOOST_CHECK_EQUAL(matrix.size(), 18);
}
}
