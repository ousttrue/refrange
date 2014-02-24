#include <refrange/text/bvh.h>
#include <refrange/binary/vmd.h>
#include <string>
#include <iostream>
#include <sstream>


class loader
{
    std::stringstream m_status;

public:
	std::string status()const{ return m_status.str(); }


    bool load(const char *path)
    {
        auto buf=refrange::readfile(path);
        if(buf.empty()){
            return false;
		}

		{
			refrange::text::bvh::loader l;
			if (l.load(refrange::vectorrange(buf))){
                m_status << "loaded: " << path << std::endl;
				return true;
			}
		}

		{
			refrange::binary::vmd::loader l;
			if (l.load(refrange::vectorrange(buf))){
                m_status << "loaded: " << path << std::endl;
				return true;
			}
		}

        m_status << "fail to load: " << path << std::endl;
        return false;
    }
};


std::ostream &operator<<(std::ostream &os, const loader &l)
{
	os
		<< l.status()
		;
    return os;
}


int main(int argc, char **argv)
{
    if(argc<2){
        return 1;
    }

    loader l;
    l.load(argv[1]);
    std::cout << l << std::endl;

    return 0;
}

