// Copyright (c) 2022 The Plonky3 Authors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.plonky3 and the LICENCE-APACHE.plonky3
// file.

#include "tachyon/math/finite_fields/baby_bear/packed_baby_bear_avx2.h"

#include <immintrin.h>

namespace tachyon::math {

namespace {

__m256i kP;
__m256i kZero;
__m256i kOne;

__m256i ToVector(const PackedBabyBearAVX2& packed) {
  return _mm256_loadu_si256(
      reinterpret_cast<const __m256i_u*>(packed.values().data()));
}

PackedBabyBearAVX2 FromVector(__m256i vector) {
  PackedBabyBearAVX2 ret;
  _mm256_storeu_si256(reinterpret_cast<__m256i_u*>(ret.values().data()),
                      vector);
  return ret;
}

__m256i Add(__m256i lhs, __m256i rhs) {
  // We want this to compile to:
  //      vpaddd   t, lhs, rhs
  //      vpsubd   u, t, P
  //      vpminud  res, t, u
  // throughput: 1 cyc/vec (8 els/cyc)
  // latency: 3 cyc

  // Let t := lhs + rhs
  //     u := (t - P) mod 2³²
  //     r := min(t, u)
  //
  // 0 ≤ lhs, rhs ≤ P - 1
  // 0 ≤ t ≤ 2P - 2
  //
  // 1) if 0 ≤ t ≤ P - 1:
  //    2³² - P ≤ u ≤ 2³² - 1
  //    2(P + 1) - P ≤ u ≤ 2³² - 1
  //    P - 1 < P + 1 ≤ u ≤ 2³² - 1
  //    r = t
  //
  // 2) otherwise P ≤ t ≤ 2P - 2:
  //    0 ≤ u ≤ P - 2 < P
  //    r = u
  //
  // In both cases, r is in {0, ..., P - 1}.
  __m256i t = _mm256_add_epi32(lhs, rhs);
  __m256i u = _mm256_sub_epi32(t, kP);
  return _mm256_min_epu32(t, u);
}

__m256i Sub(__m256i lhs, __m256i rhs) {
  // We want this to compile to:
  //      vpsubd   t, lhs, rhs
  //      vpaddd   u, t, P
  //      vpminud  r, t, u
  // throughput: 1 cyc/vec (8 els/cyc)
  // latency: 3 cyc

  // Let d := lhs - rhs
  //     t := d mod 2³²
  //     u := (t + P) mod 2³²
  //     r := min(t, u)
  //
  // 0 ≤ lhs, rhs ≤ P - 1
  // -P + 1 ≤ d ≤ P - 1
  //
  // 1) if 0 ≤ d ≤ P - 1:
  //    0 ≤ t ≤ P - 1
  //    P - 1 < P ≤ u ≤ 2P - 1
  //    r = t
  //
  // 2) otherwise -P + 1 ≤ d ≤ -1:
  //    2³² - P + 1 ≤ t ≤ 2³² - 1
  //    2(P + 1) - P + 1 ≤ t ≤ 2³² - 1
  //    P + 3 ≤ t ≤ 2³² - 1
  //    1 ≤ u ≤ P - 1 < P + 3
  //    r = u
  //
  // In both cases, r is in {0, ..., P - 1}.
  __m256i t = _mm256_sub_epi32(lhs, rhs);
  __m256i u = _mm256_add_epi32(t, kP);
  return _mm256_min_epu32(t, u);
}

__m256i Neg(__m256i val) {
  // We want this to compile to:
  //      vpsubd   t, P, val
  //      vpsignd  res, t, val
  // throughput: .67 cyc/vec (12 els/cyc)
  // latency: 2 cyc

  //   The vpsignd instruction is poorly named, because it doesn't _return_ or
  //   _copy_ the sign of
  // anything, but _multiplies_ x by the sign of y (treating both as signed
  // integers). In other words,
  //                       { x            if y > 0,
  //      vpsignd(x, y) := { 0            if y = 0,
  //                       { -x mod 2^32  if y < 0.
  //   We define t := P - val and note that t = -val (mod P). When val is in {1,
  //   ..., P - 1}, t is
  // similarly in {1, ..., P - 1}, so it's in canonical form. Otherwise, val = 0
  // and t = P.
  //   This is where we define res := vpsignd(t, val). The sign bit of val is
  //   never set so either
  // val = 0 or val >s 0. If val = 0, then res = vpsignd(t, 0) = 0, as desired.
  // Otherwise, res = vpsignd(t, val) = t passes t through.
  __m256i t = _mm256_sub_epi32(kP, val);
  return _mm256_sign_epi32(t, val);
}

__m256i Mul(__m256i lhs, __m256i rhs) { return lhs; }

}  // namespace

// static
void PackedBabyBearAVX2::Init() {
  kP = _mm256_set1_epi32(BabyBear::Config::kModulus[0]);
  kZero = _mm256_set1_epi32(0);
  kOne = _mm256_set1_epi32(1);
}

// static
PackedBabyBearAVX2 PackedBabyBearAVX2::Zero() { return FromVector(kZero); }

// static
PackedBabyBearAVX2 PackedBabyBearAVX2::One() { return FromVector(kOne); }

// static
PackedBabyBearAVX2 PackedBabyBearAVX2::Broadcast(const PrimeField& value) {
  return FromVector(_mm256_set1_epi32(value.value()));
}

PackedBabyBearAVX2 PackedBabyBearAVX2::Add(
    const PackedBabyBearAVX2& other) const {
  return FromVector(tachyon::math::Add(ToVector(*this), ToVector(other)));
}

PackedBabyBearAVX2 PackedBabyBearAVX2::Sub(
    const PackedBabyBearAVX2& other) const {
  return FromVector(tachyon::math::Sub(ToVector(*this), ToVector(other)));
}

PackedBabyBearAVX2 PackedBabyBearAVX2::Negate() const {
  return FromVector(tachyon::math::Neg(ToVector(*this)));
}

PackedBabyBearAVX2 PackedBabyBearAVX2::Mul(
    const PackedBabyBearAVX2& other) const {
  return FromVector(tachyon::math::Mul(ToVector(*this), ToVector(other)));
}

}  // namespace tachyon::math
