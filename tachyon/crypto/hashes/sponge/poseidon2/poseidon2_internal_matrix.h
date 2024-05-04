// Copyright (c) 2022 The Plonky3 Authors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.plonky3 and the LICENCE-APACHE.plonky3
// file.

#ifndef TACHYON_CRYPTO_HASHES_SPONGE_POSEIDON2_POSEIDON2_INTERNAL_MATRIX_H_
#define TACHYON_CRYPTO_HASHES_SPONGE_POSEIDON2_POSEIDON2_INTERNAL_MATRIX_H_

#include <numeric>

#include "tachyon/math/matrix/matrix_types.h"

namespace tachyon::crypto {

template <typename PrimeField>
class Poseidon2InternalMatrix {
 public:
  static void Apply(math::Vector<PrimeField>& v,
                    const math::Vector<PrimeField>& diagonal_minus_one) {
    PrimeField sum = std::accumulate(
        v.begin(), v.end(), PrimeField::Zero(),
        [](PrimeField& acc, const PrimeField& value) { return acc += value; });
    for (Eigen::Index i = 0; i < v.size(); ++i) {
      v[i] *= diagonal_minus_one[i];
      v[i] += sum;
    }
  }
};

}  // namespace tachyon::crypto

#endif  // TACHYON_CRYPTO_HASHES_SPONGE_POSEIDON2_POSEIDON2_INTERNAL_MATRIX_H_
