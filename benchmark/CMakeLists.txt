# Copyright (c) 2018 Artem Nosach
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

include_directories(
  benchmark/include
)

set(GEOMETRY_SOURCES
  src/geometry_benchmark.cc
)

set(NODE_SOURCES
  src/node_benchmark.cc
)

set(LIBRARIES
  benchmark
  pthread
)

add_executable(geometry_benchmark ${GEOMETRY_SOURCES})
target_link_libraries(geometry_benchmark ${LIBRARIES})

add_executable(node_benchmark ${NODE_SOURCES})
target_link_libraries(node_benchmark ${LIBRARIES})

add_custom_target(benchmark_run
                  COMMAND ./geometry_benchmark
                  COMMAND ./node_benchmark
                  WORKING_DIRECTORY ./)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing")

add_subdirectory(benchmark)
