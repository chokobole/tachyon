#include "tachyon/math/elliptic_curves/affine_point.h"

#include "gtest/gtest.h"

#include "tachyon/base/buffer/vector_buffer.h"
#include "tachyon/math/elliptic_curves/short_weierstrass/test/sw_curve_config.h"

namespace tachyon::math {

namespace {

class AffinePointTest : public testing::Test {
 public:
  static void SetUpTestSuite() { test::AffinePoint::Curve::Init(); }
};

}  // namespace

TEST_F(AffinePointTest, Copyable) {
  test::AffinePoint expected = test::AffinePoint::Random();
  test::AffinePoint value;

  base::VectorBuffer write_buf;
  write_buf.Write(expected);

  write_buf.set_buffer_offset(0);
  write_buf.Read(&value);

  EXPECT_EQ(expected, value);
}

}  // namespace tachyon::math
