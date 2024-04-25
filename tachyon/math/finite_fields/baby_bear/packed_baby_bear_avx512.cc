// Copyright (c) 2022 The Plonky3 Authors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.plonky3 and the LICENCE-APACHE.plonky3
// file.

#include "tachyon/math/finite_fields/baby_bear/packed_baby_bear_avx512.h"

#include <immintrin.h>

namespace tachyon::math {

namespace {

__m512i kP;
__m512i kZero;
__m512i kOne;

__m512i ToVector(const PackedBabyBearAVX512& packed) {
  return _mm512_loadu_si512(
      reinterpret_cast<const __m512i_u*>(packed.values().data()));
}

PackedBabyBearAVX512 FromVector(__m512i vector) {
  PackedBabyBearAVX512 ret;
  _mm512_storeu_si512(reinterpret_cast<__m512i_u*>(ret.values().data()),
                      vector);
  return ret;
}

__m512i Add(__m512i lhs, __m512i rhs) {
  // We want this to compile to:
  //      vpaddd   t, lhs, rhs
  //      vpsubd   u, t, P
  //      vpminud  r, t, u
  // throughput: 1.5 cyc/vec (10.67 els/cyc)
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
  __m512i t = _mm512_add_epi32(lhs, rhs);
  __m512i u = _mm512_sub_epi32(t, kP);
  return _mm512_min_epu32(t, u);
}

__m512i Sub(__m512i lhs, __m512i rhs) {
  // We want this to compile to:
  //      vpsubd   t, lhs, rhs
  //      vpaddd   u, t, P
  //      vpminud  r, t, u
  // throughput: 1.5 cyc/vec (10.67 els/cyc)
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
  __m512i t = _mm512_sub_epi32(lhs, rhs);
  __m512i u = _mm512_add_epi32(t, kP);
  return _mm512_min_epu32(t, u);
}

__m512i Neg(__m512i val) {
  // We want this to compile to:
  //      vptestmd  nonzero, val, val
  //      vpsubd    res{nonzero}{z}, P, val
  // throughput: 1 cyc/vec (16 els/cyc)
  // latency: 4 cyc

  // NOTE: This routine prioritizes throughput over latency. An alternative
  // method would be to do |Sub(kZero, val)|, which would result in shorter
  // latency, but also lower throughput.

  // If |val| is non-zero, then |val| is in {1, ..., P - 1} and |kP - val| is
  // in the same range. If |val| is zero, then the result is zeroed by masking.
  __mmask16 nonzero = _mm512_test_epi32_mask(val, val);
  return _mm512_maskz_sub_epi32(nonzero, kP, val);
}

__m512i Mul(__m512i lhs, __m512i rhs) { return lhs; }

}  // namespace

// static
void PackedBabyBearAVX512::Init() {
  kP = _mm512_set1_epi32(BabyBear::Config::kModulus[0]);
  kZero = _mm512_set1_epi32(0);
  kOne = _mm512_set1_epi32(1);
}

// static
PackedBabyBearAVX512 PackedBabyBearAVX512::Zero() { return FromVector(kZero); }

// static
PackedBabyBearAVX512 PackedBabyBearAVX512::One() { return FromVector(kOne); }

// static
PackedBabyBearAVX512 PackedBabyBearAVX512::Broadcast(const PrimeField& value) {
  return FromVector(_mm512_set1_epi32(value.value()));
}

PackedBabyBearAVX512 PackedBabyBearAVX512::Add(
    const PackedBabyBearAVX512& other) const {
  return FromVector(tachyon::math::Add(ToVector(*this), ToVector(other)));
}

PackedBabyBearAVX512 PackedBabyBearAVX512::Sub(
    const PackedBabyBearAVX512& other) const {
  return FromVector(tachyon::math::Sub(ToVector(*this), ToVector(other)));
}

PackedBabyBearAVX512 PackedBabyBearAVX512::Negate() const {
  return FromVector(tachyon::math::Neg(ToVector(*this)));
}

PackedBabyBearAVX512 PackedBabyBearAVX512::Mul(
    const PackedBabyBearAVX512& other) const {
  return FromVector(tachyon::math::Mul(ToVector(*this), ToVector(other)));
}

}  // namespace tachyon::math
