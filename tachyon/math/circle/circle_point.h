// Copyright 2024 StarkWare Industries Ltd
// Use of this source code is governed by a Apache-2.0 style license that
// can be found in the LICENSE-APACHE.stwo

#ifndef TACHYON_MATH_CIRCLE_CIRCLE_POINT_H_
#define TACHYON_MATH_CIRCLE_CIRCLE_POINT_H_

#include <string>
#include <utility>

#include "absl/strings/substitute.h"

#include "tachyon/base/logging.h"
#include "tachyon/math/base/groups.h"
#include "tachyon/math/geometry/point2.h"

namespace tachyon::math {

template <typename _Config>
class CirclePoint : public AdditiveGroup<CirclePoint<_Config>> {
 public:
  using Config = _Config;
  using Field = typename Config::Field;

  constexpr CirclePoint() : CirclePoint(Field::One(), Field::Zero()) {}
  explicit constexpr CirclePoint(const Point2<Field>& point)
      : CirclePoint(point.x, point.y) {}
  explicit constexpr CirclePoint(Point2<Field>&& point)
      : CirclePoint(std::move(point.x), std::move(point.y)) {}
  constexpr CirclePoint(const Field& x, const Field& y) : x_(x), y_(y) {}
  constexpr CirclePoint(Field&& x, Field&& y)
      : x_(std::move(x)), y_(std::move(y)) {}

  constexpr static CirclePoint CreateChecked(const Field& x, const Field& y) {
    CirclePoint ret = {x, y};
    CHECK(ret.IsOnCircle());
    return ret;
  }

  constexpr static CirclePoint CreateChecked(Field&& x, Field&& y) {
    CirclePoint ret = {std::move(x), std::move(y)};
    CHECK(ret.IsOnCircle());
    return ret;
  }

  constexpr static CirclePoint Zero() { return CirclePoint(); }

  constexpr static CirclePoint Generator() {
    return {Config::kGenerator.x, Config::kGenerator.y};
  }

  constexpr static CirclePoint Random() {
    NOTIMPLEMENTED();
    return CirclePoint();
  }

  constexpr static Field DoubleX(const Field& x) {
    // CirclePoint<Config> p = CirclePoint<Config>::Random();
    // CHECK_EQ(CirclePoint<Config>::DoubleX(p.x), (p + p).x());
    return x.Square().Double() - Field::One();
  }

  constexpr static Field& DoubleXInPlace(Field& x) {
    return x.SquareInPlace().DoubleInPlace() -= Field::One();
  }

  constexpr const Field& x() const { return x_; }
  constexpr const Field& y() const { return y_; }

  constexpr bool operator==(const CirclePoint& other) const {
    return x_ == other.x_ && y_ == other.y_;
  }
  constexpr bool operator!=(const CirclePoint& other) const {
    return !operator==(other);
  }

  constexpr bool IsZero() const { return x_.IsOne() && y_.IsZero(); }

  constexpr bool IsOnCircle() {
    NOTIMPLEMENTED();
    return false;
  }

  // Return the log order of a point.
  //
  // All points have an order of the form 2ᵏ.
  constexpr uint32_t LogOrder() const {
    // We only need the x-coordinate to check order since the only point
    // with x = 1 is the circle's identity.
    uint32_t ret = 0;
    Field cur = x_;
    while (!cur.IsOne()) {
      cur = DoubleXInPlace(cur);
      ++ret;
    }
    return ret;
  }

  constexpr CirclePoint Conjugate() const { return {x_, -y_}; }

  constexpr CirclePoint ComplexConjugate() const {
    return {x_.ComplexConjugate(), y_.ComplexConjugate()};
  }

  constexpr CirclePoint Antipode() const { return {-x_, -y_}; }

  std::string ToString() const {
    return absl::Substitute("($0, $1)", x_.ToString(), y_.ToString());
  }

  std::string ToHexString(bool pad_zero = false) const {
    return absl::Substitute("($0, $1)", x_.ToHexString(pad_zero),
                            y_.ToHexString(pad_zero));
  }

  // AdditiveSemigroup methods
  constexpr CirclePoint Add(const CirclePoint& other) const {
    return {x_ * other.x_ - y * other.y_, x_ * other.y_ + y_ * other.x_};
  }

  constexpr CirclePoint& AddInPlace(const CirclePoint& other) const {
    Field tmp = x_ * other.y_;
    x_ *= other.x_;
    x_ -= y * other.y_;
    y_ *= other.x_;
    y_ += tmp;
    return *this;
  }

  // AdditiveGroup methods
  constexpr CirclePoint Negative() const { return {x_, -y_}; }

  constexpr CirclePoint& NegInPlace() {
    y_.NegInPlace();
    return *this;
  }

 private:
  Field x_;
  Field y_;
};

}  // namespace tachyon::math

#endif  // TACHYON_MATH_CIRCLE_CIRCLE_POINT_H_
