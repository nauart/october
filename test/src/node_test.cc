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

#include "node.h"

namespace october {
namespace node {
namespace test {

using ChildsIndexesData = std::vector<std::size_t>;

// clang-format off
static const std::vector<ChildsIndexesData> childs_indexes_data = {
  {},
  {0u},
  {0u, 1u},
  {0u, 1u, 1u},
  {1u, 0u, 0u, 1u},
  {1u, 1u, 0u, 0u, 0u},
  {0u, 1u, 1u, 0u, 1u, 1u},
  {1u, 0u, 1u, 0u, 1u, 0u, 1u},
  {1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u},
};

static const std::vector<ChildsIndexesData> childs_to_process_indexes_data = {
  {},
  {0u},
  {0u, 1u},
  {0u, 1u, 2u},
  {0u, 1u, 2u, 3u},
  {0u, 1u, 2u, 3u, 4u},
  {0u, 1u, 2u, 3u, 4u, 5u},
  {0u, 1u, 2u, 3u, 4u, 5u, 6u},
  {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u}
};
// clang-format on

class NodeMock;

using PayloadTestData = std::size_t;
using ChildsTestData = std::array<std::shared_ptr<NodeMock>, 8u>;
using ParameterTestData = std::size_t;

using ProcessPayloadFunc = std::function<ChildsIndexesData(
    const PayloadTestData&, ParameterTestData, const ParameterTestData&)>;
using ProcessChildsFunc = std::function<ChildsIndexesData(
    const ChildsTestData&, ParameterTestData, const ParameterTestData&)>;
using InterpolateFunc =
    std::function<std::tuple<ParameterTestData, ParameterTestData>(
        std::size_t, ParameterTestData, const ParameterTestData&)>;

class FunctionMock {
 public:
  MOCK_CONST_METHOD3(processPayloadFunc,
                     ChildsIndexesData(const PayloadTestData&,
                                       ParameterTestData,
                                       const ParameterTestData&));
  MOCK_CONST_METHOD3(processChildsFunc,
                     ChildsIndexesData(const ChildsTestData&, ParameterTestData,
                                       const ParameterTestData&));
  MOCK_CONST_METHOD3(interpolateFunc,
                     std::tuple<ParameterTestData, const ParameterTestData&>(
                         std::size_t, ParameterTestData,
                         const ParameterTestData&));
};

class NodeMock {
 public:
  MOCK_CONST_METHOD4(processPayload,
                     void(const ProcessPayloadFunc&, const InterpolateFunc&,
                          ParameterTestData, const ParameterTestData&));
  MOCK_CONST_METHOD4(processChilds,
                     void(const ProcessChildsFunc&, const InterpolateFunc&,
                          ParameterTestData, const ParameterTestData&));
};

class NodeTest : public ::testing::TestWithParam<
                     std::tuple<ChildsIndexesData, ChildsIndexesData> > {
 protected:
  void SetUp() {
    const ChildsIndexesData& childs_indexes = std::get<0>(GetParam());
    for (std::size_t i = 0u; i < childs_indexes.size(); ++i) {
      if (0u != childs_indexes.at(i)) {
        childs_.at(i) = std::make_shared<NodeMock>();
      }
    }
  }

 protected:
  FunctionMock function_mock_;

  const ProcessPayloadFunc process_payload_func_ = std::bind(
      &FunctionMock::processPayloadFunc, &function_mock_, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3);
  const ProcessChildsFunc process_childs_func_ = std::bind(
      &FunctionMock::processChildsFunc, &function_mock_, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3);
  const InterpolateFunc interpolate_func_ = std::bind(
      &FunctionMock::interpolateFunc, &function_mock_, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3);

  ChildsTestData childs_;
};

TEST_P(NodeTest, ProcessPayload_PayloadFuncInvokedOnRootNode) {
  using namespace ::testing;

  EXPECT_CALL(function_mock_, processPayloadFunc(0u, 1u, 2u));
  EXPECT_CALL(function_mock_, processChildsFunc(_, _, _)).Times(0);
  EXPECT_CALL(function_mock_, interpolateFunc(_, _, _)).Times(0);

  Node<PayloadTestData, ChildsTestData> node(PayloadTestData(0u), childs_);
  node.processPayload(process_payload_func_, interpolate_func_, 1u, 2u);
}

TEST_P(NodeTest, ProcessChilds_ChildsFuncInvokedOnRootNode) {
  using namespace ::testing;

  EXPECT_CALL(function_mock_, processChildsFunc(ContainerEq(childs_), 1u, 2u));
  EXPECT_CALL(function_mock_, processPayloadFunc(_, _, _)).Times(0);
  EXPECT_CALL(function_mock_, interpolateFunc(_, _, _)).Times(0);

  Node<PayloadTestData, ChildsTestData> node(PayloadTestData(), childs_);
  node.processChilds(process_childs_func_, interpolate_func_, 1u, 2u);
}

TEST_P(NodeTest, ProcessPayload_PayloadFuncInvokedOnChildNodes) {
  using namespace ::testing;

  const ChildsIndexesData& childs_to_process_indexes = std::get<1>(GetParam());
  for (std::size_t i = 0u; i < childs_.size(); ++i) {
    auto& child = childs_.at(i);
    if (child) {
      if (std::any_of(childs_to_process_indexes.begin(),
                      childs_to_process_indexes.end(),
                      [&i](const auto& index) { return index == i; })) {
        EXPECT_CALL(*child,
                    processPayload(Ref(process_payload_func_),
                                   Ref(interpolate_func_), 2u, 1u));
      } else {
        EXPECT_CALL(*child, processPayload(_, _, _, _)).Times(0);
      }
      EXPECT_CALL(*child, processChilds(_, _, _, _)).Times(0);
    }
  }

  EXPECT_CALL(function_mock_, processPayloadFunc(0u, 1u, 2u))
      .WillOnce(Return(childs_to_process_indexes));
  EXPECT_CALL(function_mock_, interpolateFunc(_, 1u, 2u))
      .WillRepeatedly(Return(
          std::make_tuple(ParameterTestData(2u), ParameterTestData(1u))));

  Node<PayloadTestData, ChildsTestData> node(PayloadTestData(0u), childs_);
  node.processPayload(process_payload_func_, interpolate_func_, 1u, 2u);
}

TEST_P(NodeTest, ProcessChilds_ChildsFuncInvokedOnChildNodes) {
  using namespace ::testing;

  const ChildsIndexesData& childs_to_process_indexes = std::get<1>(GetParam());
  for (std::size_t i = 0u; i < childs_.size(); ++i) {
    auto& child = childs_.at(i);
    if (child) {
      if (std::any_of(childs_to_process_indexes.begin(),
                      childs_to_process_indexes.end(),
                      [&i](const auto& index) { return index == i; })) {
        EXPECT_CALL(*child,
                    processChilds(Ref(process_childs_func_),
                                  Ref(interpolate_func_), 2u, 1u));
      } else {
        EXPECT_CALL(*child, processChilds(_, _, _, _)).Times(0);
      }
      EXPECT_CALL(*child, processPayload(_, _, _, _)).Times(0);
    }
  }

  EXPECT_CALL(function_mock_, processChildsFunc(ContainerEq(childs_), 1u, 2u))
      .WillOnce(Return(childs_to_process_indexes));
  EXPECT_CALL(function_mock_, interpolateFunc(_, 1u, 2u))
      .WillRepeatedly(Return(
          std::make_tuple(ParameterTestData(2u), ParameterTestData(1u))));

  Node<PayloadTestData, ChildsTestData> node(PayloadTestData(), childs_);
  node.processChilds(process_childs_func_, interpolate_func_, 1u, 2u);
}

INSTANTIATE_TEST_CASE_P(
    ChildsIndexesInstantiation, NodeTest,
    ::testing::Combine(::testing::ValuesIn(childs_indexes_data),
                       ::testing::ValuesIn(childs_to_process_indexes_data)));

}  // namespace test
}  // namespace node
}  // namespace october
