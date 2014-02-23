#include <refrange/msgpack/rpc/dispatcher.h>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>


class rpc_connection
{
	std::string m_name;

    boost::asio::ip::tcp::socket m_socket;
    unsigned char m_read_buffer[1024];
    std::vector<unsigned char> m_unpack_buffer;
    std::shared_ptr<refrange::msgpack::rpc::dispatcher> m_dispatcher;

    int m_request_id;
    struct Message
    {
        std::vector<unsigned char> request;

		Message()
		{}
		Message(const unsigned char *p, size_t len)
			: request(p, p+len)
		{}
    };
    std::map<int, Message> m_messageMap;

public:
    rpc_connection(const std::string &name, boost::asio::io_service &io_service, 
            std::shared_ptr<refrange::msgpack::rpc::dispatcher> d=0)
        : m_name(name), m_socket(io_service), m_dispatcher(d), m_request_id(0)
    {
    }

    boost::asio::ip::tcp::socket &socket(){ return m_socket; }

    void begin_connect(const std::string &host, int port)
    {
        auto self=this;
        auto handle_connect=[self](const boost::system::error_code& error)
        {
            if(error){
                std::cerr << error.message() << std::endl;
                return;
            }

            self->begin_read();
        };

        auto endpoint=boost::asio::ip::tcp::endpoint(
			boost::asio::ip::address_v4::from_string(host),
                port);
        m_socket.async_connect(endpoint, handle_connect);
    }

    void begin_read()
    {
        std::cout << m_name << ".begin_read" << std::endl;

        auto self=this;
        auto handle_read=[self](const boost::system::error_code& error, 
                size_t bytes_transferred)
        {
            if(error){
                std::cerr << error.message() << std::endl;
                return;
            }

            std::cout << self->m_name << ".read: " << bytes_transferred << std::endl;

            std::copy(self->m_read_buffer, self->m_read_buffer+bytes_transferred, std::back_inserter(self->m_unpack_buffer));
            if(!self->m_unpack_buffer.empty()){
                auto unpacker=refrange::msgpack::create_unpacker(&self->m_unpack_buffer[0], self->m_unpack_buffer.size());
                while(!unpacker.range().is_end())
                {
                    try {
						auto u=unpacker;

						assert(u.is_array());
						auto c=refrange::msgpack::array();
						u >> c;
						assert(c.size==4);
						
						int type;
						u >> type;

						switch(type)
						{
						case 0:
							// request
							{
								int id;
								u >> id;

								refrange::immutable_range r;
								unpacker >> r;

								// call
								std::vector<unsigned char> response_message;
								auto response_packer=refrange::msgpack::create_external_vector_packer(response_message);
								self->m_dispatcher->dispatch(response_packer, refrange::msgpack::unpacker(r.begin(), r.end()));

								// send response
								self->request(id, response_packer.pointer(), response_packer.size());
							}
							break;

						case 1:
							// response
							{
								int id;
								u >> id;

								refrange::immutable_range r;
								unpacker >> r;

								// fire response event
							}
							break;

						case 2:
							// notification
							throw std::exception("2 not implemented");

						default:
							throw std::invalid_argument(__FUNCTION__);
						}
                    }
                    catch(std::exception &ex)
                    {
                        std::cerr << ex.what() << std::endl;
                        break;
                    }
                }
            }

            // next
            //self->begin_read();
        };

        m_socket.async_read_some(boost::asio::buffer(m_read_buffer, 1024), handle_read);
    }

    void begin_write(const unsigned char *p, size_t len)
    {
		if(!p){
			return;
		}

        std::cout << m_name << ".begin_write" << std::endl;

        auto self=this;
        auto handle_write=[self](const boost::system::error_code& error, 
                size_t bytes_transferred)
        {
            if(error){
                std::cerr << error.message() << std::endl;
                return;
            }

            std::cout << self->m_name << ".writed: " << bytes_transferred << std::endl;
        };

		m_socket.async_send(boost::asio::buffer(p, len), handle_write);
    }

    void request(const std::string &method, refrange::msgpack::packer &args_packer)
    {
		int id=++m_request_id;

		std::vector<unsigned char> buf;
        auto request_packer=refrange::msgpack::create_external_vector_packer(buf);
        refrange::msgpack::rpc::pack_request(request_packer, id, method, args_packer);

		if(buf.empty()){
			return;
		}
		request(id, &buf[0], buf.size());
	}

	void request(int id, const unsigned char *p, size_t len)
	{
        auto pair=m_messageMap.insert(std::make_pair(id, Message(p, len)));
        if(!pair.second){
            // duplicated id !
            throw std::exception(__FUNCTION__);
        }

        auto &message=pair.first->second;
        begin_write(&message.request[0], message.request.size());
    }
};


static int add(int a, int b)
{
	return a+b;
}


int main(int argc, char **argv)
{
    auto d=std::make_shared<refrange::msgpack::rpc::dispatcher>();

    // regsiter functions
    auto method=refrange::msgpack::rpc::packmethod(add);
    d->register_method("add", method);

    // listen server
    int port=4567;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor(io_service, 
            boost::asio::ip::tcp::endpoint(
			boost::asio::ip::tcp::v4(), port));

    auto connection = std::make_shared<rpc_connection>("server", io_service, d);
    auto on_accept=[connection](const boost::system::error_code& error)
    {
        if (error){
			std::cerr << error.message() << std::endl;
            return;
        }
        connection->begin_read();
    };
    // start listen
    acceptor.async_accept(connection->socket(), on_accept);

    // client
    rpc_connection client("client", io_service);
    client.begin_connect("127.0.0.1", port);

    client.request("add", refrange::msgpack::create_vector_packer() << 1 << 2);


    /*
    //////////////////////////////////////////////////////////////////////
    // call
    std::vector<unsigned char> response_message;
    auto response_packer=refrange::msgpack::create_external_vector_packer(response_message);
    d.dispatch(response_packer, request_packer.pointer(), request_packer.size());
    //////////////////////////////////////////////////////////////////////

    // unpack result
    auto response_unpacker=refrange::msgpack::create_unpacker(
            response_packer.pointer(), response_packer.size());

    // [type(int)=1, msgid(int), error(nil or not), result(any)]
    ASSERT_TRUE(response_unpacker.is_array());

    auto array=refrange::msgpack::array();
    response_unpacker >> array;
    ASSERT_EQ(4, array.size);

    int type;
    response_unpacker >> type;
    ASSERT_EQ(1, type);

    int response_id;
    response_unpacker >> response_id;
    ASSERT_EQ(request_id, response_id);

    ASSERT_TRUE(response_unpacker.is_nil());
    response_unpacker.unpack_nil();

    int n;
    response_unpacker >> n;
    EXPECT_EQ(3, n);
    */

	io_service.run();

    return 0;
}

