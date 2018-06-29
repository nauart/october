/*
 * Copyright (c) 2017 Artem Nosach
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

#include <cmath>

#include "gtest/gtest.h"

#include "geometry.h"

namespace october {
namespace geometry {
namespace test {

template <typename T>
struct IntersectionTestData {
  Shape<T> shape_;
  Ray<T> not_intersect_ray_;
  Ray<T> intersect_ray_;
  Ray<T> reflect_ray_;
  T dist_;
};

// clang-format off
static const std::vector<IntersectionTestData<float> > intersection_data = {
  {
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
  /*{
    {{0.0f,  0.0f, 0.0f},                        {2.0f, 2.0f,  2.0f}},   // shape
    {{0.5f, -0.5f, 2.5f}, normalizeVector<float>({0.0f, 1.0f,  1.0f})},  // not intersect ray
    {{0.5f, -0.5f, 2.5f}, normalizeVector<float>({0.0f, 1.0f, -1.0f})},  // intersect ray
    {{0.5f, 0.0f, 2.0f}, normalizeVector<float>({0.0f, 1.0f,  1.0f})},  // reflect ray
    1 / std::sqrt(2.0f)                                                 // dist
  }*/
};
// clang-format on

class IntersectionTest
    : public ::testing::TestWithParam<IntersectionTestData<float> > {};

TEST_P(IntersectionTest, RayShapeIntersection_Success) {
  Ray<float> reflect_ray;
  const float dist = rayShapeIntersection(GetParam().intersect_ray_,
                                          GetParam().shape_, reflect_ray);

  EXPECT_TRUE(isPositive(dist));
  EXPECT_TRUE(isEqual(GetParam().dist_, dist));

  EXPECT_TRUE(isVectorEqual(GetParam().reflect_ray_.pos_, reflect_ray.pos_));
  EXPECT_TRUE(isVectorEqual(GetParam().reflect_ray_.dir_, reflect_ray.dir_));
}

TEST_P(IntersectionTest, RayShapeIntersection_Fail) {
  Ray<float> reflect_ray;
  const float dist = rayShapeIntersection(GetParam().not_intersect_ray_,
                                          GetParam().shape_, reflect_ray);

  EXPECT_TRUE(isNegative(dist));

  EXPECT_TRUE(isZero(float(std::cos(M_PI / 2))));
  EXPECT_TRUE(isZero(float(std::sin(M_PI))));

  EXPECT_FALSE(isPositive(float(std::cos(M_PI / 2))));
  EXPECT_FALSE(isPositive(float(std::sin(M_PI))));

  EXPECT_FALSE(isNegative(float(std::cos(M_PI / 2))));
  EXPECT_FALSE(isNegative(float(std::sin(M_PI))));

  EXPECT_TRUE(isEqual(float(std::cos(M_PI / 2)), float(std::sin(M_PI))));

  //sum
}

INSTANTIATE_TEST_CASE_P(IntersectionDataInstantiation, IntersectionTest,
                        ::testing::ValuesIn(intersection_data));

}  // namespace test
}  // namespace geometry
}  // namespace october
