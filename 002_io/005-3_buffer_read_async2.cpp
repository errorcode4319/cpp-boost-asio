#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

struct Session {
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::unique_ptr<char[]> buf;
    size_t buf_size;  
};

void callback(const system::error_code& ec, size_t byte_transferred, std::shared_ptr<Session> s) {
    if (ec.value() != 0) {
        std::cerr << "Error Code: " << ec.value()
            << ". Message: " << ec.message();
        return;
    }
}

void ReadFromSocketEnhanced(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::shared_ptr<Session> s(new Session);
    constexpr size_t BUF_SIZE = 32;
    s->sock = sock;
    s->buf.reset(new char[BUF_SIZE]);
    s->buf_size = BUF_SIZE;
    asio::async_read(
        *s->sock,
        asio::buffer(static_cast<void*>(s->buf.get()), s->buf_size),
        std::bind(callback, std::placeholders::_1, std::placeholders::_2, s)
    );
}

int main() {

    std::string ip = "127.0.0.1";
    uint16_t port = 3333;

    try {
        asio::ip::tcp::endpoint ep(
            asio::ip::address::from_string(ip), port 
        );

        asio::io_service ios;
        std::shared_ptr<asio::ip::tcp::socket> sock(
            new asio::ip::tcp::socket(ios, ep.protocol())
        );
        sock->connect(ep);
        ReadFromSocketEnhanced(sock);
        ios.run();
    }
    catch(system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << ". Message: " << e.what() << std::endl;
        return e.code().value(); 
    }

    return 0;
}