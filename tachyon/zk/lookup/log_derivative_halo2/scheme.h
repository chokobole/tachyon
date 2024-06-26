#ifndef TACHYON_ZK_LOOKUP_LOG_DERIVATIVE_HALO2_SCHEME_H_
#define TACHYON_ZK_LOOKUP_LOG_DERIVATIVE_HALO2_SCHEME_H_

#include "tachyon/zk/lookup/log_derivative_halo2/evaluator.h"
#include "tachyon/zk/lookup/log_derivative_halo2/prover.h"
#include "tachyon/zk/lookup/log_derivative_halo2/verifier.h"
#include "tachyon/zk/lookup/log_derivative_halo2/verifier_data.h"
#include "tachyon/zk/lookup/type.h"
#include "tachyon/zk/plonk/halo2/proof.h"

namespace tachyon::zk::lookup::log_derivative_halo2 {

template <typename _Poly, typename _Evals, typename _Commitment>
struct Scheme {
  using Poly = _Poly;
  using Evals = _Evals;
  using Commitment = _Commitment;
  using Field = typename Poly::Field;

  using Prover = lookup::log_derivative_halo2::Prover<Poly, Evals>;
  using Verifier = lookup::log_derivative_halo2::Verifier<Field, Commitment>;
  using VerifierData =
      lookup::log_derivative_halo2::VerifierData<Field, Commitment>;
  using Evaluator = lookup::log_derivative_halo2::Evaluator<Field, Evals>;
  using Proof = plonk::halo2::LogDerivativeHalo2Proof<Field, Commitment>;

  constexpr static Type type = Type::kLogDerivativeHalo2;
};

}  // namespace tachyon::zk::lookup::log_derivative_halo2

#endif  // TACHYON_ZK_LOOKUP_LOG_DERIVATIVE_HALO2_SCHEME_H_
