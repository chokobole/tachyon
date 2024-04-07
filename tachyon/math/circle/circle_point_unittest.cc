#include "tachyon/math/circle/m31/circle_point.h"

#include "gtest/gtest.h"

namespace tachyon::math {

class CirclePointTest : public testing::Test {
 public:
  static void SetUpTestSuite() { m31::CircleConfig::Init(); }
};

TEST_F(CirclePointTest, Zero) {
  EXPECT_TRUE(m31::CirclePoint::Zero().IsZero());
}

}  // namespace tachyon::math
