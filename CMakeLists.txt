cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(Matrix VERSION ${PROJECT_VERSION})

set(DEBUG_PRINT "0")

if (DEBUG_PRINT)
	message("DEBUG_PRINT is activated!")
	add_definitions(-DDEBUG_PRINT)
endif()

configure_file(version.h.in version.h)

add_executable(Matrix_cli main.cpp)
add_library(Matrix lib.cpp)
add_executable(test_version test_version.cpp)

set_target_properties(Matrix_cli Matrix test_version PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(Matrix
    PRIVATE "${CMAKE_BINARY_DIR}"
)
set_target_properties(test_version PROPERTIES
)

target_link_libraries(Matrix_cli PRIVATE
    Matrix
)
target_link_libraries(test_version
    ${Boost_LIBRARIES}
    Matrix
)

if (MSVC)
    target_compile_options(Matrix_cli PRIVATE
        /W4
    )
    target_compile_options(Matrix PRIVATE
        /W4
    )
    target_compile_options(test_version PRIVATE
        /W4
    )
else ()
    target_compile_options(Matrix_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(Matrix PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS Matrix_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()

add_test(test_version test_version)
