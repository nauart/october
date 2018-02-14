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
#include <vector>
#include <functional>
#include <array>
#include <memory>
#include <algorithm>
#include <utility>

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

using PayloadTestData = float;
using ChildsTestData = std::array<std::unique_ptr<NodeMock>, 8u>;
using ParameterTestData = float;

using ProcessPayloadFunc = std::function<ChildsIndexesData(
    const PayloadTestData&, ParameterTestData, const ParameterTestData&)>;
using ProcessChildsFunc = std::function<ChildsIndexesData(
    const ChildsTestData&, ParameterTestData, const ParameterTestData&)>;
using InterpolateFunc =
    std::function<std::tuple<ParameterTestData, const ParameterTestData&>(
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
                     void(ProcessPayloadFunc, InterpolateFunc,
                          ParameterTestData, const ParameterTestData&));
  MOCK_CONST_METHOD4(processChilds,
                     void(ProcessChildsFunc, InterpolateFunc, ParameterTestData,
                          const ParameterTestData&));
};

class NodeTest : public ::testing::TestWithParam<
                     std::tuple<ChildsIndexesData, ChildsIndexesData> > {
 protected:
  void SetUp() {
    using namespace std::placeholders;

    process_payload_func_ = std::bind(&FunctionMock::processPayloadFunc,
                                      &function_mock_, _1, _2, _3);
    process_childs_func_ = std::bind(&FunctionMock::processChildsFunc,
                                     &function_mock_, _1, _2, _3);
    interpolate_func_ =
        std::bind(&FunctionMock::interpolateFunc, &function_mock_, _1, _2, _3);
  }

 protected:
  FunctionMock function_mock_;
  ProcessPayloadFunc process_payload_func_;
  ProcessChildsFunc process_childs_func_;
  InterpolateFunc interpolate_func_;
};

TEST_P(NodeTest, ProcessRootPayload_Success) {
  using namespace ::testing;

  Node<PayloadTestData, ChildsTestData> node((PayloadTestData(0.0f)),
                                             (ChildsTestData()));

  EXPECT_CALL(function_mock_, processPayloadFunc(0.0f, 1.0f, 2.0f));
  EXPECT_CALL(function_mock_, processChildsFunc(_, _, _)).Times(0);
  EXPECT_CALL(function_mock_, interpolateFunc(_, _, _)).Times(0);

  node.processPayload(process_payload_func_, interpolate_func_, 1.0f, 2.0f);
}

TEST_P(NodeTest, ProcessRootChilds_Success) {
  using namespace ::testing;

  Node<PayloadTestData, ChildsTestData> node((PayloadTestData()),
                                             (ChildsTestData()));

  EXPECT_CALL(function_mock_, processChildsFunc(_, 1.0f, 2.0f));
  EXPECT_CALL(function_mock_, processPayloadFunc(_, _, _)).Times(0);
  EXPECT_CALL(function_mock_, interpolateFunc(_, _, _)).Times(0);

  node.processChilds(process_childs_func_, interpolate_func_, 1.0f, 2.0f);
}

TEST_P(NodeTest, ProcessChildsPayload_Success) {
  using namespace ::testing;

  ChildsTestData childs;
  ChildsIndexesData childs_indexes = std::get<0>(GetParam());
  ChildsIndexesData childs_to_process_indexes = std::get<1>(GetParam());

  for (std::size_t i = 0; i < childs_indexes.size(); ++i) {
    if (0 != childs_indexes.at(i)) {
      childs.at(i) = std::make_unique<NodeMock>();
      if (childs_to_process_indexes.end() !=
          std::find(childs_to_process_indexes.begin(),
                    childs_to_process_indexes.end(), i)) {
        EXPECT_CALL(*childs.at(i).get(), processPayload(_, _, 2.0f, 1.0f));
      } else {
        EXPECT_CALL(*childs.at(i).get(), processPayload(_, _, _, _)).Times(0);
      }
      EXPECT_CALL(*childs.at(i).get(), processChilds(_, _, _, _)).Times(0);
    }
  }

  Node<PayloadTestData, ChildsTestData> node((PayloadTestData()),
                                             std::move(childs));

  EXPECT_CALL(function_mock_, processPayloadFunc(_, _, _))
      .WillOnce(Return(childs_to_process_indexes));
  EXPECT_CALL(function_mock_, interpolateFunc(_, 1.0f, 2.0f))
      .WillRepeatedly(Return(std::make_tuple(2.0f, 1.0f)));

  node.processPayload(process_payload_func_, interpolate_func_, 1.0f, 2.0f);
}

TEST_P(NodeTest, ProcessChildsChilds_Success) {
  using namespace ::testing;

  ChildsTestData childs;
  ChildsIndexesData childs_indexes = std::get<0>(GetParam());
  ChildsIndexesData childs_to_process_indexes = std::get<1>(GetParam());

  for (std::size_t i = 0; i < childs_indexes.size(); ++i) {
    if (0 != childs_indexes.at(i)) {
      childs.at(i) = std::make_unique<NodeMock>();
      if (childs_to_process_indexes.end() !=
          std::find(childs_to_process_indexes.begin(),
                    childs_to_process_indexes.end(), i)) {
        EXPECT_CALL(*childs.at(i).get(), processChilds(_, _, 2.0f, 1.0f));
      } else {
        EXPECT_CALL(*childs.at(i).get(), processChilds(_, _, _, _)).Times(0);
      }
      EXPECT_CALL(*childs.at(i).get(), processPayload(_, _, _, _)).Times(0);
    }
  }

  Node<PayloadTestData, ChildsTestData> node((PayloadTestData()),
                                             std::move(childs));

  EXPECT_CALL(function_mock_, processChildsFunc(_, _, _))
      .WillOnce(Return(childs_to_process_indexes));
  EXPECT_CALL(function_mock_, interpolateFunc(_, 1.0f, 2.0f))
      .WillRepeatedly(Return(std::make_tuple(2.0f, 1.0f)));

  node.processChilds(process_childs_func_, interpolate_func_, 1.0f, 2.0f);
}

INSTANTIATE_TEST_CASE_P(
    ChildsIndexesInstantiation, NodeTest,
    ::testing::Combine(::testing::ValuesIn(childs_indexes_data),
                       ::testing::ValuesIn(childs_to_process_indexes_data)));

}  // namespace test
}  // namespace node
}  // namespace october
