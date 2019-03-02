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
#include <limits>

#include "gtest/gtest.h"

#include "geometry.h"

namespace october {
namespace geometry {
namespace test {

template <typename T>
class MathTest : public ::testing::Test {
 protected:
  static constexpr T kMin = getMin<T>();
  static constexpr T kMax = getMax<T>();

  static constexpr T kEpsilon = std::numeric_limits<T>::epsilon();
  static constexpr T kExceedEpsilon = kEpsilon * 2;

  static constexpr T kExceedMinEpsilon = kEpsilon * std::abs(kMin) * 2;
  static constexpr T kExceedMaxEpsilon = kEpsilon * kMax * 2;
};

typedef ::testing::Types<float, double, long double> MathTestTypes;

TYPED_TEST_CASE(MathTest, MathTestTypes);

TYPED_TEST(MathTest, isPositive_PositiveValue_Success) {
  EXPECT_TRUE(isPositive(TypeParam(1)));
  EXPECT_TRUE(isPositive(MathTest<TypeParam>::kMax));
  EXPECT_TRUE(isPositive(MathTest<TypeParam>::kExceedEpsilon));
}

TYPED_TEST(MathTest, isPositive_NegativeValue_Fail) {
  EXPECT_FALSE(isPositive(TypeParam(-1)));
  EXPECT_FALSE(isPositive(MathTest<TypeParam>::kMin));
  EXPECT_FALSE(isPositive(-MathTest<TypeParam>::kExceedEpsilon));
}

TYPED_TEST(MathTest, isPositive_ZeroValue_Fail) {
  EXPECT_FALSE(isPositive(TypeParam(0)));
  EXPECT_FALSE(isPositive(MathTest<TypeParam>::kEpsilon));
  EXPECT_FALSE(isPositive(-MathTest<TypeParam>::kEpsilon));
}

TYPED_TEST(MathTest, isNegative_NegativeValue_Success) {
  EXPECT_TRUE(isNegative(TypeParam(-1)));
  EXPECT_TRUE(isNegative(MathTest<TypeParam>::kMin));
  EXPECT_TRUE(isNegative(-MathTest<TypeParam>::kExceedEpsilon));
}

TYPED_TEST(MathTest, isNegative_PositiveValue_Fail) {
  EXPECT_FALSE(isNegative(TypeParam(1)));
  EXPECT_FALSE(isNegative(MathTest<TypeParam>::kMax));
  EXPECT_FALSE(isNegative(MathTest<TypeParam>::kExceedEpsilon));
}

TYPED_TEST(MathTest, isNegative_ZeroValue_Fail) {
  EXPECT_FALSE(isNegative(TypeParam(0)));
  EXPECT_FALSE(isNegative(MathTest<TypeParam>::kEpsilon));
  EXPECT_FALSE(isNegative(-MathTest<TypeParam>::kEpsilon));
}

TYPED_TEST(MathTest, isZero_ZeroValue_Success) {
  EXPECT_TRUE(isZero(TypeParam(0)));
  EXPECT_TRUE(isZero(MathTest<TypeParam>::kEpsilon));
  EXPECT_TRUE(isZero(-MathTest<TypeParam>::kEpsilon));
}

TYPED_TEST(MathTest, isZero_NonZeroValue_Fail) {
  EXPECT_FALSE(isZero(TypeParam(1)));
  EXPECT_FALSE(isZero(TypeParam(-1)));
  EXPECT_FALSE(isZero(MathTest<TypeParam>::kMax));
  EXPECT_FALSE(isZero(MathTest<TypeParam>::kMin));
  EXPECT_FALSE(isZero(MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isZero(-MathTest<TypeParam>::kExceedEpsilon));
}

TYPED_TEST(MathTest, isMore_FirstValueMore_Success) {
  EXPECT_TRUE(isMore(TypeParam(1), TypeParam( 0)));
  EXPECT_TRUE(isMore(TypeParam(0), TypeParam(-1)));

  EXPECT_TRUE(isMore(MathTest<TypeParam>::kMax, TypeParam(0)));
  EXPECT_TRUE(isMore(TypeParam(0), MathTest<TypeParam>::kMin));
  EXPECT_TRUE(isMore(MathTest<TypeParam>::kMax, TypeParam(1)));
  EXPECT_TRUE(isMore(TypeParam(-1), MathTest<TypeParam>::kMin));

  EXPECT_TRUE(isMore(MathTest<TypeParam>::kExceedEpsilon, TypeParam(0)));
  EXPECT_TRUE(isMore(TypeParam(0), -MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_TRUE(isMore(TypeParam(1), MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_TRUE(isMore(-MathTest<TypeParam>::kExceedEpsilon, TypeParam(-1)));

  EXPECT_TRUE(isMore(MathTest<TypeParam>::kMax, MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedMaxEpsilon));
  EXPECT_TRUE(isMore(MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedMinEpsilon, MathTest<TypeParam>::kMin));
}

TYPED_TEST(MathTest, isMore_SecondValueMore_Fail) {
  EXPECT_FALSE(isMore(TypeParam(-1), TypeParam(0)));
  EXPECT_FALSE(isMore(TypeParam(0), TypeParam(1)));

  EXPECT_FALSE(isMore(MathTest<TypeParam>::kMin, TypeParam(0)));
  EXPECT_FALSE(isMore(TypeParam(0), MathTest<TypeParam>::kMax));
  EXPECT_FALSE(isMore(MathTest<TypeParam>::kMin, TypeParam(-1)));
  EXPECT_FALSE(isMore(TypeParam(1), MathTest<TypeParam>::kMax));

  EXPECT_FALSE(isMore(-MathTest<TypeParam>::kExceedEpsilon, TypeParam(0)));
  EXPECT_FALSE(isMore(TypeParam(0), MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isMore(TypeParam(-1), -MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isMore(MathTest<TypeParam>::kExceedEpsilon, TypeParam(1)));

  EXPECT_FALSE(isMore(MathTest<TypeParam>::kMin, MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedMinEpsilon));
  EXPECT_FALSE(isMore(MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedMaxEpsilon, MathTest<TypeParam>::kMax));
}

TYPED_TEST(MathTest, isMore_EqualValues_Fail) {
  EXPECT_FALSE(isMore(TypeParam(0), TypeParam(0)));

  EXPECT_FALSE(isMore(MathTest<TypeParam>::kEpsilon, TypeParam(0)));
  EXPECT_FALSE(isMore(TypeParam(0), MathTest<TypeParam>::kEpsilon));
  EXPECT_FALSE(isMore(-MathTest<TypeParam>::kEpsilon, TypeParam(0)));
  EXPECT_FALSE(isMore(TypeParam(0), -MathTest<TypeParam>::kEpsilon));

  EXPECT_FALSE(isMore(MathTest<TypeParam>::kEpsilon, MathTest<TypeParam>::kEpsilon));
  EXPECT_FALSE(isMore(MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kEpsilon));
  EXPECT_FALSE(isMore(MathTest<TypeParam>::kEpsilon, MathTest<TypeParam>::kExceedEpsilon));

  EXPECT_FALSE(isMore(MathTest<TypeParam>::kMax, MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isMore(MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kMax));
  EXPECT_FALSE(isMore(MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kMin));
  EXPECT_FALSE(isMore(MathTest<TypeParam>::kMin, MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedEpsilon));
}

TYPED_TEST(MathTest, isLess_SecondValueMore_Success) {
  EXPECT_TRUE(isLess(TypeParam(-1), TypeParam(0)));
  EXPECT_TRUE(isLess(TypeParam(0), TypeParam(1)));

  EXPECT_TRUE(isLess(MathTest<TypeParam>::kMin, TypeParam(0)));
  EXPECT_TRUE(isLess(TypeParam(0), MathTest<TypeParam>::kMax));
  EXPECT_TRUE(isLess(MathTest<TypeParam>::kMin, TypeParam(-1)));
  EXPECT_TRUE(isLess(TypeParam(1), MathTest<TypeParam>::kMax));

  EXPECT_TRUE(isLess(-MathTest<TypeParam>::kExceedEpsilon, TypeParam(0)));
  EXPECT_TRUE(isLess(TypeParam(0), MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_TRUE(isLess(TypeParam(-1), -MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_TRUE(isLess(MathTest<TypeParam>::kExceedEpsilon, TypeParam(1)));

  EXPECT_TRUE(isLess(MathTest<TypeParam>::kMin, MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedMinEpsilon));
  EXPECT_TRUE(isLess(MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedMaxEpsilon, MathTest<TypeParam>::kMax));
}

TYPED_TEST(MathTest, isLess_FirstValueMore_Fail) {
  EXPECT_FALSE(isLess(TypeParam(1), TypeParam( 0)));
  EXPECT_FALSE(isLess(TypeParam(0), TypeParam(-1)));

  EXPECT_FALSE(isLess(MathTest<TypeParam>::kMax, TypeParam(0)));
  EXPECT_FALSE(isLess(TypeParam(0), MathTest<TypeParam>::kMin));
  EXPECT_FALSE(isLess(MathTest<TypeParam>::kMax, TypeParam(1)));
  EXPECT_FALSE(isLess(TypeParam(-1), MathTest<TypeParam>::kMin));

  EXPECT_FALSE(isLess(MathTest<TypeParam>::kExceedEpsilon, TypeParam(0)));
  EXPECT_FALSE(isLess(TypeParam(0), -MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isLess(TypeParam(1), MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isLess(-MathTest<TypeParam>::kExceedEpsilon, TypeParam(-1)));

  EXPECT_FALSE(isLess(MathTest<TypeParam>::kMax, MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedMaxEpsilon));
  EXPECT_FALSE(isLess(MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedMinEpsilon, MathTest<TypeParam>::kMin));
}

TYPED_TEST(MathTest, isLess_EqualValues_Fail) {
  EXPECT_FALSE(isLess(TypeParam(0), TypeParam(0)));

  EXPECT_FALSE(isLess(MathTest<TypeParam>::kEpsilon, TypeParam(0)));
  EXPECT_FALSE(isLess(TypeParam(0), MathTest<TypeParam>::kEpsilon));
  EXPECT_FALSE(isLess(-MathTest<TypeParam>::kEpsilon, TypeParam(0)));
  EXPECT_FALSE(isLess(TypeParam(0), -MathTest<TypeParam>::kEpsilon));

  EXPECT_FALSE(isLess(MathTest<TypeParam>::kEpsilon, MathTest<TypeParam>::kEpsilon));
  EXPECT_FALSE(isLess(MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kEpsilon));
  EXPECT_FALSE(isLess(MathTest<TypeParam>::kEpsilon, MathTest<TypeParam>::kExceedEpsilon));

  EXPECT_FALSE(isLess(MathTest<TypeParam>::kMax, MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isLess(MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kMax));
  EXPECT_FALSE(isLess(MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kMin));
  EXPECT_FALSE(isLess(MathTest<TypeParam>::kMin, MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedEpsilon));
}

TYPED_TEST(MathTest, isEqual_EqualValues_Success) {
  EXPECT_TRUE(isEqual(TypeParam(0), TypeParam(0)));

  EXPECT_TRUE(isEqual(MathTest<TypeParam>::kEpsilon, TypeParam(0)));
  EXPECT_TRUE(isEqual(TypeParam(0), MathTest<TypeParam>::kEpsilon));
  EXPECT_TRUE(isEqual(-MathTest<TypeParam>::kEpsilon, TypeParam(0)));
  EXPECT_TRUE(isEqual(TypeParam(0), -MathTest<TypeParam>::kEpsilon));

  EXPECT_TRUE(isEqual(MathTest<TypeParam>::kEpsilon, MathTest<TypeParam>::kEpsilon));
  EXPECT_TRUE(isEqual(MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kEpsilon));
  EXPECT_TRUE(isEqual(MathTest<TypeParam>::kEpsilon, MathTest<TypeParam>::kExceedEpsilon));

  EXPECT_TRUE(isEqual(MathTest<TypeParam>::kMax, MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_TRUE(isEqual(MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kMax));
  EXPECT_TRUE(isEqual(MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedEpsilon, MathTest<TypeParam>::kMin));
  EXPECT_TRUE(isEqual(MathTest<TypeParam>::kMin, MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedEpsilon));
}

TYPED_TEST(MathTest, isEqual_NonEqualValues_Fail) {
  EXPECT_FALSE(isEqual(TypeParam(1), TypeParam( 0)));
  EXPECT_FALSE(isEqual(TypeParam(0), TypeParam(1)));
  EXPECT_FALSE(isEqual(TypeParam(0), TypeParam(-1)));
  EXPECT_FALSE(isEqual(TypeParam(-1), TypeParam(0)));

  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kMax, TypeParam(0)));
  EXPECT_FALSE(isEqual(TypeParam(0), MathTest<TypeParam>::kMax));
  EXPECT_FALSE(isEqual(TypeParam(0), MathTest<TypeParam>::kMin));
  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kMin, TypeParam(0)));

  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kMax, TypeParam(1)));
  EXPECT_FALSE(isEqual(TypeParam(1), MathTest<TypeParam>::kMax));
  EXPECT_FALSE(isEqual(TypeParam(-1), MathTest<TypeParam>::kMin));
  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kMin, TypeParam(-1)));

  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kExceedEpsilon, TypeParam(0)));
  EXPECT_FALSE(isEqual(TypeParam(0), MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isEqual(TypeParam(0), -MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isEqual(-MathTest<TypeParam>::kExceedEpsilon, TypeParam(0)));

  EXPECT_FALSE(isEqual(TypeParam(1), MathTest<TypeParam>::kExceedEpsilon));
  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kExceedEpsilon, TypeParam(1)));
  EXPECT_FALSE(isEqual(-MathTest<TypeParam>::kExceedEpsilon, TypeParam(-1)));
  EXPECT_FALSE(isEqual(TypeParam(-1), -MathTest<TypeParam>::kExceedEpsilon));

  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kMax, MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedMaxEpsilon));
  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kMax - MathTest<TypeParam>::kExceedMaxEpsilon, MathTest<TypeParam>::kMax));
  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedMinEpsilon, MathTest<TypeParam>::kMin));
  EXPECT_FALSE(isEqual(MathTest<TypeParam>::kMin, MathTest<TypeParam>::kMin + MathTest<TypeParam>::kExceedMinEpsilon));
}
/*
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
  {
    {{0.0f,  0.0f, 0.0f},                        {2.0f, 2.0f,  2.0f}},   // shape
    {{0.5f, -0.5f, 2.5f}, normalizeVector<float>({0.0f, 1.0f,  1.0f})},  // not intersect ray
    {{0.5f, -0.5f, 2.5f}, normalizeVector<float>({0.0f, 1.0f, -1.0f})},  // intersect ray
    {{0.5f, 0.0f, 2.0f}, normalizeVector<float>({0.0f, 1.0f,  1.0f})},  // reflect ray
    1 / std::sqrt(2.0f)                                                 // dist
  }
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
}

INSTANTIATE_TEST_CASE_P(IntersectionDataInstantiation, IntersectionTest,
                        ::testing::ValuesIn(intersection_data));
*/
}  // namespace test
}  // namespace geometry
}  // namespace october
