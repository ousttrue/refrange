#pragma once
#include <functional>
#include <sstream>


namespace mpack {
namespace json {

    typedef std::function<size_t(unsigned char *, size_t)> reader_t;

    class parser
    {
        reader_t &m_reader;
        int m_peek_char;

    public:
        parser(reader_t &reader)
            : m_reader(reader), m_peek_char(-1)
        {}

        bool parse(::mpack::msgpack::packer &packer, bool is_key=false)
        {
            switch(peek_char(true))
            {
                case '{': return parse_object(packer);
                case '[': return parse_array(packer);
				case '"': return parse_quated_string(packer, '"');
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					return parse_number(packer);
				default: throw std::invalid_argument(__FUNCTION__);
            }
        }

    private:
        bool is_skip(char c)
        {
            switch(c)
            {
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    return true;
            }

            return false;
        }

        char get_char(bool skip=false)
        {
            if(!skip){
                if(m_peek_char!=-1){
                    char c=m_peek_char;
                    m_peek_char=-1;
                    return c;
                }

                char c;
                size_t size=m_reader((unsigned char*)&c, 1);
                if(size==0){
                    return 0;
                }
                return c;
            }
            else{
                if(m_peek_char!=-1 && !is_skip(m_peek_char)){
                    char c=m_peek_char;
                    m_peek_char=-1;
                    return c;
                }

                m_peek_char=-1;

                while(true){
                    char c;
                    size_t size=m_reader((unsigned char*)&c, 1);
                    if(size==0){
                        throw std::exception("EOF");
                    }
                    if(!is_skip(c)){
                        return c;
                    }
                }
            }
        }

        char peek_char(bool skip=false)
        {
            if(!skip){
                if(m_peek_char==-1){
                    m_peek_char=get_char();
                }

                return m_peek_char;
            }

            if(m_peek_char!=-1 && !is_skip(m_peek_char)){
                return m_peek_char;
            }

            m_peek_char=get_char(true);
            return m_peek_char;
        }

        bool parse_object(::mpack::msgpack::packer &packer)
        {
            // nest packer
			auto nested = ::mpack::msgpack::create_vector_packer();

            // drop open brace
			assert(m_peek_char == '{');
            get_char();

            // search key
            for(int i=0; true; ++i){
                char c=peek_char(true);
                if(c=='}'){
                    // close
                    break;
                }
                if(i){
                    if(c!=','){
                        return false;
                    }
                    // drop
                    get_char();
                }

                if(!parse(nested, true)){
                    return false;
                }

				if (get_char(true) != ':'){
                    return false;
                }
                if(!parse(nested)){
                    return false;
                }
            }

            packer << ::mpack::msgpack::map(nested);

            return true;
        }

        bool parse_array(::mpack::msgpack::packer &packer)
        {
            return false;
        }

		bool is_digit(char c)
        {
            switch (c)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    return true;
            }

            return false;
        }

		bool is_non_numeric(char c)
		{
			if (is_skip(c)){
				return true;
			}
			if (is_digit(c)){
				return false;
			}

			switch (c)
			{
			case ',':
			case ':':
			case '}':
			case ']':
				return true;
			}

			return false;
		}

		bool parse_number(::mpack::msgpack::packer &packer)
		{
			std::stringstream ss;
			for (int i = 0; true; ++i)
			{
				char c = peek_char();
				if (is_non_numeric(c)){
					break;
				}
				get_char();
				ss << c;
			}

			packer << atoi(ss.str().c_str());

			return true;
		}

		bool parse_quated_string(::mpack::msgpack::packer &packer, char quote)
		{
			// drop first quote
			assert(m_peek_char == quote);
			get_char();

			std::stringstream ss;
			while (true)
			{
				char c=get_char();
				if (c == quote){
					break;
				}
				ss << c;
			}

			packer << ss.str();

			return true;
		}

    };


    typedef std::function<size_t(const unsigned char *, size_t)> writer_t;

    class converter
    {
        writer_t m_writer;

    public:
        converter(writer_t writer)
            : m_writer(writer)
        {
        }

        void convert(::mpack::msgpack::unpacker &u)
        {
            if(u.is_array()){
                write("[");
            }
            else if(u.is_map()){
                write("{");
                auto c=::mpack::msgpack::map();
                u >> c;
                for(size_t i=0; i<c.size; ++i){
                    std::string key;
                    u >> key;
                    write("\"");
                    write(key);
                    write("\"");
                    write(":");

                    if(u.is_integer()){
                        int n;
                        u >> n;
                        std::stringstream ss;
                        ss << n;
                        write(ss.str());
                    }
                    /*
                    else if(u.is_float()){
                    }
                    else if(u.is_string()){
                    }
                    else if(u.is_bool()){
                    }
                    */
                    else{
                        // todo
                        throw std::invalid_argument(__FUNCTION__);
                    }
                }
                write("}");
            }
            else{
            }
        }

        void write(const std::string  &s)
        {
            m_writer((const unsigned char*)s.c_str(), s.size());
        }
    };

} // namespace
} // namespace
