#ifndef TACHYON_MATH_MATRIX_PRIME_FIELD_NUM_TRAITS_H_
#define TACHYON_MATH_MATRIX_PRIME_FIELD_NUM_TRAITS_H_

#include <type_traits>

#include "third_party/eigen3/Eigen/Core"

#include "tachyon/math/finite_fields/finite_field_forwards.h"

namespace Eigen {

template <typename T, typename SFINAE = void>
struct CostCalculator;

template <typename Config>
struct CostCalculator<tachyon::math::PrimeField<Config>,
                      std::enable_if_t<Config::kModulusBits <= 32>> {
  constexpr static int ComputeReadCost() {
    return NumTraits<uint32_t>::ReadCost;
  }
  constexpr static int ComputeAddCost() {
    // In general, c = (a + b) % M = (a + b) > M ? (a + b) - M : (a + b)
    return NumTraits<uint32_t>::AddCost * 3 / 2;
  }
  constexpr static int ComputeMulCost() {
    // In general, c = (a * b) % M = (a * b) - [(a * b) / M] * M
    return 4 * NumTraits<uint32_t>::MulCost + NumTraits<uint32_t>::AddCost;
  }
};

template <typename Config>
struct CostCalculator<tachyon::math::PrimeField<Config>,
                      std::enable_if_t<(Config::kModulusBits > 32)>> {
  constexpr static size_t kLimbNums =
      tachyon::math::PrimeField<Config>::kLimbNums;

  constexpr static int ComputeReadCost() {
    return static_cast<int>(kLimbNums * NumTraits<uint64_t>::ReadCost);
  }
  constexpr static int ComputeAddCost() {
    // In general, c = (a + b) % M = (a + b) > M ? (a + b) - M : (a + b)
    return static_cast<int>(kLimbNums * NumTraits<uint64_t>::AddCost * 3 / 2);
  }
  constexpr static int ComputeMulCost() {
    // In general, c = (a * b) % M = (a * b) - [(a * b) / M] * M
    return static_cast<int>(kLimbNums * (4 * NumTraits<uint64_t>::MulCost +
                                         NumTraits<uint64_t>::AddCost));
  }
};

template <typename Config>
struct NumTraits<tachyon::math::PrimeField<Config>>
    : GenericNumTraits<tachyon::math::PrimeField<Config>> {
  enum {
    IsInteger = 1,
    IsSigned = 0,
    IsComplex = 0,
    RequireInitialization = 1,
    ReadCost =
        CostCalculator<tachyon::math::PrimeField<Config>>::ComputeReadCost(),
    AddCost =
        CostCalculator<tachyon::math::PrimeField<Config>>::ComputeAddCost(),
    MulCost =
        CostCalculator<tachyon::math::PrimeField<Config>>::ComputeMulCost(),
  };
};

}  // namespace Eigen

#endif  // TACHYON_MATH_MATRIX_PRIME_FIELD_NUM_TRAITS_H_
