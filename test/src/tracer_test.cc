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
#include <functional>
#include <numeric>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "geometry_data.h"
#include "tracer.h"

namespace october {
namespace tracer {
namespace test {

using ChildsIndexesData = std::vector<std::size_t>;
using PayloadTestData = std::size_t;

using InsertRemoveFunc = std::function<ChildsIndexesData(
    const geometry::Shape<float>&, geometry::Ray<float>, const float&)>;
using ProcessFunc = std::function<ChildsIndexesData(
    const PayloadTestData&, const geometry::Shape<float>&, geometry::Ray<float>,
    const float&)>;
using ShapeFunc = std::function<geometry::Shape<float>(
    std::size_t, const geometry::Shape<float>&)>;

class TreeMock {
 public:
  MOCK_CONST_METHOD4(insertNodes,
                     void(const InsertRemoveFunc&, const ShapeFunc&,
                          const geometry::Ray<float>&, const float&));
  MOCK_CONST_METHOD4(removeNodes,
                     void(const InsertRemoveFunc&, const ShapeFunc&,
                          const geometry::Ray<float>&, const float&));
  MOCK_CONST_METHOD4(processNodes,
                     void(const ProcessFunc&, const ShapeFunc&,
                          const geometry::Ray<float>&, const float&));
  MOCK_CONST_METHOD0(nodesIndexes, const ChildsIndexesData&());
};

class TracerTest
    : public ::testing::TestWithParam<
          std::tuple<geometry::test::IntersectionTestData, std::size_t> > {
 protected:
  void SetUp() {
    childs_indexes_.resize(std::get<1>(GetParam()));
    std::iota(childs_indexes_.begin(), childs_indexes_.end(), 0u);
  }

  void checkInsertRemoveFunc(const InsertRemoveFunc& func) {
    using namespace ::testing;

    const geometry::test::IntersectionTestData& data = std::get<0>(GetParam());
    const float delta = 0.001f;

    EXPECT_EQ(ChildsIndexesData(),
              func(data.shape_, data.not_intersect_ray_, data.diag_));
    EXPECT_EQ(ChildsIndexesData(),
              func(data.shape_, data.not_intersect_ray_, data.diag_ + delta));
    EXPECT_EQ(ChildsIndexesData(),
              func(data.shape_, data.not_intersect_ray_, data.diag_ - delta));

    EXPECT_EQ(ChildsIndexesData(),
              func(data.shape_, data.intersect_ray_, data.diag_));
    EXPECT_EQ(ChildsIndexesData(),
              func(data.shape_, data.intersect_ray_, data.diag_ + delta));

    EXPECT_CALL(tree_mock_, nodesIndexes())
        .WillOnce(ReturnRef(childs_indexes_));
    EXPECT_EQ(ChildsIndexesData({data.intersect_child_}),
              func(data.shape_, data.intersect_ray_, data.diag_ - delta));
  }

  void checkProcessFunc(const ProcessFunc& func) {
    using namespace ::testing;

    const geometry::test::IntersectionTestData& data = std::get<0>(GetParam());
    const geometry::Ray<float> ray = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    const float delta = 0.001f;

    dist_ = geometry::getMin<float>();
    reflect_ray_ = ray;
    reflect_payload_ = 0u;

    EXPECT_EQ(ChildsIndexesData(),
              func(1u, data.shape_, data.not_intersect_ray_, data.diag_));
    EXPECT_EQ(
        ChildsIndexesData(),
        func(1u, data.shape_, data.not_intersect_ray_, data.diag_ + delta));
    EXPECT_EQ(
        ChildsIndexesData(),
        func(1u, data.shape_, data.not_intersect_ray_, data.diag_ - delta));

    EXPECT_FALSE(geometry::isPositive(dist_));
    EXPECT_EQ(0u, reflect_payload_);

    dist_ = geometry::getMin<float>();
    reflect_ray_ = ray;
    reflect_payload_ = 0u;

    EXPECT_EQ(ChildsIndexesData(),
              func(1u, data.shape_, data.intersect_ray_, data.diag_));

    EXPECT_TRUE(geometry::isPositive(dist_));
    EXPECT_TRUE(
        geometry::isVectorEqual(data.reflect_ray_.pos_, reflect_ray_.pos_));
    EXPECT_TRUE(
        geometry::isVectorEqual(data.reflect_ray_.dir_, reflect_ray_.dir_));
    EXPECT_EQ(1u, reflect_payload_);

    dist_ = geometry::getMin<float>();
    reflect_ray_ = ray;
    reflect_payload_ = 0u;

    EXPECT_EQ(ChildsIndexesData(),
              func(1u, data.shape_, data.intersect_ray_, data.diag_ + delta));

    EXPECT_TRUE(geometry::isPositive(dist_));
    EXPECT_TRUE(
        geometry::isVectorEqual(data.reflect_ray_.pos_, reflect_ray_.pos_));
    EXPECT_TRUE(
        geometry::isVectorEqual(data.reflect_ray_.dir_, reflect_ray_.dir_));
    EXPECT_EQ(1u, reflect_payload_);

    dist_ = geometry::getMin<float>();
    reflect_ray_ = ray;
    reflect_payload_ = 0u;

    EXPECT_CALL(tree_mock_, nodesIndexes())
        .WillRepeatedly(ReturnRef(childs_indexes_));
    EXPECT_EQ(childs_indexes_,
              func(1u, data.shape_, data.intersect_ray_, data.diag_ - delta));

    EXPECT_TRUE(geometry::isPositive(dist_));
    EXPECT_TRUE(
        geometry::isVectorEqual(data.reflect_ray_.pos_, reflect_ray_.pos_));
    EXPECT_TRUE(
        geometry::isVectorEqual(data.reflect_ray_.dir_, reflect_ray_.dir_));
    EXPECT_EQ(1u, reflect_payload_);
  }

  void checkShapeFunc(const ShapeFunc& func) {
    const geometry::test::IntersectionTestData& data = std::get<0>(GetParam());

    for (std::size_t i = 0u; i < childs_indexes_.size(); ++i) {
      const geometry::Shape<float>& res =
          func(childs_indexes_.at(i), data.shape_);

      EXPECT_TRUE(geometry::isVectorEqual(data.childs_[i].min_, res.min_));
      EXPECT_TRUE(geometry::isVectorEqual(data.childs_[i].max_, res.max_));
    }
  }

 protected:
  TreeMock tree_mock_;
  ChildsIndexesData childs_indexes_;

  float dist_;
  geometry::Ray<float> reflect_ray_;
  PayloadTestData reflect_payload_;
};

MATCHER_P(RayMatcher, ray, "Rays comparator") {
  return geometry::isVectorEqual(arg.pos_, ray.pos_) &&
         geometry::isVectorEqual(arg.dir_, ray.dir_);
}

TEST_P(TracerTest, BuildTree_Success) {
  using namespace ::testing;

  const geometry::Ray<float> ray = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  InsertRemoveFunc insert_func;
  ShapeFunc shape_func;
  EXPECT_CALL(this->tree_mock_, insertNodes(_, _, RayMatcher(ray), 1.0f))
      .WillOnce(DoAll(SaveArg<0>(&insert_func), SaveArg<1>(&shape_func)));

  Tracer<float>::buildTree(this->tree_mock_, ray, 1.0f);

  this->checkInsertRemoveFunc(insert_func);
  this->checkShapeFunc(shape_func);
}

TEST_P(TracerTest, BurnTree_Success) {
  using namespace ::testing;

  const geometry::Ray<float> ray = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  InsertRemoveFunc remove_func;
  ShapeFunc shape_func;
  EXPECT_CALL(this->tree_mock_, removeNodes(_, _, RayMatcher(ray), 1.0f))
      .WillOnce(DoAll(SaveArg<0>(&remove_func), SaveArg<1>(&shape_func)));

  Tracer<float>::burnTree(this->tree_mock_, ray, 1.0f);

  this->checkInsertRemoveFunc(remove_func);
  this->checkShapeFunc(shape_func);
}

TEST_P(TracerTest, CastTree_Success) {
  using namespace ::testing;

  const geometry::Ray<float> ray = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  ProcessFunc process_func;
  ShapeFunc shape_func;
  EXPECT_CALL(this->tree_mock_, processNodes(_, _, RayMatcher(ray), 1.0f))
      .WillOnce(DoAll(SaveArg<0>(&process_func), SaveArg<1>(&shape_func)));

  Tracer<float>::castTree(this->tree_mock_, ray, 1.0f, this->dist_,
                          this->reflect_ray_, this->reflect_payload_);
  EXPECT_TRUE(geometry::isEqual(geometry::getMin<float>(), this->dist_));

  this->checkProcessFunc(process_func);
  this->checkShapeFunc(shape_func);
}

INSTANTIATE_TEST_CASE_P(
    TracerDataInstantiation, TracerTest,
    ::testing::Combine(
        ::testing::ValuesIn(geometry::test::kIntersectionTestData),
        ::testing::Values(0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u)));

}  // namespace test
}  // namespace tracer
}  // namespace october
