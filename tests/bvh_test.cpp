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
#if _MSC_VER >= 1800
    // vc2013
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
#else
    std::vector<refrange::text::bvh::joint> joints;
    {
        joints.push_back(refrange::text::bvh::joint());
        auto &joint=joints.back();
        joint.name="root_name";
        joint.offset.x=0;
        joint.offset.y=0;
        joint.offset.z=0;
        joint.channels.push_back(refrange::text::bvh::channel_Xposition);
        joint.channels.push_back(refrange::text::bvh::channel_Yposition);
        joint.channels.push_back(refrange::text::bvh::channel_Zposition);
        joint.channels.push_back(refrange::text::bvh::channel_Xrotation);
        joint.channels.push_back(refrange::text::bvh::channel_Yrotation);
        joint.channels.push_back(refrange::text::bvh::channel_Zrotation);
    }
#endif

	refrange::text::bvh::hierarchy hierarchy;
    hierarchy.value=0;

	// j1
	hierarchy.children.push_back(refrange::text::bvh::hierarchy());
    hierarchy.children.back().value=1;
	// j2
	hierarchy.children.push_back(refrange::text::bvh::hierarchy());
    hierarchy.children.back().value=2;

	EXPECT_EQ(hierarchy, bvh.get_hierarchy());

    EXPECT_EQ(joints, bvh.get_joints());

	EXPECT_EQ(bvh.get_channel_count(), bvh.get_frames().front().values.size());
}


TEST(BvhTest, load_from_file) 
{
    auto path="../../../refrangetest_samples/sample.bvh";
	auto buf = refrange::readfile(path);
	ASSERT_FALSE(buf.empty());

	refrange::text::bvh::loader bvh;
    ASSERT_TRUE(bvh.load(refrange::vectorrange(buf)));

#if _MSC_VER >= 1800
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
#else
    refrange::text::bvh::joint root;
    {
        root.name="root_name";
        root.offset.x=0;
        root.offset.y=0;
        root.offset.z=0;
        root.channels.push_back(refrange::text::bvh::channel_Xposition);
        root.channels.push_back(refrange::text::bvh::channel_Yposition);
        root.channels.push_back(refrange::text::bvh::channel_Zposition);
        root.channels.push_back(refrange::text::bvh::channel_Zrotation);
        root.channels.push_back(refrange::text::bvh::channel_Yrotation);
        root.channels.push_back(refrange::text::bvh::channel_Xrotation);
    }
#endif

    EXPECT_EQ(root, bvh.get_joints().front());
	EXPECT_EQ(bvh.get_channel_count(), bvh.get_frames().front().values.size());
}

