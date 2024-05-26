#ifndef VENDORS_CIRCOM_CIRCOMLIB_CIRCUIT_CIRCUIT_TEST_H_
#define VENDORS_CIRCOM_CIRCOMLIB_CIRCUIT_CIRCUIT_TEST_H_

#include <memory>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "circomlib/circuit/circuit.h"
#include "circomlib/r1cs/r1cs.h"
#include "circomlib/zkey/zkey.h"
#include "tachyon/math/elliptic_curves/bn/bn254/bn254.h"
#include "tachyon/math/polynomials/univariate/univariate_evaluation_domain_factory.h"
#include "tachyon/zk/r1cs/groth16/owned_proving_key.h"
#include "tachyon/zk/r1cs/groth16/prove.h"
#include "tachyon/zk/r1cs/groth16/verify.h"

namespace tachyon::circom {

using F = math::bn254::Fr;
using Curve = math::bn254::BN254Curve;

class CircuitTest : public testing::Test {
 public:
  static void SetUpTestSuite() { Curve::Init(); }

 protected:
  void SynthesizeTest() {
    zk::r1cs::ConstraintSystem<F> constraint_system;
    circuit_->Synthesize(constraint_system);
    ASSERT_TRUE(constraint_system.IsSatisfied());
  }

  template <size_t MaxDegree, typename QAP>
  void Groth16ProveAndVerifyTest() {
    zk::r1cs::groth16::ToxicWaste<Curve> toxic_waste =
        zk::r1cs::groth16::ToxicWaste<Curve>::RandomWithoutX();
    zk::r1cs::groth16::OwnedProvingKey<Curve> opk;
    bool loaded = opk.Load<MaxDegree, QAP>(toxic_waste, *circuit_);
    ASSERT_TRUE(loaded);
    zk::r1cs::groth16::Proof<Curve> proof =
        zk::r1cs::groth16::CreateProofWithReductionZK<MaxDegree, QAP>(*circuit_,
                                                                      opk);
    zk::r1cs::groth16::PreparedVerifyingKey<Curve> pvk =
        std::move(opk).TakeOwnedVerifyingKey().ToPreparedVerifyingKey();
    std::vector<F> public_inputs = circuit_->GetPublicInputs();
    ASSERT_TRUE(VerifyProof(pvk, proof, public_inputs));
  }

  template <size_t MaxDegree, typename QAP>
  void Groth16ProveAndVerifyUsingZKeyTest(
      ZKey<Curve>&& zkey, absl::Span<const F> full_assignments) {
    using Domain = math::UnivariateEvaluationDomain<F, MaxDegree>;

    zk::r1cs::groth16::ProvingKey<Curve> pk =
        zkey.GetProvingKey().ToNativeProvingKey();

    LOG(ERROR) << "alpha_g1: " << pk.verifying_key().alpha_g1();
    LOG(ERROR) << "beta_g2: " << pk.verifying_key().beta_g2();
    LOG(ERROR) << "gamma_g2: " << pk.verifying_key().gamma_g2();
    LOG(ERROR) << "delta_g2: " << pk.verifying_key().delta_g2();
    LOG(ERROR) << "l_g1_query: "
               << base::ContainerToString(pk.verifying_key().l_g1_query());
    LOG(ERROR) << "beta_g1: " << pk.beta_g1();
    LOG(ERROR) << "delta_g1: " << pk.delta_g1();
    LOG(ERROR) << "a_g1_query: " << base::ContainerToString(pk.a_g1_query());
    LOG(ERROR) << "b_g1_query: " << base::ContainerToString(pk.b_g1_query());
    LOG(ERROR) << "b_g2_query: " << base::ContainerToString(pk.b_g2_query());
    LOG(ERROR) << "h_g1_query: " << base::ContainerToString(pk.h_g1_query());
    LOG(ERROR) << "l_g1_query: " << base::ContainerToString(pk.l_g1_query());

    zk::r1cs::ConstraintMatrices<F> constraint_matrices =
        std::move(zkey).TakeConstraintMatrices().ToNative();

    std::unique_ptr<Domain> domain =
        Domain::Create(constraint_matrices.num_constraints +
                       constraint_matrices.num_instance_variables);
    std::vector<F> h_evals = QAP::WitnessMapFromMatrices(
        domain.get(), constraint_matrices, full_assignments);

    zk::r1cs::groth16::Proof<Curve> proof =
        zk::r1cs::groth16::CreateProofWithAssignmentZK(
            pk, absl::MakeConstSpan(h_evals),
            full_assignments.subspan(
                1, constraint_matrices.num_instance_variables - 1),
            full_assignments.subspan(
                constraint_matrices.num_instance_variables),
            full_assignments.subspan(1));
    zk::r1cs::groth16::PreparedVerifyingKey<Curve> pvk =
        pk.verifying_key().ToPreparedVerifyingKey();
    std::vector<F> public_inputs = circuit_->GetPublicInputs();
    ASSERT_TRUE(VerifyProof(pvk, proof, public_inputs));
  }

  std::unique_ptr<R1CS<F>> r1cs_;
  std::unique_ptr<Circuit<F>> circuit_;
};

}  // namespace tachyon::circom

#endif  // VENDORS_CIRCOM_CIRCOMLIB_CIRCUIT_CIRCUIT_TEST_H_
