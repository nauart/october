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

#include <cstddef>
#include <vector>
#include <utility>
#include <algorithm>

#include "gtest/gtest.h"

#include "node.h"

namespace october {
namespace node {
namespace test {

using ChildNodesTestData = std::vector<std::size_t>;

const std::vector<ChildNodesTestData> fillChildNodesData(
    const std::size_t range) {
  std::vector<ChildNodesTestData> result;
  for (std::size_t i = 0u; i < range; ++i) {
    for (std::size_t j = 0u; j < range; ++j) {
      for (std::size_t k = 0u; k < range; ++k) {
        for (std::size_t l = 0u; l < range; ++l) {
          result.push_back({i, j, k, l});
        }
      }
    }
  }
  return result;
}

static const std::vector<ChildNodesTestData> child_nodes_data =
    fillChildNodesData(2u);
static const std::vector<ChildNodesTestData> child_nodes_to_process_data =
    fillChildNodesData(4u);

template <std::size_t N>
struct TypeValue {
  static const std::size_t value_ = N;
};

struct PayloadTestData {
  PayloadTestData() : value_(0u) {}
  explicit PayloadTestData(const std::size_t value) : value_(value) {}

  std::size_t value_;
};

ChildNodesTestData writeFunc(PayloadTestData& origin,
                             const PayloadTestData& input,
                             const ChildNodesTestData& result) {
  origin = input;
  return result;
}

ChildNodesTestData readFunc(const PayloadTestData& origin,
                            PayloadTestData& output,
                            const ChildNodesTestData& result) {
  output = origin;
  return result;
}

template <typename T>
class NodeTest : public ::testing::Test {
 protected:
  using NodeType = Node<PayloadTestData, T::value_>;
  using ChildNodesContainerType = typename NodeType::ChildNodesContainerType;

  void setNode(const ChildNodesTestData& child_nodes) {
    ChildNodesContainerType childs;
    for (std::size_t i = 0; i < child_nodes.size(); ++i) {
      if (i < childs.size() && 0u != child_nodes.at(i)) {
        childs[i].reset(
            new NodeType(PayloadTestData(), ChildNodesContainerType()));
      }
    }
    node_.reset(new NodeType(PayloadTestData(), std::move(childs)));
  }

  template <typename F, typename... Args>
  void processNode(const F& function, Args&&... args) {
    node_->process(function, std::forward<Args>(args)...);
  }

 private:
  NodePointer<NodeType> node_;
};

typedef ::testing::Types<TypeValue<0u>, TypeValue<1u>, TypeValue<2u>,
                         TypeValue<3u>, TypeValue<4u> > TestTypes;

TYPED_TEST_CASE(NodeTest, TestTypes);

TYPED_TEST(NodeTest, ProcessRootNode_Success) {
  const std::size_t payload_value = 8u;
  PayloadTestData payload;

  for (const auto& child_nodes : child_nodes_data) {
    this->setNode(child_nodes);
    this->processNode(readFunc, payload, ChildNodesTestData());
    EXPECT_NE(payload_value, payload.value_);

    this->processNode(writeFunc, PayloadTestData(payload_value),
                      ChildNodesTestData());
    this->processNode(readFunc, payload, ChildNodesTestData());
    EXPECT_EQ(payload_value, payload.value_);
  }
}

TYPED_TEST(NodeTest, ProcessChildNodes_Success) {
  const std::size_t payload_value = 8u;
  PayloadTestData payload;

  for (const auto& child_nodes : child_nodes_data) {
    for (const auto& to_process : child_nodes_to_process_data) {
      this->setNode(child_nodes);
      for (std::size_t i = 0; i < child_nodes.size(); ++i) {
        if (i < TypeParam::value_ && 0u != child_nodes.at(i)) {
          this->processNode(readFunc, payload, ChildNodesTestData({i}));
          EXPECT_NE(payload_value, payload.value_);
        }
      }

      this->processNode(writeFunc, PayloadTestData(payload_value), to_process);
      for (std::size_t i = 0; i < child_nodes.size(); ++i) {
        if (i < TypeParam::value_ && 0u != child_nodes.at(i)) {
          this->processNode(readFunc, payload, ChildNodesTestData({i}));
          if (to_process.end() !=
              std::find(to_process.begin(), to_process.end(), i)) {
            EXPECT_EQ(payload_value, payload.value_);
          } else {
            EXPECT_NE(payload_value, payload.value_);
          }
        }
      }
    }
  }
}

}  // namespace test
}  // namespace node
}  // namespace october
