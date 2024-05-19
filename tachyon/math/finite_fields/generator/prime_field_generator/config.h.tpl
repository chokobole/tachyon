// clang-format off
#include "tachyon/export.h"
#include "tachyon/build/build_config.h"
%{if !kIsSmallField}
#include "tachyon/math/base/big_int.h"
%{endif !kIsSmallField}

namespace %{namespace} {

class TACHYON_EXPORT %{class}Config {
 public:
  constexpr static const char* kName = "%{namespace}::%{class}";

  constexpr static bool kUseMontgomery = %{use_montgomery};
%{if kIsSmallField}
  constexpr static bool kIsSpecialPrime = false;
%{endif kIsSmallField}
%{if !kIsSmallField}
%{if kUseAsm}
#if ARCH_CPU_X86_64
  constexpr static bool kIsSpecialPrime = true;
  constexpr static bool %{flag} = true;
#else
%{endif kUseAsm}
  constexpr static bool kIsSpecialPrime = false;
%{if kUseAsm}
#endif
%{endif kUseAsm}
%{endif !kIsSmallField}

  constexpr static size_t kModulusBits = %{modulus_bits};
  constexpr static %{modulus_type} kModulus = %{modulus_type}({
    %{modulus}
  });
  constexpr static %{value_type} kModulusMinusOneDivTwo = %{value_type}({
    %{modulus_minus_one_div_two}
  });
  constexpr static %{value_type} kModulusPlusOneDivFour = %{value_type}({
    %{modulus_plus_one_div_four}
  });
  constexpr static %{value_type} kTrace = %{value_type}({
    %{trace}
  });
  constexpr static %{value_type} kTraceMinusOneDivTwo = %{value_type}({
    %{trace_minus_one_div_two}
  });
  constexpr static bool kModulusModFourIsThree = %{modulus_mod_four_is_three};
  constexpr static bool kModulusModSixIsOne = %{modulus_mod_six_is_one};
  constexpr static bool kModulusHasSpareBit = %{modulus_has_spare_bit};
%{if !kIsSmallField}
  constexpr static bool kCanUseNoCarryMulOptimization = %{can_use_no_carry_mul_optimization};
  constexpr static %{value_type} kMontgomeryR = %{value_type}({
    %{r}
  });
  constexpr static %{value_type} kMontgomeryR2 = %{value_type}({
    %{r2}
  });
  constexpr static %{value_type} kMontgomeryR3 = %{value_type}({
    %{r3}
  });
  constexpr static uint64_t kInverse64 = UINT64_C(%{inverse64});
%{endif !kIsSmallField}
  constexpr static uint32_t kInverse32 = %{inverse32};

  constexpr static %{value_type} kOne = %{value_type}({
    %{one}
  });

  constexpr static bool kHasTwoAdicRootOfUnity = %{has_two_adic_root_of_unity};

  constexpr static bool kHasLargeSubgroupRootOfUnity = %{has_large_subgroup_root_of_unity};

%{if kHasTwoAdicRootOfUnity}
  constexpr static %{value_type} kSubgroupGenerator = %{value_type}({
    %{subgroup_generator}
  });
  constexpr static uint32_t kTwoAdicity = %{two_adicity};
  constexpr static %{value_type} kTwoAdicRootOfUnity = %{value_type}({
    %{two_adic_root_of_unity}
  });


%{if kHasLargeSubgroupRootOfUnity}
  constexpr static uint32_t kSmallSubgroupBase = %{small_subgroup_base};
  constexpr static uint32_t kSmallSubgroupAdicity = %{small_subgroup_adicity};
  constexpr static %{value_type} kLargeSubgroupRootOfUnity = %{value_type}({
    %{large_subgroup_root_of_unity}
  });
%{endif kHasLargeSubgroupRootOfUnity}
%{endif kHasTwoAdicRootOfUnity}

%{if kIsSmallField}
  constexpr static uint32_t AddMod(uint32_t a, uint32_t b) {
    // NOTE(chokobole): This assumes that the 2m - 2 < 2³², where m is modulus.
    return Reduce(a + b);
  }

  constexpr static uint32_t SubMod(uint32_t a, uint32_t b) {
    // NOTE(chokobole): This assumes that the 2m - 2 < 2³², where m is modulus.
    return Reduce(a + static_cast<uint32_t>(kModulus[0]) - b);
  }

  constexpr static uint32_t Reduce(uint32_t v) {
    %{reduce32}
  }

  constexpr static uint32_t Reduce(uint64_t v) {
    %{reduce64}
  }

  constexpr static uint32_t ToMontgomery(uint32_t v) {
    return (uint64_t{v} << 32) % static_cast<uint32_t>(kModulus[0]);
  }

  constexpr static uint32_t FromMontgomery(uint64_t v) {
    constexpr uint64_t kMask = (uint64_t{1} << 32) - 1;
    uint64_t t = (v * kInverse32) & kMask;
    uint64_t u = t * kModulus[0];
    uint32_t ret = (v - u) >> 32;
    if (v < u) {
      return ret + static_cast<uint32_t>(kModulus[0]);
    } else {
      return ret;
    }
  }
%{endif kIsSmallField}
};

}  // namespace %{namespace}
// clang-format on
