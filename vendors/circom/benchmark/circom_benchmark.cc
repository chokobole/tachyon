#include <stddef.h>
#include <stdint.h>

#include <iostream>

#include "openssl/sha.h"

// clang-format off
#include "benchmark/bit_conversion.h"
// clang-format on
#include "circomlib/circuit/circuit.h"
#include "circomlib/r1cs/r1cs_parser.h"
#include "tachyon/math/elliptic_curves/bn/bn254/bn254.h"
#include "tachyon/math/polynomials/univariate/univariate_evaluation_domain_factory.h"
#include "tachyon/zk/r1cs/groth16/prove.h"
#include "tachyon/zk/r1cs/groth16/verify.h"

namespace tachyon::circom {

using namespace math;

using F = bn254::Fr;
using Curve = math::bn254::BN254Curve;

constexpr size_t MaxDegree = (size_t{1} << 16) - 1;

void CheckPublicInput(const std::vector<uint8_t>& in,
                      const std::vector<F>& public_inputs) {
  SHA256_CTX state;
  SHA256_Init(&state);

  SHA256_Update(&state, in.data(), in.size());
  uint8_t result[SHA256_DIGEST_LENGTH];
  SHA256_Final(result, &state);

  std::vector<uint8_t> uint8_vec = BitToUint8Vector(public_inputs);
  std::vector<uint8_t> result_vec(std::begin(result), std::end(result));
  CHECK(uint8_vec == result_vec);
}

int RealMain(int argc, char** argv) {
  Curve::Init();

  R1CSParser parser;
  std::unique_ptr<R1CS> r1cs = parser.Parse(
      base::FilePath("/home/ryan/Workspace/mainnet/lightscale/tachyon/vendors/"
                     "circom/bazel-bin/examples/sha256_512.r1cs"));
  CHECK(r1cs);

  Circuit<F> circuit(
      r1cs.get(),
      base::FilePath(
          "/home/ryan/Workspace/mainnet/lightscale/tachyon/vendors/circom/"
          "bazel-bin/examples/sha256_512_cpp/sha256_512.dat"));

  std::vector<uint8_t> in = base::CreateVector(
      64, []() { return base::Uniform(base::Range<uint8_t>()); });

  circuit.witness_loader().Set("in", Uint8ToBitVector<F>(in));
  circuit.witness_loader().Load();

  std::vector<F> public_inputs = circuit.GetPublicInputs();
  CheckPublicInput(in, public_inputs);

  base::TimeTicks now = base::TimeTicks::Now();

  zk::r1cs::groth16::ToxicWaste<Curve> toxic_waste =
      zk::r1cs::groth16::ToxicWaste<Curve>::RandomWithoutX();
  zk::r1cs::groth16::ProvingKey<Curve> pk;
  CHECK(pk.Load<MaxDegree>(toxic_waste, circuit));
  zk::r1cs::groth16::Proof<Curve> proof =
      zk::r1cs::groth16::CreateProofWithReductionZK<MaxDegree>(circuit, pk);

  LOG(ERROR) << base::TimeTicks::Now() - now;

  zk::r1cs::groth16::PreparedVerifyingKey<Curve> pvk =
      std::move(pk).TakeVerifyingKey().ToPreparedVerifyingKey();
  CHECK(Verify(pvk, proof, public_inputs));

  std::cout << "success" << std::endl;
  return 0;
}

}  // namespace tachyon::circom

int main(int argc, char** argv) {
  return tachyon::circom::RealMain(argc, argv);
}
