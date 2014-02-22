#include <refrange/text.h>
#include <gtest/gtest.h>


TEST(TextTest, text) 
{
    EXPECT_TRUE(refrange::text::is_space(" "));
}

