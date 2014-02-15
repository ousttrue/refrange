#include <sstream>
#include <mpack.h>
#include <gtest/gtest.h>


TEST(JsonTest, parse) 
{
    // json to msgpack
    const char *json_object="{\"key\":1}";
    std::istringstream ss(json_object);
	std::vector<unsigned char> buffer;
	mpack::msgpack::vector_packer p(buffer);
    mpack::json::reader_t reader=[&ss](unsigned char *p, size_t len)->size_t{
        ss.read((char*)p, len);
		return ss.gcount();
    };

    mpack::json::parser parser(p, reader);
    ASSERT_TRUE(parser.parse());

    // msgpack to json
    {
        auto u = mpack::msgpack::memory_unpacker(&buffer[0], buffer.size());

        std::string out;
        mpack::json::writer_t writer=[&out](const unsigned char *p, size_t len)->size_t{
            for(size_t i=0; i<len; ++i, ++p){
                out.push_back(*p);
            }
            return len;
        };
        mpack::json::converter converter(writer);
        converter.convert(u);
        EXPECT_EQ(out, json_object);
    }
}

