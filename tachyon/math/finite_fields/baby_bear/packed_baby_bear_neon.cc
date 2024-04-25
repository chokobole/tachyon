// Copyright (c) 2022 The Plonky3 Authors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.plonky3 and the LICENCE-APACHE.plonky3
// file.

#include "tachyon/math/finite_fields/baby_bear/packed_baby_bear_neon.h"

#include <arm_neon.h>

namespace tachyon::math {

namespace {

uint32x4_t kP;
uint32x4_t kZero;
uint32x4_t kOne;

uint32x4_t ToVector(const PackedBabyBearNeon& packed) {
  return vld1q_u32(reinterpret_cast<const uint32_t*>(packed.values().data()));
}

PackedBabyBearNeon FromVector(uint32x4_t vector) {
  PackedBabyBearNeon ret;
  vst1q_u32(reinterpret_cast<uint32_t*>(ret.values().data()), vector);
  return ret;
}

uint32x4_t Add(uint32x4_t lhs, uint32x4_t rhs) {
  // We want this to compile to:
  //      add   t.4s, lhs.4s, rhs.4s
  //      sub   u.4s, t.4s, P.4s
  //      umin  r.4s, t.4s, u.4s
  // throughput: .75 cyc/vec (5.33 els/cyc)
  // latency: 6 cyc

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
  uint32x4_t t = vaddq_u32(lhs, rhs);
  uint32x4_t u = vsubq_u32(t, kP);
  return vminq_u32(t, u);
}
}  // namespace

uint32x4_t Sub(uint32x4_t lhs, uint32x4_t rhs) {
  // We want this to compile to:
  //      sub   r.4s, lhs.4s, rhs.4s
  //      cmhi  underflow.4s, rhs.4s, lhs.4s
  //      mls   r.4s, underflow.4s, P.4s
  // throughput: .75 cyc/vec (5.33 els/cyc)
  // latency: 5 cyc

  // Let t := lhs - rhs
  //     diff := t mod 2³²
  //     underflow := 0 if lhs ≥ rhs else 2³² - 1
  //     r := (diff - underflow * P) mod 2³²
  //
  // 0 ≤ lhs, rhs ≤ P - 1
  //
  // 1) lhs ≥ rhs -> underflow = 0:
  //    0 ≤ t ≤ P - 1
  //    0 ≤ r = diff ≤ P - 1
  //
  // 2) otherwise lhs < rhs -> underflow = 2³² - 1:
  //    -P + 1 ≤ t ≤ -1
  //    2³² + -P + 1 ≤ diff ≤ 2³² -1
  //    2³² -P + 1 - (2³² - 1)P ≤ r ≤ 2³² - 1 - (2³² - 1)P
  //    2³²(1 - P) + 1 ≤ r ≤ 2³²(1 - P) + P - 1
  //    1 ≤ r ≤ P - 1
  //
  // In both cases, r is in {0, ..., P - 1}.
  uint32x4_t diff = vsubq_u32(lhs, rhs);
  uint32x4_t underflow = vcltq_u32(lhs, rhs);
  return vmlsq_u32(diff, underflow, kP);
}

uint32x4_t Neg(uint32x4_t val) {
  // We want this to compile to:
  //      sub   t.4s, P.4s, val.4s
  //      cmeq  is_zero.4s, val.4s, #0
  //      bic   r.4s, t.4s, is_zero.4s
  // throughput: .75 cyc/vec (5.33 els/cyc)
  // latency: 4 cyc

  // This has the same throughput as |Sub(kZero, val)| but slightly lower
  // latency.
  //
  // Let t := P - val
  //     is_zero := 2³² - 1 if val = 0 else 0
  //     r := t & ~is_zero
  //
  // 0 ≤ val ≤ P - 1
  //
  // 1) val = 0 -> is_zero = 2³² - 1:
  //    r = 0
  //
  // 2) otherwise 1 ≤ val ≤ P - 1 -> is_zero = 0:
  //    r = t
  uint32x4_t t = vsubq_u32(kP, val);
  uint32x4_t is_zero = vceqzq_u32(val);
  return vbicq_u32(t, is_zero);
}

uint32x4_t Mul(uint32x4_t lhs, uint32x4_t rhs) { return lhs; }

}  // namespace

// static
void PackedBabyBearNeon::Init() {
  kP = vdupq_n_u32(BabyBear::Config::kModulus[0]);
  kZero = vdupq_n_u32(0);
  kOne = vdupq_n_u32(1);
}

// static
PackedBabyBearNeon PackedBabyBearNeon::Zero() { return FromVector(kZero); }

// static
PackedBabyBearNeon PackedBabyBearNeon::One() { return FromVector(kOne); }

// static
PackedBabyBearNeon PackedBabyBearNeon::Broadcast(const PrimeField& value) {
  return FromVector(vdupq_n_u32(value.value()));
}

PackedBabyBearNeon PackedBabyBearNeon::Add(
    const PackedBabyBearNeon& other) const {
  return FromVector(tachyon::math::Add(ToVector(*this), ToVector(other)));
}

PackedBabyBearNeon PackedBabyBearNeon::Sub(
    const PackedBabyBearNeon& other) const {
  return FromVector(tachyon::math::Sub(ToVector(*this), ToVector(other)));
}

PackedBabyBearNeon PackedBabyBearNeon::Negate() const {
  return FromVector(tachyon::math::Neg(ToVector(*this)));
}

PackedBabyBearNeon PackedBabyBearNeon::Mul(
    const PackedBabyBearNeon& other) const {
  return FromVector(tachyon::math::Mul(ToVector(*this), ToVector(other)));
}

}  // namespace tachyon::math
