#include <refrange/bvh.h>
#include <gtest/gtest.h>


auto src=
"HIERARCHY\n"
"ROOT root_name\n"
"{\n"
"  OFFSET 0 0 0\n"
"  CHANNELS 6 Xposition Yposition Zposition Xrotation Yrotation Zrotation\n"
"  End Site\n"
"  {\n"
"    OFFSET 0 10 0\n"
"  }\n"
"}\n"
"MOTION\n"
"Frames: 1\n"
"Frame Time: 0.033\n"
"0 0 0 0 0 0\n"
;

TEST(BvhTest, loader) 
{
    refrange::bvh::loader bvh;
    EXPECT_TRUE(bvh.load(refrange::strrange(src)));

    // hierarchy
	refrange::bvh::hierarchy hierarchy;

	refrange::bvh::joint root{
		"root_name",
		{ 0, 0, 0 }
	};
	hierarchy.value = root;

	refrange::bvh::joint end_site{
		"Site",
		{ 0, 10, 0 }
	};
	hierarchy.children.push_back({end_site});

    EXPECT_EQ(hierarchy, bvh.get_hierarchy());
}

