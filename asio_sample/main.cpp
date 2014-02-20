#include <mpack.h>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>


class rpc_connection
{
    boost::asio::ip::tcp::socket m_socket;
    unsigned char m_read_buffer[1024];
    std::vector<unsigned char> m_unpack_buffer;
    std::shared_ptr<mpack::msgpack::rpc::dispatcher> m_dispatcher;

    int m_request_id;
    struct Message
    {
        std::vector<unsigned char> request;
    };
    std::map<int, Message> m_messageMap;

public:
    rpc_connection(boost::asio::io_service &io_service, 
            std::shared_ptr<mpack::msgpack::rpc::dispatcher> d=0)
        : m_socket(io_service), m_dispatcher(d), m_request_id(0)
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
        auto self=this;
        auto handle_read=[self](const boost::system::error_code& error, 
                size_t bytes_transferred)
        {
            if(error){
                std::cerr << error.message() << std::endl;
                return;
            }

            std::cout << "read: " << bytes_transferred << std::endl;

            std::copy(self->m_read_buffer, self->m_read_buffer+bytes_transferred, std::back_inserter(self->m_unpack_buffer));
            if(!self->m_unpack_buffer.empty()){
                auto unpacker=mpack::msgpack::create_unpacker(&self->m_unpack_buffer[0], self->m_unpack_buffer.size());
                while(true)
                {
                    try {
						mpack::msgpack::byte_range r;
						unpacker >> r;

						// call
						std::vector<unsigned char> response_message;
						auto response_packer=mpack::msgpack::create_external_vector_packer(response_message);
						self->m_dispatcher->dispatch(response_packer, mpack::msgpack::unpacker(r.begin(), r.end()));
                    }
                    catch(std::exception &ex)
                    {
                        std::cerr << ex.what() << std::endl;
                        break;
                    }

                    // send response
                    //begin_write(response_packer.pointer(), response_packer.size());
                }
            }

            // next
            self->begin_read();
        };

        m_socket.async_read_some(boost::asio::buffer(m_read_buffer, 1024), handle_read);
    }

    void begin_write(const unsigned char *p, size_t len)
    {
        auto self=this;
        auto handle_write=[self](const boost::system::error_code& error, 
                size_t bytes_transferred)
        {
            if(error){
                std::cerr << error.message() << std::endl;
                return;
            }

            std::cout << "writed: " << bytes_transferred << std::endl;
        };

		m_socket.async_send(boost::asio::buffer(p, len), handle_write);
    }

    void request(const std::string &method, mpack::msgpack::packer &args_packer)
    {
        auto pair=m_messageMap.insert(std::make_pair(++m_request_id, Message()));
        if(!pair.second){
            // duplicated id !
            throw std::exception(__FUNCTION__);
        }

        auto &message=pair.first->second;
        auto request_packer=mpack::msgpack::create_external_vector_packer(message.request);
        mpack::msgpack::rpc::pack_request(
                request_packer, pair.first->first, method, args_packer);

        begin_write(&message.request[0], message.request.size());
    }
};


static int add(int a, int b)
{
	return a+b;
}


int main(int argc, char **argv)
{
    auto d=std::make_shared<mpack::msgpack::rpc::dispatcher>();

    // regsiter functions
    auto method=mpack::msgpack::rpc::packmethod(add);
    d->register_method("add", method);

    // listen server
    int port=4567;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor(io_service, 
            boost::asio::ip::tcp::endpoint(
			boost::asio::ip::tcp::v4(), port));

    auto connection = std::make_shared<rpc_connection>(io_service, d);
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
    rpc_connection client(io_service);
    client.begin_connect("127.0.0.1", port);

    client.request("add", mpack::msgpack::create_vector_packer() << 1 << 2);

    /*
    //////////////////////////////////////////////////////////////////////
    // call
    std::vector<unsigned char> response_message;
    auto response_packer=mpack::msgpack::create_external_vector_packer(response_message);
    d.dispatch(response_packer, request_packer.pointer(), request_packer.size());
    //////////////////////////////////////////////////////////////////////

    // unpack result
    auto response_unpacker=mpack::msgpack::create_unpacker(
            response_packer.pointer(), response_packer.size());

    // [type(int)=1, msgid(int), error(nil or not), result(any)]
    ASSERT_TRUE(response_unpacker.is_array());

    auto array=mpack::msgpack::array();
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

