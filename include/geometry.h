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
#include <utility>

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
 * @brief Represents axis-aligned bounding box in 3D space
 */
template <typename T>
struct Box {
  Vec3<T> min_;
  Vec3<T> max_;
};

/**
 * @brief Returns minimal possible value for certain type
 * @return Minimal value
 */
template <typename T>
constexpr T getMin() {
  return std::numeric_limits<T>::lowest();
}

/**
 * @brief Returns maximal possible value for certain type
 * @return Maximal value
 */
template <typename T>
constexpr T getMax() {
  return std::numeric_limits<T>::max();
}

/**
 * @brief Checks if value is greater than zero
 * @param value Value to be checked
 * @return True if given value is positive, false otherwise
 */
template <typename T>
constexpr bool isPositive(const T& value) {
  return value > std::numeric_limits<T>::epsilon();
}

/**
 * @brief Checks if value is less than zero
 * @param value Value to be checked
 * @return True if given value is negative, false otherwise
 */
template <typename T>
constexpr bool isNegative(const T& value) {
  return value < -std::numeric_limits<T>::epsilon();
}

/**
 * @brief Checks if value is equal to zero
 * @param value Value to be checked
 * @return True if given value is equal to zero, false otherwise
 */
template <typename T>
constexpr bool isZero(const T& value) {
  return std::abs(value) <= std::numeric_limits<T>::epsilon();
}

/**
 * @brief Checks if first value is more than second
 * @param a First value
 * @param b Second value
 * @return True if first value is more than second, false otherwise
 */
template <typename T>
constexpr bool isMore(const T& a, const T& b) {
  return a - b > std::max(std::numeric_limits<T>::epsilon(),
                          std::numeric_limits<T>::epsilon() *
                              std::max(std::abs(a), std::abs(b)));
}

/**
 * @brief Checks if first value is less than second
 * @param a First value
 * @param b Second value
 * @return True if first value is less than second, false otherwise
 */
template <typename T>
constexpr bool isLess(const T& a, const T& b) {
  return a - b < -std::max(std::numeric_limits<T>::epsilon(),
                           std::numeric_limits<T>::epsilon() *
                               std::max(std::abs(a), std::abs(b)));
}

/**
 * @brief Checks if two values are equal
 * @param a First value
 * @param b Second value
 * @return True if given values are equal, false otherwise
 */
template <typename T>
constexpr bool isEqual(const T& a, const T& b) {
  return std::abs(a - b) <= std::max(std::numeric_limits<T>::epsilon(),
                                     std::numeric_limits<T>::epsilon() *
                                         std::max(std::abs(a), std::abs(b)));
}

/**
 * @brief Checks if value belongs to specified range
 * @param value Value to be checked
 * @param low Range low bound
 * @param high Range high bound
 * @return True if value belongs to the range, false otherwise
 */
template <typename T>
constexpr bool inRange(const T& value, const T& low, const T& high) {
  return !isLess(value, low) && !isMore(value, high);
}

/**
 * @brief Checks if 3D point belongs to specified bounding box (AABB)
 * @param point Point to be checked
 * @param box Target bounding box
 * @return True if point belongs to the volume, false otherwise
 */
template <typename T>
constexpr bool inBox(const Vec3<T>& point, const Box<T>& box) {
  return inRange(point.x_, box.min_.x_, box.max_.x_) &&
         inRange(point.y_, box.min_.y_, box.max_.y_) &&
         inRange(point.z_, box.min_.z_, box.max_.z_);
}

/**
 * @brief Checks if two vectors are equal
 * @param a First vector
 * @param b Second vector
 * @return True if given vectors are equal, false otherwise
 */
template <typename T>
constexpr bool isVectorEqual(const Vec3<T>& a, const Vec3<T>& b) {
  return isEqual(a.x_, b.x_) && isEqual(a.y_, b.y_) && isEqual(a.z_, b.z_);
}

/**
 * @brief Scales vector by specified scalar value
 * @param vector Vector to be scaled
 * @param scalar Scalar value
 * @return Result vector
 */
template <typename T>
constexpr Vec3<T> scaleVector(const Vec3<T>& vector, const T& scalar) {
  return {vector.x_ * scalar, vector.y_ * scalar, vector.z_ * scalar};
}

/**
 * @brief Adds two vectors
 * @param a First vector
 * @param b Second vector
 * @return Result vector
 */
template <typename T>
constexpr Vec3<T> addVectors(const Vec3<T>& a, const Vec3<T>& b) {
  return {a.x_ + b.x_, a.y_ + b.y_, a.z_ + b.z_};
}

/**
 * @brief Calculates dot product of two vectors
 * @param a First vector
 * @param b Second vector
 * @return Result scalar value
 */
template <typename T>
constexpr T dotProduct(const Vec3<T>& a, const Vec3<T>& b) {
  return a.x_ * b.x_ + a.y_ * b.y_ + a.z_ * b.z_;
}

/**
 * @brief Calculates cross product of two vectors
 * @param a First vector
 * @param b Second vector
 * @return Result vector
 */
template <typename T>
constexpr Vec3<T> crossProduct(const Vec3<T>& a, const Vec3<T>& b) {
  return {a.y_ * b.z_ - a.z_ * b.y_, a.z_ * b.x_ - a.x_ * b.z_,
          a.x_ * b.y_ - a.y_ * b.x_};
}

/**
 * @brief Calculates vector length
 * @param vector Input vector
 * @return Length of given vector
 */
template <typename T>
constexpr T vectorLength(const Vec3<T>& vector) {
  return std::sqrt(vector.x_ * vector.x_ + vector.y_ * vector.y_ +
                   vector.z_ * vector.z_);
}

/**
 * @brief Normalizes vector (divides each coord by vector length)
 * @param vector Input vector
 * @return Normalized vector
 */
template <typename T>
constexpr Vec3<T> normalizeVector(const Vec3<T>& vector) {
  const T& length = vectorLength(vector);
  return {vector.x_ / length, vector.y_ / length, vector.z_ / length};
}

/**
 * @brief shapeDiag
 * @param shape
 */
/*template <typename T>
constexpr T shapeDiag(const Box<T>& box) {
  return vectorLength({box.max_.x_ - box.min_.x_,
                       box.max_.y_ - box.min_.y_,
                       box.max_.z_ - box.min_.z_});
}*/

/**
 * @brief shapeFunc
 * (axis-aligned box)
 * @param child_index
 * @param shape
 * @return
 */
/*template <typename T, typename I>
constexpr Box<T> childShape(const Box<T>& box, const I& child_index) {
  const I x = child_index % 2u;
  const I y = child_index % 4u / 2u;
  const I z = child_index % 8u / 4u;

  const Vec3<T> half = {(box.max_.x_ - box.min_.x_) / 2u,
                         (box.max_.y_ - box.min_.y_) / 2u,
                         (box.max_.z_ - box.min_.z_) / 2u};

  return {
      {box.min_.x_ + x * half.x_, box.min_.y_ + y * half.y_, box.min_.z_ + z * half.z_},
      {box.max_.x_ - (x ^ 1u) * half.x_, box.max_.y_ - (y ^ 1u) * half.y_, box.max_.z_ - (z ^ 1u) * half.z_}};
}*/

/**
 * @brief Checks for intersection between ray and opposite faces
 * of axis-aligned box in 3D
 *
 * @param alpha Cos of the angle between ray and faces
 * @param a Distance from start of the ray to first of two faces
 * @param b Distance from start of the ray to second of two faces
 * @return Pair of distances from start of the ray to point of intersection
 * with each of faces in ascending order, pair of negative values
 * if intersection does not exist
 */
template <typename T>
std::pair<T, T> rayFacesIntersection(const T& alpha, const T& a, const T& b) {
  if (isPositive(alpha)) {
    return {a / alpha, b / alpha};
  } else if (isNegative(alpha)) {
    return {b / alpha, a / alpha};
  } else {
    return {getMin<T>(), getMin<T>()};
  }
}

/**
 * @brief Checks for intersection between ray and axis-aligned box in 3D
 * @param ray Ray to be checked for intersection
 * @param box Box to be checked for intersection (axis-aligned box)
 * @param reflect_ray Output parameter for reflect ray
 * (contains reflect ray if intersection exists)
 *
 * @return Distance from start of the ray to intersection point if it exists,
 * negative value otherwise
 */
template <typename T>
T rayShapeIntersection(const Ray<T>& ray, const Box<T>& box, Ray<T>& reflect_ray) {
  const T x_v = isPositive(ray.dir_.x_) ? box.min_.x_ : box.max_.x_;
  const T x_min = box.min_.x_ - ray.pos_.x_ / ray.dir_.x_;
  const T x_max = box.max_.x_ - ray.pos_.x_ / ray.dir_.x_;

  printf("ANosach0: %f %f %f\n", dist_x, dist_y, dist_z);
  T res = dist_x;
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

  //reflect_ray.dir_ = normalizeVector(reflect_ray.dir_);
  return res;
}

}  // namespace geometry
}  // namespace october
