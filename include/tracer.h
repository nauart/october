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
  template <typename Tree>
  static void buildTree(Tree& tree, const geometry::Ray<T>& ray,
                        const T& power) {
    tree.insertNodes(
        [&tree](const geometry::Shape<T>& shape, const geometry::Ray<T>& ray,
                const T& power) {
          geometry::Ray<T> reflect_ray;
          const auto dist =
              geometry::rayShapeIntersection(ray, shape, reflect_ray);
          if (geometry::isPositive(dist) &&
              power < shapeDiag(shape)) {  // isLess()
            return std::vector<std::size_t>(
                {predictChild(shape, reflect_ray.pos_, tree.nodesIndexes())});
          }
          return std::vector<std::size_t>();
        },
        shapeChild, ray, power);
  }

  /**
   *
   */
  template <typename Tree>
  static void burnTree(Tree& tree, const geometry::Ray<T>& ray,
                       const T& power) {
    tree.removeNodes(
        [&tree](const geometry::Shape<T>& shape, const geometry::Ray<T>& ray,
                const T& power) {
          geometry::Ray<T> reflect_ray;
          const auto dist =
              geometry::rayShapeIntersection(ray, shape, reflect_ray);
          if (geometry::isPositive(dist) &&
              power < shapeDiag(shape)) {  // isLess()
            return std::vector<std::size_t>(
                {predictChild(shape, reflect_ray.pos_, tree.nodesIndexes())});
          }
          return std::vector<std::size_t>();
        },
        shapeChild, ray, power);
  }

  /**
   *
   */
  template <typename Tree, typename Payload>
  static void castTree(Tree& tree, const geometry::Ray<T>& ray, const T& power,
                       T& dist, geometry::Ray<T>& reflect_ray,
                       Payload& reflect_payload) {
    dist = geometry::getMin<T>();
    tree.processNodes(
        [&tree, &reflect_ray, &reflect_payload, &dist](
            const Payload& payload, const geometry::Shape<T>& shape,
            const geometry::Ray<T>& ray, const T& power) {
          dist = geometry::rayShapeIntersection(ray, shape, reflect_ray);
          if (geometry::isPositive(dist)) {
            reflect_payload = payload;
            if (power < shapeDiag(shape)) {  // isLess()
              return std::vector<std::size_t>(tree.nodesIndexes().begin(),
                                              tree.nodesIndexes().end());
            }
          }
          return std::vector<std::size_t>();
        },
        shapeChild, ray, power);
  }

 private:
  /**
   * @brief shapeHalf
   * @param shape
   * @return
   */
  static geometry::Vec3<T> shapeHalf(const geometry::Shape<T>& shape) {
    return {(shape.max_.x_ - shape.min_.x_) / 2u,
            (shape.max_.y_ - shape.min_.y_) / 2u,
            (shape.max_.z_ - shape.min_.z_) / 2u};
  }

  /**
   * @brief shapeDiag
   * @param shape
   */
  static auto shapeDiag(const geometry::Shape<T>& shape) {
    return geometry::vectorLength(geometry::Vec3<T>(
        {shape.max_.x_ - shape.min_.x_, shape.max_.y_ - shape.min_.y_,
         shape.max_.z_ - shape.min_.z_}));
  }

  /**
   * @brief shapeFunc
   * (axis-aligned box)
   * @param child_index
   * @param shape
   * @return
   */
  static geometry::Shape<T> shapeChild(const std::size_t& child_index,
                                       const geometry::Shape<T>& shape) {
    const std::uint8_t x = child_index % 2u;
    const std::uint8_t y = child_index % 4u / 2u;
    const std::uint8_t z = child_index % 8u / 4u;

    const geometry::Vec3<T>& half = shapeHalf(shape);

    return {
        {shape.min_.x_ + x * half.x_, shape.min_.y_ + y * half.y_,
         shape.min_.z_ + z * half.z_},
        {shape.max_.x_ - (x ^ 1u) * half.x_, shape.max_.y_ - (y ^ 1u) * half.y_,
         shape.max_.z_ - (z ^ 1u) * half.z_}};
  }

  /**
   * @brief predictChild
   * @param shape
   * @param point
   * @return
   */
  template <typename NodesIndexes>
  static std::size_t predictChild(const geometry::Shape<T>& shape,
                                  const geometry::Vec3<T>& point,
                                  const NodesIndexes& indexes) {
    const geometry::Vec3<T>& half = shapeHalf(shape);

    const std::array<std::size_t, 4u>& target_x =
        point.x_ < half.x_ ? std::array<std::size_t, 4u>({0u, 2u, 4u, 6u})
                           : std::array<std::size_t, 4u>({1u, 3u, 5u, 7u});

    const std::array<std::size_t, 4u>& target_y =
        point.y_ < half.y_ ? std::array<std::size_t, 4u>({0u, 1u, 4u, 5u})
                           : std::array<std::size_t, 4u>({2u, 3u, 6u, 7u});

    const std::array<std::size_t, 4u>& target_z =
        point.z_ < half.z_ ? std::array<std::size_t, 4u>({0u, 1u, 2u, 3u})
                           : std::array<std::size_t, 4u>({4u, 5u, 6u, 7u});

    std::vector<std::size_t> res_a;
    std::set_intersection(indexes.begin(), indexes.end(), target_x.begin(),
                          target_x.end(), std::inserter(res_a, res_a.begin()));

    std::vector<std::size_t> res_b;
    std::set_intersection(target_y.begin(), target_y.end(), target_z.begin(),
                          target_z.end(), std::inserter(res_b, res_b.begin()));

    std::vector<std::size_t> res;
    std::set_intersection(res_a.begin(), res_a.end(), res_b.begin(),
                          res_b.end(), std::inserter(res, res.begin()));

    // isLess()!!!
    return res.size() > 0u ? res.at(0u) : 0u;
  }
};

}  // namespace tracer
}  // namespace october
