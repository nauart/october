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

#pragma once

#include <cstddef>
#include <array>
#include <memory>
#include <utility>
#include <algorithm>

namespace october {
namespace node {

/**
 * @brief Represents type of pointer to node
 */
template <typename NodeType>
using NodePointer = std::unique_ptr<NodeType>;

/**
 * @brief Represents tree node with custom payload and arbitrary childs count
 */
template <typename PayloadType, std::size_t N>
class Node {
 public:
  /**
   * @brief Type of container for pointers to child nodes
   */
  using ChildNodesContainerType = std::array<NodePointer<Node>, N>;

  /**
   * @brief Constructor
   * @param payload Payload to be stored in the node
   * @param childs STL container of pointers to child nodes (takes ownership)
   */
  template <typename U, typename V>
  Node(U&& payload, V&& childs)
      : payload_(std::forward<U>(payload)), childs_(std::forward<V>(childs)) {}

  /**
   * @brief Processes node recursively with given user function.
   * Uses DFS pre-order algorithm with selective child nodes processing
   * (user function returns scope of child nodes to be processed on next step)
   *
   * @param function Function to be applied to nodes recursively.
   * Function is expected to have following signature:
   * "Container function(PayloadType&, ...)"
   * where "Container" type is STL container of child nodes indexes
   *
   * @param args Various arguments list to be passed to user function
   */
  template <typename F, typename... Args>
  void process(const F& function, Args&&... args) {
    const auto& target_childs = function(payload_, std::forward<Args>(args)...);
    std::for_each(
        target_childs.begin(), target_childs.end(),
        [&](const auto& child_index) {
          if (child_index < childs_.size() && childs_.at(child_index)) {
            childs_.at(child_index)
                ->process(function, std::forward<Args>(args)...);
          }
        });
  }

 private:
  PayloadType payload_;
  ChildNodesContainerType childs_;
};

}  // namespace node
}  // namespace october
