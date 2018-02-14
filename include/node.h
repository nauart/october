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

#pragma once

#include <utility>
#include <functional>
#include <algorithm>
#include <tuple>

namespace october {
namespace node {

/**
 * @brief Represents node with custom payload and childs container
 */
template <typename Payload, typename ChildsContainer>
class Node {
 public:
  /**
   * @brief Constructor
   * @param payload Payload to be stored in the node
   * @param childs STL container of pointers to child nodes
   */
  template <typename U, typename V>
  explicit Node(U&& payload, V&& childs)
      : payload_(std::forward<U>(payload)), childs_(std::forward<V>(childs)) {}

  /**
   * @brief Processes node's payload recursively with given user function.
   * Uses DFS pre-order algorithm with selective child nodes processing
   * (user function returns scope of child nodes to be processed on next step)
   *
   * @param process_func Function to be applied to nodes recursively.
   * Function is expected to have following signature:
   * "Container function(Payload&, ...)"
   * where "Container" type is STL container of child nodes indexes
   *
   * @param interpolate_func Function to be applied to arguments list
   * before each child processing for modifying arguments according to
   * child's index (arguments interpolation).
   * Function is expected to have following signature:
   * "std::tuple<Args...> function(Args...)"
   * where returned tuple will be unpacked and passed to child's
   * processPayload()
   *
   * @param args Various arguments list to be passed to user function
   */
  template <typename ProcessFunc, typename InterpolateFunc, typename... Args>
  void processPayload(const ProcessFunc& process_func,
                      const InterpolateFunc& interpolate_func, Args&&... args) {
    const auto& target_childs = std::invoke(process_func, payload_, args...);
    std::for_each(
        target_childs.begin(), target_childs.end(),
        [&](const auto& child_index) {
          if (child_index < childs_.size() && childs_.at(child_index)) {
            processChildNodePayload(
                childs_.at(child_index), process_func, interpolate_func,
                std::tuple(std::invoke(interpolate_func, child_index, args...)),
                std::make_index_sequence<sizeof...(Args)>{});
          }
        });
  }

  /**
   * @brief Processes node's childs recursively with given user function.
   * Uses DFS pre-order algorithm with selective child nodes processing
   * (user function returns scope of child nodes to be processed on next step)
   *
   * @param process_func Function to be applied to nodes recursively.
   * Function is expected to have following signature:
   * "Container function(ChildsContainer&, ...)"
   * where "Container" type is STL container of child nodes indexes
   *
   * @param interpolate_func Function to be applied to arguments list
   * before each child processing for modifying arguments according to
   * child's index (arguments interpolation).
   * Function is expected to have following signature:
   * "std::tuple<Args...> function(Args...)"
   * where returned tuple will be unpacked and passed to child's
   * processChilds()
   *
   * @param args Various arguments list to be passed to user function
   */
  template <typename ProcessFunc, typename InterpolateFunc, typename... Args>
  void processChilds(const ProcessFunc& process_func,
                     const InterpolateFunc& interpolate_func, Args&&... args) {
    const auto& target_childs = std::invoke(process_func, childs_, args...);
    std::for_each(
        target_childs.begin(), target_childs.end(),
        [&](const auto& child_index) {
          if (child_index < childs_.size() && childs_.at(child_index)) {
            processChildNodeChilds(
                childs_.at(child_index), process_func, interpolate_func,
                std::tuple(std::invoke(interpolate_func, child_index, args...)),
                std::make_index_sequence<sizeof...(Args)>{});
          }
        });
  }

 private:
  /**
   * @brief Invokes processPayload() method for certain child node
   * @param child_node Child node for which function call will be performed
   * @param process_func Process function to be passed to function call
   * @param interpolate_func Interpolate function to be passed to function call
   * @param tuple Tuple of arguments to be unpacked and passed to function call
   */
  template <typename ChildNode, typename ProcessFunc, typename InterpolateFunc,
            typename Tuple, auto... I>
  static void processChildNodePayload(const ChildNode& child_node,
                                      const ProcessFunc& process_func,
                                      const InterpolateFunc& interpolate_func,
                                      Tuple&& tuple,
                                      std::index_sequence<I...>) {
    child_node->processPayload(process_func, interpolate_func,
                               std::get<I>(std::forward<Tuple>(tuple))...);
  }

  /**
   * @brief Invokes processChilds() method for certain child node
   * @param child_node Child node for which function call will be performed
   * @param process_func Process function to be passed to function call
   * @param interpolate_func Interpolate function to be passed to function call
   * @param tuple Tuple of arguments to be unpacked and passed to function call
   */
  template <typename ChildNode, typename ProcessFunc, typename InterpolateFunc,
            typename Tuple, auto... I>
  static void processChildNodeChilds(const ChildNode& child_node,
                                     const ProcessFunc& process_func,
                                     const InterpolateFunc& interpolate_func,
                                     Tuple&& tuple, std::index_sequence<I...>) {
    child_node->processChilds(process_func, interpolate_func,
                              std::get<I>(std::forward<Tuple>(tuple))...);
  }

 private:
  Payload payload_;
  ChildsContainer childs_;
};

}  // namespace node
}  // namespace october
