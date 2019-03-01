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

#include <algorithm>
#include <cmath>
#include <limits>

namespace october {
namespace geometry {

/**
 * @brief Represents vector in 3D space
 */
template <typename T>
struct Vec3 {
  T x_;
  T y_;
  T z_;
};

/**
 * @brief Represents ray in 3D space
 */
template <typename T>
struct Ray {
  Vec3<T> pos_;
  Vec3<T> dir_;
};

/**
 * @brief Represents shape's bound in 3D space
 */
template <typename T>
struct Shape {
  Vec3<T> min_;
  Vec3<T> max_;
};

/**
 * @brief Returns minimal possible value for certain type
 * @return Minimal value
 */
template <typename T>
T getMin() {
  return std::numeric_limits<T>::lowest();
}

/**
 * @brief Returns maximal possible value for certain type
 * @return Maximal value
 */
template <typename T>
T getMax() {
  return std::numeric_limits<T>::max();
}

/**
 * @brief Checks if value is greater than zero
 * @param value Value to be checked
 * @return True if given value is positive, false otherwise
 */
template <typename T>
bool isPositive(const T& value) {
  return value > std::numeric_limits<T>::epsilon();
}

/**
 * @brief Checks if value is less than zero
 * @param value Value to be checked
 * @return True if given value is negative, false otherwise
 */
template <typename T>
bool isNegative(const T& value) {
  return value < -std::numeric_limits<T>::epsilon();
}

/**
 * @brief Checks if value is equal to zero
 * @param value Value to be checked
 * @return True if given value is equal to zero, false otherwise
 */
template <typename T>
bool isZero(const T& value) {
  return !isPositive(value) && !isNegative(value);
}

/**
 * @brief Checks if two values are equal
 * @param a First value
 * @param b Second value
 * @return True if given values are equal, false otherwise
 */
template <typename T>
bool isEqual(const T& a, const T& b) {
  return std::abs(a - b) <=
         std::numeric_limits<T>::epsilon() * std::max(std::abs(a), std::abs(b));
}

/**
 * @brief Checks if two vectors are equal
 * @param a First vector
 * @param b Second vector
 * @return True if given vectors are equal, false otherwise
 */
template <typename T>
bool isVectorEqual(const Vec3<T>& a, const Vec3<T>& b) {
  return isEqual(a.x_, b.x_) && isEqual(a.y_, b.y_) && isEqual(a.z_, b.z_);
}

/**
 * @brief Checks if value belongs to specified range
 * @param value Value to be checked
 * @param low Range low bound
 * @param high Range high bound
 * @return True if value belongs to the range, false otherwise
 */
template <typename T>
bool inRange(const T& value, const T& low, const T& high) {
  return value >= low && value <= high;
}

/**
 * @brief Checks if 3D point belongs to specified volume (shape)
 * @param point Point to be checked
 * @param volume Specified volume
 * @return True if point belongs to the volume, false otherwise
 */
template <typename T>
bool inVolume(const Vec3<T>& point, const Shape<T>& volume) {
  return inRange(point.x_, volume.min_.x_, volume.max_.x_) &&
         inRange(point.y_, volume.min_.y_, volume.max_.y_) &&
         inRange(point.z_, volume.min_.z_, volume.max_.z_);
}

/**
 * @brief Scales vector by specified scalar value
 * @param vector Vector to be scaled
 * @param scalar Scalar value
 * @return Result vector
 */
template <typename T>
Vec3<T> scaleVector(const Vec3<T>& vector, const T& scalar) {
  return {vector.x_ * scalar, vector.y_ * scalar, vector.z_ * scalar};
}

/**
 * @brief Adds two vectors
 * @param a First vector
 * @param b Second vector
 * @return Result vector
 */
template <typename T>
Vec3<T> addVectors(const Vec3<T>& a, const Vec3<T>& b) {
  return {a.x_ + b.x_, a.y_ + b.y_, a.z_ + b.z_};
}

/**
 * @brief Calculates dot product of two vectors
 * @param a First vector
 * @param b Second vector
 * @return Result scalar value
 */
template <typename T>
T dotProduct(const Vec3<T>& a, const Vec3<T>& b) {
  return a.x_ * b.x_ + a.y_ * b.y_ + a.z_ * b.z_;
}

/**
 * @brief Calculates cross product of two vectors
 * @param a First vector
 * @param b Second vector
 * @return Result vector
 */
template <typename T>
Vec3<T> crossProduct(const Vec3<T>& a, const Vec3<T>& b) {
  return {a.y_ * b.z_ - a.z_ * b.y_, a.z_ * b.x_ - a.x_ * b.z_,
          a.x_ * b.y_ - a.y_ * b.x_};
}

/**
 * @brief Calculates vector length
 * @param vector Input vector
 * @return Length of given vector
 */
template <typename T>
auto vectorLength(const Vec3<T>& vector) {
  return std::sqrt(vector.x_ * vector.x_ + vector.y_ * vector.y_ +
                   vector.z_ * vector.z_);
}

/**
 * @brief Normalizes vector (divides each coord by vector length)
 * @param vector Input vector
 * @return Normalized vector
 */
template <typename T>
Vec3<T> normalizeVector(const Vec3<T>& vector) {
  const T& length = vectorLength(vector);
  if (!isZero(length)) {
    return {vector.x_ / length, vector.y_ / length, vector.z_ / length};
  }
  return vector;
}

/**
 * @brief Checks for intersection between ray and opposite faces
 * of axis-aligned box in 3D
 *
 * @param ray Ray to be checked for intersection
 * @param shape Shape to be checked for intersection (axis-aligned box)
 * @param alpha Cos of the angle between ray and face
 * @param dis_a Distance from start of the ray to first of two faces
 * @param dis_b Distance from start of the ray to second of two faces
 * @param point Output parameter for intersection point
 * (contains intersection point if intersection exists)
 *
 * @return Distance from start of the ray to intersection point if it exists,
 * negative value otherwise
 */
template <typename T>
auto rayFaceIntersection(const Ray<T>& ray, const Shape<T>& shape,
                         const T& alpha, const T& dist_a, const T& dist_b,
                         Vec3<T>& point) {
  if (!isZero(alpha)) {
    const auto dist = std::min(dist_a / alpha, dist_b / alpha);
    if (isPositive(dist)) {
      point = addVectors(ray.pos_, scaleVector(ray.dir_, dist));
      if (inVolume(point, shape)) {
        return dist;
      }
    }
  }
  return getMin<T>();
}

/**
 * @brief Checks for intersection between ray and axis-aligned box in 3D
 * @param ray Ray to be checked for intersection
 * @param shape Shape to be checked for intersection (axis-aligned box)
 * @param reflect_ray Output parameter for reflect ray
 * (contains reflect ray if intersection exists)
 *
 * @return Distance from start of the ray to intersection point if it exists,
 * negative value otherwise
 */
template <typename T>
auto rayShapeIntersection(const Ray<T>& ray, const Shape<T>& shape,
                          Ray<T>& reflect_ray) {
  Vec3<T> point_x, point_y, point_z;
  const auto dist_x =
      rayFaceIntersection(ray, shape, ray.dir_.x_, shape.min_.x_ - ray.pos_.x_,
                          shape.max_.x_ - ray.pos_.x_, point_x);
  const auto dist_y =
      rayFaceIntersection(ray, shape, ray.dir_.y_, shape.min_.y_ - ray.pos_.y_,
                          shape.max_.y_ - ray.pos_.y_, point_y);
  const auto dist_z =
      rayFaceIntersection(ray, shape, ray.dir_.z_, shape.min_.z_ - ray.pos_.z_,
                          shape.max_.z_ - ray.pos_.z_, point_z);

  auto res = dist_x;
  reflect_ray.pos_ = point_x;
  reflect_ray.dir_ = {-ray.dir_.x_, ray.dir_.y_, ray.dir_.z_};

  if (isPositive(dist_y) && (isNegative(res) || dist_y < res)) {
    res = dist_y;
    reflect_ray.pos_ = point_y;
    reflect_ray.dir_ = {ray.dir_.x_, -ray.dir_.y_, ray.dir_.z_};
  }
  if (isPositive(dist_z) && (isNegative(res) || dist_z < res)) {
    res = dist_z;
    reflect_ray.pos_ = point_z;
    reflect_ray.dir_ = {ray.dir_.x_, ray.dir_.y_, -ray.dir_.z_};
  }

  return res;
}

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
template <typename Shape>
static Shape childShape(const std::size_t& child_index, const Shape& shape) {
  const std::uint8_t x = child_index % 2u;
  const std::uint8_t y = child_index % 4u / 2u;
  const std::uint8_t z = child_index % 8u / 4u;

  const geometry::Vec3<T>& half = geometry::shapeHalf(shape);

  return {
      {shape.min_.x_ + x * half.x_, shape.min_.y_ + y * half.y_, shape.min_.z_ + z * half.z_},
      {shape.max_.x_ - (x ^ 1u) * half.x_, shape.max_.y_ - (y ^ 1u) * half.y_, shape.max_.z_ - (z ^ 1u) * half.z_}};
}

}  // namespace geometry
}  // namespace october
