// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_PLONK_PERMUTATION_PERMUTATION_PROVING_KEY_H_
#define TACHYON_ZK_PLONK_PERMUTATION_PERMUTATION_PROVING_KEY_H_

#include <utility>
#include <vector>

#include "tachyon/base/buffer/copyable.h"
#include "tachyon/zk/plonk/permutation/unpermuted_table.h"

namespace tachyon {
namespace zk::plonk {

template <typename Poly, typename Evals>
class PermutationProvingKey {
 public:
  PermutationProvingKey() = default;
  PermutationProvingKey(const UnpermutedTable<Evals>& unpermuted_table,
                        const std::vector<Evals>& permutations,
                        const std::vector<Poly>& polys)
      : unpermuted_table_(unpermuted_table),
        permutations_(permutations),
        polys_(polys) {}
  PermutationProvingKey(UnpermutedTable<Evals>&& unpermuted_table,
                        std::vector<Evals>&& permutations,
                        std::vector<Poly>&& polys)
      : unpermuted_table_(std::move(unpermuted_table)),
        permutations_(std::move(permutations)),
        polys_(std::move(polys)) {}

  const UnpermutedTable<Evals>& unpermuted_table() const {
    return unpermuted_table_;
  }
  const std::vector<Evals>& permutations() const { return permutations_; }
  const std::vector<Poly>& polys() const { return polys_; }
  std::vector<Poly>& polys() { return polys_; }

  size_t BytesLength() const { return base::EstimateSize(this); }

  bool operator==(const PermutationProvingKey& other) const {
    return permutations_ == other.permutations_ && polys_ == other.polys_;
  }
  bool operator!=(const PermutationProvingKey& other) const {
    return !operator==(other);
  }

 private:
  UnpermutedTable<Evals> unpermuted_table_;
  std::vector<Evals> permutations_;
  std::vector<Poly> polys_;
};

}  // namespace zk::plonk

namespace base {

template <typename Poly, typename Evals>
class Copyable<zk::plonk::PermutationProvingKey<Poly, Evals>> {
 public:
  static bool WriteTo(const zk::plonk::PermutationProvingKey<Poly, Evals>& pk,
                      Buffer* buffer) {
    return buffer->WriteMany(pk.permutations(), pk.polys());
  }

  static bool ReadFrom(const ReadOnlyBuffer& buffer,
                       zk::plonk::PermutationProvingKey<Poly, Evals>* pk) {
    std::vector<Evals> perms;
    std::vector<Poly> poly;
    if (!buffer.ReadMany(&perms, &poly)) return false;

    *pk = zk::plonk::PermutationProvingKey<Poly, Evals>(std::move(perms),
                                                        std::move(poly));
    return true;
  }

  static size_t EstimateSize(
      const zk::plonk::PermutationProvingKey<Poly, Evals>& pk) {
    return base::EstimateSize(pk.permutations(), pk.polys());
  }
};

}  // namespace base
}  // namespace tachyon

#endif  // TACHYON_ZK_PLONK_PERMUTATION_PERMUTATION_PROVING_KEY_H_
