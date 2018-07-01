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

TEST(MathTest, ComparisonZero_Success) {
  const float first = std::sin(M_PI);
  const float second = std::cos(M_PI / 2.0f);

  EXPECT_TRUE(isZero(first));
  EXPECT_TRUE(isZero(second));

  EXPECT_FALSE(isPositive(first));
  EXPECT_FALSE(isPositive(second));

  EXPECT_FALSE(isNegative(first));
  EXPECT_FALSE(isNegative(second));

  EXPECT_TRUE(isEqual(0.0f, first));
  EXPECT_TRUE(isEqual(0.0f, second));
  EXPECT_TRUE(isEqual(first, second));
}

TEST(MathTest, ComparisonZero_Fail) {
  const float first = std::pow(10, -12);
  const float second = -std::pow(10, -12);

  EXPECT_FALSE(isZero(first));
  EXPECT_FALSE(isZero(second));

  EXPECT_TRUE(isPositive(first));
  EXPECT_FALSE(isPositive(second));

  EXPECT_FALSE(isNegative(first));
  EXPECT_TRUE(isNegative(second));

  EXPECT_FALSE(isEqual(0.0f, first));
  EXPECT_FALSE(isEqual(0.0f, second));
  EXPECT_FALSE(isEqual(first, second));

  //nextafter?
}

//999999999999999939709166371603178586112.0000000000000000000000000
//999999999999999788593438919774531747840.0000000000000000000000000
               //2
       //40564816789451701618636153159680.0000000000000000000000000

//340282346638528859811704183484516925440.0000000000000000000000000
//340282326356119256160033759537265639424.0000000000000000000000000
       //2
       //20282408394725850809318076579840.0000000000000000000000000


TEST(MathTest, ComparisonMax_Success) {
  const float first = std::nextafter(getMax<float>(), getMin<float>());
  const float second = -std::pow(10, 30);

  printf("%1.25f %1.25f %1.25f\n\n", getMax<float>(), getMax<float>()- std::nextafter(getMax<float>(), getMin<float>()),
         (getMax<float>() *  std::numeric_limits<float>::epsilon()));

  EXPECT_TRUE(isPositive(first));
  EXPECT_FALSE(isPositive(second));

  EXPECT_FALSE(isNegative(first));
  EXPECT_TRUE(isNegative(second));

  EXPECT_TRUE(isEqual(getMax<float>(), first));
  //EXPECT_TRUE(isEqual(getMin<float>(), second));
}

/*TEST(MathTest, ComparisonMax_Fail) {
  const float first =  std::nextafter(getMax<float>() / 2.0f, getMin<float>());
  const float second = std::nextafter(getMin<float>() / 2.0f, getMax<float>());

  EXPECT_TRUE(isPositive(first));
  EXPECT_FALSE(isPositive(second));

  EXPECT_FALSE(isNegative(first));
  EXPECT_TRUE(isNegative(second));

  EXPECT_FALSE(isEqual(getMax<float>(), first));
  EXPECT_FALSE(isEqual(getMin<float>(), second));
}*/

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
}

INSTANTIATE_TEST_CASE_P(IntersectionDataInstantiation, IntersectionTest,
                        ::testing::ValuesIn(intersection_data));

}  // namespace test
}  // namespace geometry
}  // namespace october
