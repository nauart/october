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

#include <array>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

#include "geometry.h"

namespace october {
namespace tracer {

/**
 * @brief
 */
template <typename T>
class Tracer {
 public:
  /**
   *
   */
  template <typename Tree, typename Shape>
  static void buildTree(Tree& tree, const geometry::Ray<T>& ray, const T& power) {
    tree.insertNodes(
        ,
        geometry::shapeChild, ray, power);
  }

  /**
   *
   */
  template <typename Tree, typename Shape>
  static void burnTree(Tree& tree, const geometry::Ray<T>& ray, const T& power) {
    tree.removeNodes(
        ,
        geometry::shapeChild, ray, power);
  }

  /**
   *
   */
  template <typename Tree, typename Shape, typename Payload>
  static void fillTree(Tree& tree, const geometry::Ray<T>& ray,
                       const T& power, const Payload& new_payload) {
    tree.processNodes(
        [&new_payload](Payload& payload, const Shape& shape, const geometry::Ray<T>& ray, const T& power) {
          geometry::Ray<T> reflect_ray;
          const T& dist = geometry::rayShapeIntersection(ray, shape, reflect_ray);
          if (geometry::isPositive(dist)) {
              if (geometry::isLess(power, geometry::shapeDiag(shape))) {
                  return std::vector<std::size_t>(Tree::childsIndexes().begin(),
                                                  Tree::childsIndexes().end());
              }
              payload = new_payload;
          }
          return std::vector<std::size_t>();
        },
        geometry::shapeChild, ray, power);
  }

  /**
   *
   */
  template <typename Tree, typename Shape, typename Payload>
  static void castTree(const Tree& tree, const geometry::Ray<T>& ray, const T& power,
                       T& dist, geometry::Ray<T>& reflect_ray,
                       Payload& reflect_payload) {
    dist = geometry::getMin<T>();
    tree.processNodes(
        [&dist, &reflect_ray, &reflect_payload](
            const Payload& payload, const Shape& shape,
            const geometry::Ray<T>& ray, const T& power) {
          dist = geometry::rayShapeIntersection(ray, shape, reflect_ray);
          if (geometry::isPositive(dist)) {
            reflect_payload = payload;
            if (geometry::isLess(power, geometry::shapeDiag(shape))) {
              return std::vector<std::size_t>(Tree::childsIndexes().begin(),
                                              Tree::childsIndexes().end());
            }
          }
          return std::vector<std::size_t>();
        },
        geometry::shapeChild, ray, power);
  }

 private:
  /**
   * @brief predictChild
   * @param shape
   * @param point
   * @return
   */
  template <typename Shape>
  static std::vector<std::size_t> childIndexRenameMe(const Shape& shape,
                                const geometry::Ray<T>& ray,
                                const T& power) {
      geometry::Ray<T> reflect_ray;
      T dist = geometry::rayShapeIntersection(ray, shape, reflect_ray);

      if (geometry::isPositive(dist) && geometry::isLess(power, geometry::shapeDiag(shape))) {
          std::size_t index = indexes.begin();
          std::for_each(Tree::childsIndexes().begin(), Tree::childsIndexes().end(),
                        [&shape, &ray](const std::size_t& child_index) {
              const T& child_dist = geometry::rayShapeIntersection(
                  ray, geometry::shapeChild(shape, child_index), reflect_ray);
              if (geometry::isPositive(child_dist) && geometry::isLess(child_dist, dist)) {
                  index = child_index;
              }
          });
          return {index};
      }
      return {};
  }
};

}  // namespace tracer
}  // namespace october
