/*
 * Copyright (c) 2019 Artem Nosach
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

#pragma once

#include <cstdint>
#include <vector>

#include "geometry.h"

namespace october {
namespace geometry {
namespace test {

template <typename T>
struct BoxIntersectionData {
  Box<T> box_;
  Box<T> childs_[8u];
  T diag_;

  Ray<T> not_intersect_ray_;
  Ray<T> intersect_ray_;
  Ray<T> reflect_ray_;

  std::size_t intersect_child_;
  T dist_;
};

// clang-format off
static const std::vector<BoxIntersectionData<float> > intersection_data = {
  /**
   * @brief Manually prepared examples to check basic use cases
   */
  /*{
    {{0.0f, 0.0f,  0.0f}, {1.0f, 1.0f,  1.0f}},  // shape
    {{0.0f, 0.0f,  0.0f}, {0.0f, 0.0f,  1.0f}},  // not intersect ray
    {{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f,  1.0f}},  // intersect ray
    {{0.0f, 0.0f,  0.0f}, {0.0f, 0.0f, -1.0f}},  // reflect ray
    1.0f                                         // dist
  },
  {
    {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f,  1.0f}},  // shape
    {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f,  0.0f}},  // not intersect ray
    {{0.0f, 0.0f, 2.0f}, {0.0f, 0.0f, -1.0f}},  // intersect ray
    {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f,  1.0f}},  // reflect ray
    1.0f                                        // dist
  },
  {
    {{0.0f,  0.0f, 0.0f}, {1.0f,  1.0f, 1.0f}},  // shape
    {{0.5f,  1.0f, 1.0f}, {1.0f,  0.0f, 0.0f}},  // not intersect ray
    {{0.0f, -7.4f, 0.0f}, {0.0f,  1.0f, 0.0f}},  // intersect ray
    {{0.0f,  0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},  // reflect ray
    7.4f                                         // dist
  },
  {
    {{    0.0f, 0.0f,    0.0f}, {1.0f, 1.0f,  1.0f}},  // shape
    {{-0.0001f, 0.0f,   -1.0f}, {0.0f, 0.0f,  1.0f}},  // not intersect ray
    {{ 0.0002f, 0.0f, -0.001f}, {0.0f, 0.0f,  1.0f}},  // intersect ray
    {{ 0.0002f, 0.0f,    0.0f}, {0.0f, 0.0f, -1.0f}},  // reflect ray
    0.001f                                             // dist
  },
  {
    {{0.0f, 0.0f,     0.0f},                        {1.0f, 1.0f,  1.0f}},   // shape
    {{0.0f, 0.0f, -1.0001f}, normalizeVector<float>({1.0f, 1.0f,  1.0f})},  // not intersect ray
    {{0.0f, 0.0f,    -1.0f}, normalizeVector<float>({1.0f, 1.0f,  1.0f})},  // intersect ray
    {{1.0f, 1.0f,     0.0f}, normalizeVector<float>({1.0f, 1.0f, -1.0f})},  // reflect ray
    std::sqrt(3.0f)                                                         // dist
  },
  {
    {{0.0f,    0.0f,  0.0f}, {1.0f, 0.0f,  1.0f}},  // shape
    {{0.0f, 0.0001f, -2.0f}, {0.0f, 0.0f,  1.0f}},  // not intersect ray
    {{0.0f,    0.0f, -2.0f}, {0.0f, 0.0f,  1.0f}},  // intersect ray
    {{0.0f,    0.0f,  0.0f}, {0.0f, 0.0f, -1.0f}},  // reflect ray
    2.0f                                            // dist
  },
  {
    {{0.0f, 0.0f,     0.0f},                        {1.0f, 1.0f,  0.0f}},   // shape
    {{0.0f, 0.0f, -1.0001f}, normalizeVector<float>({1.0f, 1.0f,  1.0f})},  // not intersect ray
    {{0.0f, 0.0f,    -1.0f}, normalizeVector<float>({1.0f, 1.0f,  1.0f})},  // intersect ray
    {{1.0f, 1.0f,     0.0f}, normalizeVector<float>({1.0f, 1.0f, -1.0f})},  // reflect ray
    std::sqrt(3.0f)                                                         // dist
  },
  {
    {{0.0f,  0.0f, 0.0f},                        {2.0f, 2.0f,  2.0f}},   // shape
    {{0.5f, -0.5f, 2.5f}, normalizeVector<float>({0.0f, 1.0f,  1.0f})},  // not intersect ray
    {{0.5f, -0.5f, 2.5f}, normalizeVector<float>({0.0f, 1.0f, -1.0f})},  // intersect ray
    {{0.5f, 0.0f, 2.0f}, normalizeVector<float>({0.0f, 1.0f,  1.0f})},  // reflect ray
    1 / std::sqrt(2.0f)                                                 // dist
  },*/

  /**
   * @brief Automatically generated examples
   */
  #include "generated_data.txt"
};
// clang-format on

}  // namespace test
}  // namespace geometry
}  // namespace october
