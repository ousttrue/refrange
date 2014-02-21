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
}

