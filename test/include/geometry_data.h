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
#include <cmath>
#include <vector>

#include "geometry.h"

namespace october {
namespace geometry {
namespace test {

struct IntersectionTestData {
  geometry::Shape<float> shape_;
  geometry::Shape<float> childs_[8];
  float diag_;

  geometry::Ray<float> not_intersect_ray_;
  geometry::Ray<float> intersect_ray_;
  geometry::Ray<float> reflect_ray_;
  std::size_t intersect_child_;
  float intersect_dist_;
};

// clang-format off
static const std::vector<IntersectionTestData> kIntersectionTestData = {
  {
    {{0.0f, 0.0f,  0.0f}, {1.0f, 1.0f,  1.0f}},  // shape
    {
      {{0.0f, 0.0f,  0.0f}, {0.5f, 0.5f,  0.5f}},
      {{0.5f, 0.0f,  0.0f}, {1.0f, 0.5f,  0.5f}},
      {{0.0f, 0.5f,  0.0f}, {0.5f, 1.0f,  0.5f}},
      {{0.5f, 0.5f,  0.0f}, {1.0f, 1.0f,  0.5f}},

      {{0.0f, 0.0f,  0.5f}, {0.5f, 0.5f,  1.0f}},
      {{0.5f, 0.0f,  0.5f}, {1.0f, 0.5f,  1.0f}},
      {{0.0f, 0.5f,  0.5f}, {0.5f, 1.0f,  1.0f}},
      {{0.5f, 0.5f,  0.5f}, {1.0f, 1.0f,  1.0f}}
    },                                           // childs
    std::sqrt(3.0f),                             // diag

    {{0.0f, 0.0f,  0.0f}, {0.0f, 0.0f,  1.0f}},  // not intersect ray
    {{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f,  1.0f}},  // intersect ray
    {{0.0f, 0.0f,  0.0f}, {0.0f, 0.0f, -1.0f}},  // reflect ray
    0u,                                          // intersect child
    1.0f                                         // intersect dist
  },
  /*{
    {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f,  1.0f}},  // shape
    {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f,  0.0f}},  // not intersect ray
    {{0.0f, 0.0f, 2.0f}, {0.0f, 0.0f, -1.0f}},  // intersect ray
    {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f,  1.0f}},  // reflect ray
    0u,                                         // child
    0.0f,                                       // diag
    1.0f                                        // dist
  },
  {
    {{0.0f,  0.0f, 0.0f}, {1.0f,  1.0f, 1.0f}},  // shape
    {{0.5f,  1.0f, 1.0f}, {1.0f,  0.0f, 0.0f}},  // not intersect ray
    {{0.0f, -7.4f, 0.0f}, {0.0f,  1.0f, 0.0f}},  // intersect ray
    {{0.0f,  0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},  // reflect ray
    0u,                                          // child
    0.0f,                                        // diag
    7.4f                                         // dist
  },
  {
    {{    0.0f, 0.0f,    0.0f}, {1.0f, 1.0f,  1.0f}},  // shape
    {{-0.0001f, 0.0f,   -1.0f}, {0.0f, 0.0f,  1.0f}},  // not intersect ray
    {{ 0.0002f, 0.0f, -0.001f}, {0.0f, 0.0f,  1.0f}},  // intersect ray
    {{ 0.0002f, 0.0f,    0.0f}, {0.0f, 0.0f, -1.0f}},  // reflect ray
    0u,                                                // child
    0.0f,                                              // diag
    0.001f                                             // dist
  },
  {
    {{0.0f, 0.0f,     0.0f},                        {1.0f, 1.0f,  1.0f}},   // shape
    {{0.0f, 0.0f, -1.0001f}, normalizeVector<float>({1.0f, 1.0f,  1.0f})},  // not intersect ray
    {{0.0f, 0.0f,    -1.0f}, normalizeVector<float>({1.0f, 1.0f,  1.0f})},  // intersect ray
    {{1.0f, 1.0f,     0.0f}, normalizeVector<float>({1.0f, 1.0f, -1.0f})},  // reflect ray
    0u,                                                                     // child
    0.0f,                                                                   // diag
    std::sqrt(3.0f)                                                         // dist
  },
  {
    {{0.0f,    0.0f,  0.0f}, {1.0f, 0.0f,  1.0f}},  // shape
    {{0.0f, 0.0001f, -2.0f}, {0.0f, 0.0f,  1.0f}},  // not intersect ray
    {{0.0f,    0.0f, -2.0f}, {0.0f, 0.0f,  1.0f}},  // intersect ray
    {{0.0f,    0.0f,  0.0f}, {0.0f, 0.0f, -1.0f}},  // reflect ray
    0u,                                             // child
    0.0f,                                           // diag
    2.0f                                            // dist
  },
  {
    {{0.0f, 0.0f,     0.0f},                        {1.0f, 1.0f,  0.0f}},   // shape
    {{0.0f, 0.0f, -1.0001f}, normalizeVector<float>({1.0f, 1.0f,  1.0f})},  // not intersect ray
    {{0.0f, 0.0f,    -1.0f}, normalizeVector<float>({1.0f, 1.0f,  1.0f})},  // intersect ray
    {{1.0f, 1.0f,     0.0f}, normalizeVector<float>({1.0f, 1.0f, -1.0f})},  // reflect ray
    0u,                                                                     // child
    0.0f,                                                                   // diag
    std::sqrt(3.0f)                                                         // dist
  },
  {
    {{0.0f, 0.0f, 0.0f},                        {2.0f, 2.0f,  2.0f}},   // shape
    {{0.5f, 0.5f, 2.5f}, normalizeVector<float>({0.0f, 1.0f,  1.0f})},  // not intersect ray
    {{0.5f, 0.5f, 2.5f}, normalizeVector<float>({0.0f, 1.0f, -1.0f})},  // intersect ray
    {{0.5f, 1.0f, 2.0f}, normalizeVector<float>({0.0f, 1.0f,  1.0f})},  // reflect ray
    0u,                                                                 // child
    0.0f,                                                               // diag
    1 / std::sqrt(2.0f)                                                 // dist
  }*/
};
// clang-format on

}  // namespace test
}  // namespace geometry
}  // namespace october
