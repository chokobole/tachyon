// Copyright 2024 StarkWare Industries Ltd
// Use of this source code is governed by a Apache-2.0 style license that
// can be found in the LICENSE-APACHE.stwo

#ifndef TACHYON_MATH_CIRCLE_CIRCLE_ADDITIVE_SUBGROUP_H_
#define TACHYON_MATH_CIRCLE_CIRCLE_ADDITIVE_SUBGROUP_H_

namespace tachyon::math {

template <typename Derived>
class CircleAdditiveSubgroup {
 public:
  [[nodiscard]] constexpr F CircleDouble() const {
    F f = *static_cast<const F*>(this);
    return f.CyclotomicSquareInPlace();
  }

  constexpr F& CircleDoubleInPlace() {
    F* f = static_cast<F*>(this);
    if constexpr (internal::SupportsFastCyclotomicSquareInPlace<F>::value) {
      return f->FastCyclotomicSquareInPlace();
    } else {
      return f->SquareInPlace();
    }
  }
};

}  // namespace tachyon::math

#endif  // TACHYON_MATH_CIRCLE_CIRCLE_ADDITIVE_SUBGROUP_H_
