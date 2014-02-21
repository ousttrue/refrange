#include <refrange/bvh.h>
#include <gtest/gtest.h>


auto src=
"HIERARCHY"
"ROOT root_name"
"{"
"  OFFSET 0 0 0"
"  CHANNELS 6 Xposition Yposition Zposition Xrotation Yrotation Zrotation"
"  End Site"
"  {"
"    OFFSET 0 10 0"
"  }"
"}"
"MOTION"
"Frames: 1"
"Frame Time: 0.033"
"0 0 0 0 0 0"
;

/*
TEST(BvhTest, loader) 
{
    refrange::bvh::loader bvh;
    EXPECT_TRUE(bvh.load(refrange::strrange(src)));
}
*/
