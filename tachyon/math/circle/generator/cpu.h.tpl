// clang-format off
#include "tachyon/base/logging.h"
#include "tachyon/math/geometry/point2.h"
#include "%{field_hdr}"

namespace %{namespace} {

template <typename _Field>
class %{class}CircleConfig {
 public:
  using Field = _Field;

  static Point2<Field> kGenerator;

  static void Init() {
%{x_init}
%{y_init}
    VLOG(1) << "%{namespace}::%{class} initialized";
  }
};

template <typename F>
Point2<F> %{class}CurveConfig<F>::kGenerator;

using %{class}CirclePoint = CirclePoint<%{class}CircleConfig>;

}  // namespace %{namespace}
     // clang-format on
