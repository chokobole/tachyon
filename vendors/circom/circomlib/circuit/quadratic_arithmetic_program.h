// Copyright 2022 arkworks contributors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.arkworks and the LICENCE-APACHE.arkworks
// file.

#ifndef VENDORS_CIRCOM_CIRCOMLIB_CIRCUIT_QUADRATIC_ARITHMETIC_PROGRAM_H_
#define VENDORS_CIRCOM_CIRCOMLIB_CIRCUIT_QUADRATIC_ARITHMETIC_PROGRAM_H_

#include <stddef.h>

#include <optional>
#include <utility>
#include <vector>

#include "tachyon/zk/r1cs/constraint_system/constraint_system.h"
#include "tachyon/zk/r1cs/constraint_system/qap_instance_map_result.h"
#include "tachyon/zk/r1cs/constraint_system/qap_witness_map_result.h"
#include "tachyon/zk/r1cs/constraint_system/quadratic_arithmetic_program_forward.h"

namespace tachyon::circom {

template <typename Circuit>
class QuadraticArithmeticProgram<Circuit,
                                 std::enable_if_t<Circuit::kIsCircom>> {
 public:
  using F = typename Circuit::Field;

  QuadraticArithmeticProgram() = delete;

  // This implementation is same as
  // tachyon/zk/r1cs/constraint_system/quadratic_arithmetic_program.h.
  template <typename Domain>
  static zk::r1cs::QAPInstanceMapResult<F> InstanceMap(
      const Domain* domain, const zk::r1cs::ConstraintSystem<F>& cs,
      const F& x) {
    std::optional<zk::r1cs::ConstraintMatrices<F>> matrices = cs.ToMatrices();
    // |num_constraint| = n
    size_t num_constraints = cs.num_constraints();
    // |num_instance_variables| = l + 1
    size_t num_instance_variables = cs.num_instance_variables();
    // |num_witness_variables| = m - l
    size_t num_witness_variables = cs.num_witness_variables();

    // t(x) = xⁿ⁺ˡ⁺¹ - 1
    F t_x = domain->EvaluateVanishingPolynomial(x);

    std::vector<F> l = domain->EvaluateAllLagrangeCoefficients(x);

    // |num_qap_variables| = m = (l + 1 - 1) + m - l
    size_t num_qap_variables =
        (num_instance_variables - 1) + num_witness_variables;

    std::vector<F> a(num_qap_variables + 1);
    std::vector<F> b(num_qap_variables + 1);
    std::vector<F> c(num_qap_variables + 1);

    // clang-format off
    // |a[i]| = lₙ₊ᵢ(x) +  Σⱼ₌₀..ₙ₋₁ (lⱼ(x) * Aⱼ,ᵢ) (if i < |num_instance_variables|)
    //        = Σⱼ₌₀..ₙ₋₁ (lⱼ(x) * Aⱼ,ᵢ)            (otherwise)
    // |b[i]| = Σⱼ₌₀..ₙ₋₁ (lⱼ(x) * Bⱼ,ᵢ)
    // |c[i]| = Σⱼ₌₀..ₙ₋₁ (lⱼ(x) * Cⱼ,ᵢ)
    // clang-format on
    for (size_t i = 0; i < num_instance_variables; ++i) {
      a[i] = l[num_constraints + i];
    }

    for (size_t i = 0; i < num_constraints; ++i) {
      for (const zk::r1cs::Cell<F>& cell : matrices->a[i]) {
        a[cell.index] += (l[i] * cell.coefficient);
      }
      for (const zk::r1cs::Cell<F>& cell : matrices->b[i]) {
        b[cell.index] += (l[i] * cell.coefficient);
      }
      for (const zk::r1cs::Cell<F>& cell : matrices->c[i]) {
        c[cell.index] += (l[i] * cell.coefficient);
      }
    }

    return {std::move(a),          std::move(b),     std::move(c),
            std::move(t_x),        num_constraints,  num_instance_variables,
            num_witness_variables, num_qap_variables};
  }

  template <typename Domain>
  static std::vector<F> WitnessMapFromMatrices(
      const Domain* domain, const zk::r1cs::ConstraintMatrices<F>& matrices,
      size_t num_instance_variables, size_t num_constraints,
      absl::Span<const F> full_assignments) {
    using Evals = typename Domain::Evals;
    using DensePoly = typename Domain::DensePoly;

    std::vector<F> a(domain->size());
    std::vector<F> b(domain->size());
    std::vector<F> c(domain->size());

    OPENMP_PARALLEL_FOR(size_t i = 0; i < num_constraints; ++i) {
      a[i] = EvaluateConstraint(matrices.a[i], full_assignments);
      b[i] = EvaluateConstraint(matrices.b[i], full_assignments);
      c[i] *= a[i] * b[i];
    }

    for (size_t i = num_constraints;
         i < num_constraints + num_instance_variables; ++i) {
      a[i] = full_assignments[i - num_constraints];
    }

    Evals a_evals(std::move(a));
    DensePoly a_poly = domain->IFFT(std::move(a_evals));
    Evals b_evals(std::move(b));
    DensePoly b_poly = domain->IFFT(std::move(b_evals));
    Evals c_evals(std::move(c));
    DensePoly c_poly = domain->IFFT(std::move(c_evals));

    F root_of_unity;
    {
      std::unique_ptr<Domain> extended_domain =
          Domain::Create(2 * domain->size());
      root_of_unity = extended_domain->GetElement(1);
    }
    Domain::DistributePowers(a_poly, root_of_unity);
    Domain::DistributePowers(b_poly, root_of_unity);
    Domain::DistributePowers(c_poly, root_of_unity);

    a_evals = coset_domain->FFT(std::move(a_poly));
    b_evals = coset_domain->FFT(std::move(b_poly));
    c_evals = coset_domain->FFT(std::move(c_poly));

    OPENMP_PARALLEL_FOR(size_t i = 0; i < domain->size(); ++i) {
      F& h_evals_i = a_evals.at(i);
      h_evals_i *= b_evals[i];
      h_evals_i -= c_evals[i];
    }

    return a_evals.TakeEvaluations();
  }

  template <typename Domain>
  static std::vector<F> ComputeHQuery(const Domain* domain, const F& t_x,
                                      const F& x, const F& delta_inverse) {
    using Evals = typename Domain::Evals;
    using DensePoly = typename Domain::DensePoly;

    // The usual H query has domain - 1 powers. Z has domain powers. So HZ has
    // 2 * domain - 1 powers.
    std::unique_ptr<Domain> extended_domain =
        Domain::Create(domain->size() * 2 + 1);
    Evals evals(
        F::GetSuccessivePowers(extended_domain->size(), t_x, delta_inverse));
    DensePoly poly = extended_domain->IFFT(std::move(evals));
    std::vector<F> ret(domain->size());
    OPENMP_PARALLEL_FOR(size_t i = 0; i < domain->size(); ++i) {
      ret[i] = poly[2 * i + 1];
    }
    return ret;
  }
};

}  // namespace tachyon::circom

#endif  // VENDORS_CIRCOM_CIRCOMLIB_CIRCUIT_QUADRATIC_ARITHMETIC_PROGRAM_H_
