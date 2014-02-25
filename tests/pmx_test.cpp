#include <refrange/binary/pmx.h>
#include <gtest/gtest.h>


TEST(PmxTest, pmx) 
{
    auto path="../../samples/sample.pmx";
    auto buf=refrange::readfile(path);
    ASSERT_FALSE(buf.empty());
    ASSERT_EQ(726361, buf.size());

    refrange::binary::pmx::loader pmx;
    ASSERT_TRUE(pmx.load(refrange::vectorrange(buf)));

	EXPECT_STREQ(L"初音ミク", pmx.get_name().c_str());

    EXPECT_EQ(12354, pmx.get_vertices().size());
    EXPECT_EQ(22961 * 3, pmx.get_indices().size());
    EXPECT_EQ(17, pmx.get_materials().size());

	EXPECT_EQ(140, pmx.get_bones().size());
	auto &b=pmx.get_bones().front();
	EXPECT_STREQ(L"センター", b.name.c_str());

    EXPECT_EQ(30, pmx.get_morphs().size());
    EXPECT_EQ(45, pmx.get_rigidbodies().size());
    EXPECT_EQ(27, pmx.get_joints().size());

	return;
}
