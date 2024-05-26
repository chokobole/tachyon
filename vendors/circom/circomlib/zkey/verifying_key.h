#ifndef VENDORS_CIRCOM_CIRCOMLIB_ZKEY_VERIFYING_KEY_H_
#define VENDORS_CIRCOM_CIRCOMLIB_ZKEY_VERIFYING_KEY_H_

#include <stddef.h>

#include <string>

#include "absl/strings/substitute.h"

#include "tachyon/base/buffer/copyable.h"
#include "tachyon/base/numerics/checked_math.h"

namespace tachyon {
namespace circom {

template <typename Curve>
struct VerifyingKey {
  using G1AffinePoint = typename Curve::G1Curve::AffinePoint;
  using G2AffinePoint = typename Curve::G2Curve::AffinePoint;

  const G1AffinePoint* alpha_g1 = nullptr;
  const G1AffinePoint* beta_g1 = nullptr;
  const G2AffinePoint* beta_g2 = nullptr;
  const G2AffinePoint* gamma_g2 = nullptr;
  const G1AffinePoint* delta_g1 = nullptr;
  const G2AffinePoint* delta_g2 = nullptr;

  bool operator==(const VerifyingKey& other) const {
#define EQ(x) (x == nullptr ? other.x == nullptr : *x == *other.x)
    return EQ(alpha_g1) && EQ(beta_g1) && EQ(beta_g2) && EQ(gamma_g2) &&
           EQ(delta_g1) && EQ(delta_g2);
#undef EQ
  }
  bool operator!=(const VerifyingKey& other) const {
    return !operator==(other);
  }

  // NOTE(chokobole): the fields are represented in montgomery form.
  std::string ToString() const {
#define TO_STRING(x) (x == nullptr ? "null" : x->ToString())
    return absl::Substitute(
        "{alpha_g1: $0, beta_g1: $1, beta_g2: $2, gamma_g2: $3, delta_g1: $4, "
        "delta_g2: $5}",
        TO_STRING(alpha_g1), TO_STRING(beta_g1), TO_STRING(beta_g2),
        TO_STRING(gamma_g2), TO_STRING(delta_g1), TO_STRING(delta_g2));
#undef TO_STRING
  }
};

}  // namespace circom

namespace base {

template <typename Curve>
class Copyable<circom::VerifyingKey<Curve>> {
 public:
  static bool WriteTo(const circom::VerifyingKey<Curve>& vk, Buffer* buffer) {
    NOTREACHED();
    return false;
  }

  static bool ReadFrom(const ReadOnlyBuffer& buffer,
                       circom::VerifyingKey<Curve>* vk) {
    using G1AffinePoint = typename Curve::G1Curve::AffinePoint;
    using G2AffinePoint = typename Curve::G2Curve::AffinePoint;

    base::CheckedNumeric<size_t> len = buffer.buffer_offset();
    size_t size = sizeof(G1AffinePoint) * 3 + sizeof(G2AffinePoint) * 3;
    size_t size_needed;
    if (!(len + size).AssignIfValid(&size_needed)) return false;
    if (size_needed > buffer.buffer_len()) {
      return false;
    }

    vk->alpha_g1 = reinterpret_cast<const G1AffinePoint*>(
        reinterpret_cast<const uint8_t*>(buffer.buffer()) +
        buffer.buffer_offset());
    buffer.set_buffer_offset(buffer.buffer_offset() + sizeof(G1AffinePoint));
    vk->beta_g1 = reinterpret_cast<const G1AffinePoint*>(
        reinterpret_cast<const uint8_t*>(buffer.buffer()) +
        buffer.buffer_offset());
    buffer.set_buffer_offset(buffer.buffer_offset() + sizeof(G1AffinePoint));
    vk->beta_g2 = reinterpret_cast<const G2AffinePoint*>(
        reinterpret_cast<const uint8_t*>(buffer.buffer()) +
        buffer.buffer_offset());
    buffer.set_buffer_offset(buffer.buffer_offset() + sizeof(G2AffinePoint));
    vk->gamma_g2 = reinterpret_cast<const G2AffinePoint*>(
        reinterpret_cast<const uint8_t*>(buffer.buffer()) +
        buffer.buffer_offset());
    buffer.set_buffer_offset(buffer.buffer_offset() + sizeof(G2AffinePoint));
    vk->delta_g1 = reinterpret_cast<const G1AffinePoint*>(
        reinterpret_cast<const uint8_t*>(buffer.buffer()) +
        buffer.buffer_offset());
    buffer.set_buffer_offset(buffer.buffer_offset() + sizeof(G1AffinePoint));
    vk->delta_g2 = reinterpret_cast<const G2AffinePoint*>(
        reinterpret_cast<const uint8_t*>(buffer.buffer()) +
        buffer.buffer_offset());
    buffer.set_buffer_offset(buffer.buffer_offset() + sizeof(G2AffinePoint));
    return true;
  }

  static size_t EstimateSize(const circom::VerifyingKey<Curve>& vk) {
    NOTREACHED();
    return 0;
  }
};

}  // namespace base
}  // namespace tachyon

#endif  // VENDORS_CIRCOM_CIRCOMLIB_ZKEY_VERIFYING_KEY_H_
