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

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tree.h"
#include "tracer.h"

namespace october {
namespace tracer {
namespace test {
/*
october::tree::Tree<float, 5u, geometry::Shape<float> > tree((geometry::Shape<float>()));
october::tracer::Tracer<float> tracer;

class TreeTest {
 public:
  TreeTest() {
    tracer.buildTree(tree, geometry::Ray<float>(), 1.0f);
    tracer.burnTree(tree, geometry::Ray<float>(), 1.0f);

    float p;
    geometry::Ray<float> r;
    tracer.castTree(tree, geometry::Ray<float>(), 1.0f, r, p);
  }
};

TreeTest test;*/

}  // namespace test
}  // namespace tracer
}  // namespace october
