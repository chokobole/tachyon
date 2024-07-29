#include "vendors/scroll-halo2/include/bn254_msm_gpu.h"

#include <memory>

#include "tachyon/c/math/elliptic_curves/bn/bn254/g1_point_type_traits.h"
#include "tachyon/c/math/elliptic_curves/bn/bn254/msm_gpu.h"
#include "vendors/scroll-halo2/src/bn254.rs.h"

namespace tachyon::halo2_api::bn254 {

rust::Box<G1MSMGpu> create_g1_msm_gpu(uint8_t degree) {
  return rust::Box<G1MSMGpu>::from_raw(
      reinterpret_cast<G1MSMGpu*>(tachyon_bn254_g1_create_msm_gpu(degree)));
}

void destroy_g1_msm_gpu(rust::Box<G1MSMGpu> msm) {
  tachyon_bn254_g1_destroy_msm_gpu(
      reinterpret_cast<tachyon_bn254_g1_msm_gpu_ptr>(msm.into_raw()));
}

rust::Box<G1ProjectivePoint> g1_point2_msm_gpu(
    G1MSMGpu* msm, rust::Slice<const G1Point2> bases,
    rust::Slice<const Fr> scalars) {
  std::unique_ptr<tachyon_bn254_g1_jacobian> jacobian(
      tachyon_bn254_g1_point2_msm_gpu(
          reinterpret_cast<tachyon_bn254_g1_msm_gpu_ptr>(msm),
          reinterpret_cast<const tachyon_bn254_g1_point2*>(bases.data()),
          reinterpret_cast<const tachyon_bn254_fr*>(scalars.data()),
          scalars.length()));
  auto ret = new math::bn254::G1ProjectivePoint(
      c::base::native_cast(jacobian.get())->ToProjective());
  return rust::Box<G1ProjectivePoint>::from_raw(
      reinterpret_cast<G1ProjectivePoint*>(ret));
}

}  // namespace tachyon::halo2_api::bn254
