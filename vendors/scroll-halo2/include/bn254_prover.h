#ifndef VENDORS_SCROLL_HALO2_INCLUDE_BN254_PROVER_H_
#define VENDORS_SCROLL_HALO2_INCLUDE_BN254_PROVER_H_

#include <stdint.h>

#include <memory>

#include "rust/cxx.h"

#include "tachyon/c/zk/plonk/halo2/bn254_prover.h"

namespace tachyon::halo2_api::bn254 {

struct Fr;
struct G1AffinePoint;
struct G1ProjectivePoint;
struct G2AffinePoint;
struct InstanceSingle;
struct AdviceSingle;
class ProvingKey;
class Evals;
class RationalEvals;
class Poly;

class Prover {
 public:
  Prover(uint8_t pcs_type, uint8_t transcript_type, uint32_t k, const Fr& s);
  Prover(uint8_t pcs_type, uint8_t transcript_type, uint32_t k,
         const uint8_t* params, size_t params_len);
  Prover(const Prover& other) = delete;
  Prover& operator=(const Prover& other) = delete;
  ~Prover();

  const tachyon_halo2_bn254_prover* prover() const { return prover_; }

  uint32_t k() const;
  uint64_t n() const;
  const G2AffinePoint& s_g2() const;
  rust::Box<G1ProjectivePoint> commit(const Poly& poly) const;
  rust::Box<G1ProjectivePoint> commit_lagrange(const Evals& evals) const;
  void batch_start(size_t len) const;
  void batch_commit(const Poly& poly, size_t i) const;
  void batch_commit_lagrange(const Evals& evals, size_t i) const;
  void batch_end(rust::Slice<G1AffinePoint> points) const;
  std::unique_ptr<Evals> empty_evals() const;
  std::unique_ptr<RationalEvals> empty_rational_evals() const;
  std::unique_ptr<Poly> ifft(const Evals& evals) const;
  void batch_evaluate(
      rust::Slice<const std::unique_ptr<RationalEvals>> rational_evals,
      rust::Slice<std::unique_ptr<Evals>> evals) const;
  void set_rng(uint8_t rng_type, rust::Slice<const uint8_t> state);
  void set_transcript(rust::Slice<const uint8_t> state);
  void set_extended_domain(const ProvingKey& pk);
  void create_proof(ProvingKey& key,
                    rust::Slice<InstanceSingle> instance_singles,
                    rust::Slice<AdviceSingle> advice_singles,
                    rust::Slice<const Fr> challenges);
  rust::Vec<uint8_t> get_proof() const;

 private:
  tachyon_halo2_bn254_prover* prover_;
};

std::unique_ptr<Prover> new_prover(uint8_t pcs_type, uint8_t transcript_type,
                                   uint32_t k, const Fr& s);

std::unique_ptr<Prover> new_prover_from_params(
    uint8_t pcs_type, uint8_t transcript_type, uint32_t k,
    rust::Slice<const uint8_t> params);

}  // namespace tachyon::halo2_api::bn254

#endif  // VENDORS_SCROLL_HALO2_INCLUDE_BN254_PROVER_H_
