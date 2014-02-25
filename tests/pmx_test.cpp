#include <refrange/binary/pmx.h>
#include <gtest/gtest.h>


TEST(PmxTest, pmx) 
{
    auto path="../../samples/sample.pmx";
    auto buf=refrange::readfile(path);
    ASSERT_FALSE(buf.empty());

    refrange::binary::pmx::loader pmx;
    ASSERT_TRUE(pmx.load(refrange::vectorrange(buf)));

	EXPECT_STREQ(L"èââπÉ~ÉN", pmx.get_name().c_str());

    EXPECT_EQ(12354, pmx.get_vertices().size());

	return;
}

