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

#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>

#include "geometry.h"

namespace october {
namespace tracer {

/**
 * @brief
 */
template <typename T>
class Tracer {
 public:
  template <typename Tree>
  void buildTree(Tree& tree, const geometry::Ray<T>& ray, const T& power) {
    tree.insertNodes(
        [](const geometry::Shape<T>& , const geometry::Ray<T>& ,
           const T& ) { return std::vector<std::size_t>(); },
        shapeFunc, ray, power);
  }

  template <typename Tree>
  void burnTree(Tree& tree, const geometry::Ray<T>& ray, const T& power) {
    tree.removeNodes(
        [](const geometry::Shape<T>& , const geometry::Ray<T>& ,
           const T& ) { return std::vector<std::size_t>(); },
        shapeFunc, ray, power);
  }

  template <typename Tree, typename Payload>
  auto castTree(Tree& tree, const geometry::Ray<T>& ray, const T& power,
             geometry::Ray<T>& reflect_ray, Payload& reflect_payload) {
    auto res = geometry::getMin<T>();
    tree.processNodes(
        [&](const Payload& payload, const geometry::Shape<T>& shape,
            const geometry::Ray<T>& ray, const T& power) {
          res = geometry::rayShapeIntersection(ray, shape, reflect_ray);
          if (geometry::isPositive(res)) {
            reflect_payload = payload;
            // if width > power
            // predict
            return std::vector<std::size_t>();
          }
          return std::vector<std::size_t>();
        },
        shapeFunc, ray, power);
    return res;
  }

 private:
  /**
   * @brief shapeFunc
   * (axis-aligned box)
   * @param child_index
   * @param shape
   * @return
   */
  static geometry::Shape<T> shapeFunc(const std::size_t& child_index,
                                      const geometry::Shape<T>& shape) {
    const std::uint8_t x = child_index % 2u;
    const std::uint8_t y = child_index % 4u / 2u;
    const std::uint8_t z = child_index % 8u / 4u;

    const geometry::Vec3<T> half({(shape.max_.x_ - shape.min_.x_) / 2u,
                                  (shape.max_.y_ - shape.min_.y_) / 2u,
                                  (shape.max_.z_ - shape.min_.z_) / 2u});

    return {
        {shape.min_.x_ + x * half.x_, shape.min_.y_ + y * half.y_, shape.min_.z_ + z * half.z_},
        {shape.max_.x_ - (x ^ 1u) * half.x_, shape.max_.y_ - (y ^ 1u) * half.y_, shape.max_.z_ - (z ^ 1u) * half.z_}
    };
  }

  /*void predict() {
    if (intersect.x < shape_half.x) {
      // 0, 2, 4, 6
    } else {
      // 1, 3, 5, 7
    }

    if (intersect.y < shape_half.y) {
      // 0, 1, 4, 5
    } else {
      // 2, 3, 6, 7
    }

    if (intersect.z < shape_half.z) {
      // 0, 1, 2, 3
    } else {
      // 4, 5, 6, 7
    }

    // intersect
  }*/
};

}  // namespace tracer
}  // namespace october
