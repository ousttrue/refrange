#include <refrange/text/bvh.h>
#include <gtest/gtest.h>


auto src=
"HIERARCHY\n"
"ROOT root_name\n"
"{\n"
"  OFFSET 0 0 0\n"
"  CHANNELS 6 Xposition Yposition Zposition Xrotation Yrotation Zrotation\n"
"  JOINT joint1\n"
"  {\n"
"    OFFSET 0 0 0\n"
"    CHANNELS 3 Xrotation Yrotation Zrotation\n"
"    End Site\n"
"    {\n"
"      OFFSET 0 10 0\n"
"    }\n"
"  }\n"
"  JOINT joint2\n"
"  {\n"
"    OFFSET 0 0 0\n"
"    CHANNELS 3 Xrotation Yrotation Zrotation\n"
"    End Site\n"
"    {\n"
"      OFFSET 0 10 0\n"
"    }\n"
"  }\n"
"}\n"
"MOTION\n"
"Frames: 1\n"
"Frame Time: 0.033\n"
"0 0 0 0 0 0\n"
;

TEST(BvhTest, loader) 
{
    refrange::text::bvh::loader bvh;
    ASSERT_TRUE(bvh.load(refrange::strrange(src)));

    // hierarchy
	refrange::text::bvh::hierarchy hierarchy;

	refrange::text::bvh::joint root(
		"root_name",
		{ 0, 0, 0 }
	);
	hierarchy.value = root;

	// j1
	refrange::text::bvh::joint j1(
		"joint1",
		{ 0, 0, 0 }
	);
	hierarchy.children.push_back({ j1 });

	refrange::text::bvh::joint end_site{
		"",
		{ 0, 10, 0 }
	};
	hierarchy.children.back().children.push_back({ end_site });

	// j2
	refrange::text::bvh::joint j2{
		"joint2",
		{ 0, 0, 0 }
	};
	hierarchy.children.push_back({ j2 });

	/*
	refrange::text::bvh::joint end_site{
		"",
		{ 0, 10, 0 }
	};
	*/

	hierarchy.children.back().children.push_back({ end_site });
	EXPECT_EQ(hierarchy, bvh.get_hierarchy());

    std::vector<refrange::text::bvh::channel_t> root_channels = {
		refrange::text::bvh::channel_Xposition,
		refrange::text::bvh::channel_Yposition,
		refrange::text::bvh::channel_Zposition,
		refrange::text::bvh::channel_Xrotation,
		refrange::text::bvh::channel_Yrotation,
		refrange::text::bvh::channel_Zrotation,
	};
    EXPECT_EQ(root_channels, bvh.get_hierarchy().value.channels);
}


TEST(BvhTest, load_from_file) 
{
    auto path="../../samples/sample.bvh";
    auto buf=refrange::readfile(path);

	refrange::text::bvh::loader bvh;
    EXPECT_TRUE(bvh.load(refrange::vectorrange(buf)));

    refrange::text::bvh::joint root{"Hips", {0, 0, 0}};
    EXPECT_EQ(root, bvh.get_hierarchy().value);
}

