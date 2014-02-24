#include <refrange/text/vpd.h>
#include <gtest/gtest.h>


TEST(VpdTest, loader) 
{
    auto path="../../samples/sample.vpd";
    auto buf=refrange::readfile(path);
    refrange::text::vpd::loader vpd;
    //ASSERT_TRUE(vpd.load(refrange::vectorrange(buf)));
}

