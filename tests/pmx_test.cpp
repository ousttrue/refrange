#include <refrange/binary/pmx.h>
#include <refrange/binary/pmd.h>
#include <gtest/gtest.h>


TEST(PmxTest, pmx) 
{
    auto path="../../../refrangetest_samples/miku_v2.pmx";
    auto buf=refrange::readfile(path);
    ASSERT_FALSE(buf.empty());
    //ASSERT_EQ(726361, buf.size());

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

TEST(PmdTest, pmd) 
{
    auto path="../../../refrangetest_samples/miku_v2.pmd";
    auto buf=refrange::readfile(path);
    ASSERT_FALSE(buf.empty());
    //ASSERT_EQ(726361, buf.size());

    refrange::binary::pmd::loader pmd;
    ASSERT_TRUE(pmd.load(refrange::vectorrange(buf)));

	EXPECT_STREQ("初音ミク", pmd.get_name().c_str());

    EXPECT_EQ(12354, pmd.get_vertices().size());
    EXPECT_EQ(22961 * 3, pmd.get_indices().size());
    EXPECT_EQ(17, pmd.get_materials().size());

	EXPECT_EQ(140, pmd.get_bones().size());
	//auto &b=pmd.get_bones().front();
	//EXPECT_STREQ("センター", b.name.c_str());

    EXPECT_EQ(30, pmd.get_morphs().size());
    EXPECT_EQ(45, pmd.get_rigidbodies().size());
    EXPECT_EQ(27, pmd.get_joints().size());

	return;
}

