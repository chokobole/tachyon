#ifndef TACHYON_MATH_ELLIPTIC_CURVES_BN_BN254_POSEIDON2_H_
#define TACHYON_MATH_ELLIPTIC_CURVES_BN_BN254_POSEIDON2_H_

#include <array>

#include "tachyon/base/types/always_false.h"
#include "tachyon/math/elliptic_curves/bn/bn254/fr.h"

namespace tachyon::math::bn254 {

template <size_t N>
std::array<Fr, N> GetPoseidon2InternalDiagonalVector() {
  // TODO(chokobole): remove this function once we can generate these parameters
  // internally.
  // This is taken and modified from
  // https://github.com/HorizenLabs/poseidon2/blob/bb476b9ca38198cf5092487283c8b8c5d4317c4e/plain_implementations/src/poseidon2/poseidon2_instance_bn256.rs.
  if constexpr (N == 16) {
    // Generated with rate: 15, alpha: 7, full_round: 8 and partial_round: 13.
    return {
        Fr(0x0a632d94), Fr(0x6db657b7), Fr(0x56fbdc9e), Fr(0x052b3d8a),
        Fr(0x33745201), Fr(0x5c03108c), Fr(0x0beba37b), Fr(0x258c2e8b),
        Fr(0x12029f39), Fr(0x694909ce), Fr(0x6d231724), Fr(0x21c3b222),
        Fr(0x3c0904a5), Fr(0x01d6acda), Fr(0x27705c83), Fr(0x5231c802),
    };
  } else if constexpr (N == 24) {
    // Generated with rate: 23, alpha: 7, full_round: 8 and partial_round: 21.
    return {
        Fr(0x409133f0), Fr(0x1667a8a1), Fr(0x06a6c7b6), Fr(0x6f53160e),
        Fr(0x273b11d1), Fr(0x03176c5d), Fr(0x72f9bbf9), Fr(0x73ceba91),
        Fr(0x5cdef81d), Fr(0x01393285), Fr(0x46daee06), Fr(0x065d7ba6),
        Fr(0x52d72d6f), Fr(0x05dd05e0), Fr(0x3bab4b63), Fr(0x6ada3842),
        Fr(0x2fc5fbec), Fr(0x770d61b0), Fr(0x5715aae9), Fr(0x03ef0e90),
        Fr(0x75b6c770), Fr(0x242adf5f), Fr(0x00d0ca4c), Fr(0x36c0e388),
    };
  } else {
    static_assert(base::AlwaysFalse<std::array<Fr, N>>());
  }
}

}  // namespace tachyon::math::bn254

#endif  // TACHYON_MATH_ELLIPTIC_CURVES_BN_BN254_POSEIDON2_H_
