/*
 * Copyright (c) 2018 Artem Nosach
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cstddef>
#include <memory>
#include <vector>

#include "benchmark/benchmark.h"

#include "tree.h"

namespace october {
namespace node {
namespace benchmark {

class TreeBenchmark : public ::benchmark::Fixture {};

BENCHMARK_F(TreeBenchmark, insertRemoveNodesBenchmark)
(::benchmark::State& state) {
  tree::Tree<std::size_t, 8u, std::size_t> tree(0u);

  for (auto _ : state) {
    std::size_t depth = 0u;

    tree.insertNodes(
        [&depth](const std::size_t&, const std::size_t&) {
          if (depth++ < 8u) {
            return std::vector<std::size_t>({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
          } else {
            return std::vector<std::size_t>();
          }
        },
        [](const std::size_t&, const std::size_t&) { return 0u; }, 0u);

    tree.removeNodes(
        [](const std::size_t&, const std::size_t&) {
          return std::vector<std::size_t>({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
        },
        [](const std::size_t&, const std::size_t&) { return 0u; }, 0u);
  }
}

BENCHMARK_F(TreeBenchmark, processNodesBenchmark)
(::benchmark::State& state) {
  tree::Tree<std::size_t, 8u, std::size_t> tree(0u);

  for (auto _ : state) {
    std::size_t depth = 0u;

    tree.insertNodes(
        [&depth](const std::size_t&, const std::size_t&) {
          if (depth++ < 8u) {
            return std::vector<std::size_t>({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
          } else {
            return std::vector<std::size_t>();
          }
        },
        [](const std::size_t&, const std::size_t&) { return 0u; }, 0u);

    tree.processNodes(
        [](const std::size_t&, const std::size_t&, const std::size_t&) {
          return std::vector<std::size_t>({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
        },
        [](const std::size_t&, const std::size_t&) { return 0u; }, 0u);

    tree.removeNodes(
        [](const std::size_t&, const std::size_t&) {
          return std::vector<std::size_t>({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
        },
        [](const std::size_t&, const std::size_t&) { return 0u; }, 0u);
  }
}

}  // namespace benchmark
}  // namespace node
}  // namespace october

BENCHMARK_MAIN();
