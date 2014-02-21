#include <refrange/range.h>
#include <gtest/gtest.h>


static bool is_space(const unsigned char *p)
{
    return *p==' ';
}

static bool is_not_space(const unsigned char *p)
{
    return !is_space(p);
}


TEST(RangeTest, bits) 
{
    auto buf="  abc ef ghi  ";

    auto range=refrange::strrange(buf);
    auto found=range.find_range_if(&is_not_space);

    EXPECT_EQ(refrange::immutable_range(
		(const unsigned char*)(buf+2), 
		(const unsigned char*)(buf+5)), 
		found);
}
