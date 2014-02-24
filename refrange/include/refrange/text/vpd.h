#pragma once
#include "text_reader.h"


namespace refrange {
namespace text {
namespace vpd {

class loader
{
public:
    bool load(const immutable_range &r)
    {
        line_reader reader(r);

        if(reader.get_line()!="Vocaloid Pose Data file"){
            return false;
		}

		{
			auto line = reader.get_line(); //miku.osm;		// 親ファイル名
			int a = 0;
		}

		{
			auto line = reader.get_line();
			int bonenum = line.to_int();
		}

        return false;
    }
};

}  // namespace
}  // namespace
}  // namespace

