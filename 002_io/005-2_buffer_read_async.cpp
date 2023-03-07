#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

struct Session {
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::unique_ptr<char[]> buf;
    size_t total_bytes_read;
    size_t buf_size;  
};

void callback(const system::error_code& ec, size_t byte_transferred, std::shared_ptr<Session> s) {
    if (ec.value() != 0) {
        std::cerr << "Error Code: " << ec.value()
            << ". Message: " << ec.message();
        return;
    }
    s->total_bytes_read += byte_transferred;
    if (s->total_bytes_read == s->buf_size) {
        return;
    }

    s->sock->async_read_some(
        asio::buffer(
            s->buf.get() + s->total_bytes_read,
            s->buf_size - s->total_bytes_read
        ),
        std::bind(callback, std::placeholders::_1, std::placeholders::_2, s)
    );
}

void ReadFromSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::shared_ptr<Session> s(new Session);
    constexpr size_t BUF_SIZE = 32;
    s->sock = sock;
    s->buf.reset(new char[BUF_SIZE]);
    s->total_bytes_read = 0;
    s->buf_size = BUF_SIZE;

    s->sock->async_read_some(
        asio::buffer(
            s->buf.get() + s->total_bytes_read,
            s->buf_size - s->total_bytes_read
        ),
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
        ReadFromSocket(sock);
        ios.run();
    }
    catch(system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << ". Message: " << e.what() << std::endl;
        return e.code().value(); 
    }

    return 0;
}