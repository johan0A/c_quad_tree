cmake_minimum_required(VERSION 3.5...3.21)
project(c_quad_tree VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(c_quad_tree main.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)