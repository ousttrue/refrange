#include <gtest/gtest.h>
#include <mpack.h>

// Tests factorial of negative numbers.
TEST(MsgpackTest, Sample) {

    mpack::packer p;
    p << 1;
    //std::cout << p.get_type() << std::endl;

    auto buffer=p.packed_buffer;
    if(buffer.empty()){
        return;
    }

    mpack::reference_unpacker u(&buffer[0], buffer.size());

    int n=-1;
    if(!u.unpack(n)){
        return;    
    }

    EXPECT_EQ(1, n);
}
