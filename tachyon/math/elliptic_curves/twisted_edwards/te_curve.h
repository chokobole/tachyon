#ifndef TACHYON_MATH_ELLIPTIC_CURVES_TWISTED_EDWARDS_TE_CURVE_H_
#define TACHYON_MATH_ELLIPTIC_CURVES_TWISTED_EDWARDS_TE_CURVE_H_

#include <utility>

#include "tachyon/math/elliptic_curves/affine_point.h"
#include "tachyon/math/elliptic_curves/curve_type.h"
#include "tachyon/math/elliptic_curves/jacobian_point.h"
#include "tachyon/math/elliptic_curves/point_conversions.h"
#include "tachyon/math/elliptic_curves/point_xyzz.h"
#include "tachyon/math/elliptic_curves/projective_point.h"
#include "tachyon/math/elliptic_curves/short_weierstrass/sw_curve_traits.h"

namespace tachyon::math {

// Config for Twisted Edwards model.
// See https://www.hyperelliptic.org/EFD/g1p/auto-twisted.html for more details.
// This config represents a * x² + y² = 1 + d * x² * y², where a d are
// constants.
template <typename TECurveConfig>
class TECurve {
 public:
  using Config = TECurveConfig;

  using BaseField = typename Config::BaseField;
  using ScalarField = typename Config::ScalarField;
  using AffinePointTy = typename SWCurveTraits<Config>::AffinePointTy;
  using JacobianPointTy = typename SWCurveTraits<Config>::JacobianPointTy;

  constexpr static CurveType kType = CurveType::kTwistedEdwards;

  static void Init() {
    BaseField::Init();
    ScalarField::Init();

    Config::Init();
  }

  constexpr static bool IsOnCurve(const AffinePointTy& point) {
    BaseField x2 = point.x().Square();
    BaseField y2 = point.y().Square();
    BaseField left = y2;
    if constexpr (!Config::kAIsZero) {
      left += Config::kA * x2;
    }
    BaseField right = BaseField::One() + Config::kD * x2 * y2;
    return left == right;
  }
};

}  // namespace tachyon::math

#endif  // TACHYON_MATH_ELLIPTIC_CURVES_TWISTED_EDWARDS_TE_CURVE_H_
