#include <boost/asio.hpp>
#include <iostream>
#include <memory>

using namespace boost; 

struct Session {
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::string buf;
    size_t total_bytes_written;
};

void callback(
    const system::error_code& ec, 
    std::size_t bytes_transferred,
    std::shared_ptr<Session> s
) {
    if (ec.value() != 0) {
        std::cerr << "Error Code: " << ec.value() 
            << ". Message: " << ec.message();
        return;
    }

    s->total_bytes_written += bytes_transferred;

    if (s->total_bytes_written == s->buf.length()) {
        return; 
    }
    s->sock->async_write_some(
        asio::buffer(
            s->buf.c_str() + s->total_bytes_written,
            s->buf.length() - s->total_bytes_written
        ),
        std::bind(callback, std::placeholders::_1, std::placeholders::_2, s)
    );
}

void WriteToSocketAsync(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::shared_ptr<Session> s(new Session);

    s->buf = std::string("Message");
    s->total_bytes_written = 0;
    s->sock = sock;

    s->sock->async_write_some(
        asio::buffer(s->buf),
        std::bind(callback, std::placeholders::_1, std::placeholders::_2, s)
    );
}

int main() {
    std::string ip = "127.0.0.1";
    uint16_t port = 3333;

    try {
        asio::ip::tcp::endpoint ep(
            asio::ip::address::from_string(ip), port);

        asio::io_service ios;
        std::shared_ptr<asio::ip::tcp::socket> sock(
            new asio::ip::tcp::socket(ios, ep.protocol()));
        sock->connect(ep);
        WriteToSocketAsync(sock);
    
        ios.run();
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << ". Message: " << e.what() << std::endl;
        return e.code().value(); 
    }

    return 0;
}