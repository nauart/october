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

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>

#include "node.h"

namespace october {
namespace tree {

/**
 * @brief Represents tree node with custom payload and arbitrary childs count
 * (uses itself recursively to represent childs)
 */
template <typename Payload, std::size_t N>
class TreeNode
    : public node::Node<
          Payload, std::array<std::unique_ptr<TreeNode<Payload, N> >, N> > {
 public:
  /**
   * @brief Constructor.
   * Forwards all parameters to node::Node constructor
   *
   * @param args Parameters to be forwarded
   */
  template <typename... Args>
  explicit TreeNode(Args&&... args)
      : node::Node<Payload,
                   std::array<std::unique_ptr<TreeNode<Payload, N> >, N> >(
            std::forward<Args>(args)...) {}
};

/**
 * @brief Represents basic tree with custom node payload, arbitrary subtrees
 * count and custom shape type (uses TreeNode to represent nodes by default)
 */
template <typename Payload, std::size_t N, typename Shape,
          typename Node = TreeNode<Payload, N> >
class Tree {
 public:
  /**
   * @brief Constructor
   * @param shape Base shape of the tree
   */
  template <typename T>
  explicit Tree(T&& shape)
      : root_(Payload(), std::array<std::unique_ptr<Node>, N>())
      , shape_(std::forward<T>(shape)) {}

  /**
   * @brief Inserts nodes to tree recursively.
   * Newly inserted nodes will be processed with the same user functions
   *
   * @param insert_func Function to be applied to tree nodes recursively for
   * child nodes insertion. Function is expected to have following signature:
   * "Container function(const Shape&, ...)"
   * where "Container" type is STL container of childs indexes to be inserted
   *
   * @param shape_func Function to be applied to each node's shape to calculate
   * child's shape. Function is expected to have following signature:
   * "Shape function(std::size_t, const Shape&)"
   * where first parameter is an index of target child
   *
   * @param args Various arguments list to be passed to user functions
   */
  template <typename InsertFunc, typename ShapeFunc, typename... Args>
  void insertNodes(const InsertFunc& insert_func, const ShapeFunc& shape_func,
                   Args&&... args) {
    root_.processChilds(
        [&insert_func](auto& childs, const Shape& shape, Args... args) {
          const auto& target_childs = std::invoke(insert_func, shape, args...);
          std::for_each(
              target_childs.begin(), target_childs.end(),
              [&childs](const auto& child_index) {
                if (child_index < childs.size() && !childs.at(child_index)) {
                  childs.at(child_index) = std::make_unique<Node>(
                      Payload(), std::array<std::unique_ptr<Node>, N>());
                }
              });
          return target_childs;
        },
        [&shape_func](const auto& child_index, const Shape& shape,
                      Args... args) {
          return std::tuple<Shape, Args&&...>(shape_func(child_index, shape),
                                              std::forward<Args>(args)...);
        },
        shape_, std::forward<Args>(args)...);
  }

  /**
   * @brief Removes nodes from tree recursively.
   * All remaining nodes will be processed with the same user functions
   *
   * @param remove_func Function to be applied to tree nodes recursively for
   * child nodes removing. Function is expected to have following signature:
   * "Container function(const Shape&, ...)"
   * where "Container" type is STL container of childs indexes to be removed
   *
   * @param shape_func Function to be applied to each node's shape to calculate
   * child's shape. Function is expected to have following signature:
   * "Shape function(std::size_t, const Shape&)"
   * where first parameter is an index of target child
   *
   * @param args Various arguments list to be passed to user functions
   */
  template <typename RemoveFunc, typename ShapeFunc, typename... Args>
  void removeNodes(const RemoveFunc& remove_func, const ShapeFunc& shape_func,
                   Args&&... args) {
    root_.processChilds(
        [&remove_func](auto& childs, const Shape& shape, Args... args) {
          auto target_childs = std::invoke(remove_func, shape, args...);
          std::for_each(
              target_childs.begin(), target_childs.end(),
              [&childs](const auto& child_index) {
                if (child_index < childs.size() && childs.at(child_index)) {
                  childs.at(child_index).reset();
                }
              });

          std::vector<std::size_t> res;
          std::sort(target_childs.begin(), target_childs.end());
          std::set_difference(kChildsIndexes.begin(), kChildsIndexes.end(),
                              target_childs.begin(), target_childs.end(),
                              std::inserter(res, res.begin()));
          return res;
        },
        [&shape_func](const auto& child_index, const Shape& shape,
                      Args... args) {
          return std::tuple<Shape, Args&&...>(shape_func(child_index, shape),
                                              std::forward<Args>(args)...);
        },
        shape_, std::forward<Args>(args)...);
  }

  /**
   * @brief Processes tree nodes recursively
   * @param process_func Function to be applied to tree nodes recursively.
   * Function is expected to have following signature:
   * "Container function(Payload&, const Shape&, ...)"
   * where "Container" type is STL container of childs indexes to be processed
   *
   * @param shape_func Function to be applied to each node's shape to calculate
   * child's shape. Function is expected to have following signature:
   * "Shape function(std::size_t, const Shape&)"
   * where first parameter is an index of target child
   *
   * @param args Various arguments list to be passed to user functions
   */
  template <typename ProcessFunc, typename ShapeFunc, typename... Args>
  void processNodes(const ProcessFunc& process_func,
                    const ShapeFunc& shape_func, Args&&... args) {
    root_.processPayload(process_func,
                         [&shape_func](const auto& child_index,
                                       const Shape& shape, Args... args) {
                           return std::tuple<Shape, Args&&...>(
                               shape_func(child_index, shape),
                               std::forward<Args>(args)...);
                         },
                         shape_, std::forward<Args>(args)...);
  }

 private:
  /**
   * @brief Fills array of all childs indexes based on index sequence
   * @param Index sequence from 0 to N
   * @return Array of childs indexes
   */
  template <auto... I>
  static constexpr std::array<std::size_t, N> fillChildsIndexes(
      std::index_sequence<I...>) {
    return {I...};
  }

 private:
  static constexpr std::array<std::size_t, N> kChildsIndexes =
      fillChildsIndexes(std::make_index_sequence<N>{});

  Node root_;
  Shape shape_;
};

}  // namespace tree
}  // namespace october
