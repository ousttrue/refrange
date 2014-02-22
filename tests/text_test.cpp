#include <refrange/range.h>
#include <gtest/gtest.h>


TEST(TextTest, text) 
{
    EXPECT_TRUE(refrange::text::is_space(" "));

    EXPECT_EQ(1, refrange::strrange("1").to_int());
    EXPECT_EQ(-1, refrange::strrange("-1").to_int());

    EXPECT_EQ(1.5, refrange::strrange("1.5").to_double());
}

