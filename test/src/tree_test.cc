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

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tree.h"

namespace october {
namespace tree {
namespace test {

using ChildsIndexesData = std::vector<std::size_t>;

// clang-format off
static const std::vector<ChildsIndexesData> childs_indexes_data = {
  {},
  {0u},
  {0u, 1u},
  {0u, 1u, 2u},
  {0u, 1u, 2u, 3u},
  {0u, 1u, 2u, 3u, 4u},
  {0u, 1u, 2u, 3u, 4u, 5u},
  {0u, 1u, 2u, 3u, 4u, 5u, 6u},
  {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u},
};
// clang-format on

class TreeNodeMock;

using PayloadTestData = std::size_t;
using ChildsTestData = std::array<std::unique_ptr<TreeNodeMock>, 8u>;
using ShapeTestData = std::size_t;
using ParameterTestData = std::size_t;

using ProcessPayloadFunc = std::function<ChildsIndexesData(
    const PayloadTestData&, const ShapeTestData&, ParameterTestData,
    const ParameterTestData&)>;
using ProcessChildsFunc = std::function<ChildsIndexesData(
    ChildsTestData&, const ShapeTestData&, ParameterTestData,
    const ParameterTestData&)>;
using InterpolateFunc = std::function<
    std::tuple<ShapeTestData, ParameterTestData, ParameterTestData>(
        std::size_t, const ShapeTestData&, ParameterTestData,
        const ParameterTestData&)>;

using InsertRemoveFunc = std::function<ChildsIndexesData(
    const ShapeTestData&, ParameterTestData, const ParameterTestData&)>;
using ShapeFunc =
    std::function<ShapeTestData(std::size_t, const ShapeTestData&)>;

class FunctionMock {
 public:
  MOCK_CONST_METHOD3(insertRemoveFunc,
                     ChildsIndexesData(const ShapeTestData&, ParameterTestData,
                                       const ParameterTestData&));
  MOCK_CONST_METHOD2(shapeFunc,
                     ShapeTestData(std::size_t, const ShapeTestData&));
};

class NodeMock {
 public:
  MOCK_CONST_METHOD5(processPayload,
                     void(const ProcessPayloadFunc&, const InterpolateFunc&,
                          const ShapeTestData&, ParameterTestData,
                          const ParameterTestData&));
  MOCK_CONST_METHOD5(processChilds,
                     void(const ProcessChildsFunc&, const InterpolateFunc&,
                          const ShapeTestData&, ParameterTestData,
                          const ParameterTestData&));
};

class TreeNodeMock {
 public:
  static std::unique_ptr<NodeMock> node_mock_;

  template <typename... Args>
  explicit TreeNodeMock(Args&&...) {
    node_mock_ = std::make_unique<NodeMock>();
  }
  ~TreeNodeMock() {
    node_mock_.reset();
  }

  template <typename... Args>
  void processPayload(Args&&... args) {
    node_mock_->processPayload(std::forward<Args>(args)...);
  }
  template <typename... Args>
  void processChilds(Args&&... args) {
    node_mock_->processChilds(std::forward<Args>(args)...);
  }
};

std::unique_ptr<NodeMock> TreeNodeMock::node_mock_;

template <std::size_t N>
struct TypeValue {
  static const std::size_t value = N;
};

template <typename T>
class TreeTest : public ::testing::Test {
 protected:
  void checkInsertChildsFunc(const ProcessChildsFunc& func) {
    using namespace ::testing;

    for (std::size_t i = 0u; i < childs_indexes_data.size(); ++i) {
      const ChildsIndexesData& indexes = childs_indexes_data.at(i);

      ChildsTestData childs;
      EXPECT_CALL(function_mock_, insertRemoveFunc(0u, 1u, 2u))
          .WillOnce(Return(indexes));
      EXPECT_EQ(indexes, func(childs, 0u, 1u, 2u));

      for (std::size_t j = 0u; j < childs.size(); ++j) {
        if (indexes.end() != std::find(indexes.begin(), indexes.end(), j)) {
          EXPECT_TRUE(childs.at(j));
        } else {
          EXPECT_FALSE(childs.at(j));
        }
      }
    }
  }

  void checkRemoveChildsFunc(const ProcessChildsFunc& func) {
    using namespace ::testing;

    /*
     * expected indexes will look like
     *
     * N  indexes by iterations
     * 0  {}
     * 1  {0u}, {}, ...
     * 2  {0u, 1u}, {1u}, {}, ...
     * 3  {0u, 1u, 2u}, {1u, 2u}, {2u}, {}, ...
     * ---
     */
    ChildsIndexesData expected_indexes = childs_indexes_data.at(value_);
    for (std::size_t i = 0u; i < childs_indexes_data.size(); ++i) {
      const ChildsIndexesData& indexes = childs_indexes_data.at(i);

      ChildsTestData childs;
      std::generate(childs.begin(), childs.end(),
                    []() { return std::make_unique<TreeNodeMock>(); });
      EXPECT_CALL(function_mock_, insertRemoveFunc(0u, 1u, 2u))
          .WillOnce(Return(indexes));
      EXPECT_EQ(expected_indexes, func(childs, 0u, 1u, 2u));

      for (std::size_t j = 0u; j < childs.size(); ++j) {
        if (indexes.end() != std::find(indexes.begin(), indexes.end(), j)) {
          EXPECT_FALSE(childs.at(j));
        } else {
          EXPECT_TRUE(childs.at(j));
        }
      }

      if (expected_indexes.end() != expected_indexes.begin()) {
        expected_indexes.erase(expected_indexes.begin());
      }
    }
  }

  void checkInterpolateFunc(const InterpolateFunc& func) {
    using namespace ::testing;

    EXPECT_CALL(function_mock_, shapeFunc(0u, 0u))
        .WillOnce(Return(ShapeTestData(1u)));
    EXPECT_EQ(std::make_tuple(ShapeTestData(1u), ParameterTestData(1u),
                              ParameterTestData(2u)),
              func(0u, 0u, 1u, 2u));
  }

 protected:
  static const std::size_t value_ = T::value;

  FunctionMock function_mock_;
  const ProcessPayloadFunc process_func_;
  const InsertRemoveFunc insert_remove_func_ = std::bind(
      &FunctionMock::insertRemoveFunc, &function_mock_, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3);
  const ShapeFunc shape_func_ =
      std::bind(&FunctionMock::shapeFunc, &function_mock_,
                std::placeholders::_1, std::placeholders::_2);
};

typedef ::testing::Types<TypeValue<0u>, TypeValue<1u>, TypeValue<2u>,
                         TypeValue<3u>, TypeValue<4u>, TypeValue<5u>,
                         TypeValue<6u>, TypeValue<7u>, TypeValue<8u> >
    TestTypes;

TYPED_TEST_CASE(TreeTest, TestTypes);

TYPED_TEST(TreeTest, InsertNodes_Success) {
  using namespace ::testing;

  Tree<PayloadTestData, this->value_, ShapeTestData, TreeNodeMock> tree(
      ShapeTestData(0u));

  ProcessChildsFunc insert_childs_func;
  InterpolateFunc interpolate_func;
  EXPECT_CALL(*TreeNodeMock::node_mock_.get(), processChilds(_, _, 0u, 1u, 2u))
      .WillOnce(DoAll(SaveArg<0>(&insert_childs_func),
                      SaveArg<1>(&interpolate_func)));

  tree.insertNodes(this->insert_remove_func_, this->shape_func_, 1u, 2u);

  this->checkInsertChildsFunc(insert_childs_func);
  this->checkInterpolateFunc(interpolate_func);
}

TYPED_TEST(TreeTest, RemoveNodes_Success) {
  using namespace ::testing;

  Tree<PayloadTestData, this->value_, ShapeTestData, TreeNodeMock> tree(
      ShapeTestData(0u));

  ProcessChildsFunc remove_childs_func;
  InterpolateFunc interpolate_func;
  EXPECT_CALL(*TreeNodeMock::node_mock_.get(), processChilds(_, _, 0u, 1u, 2u))
      .WillOnce(DoAll(SaveArg<0>(&remove_childs_func),
                      SaveArg<1>(&interpolate_func)));

  tree.removeNodes(this->insert_remove_func_, this->shape_func_, 1u, 2u);

  this->checkRemoveChildsFunc(remove_childs_func);
  this->checkInterpolateFunc(interpolate_func);
}

TYPED_TEST(TreeTest, ProcessNodes_Success) {
  using namespace ::testing;

  Tree<PayloadTestData, this->value_, ShapeTestData, TreeNodeMock> tree(
      ShapeTestData(0u));

  InterpolateFunc interpolate_func;
  EXPECT_CALL(*TreeNodeMock::node_mock_.get(),
              processPayload(Ref(this->process_func_), _, 0u, 1u, 2u))
      .WillOnce(SaveArg<1>(&interpolate_func));

  tree.processNodes(this->process_func_, this->shape_func_,
                    ParameterTestData(1u), ParameterTestData(2u));

  this->checkInterpolateFunc(interpolate_func);
}

}  // namespace test
}  // namespace tree
}  // namespace october
