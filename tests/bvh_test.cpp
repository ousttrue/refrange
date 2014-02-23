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
"0 0 0 0 0 0 0 0 0 0 0 0\n"
;

TEST(BvhTest, loader) 
{
    refrange::text::bvh::loader bvh;
    ASSERT_TRUE(bvh.load(refrange::strrange(src)));

    // hierarchy
	std::vector<refrange::text::bvh::joint> joints{
		{
			"root_name"
			, { 0, 0, 0 }
			, {
				refrange::text::bvh::channel_Xposition
				, refrange::text::bvh::channel_Yposition
				, refrange::text::bvh::channel_Zposition
				, refrange::text::bvh::channel_Xrotation
				, refrange::text::bvh::channel_Yrotation
				, refrange::text::bvh::channel_Zrotation
			}
		}
		, { 
			"joint1"
			, { 0, 0, 0 }
			, {
				refrange::text::bvh::channel_Xrotation
				, refrange::text::bvh::channel_Yrotation
				, refrange::text::bvh::channel_Zrotation
			}
			, {
				{ 0, 10, 0 }
			}
		}
		, {
			"joint2"
			, { 0, 0, 0 }
			, {
				refrange::text::bvh::channel_Xrotation
				, refrange::text::bvh::channel_Yrotation
				, refrange::text::bvh::channel_Zrotation
			}
            , {
                { 0, 10, 0 }
            }
		}
	};

	refrange::text::bvh::hierarchy hierarchy{ 0 };

	// j1
	hierarchy.children.push_back({1});
	// j2
	hierarchy.children.push_back({2});

	EXPECT_EQ(hierarchy, bvh.get_hierarchy());

    EXPECT_EQ(joints, bvh.get_joints());

	EXPECT_EQ(bvh.get_channel_count(), bvh.get_frames().front().values.size());
}


TEST(BvhTest, load_from_file) 
{
    //auto path="../../samples/sample.bvh";
	auto path = "../../samples/kinect_sample.bvh";
	auto buf = refrange::readfile(path);

	refrange::text::bvh::loader bvh;
    EXPECT_TRUE(bvh.load(refrange::vectorrange(buf)));

    refrange::text::bvh::joint root{
		"Hips"
		, {0, 0, 0}
		, {
			refrange::text::bvh::channel_Xposition
			, refrange::text::bvh::channel_Yposition
			, refrange::text::bvh::channel_Zposition
			, refrange::text::bvh::channel_Zrotation
			, refrange::text::bvh::channel_Yrotation
			, refrange::text::bvh::channel_Xrotation
		}
	};
    EXPECT_EQ(root, bvh.get_joints().front());

	EXPECT_EQ(bvh.get_channel_count(), bvh.get_frames().front().values.size());
}

