#include <refrange/binary/pmx.h>
#include <gtest/gtest.h>


TEST(PmdTest, loader) 
{
    auto path="../../samples/sample.pmd";
    auto buf=refrange::readfile(path);
    refrange::binary::pmx::loader pmx;
    ASSERT_TRUE(pmx.load(refrange::vectorrange(buf)));

	return;
}

