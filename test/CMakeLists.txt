# Copyright (c) 2017 Artem Nosach
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 2.8.7)

project(october_test)

set(GCC_COVERAGE_COMPILE_FLAGS "-fno-exceptions -O2 -Wall -Wextra -Werror -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(
  ../include
  googletest/googletest/include
  googletest/googlemock/include
)

set(GEOMETRY_SOURCES
  src/geometry_test.cc
)

set(NODE_SOURCES
  src/node_test.cc
)

set(LIBRARIES
  gtest
  gtest_main
  gmock
  gmock_main
  pthread
)

add_executable(geometry_test ${GEOMETRY_SOURCES})
target_link_libraries(geometry_test ${LIBRARIES})

add_executable(node_test ${NODE_SOURCES})
target_link_libraries(node_test ${LIBRARIES})

enable_testing()

add_test(NAME geometry COMMAND geometry_test)
add_test(NAME node COMMAND node_test)

add_subdirectory(googletest)
