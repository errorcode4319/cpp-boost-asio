#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

void Communicate(asio::ip::tcp::socket& sock) {
    const char request_buf[] = {0x48, 0x65, 0x0, 0x6c, 0x6c, 0x6f};

    asio::write(sock, asio::buffer(request_buf));
    std::cout << "Request send" << std::endl;

    sock.shutdown(asio::socket_base::shutdown_send);

    asio::streambuf response_buf;
    system::error_code ec;
    asio::read(sock, response_buf, ec);
    std::cout << "Response Received" << std::endl;
    if (ec == asio::error::eof) {
        // Response Data Received
        // Processing...


    }
    else {
        throw system::system_error(ec);
    }
}

int main() {

    std::string ip = "127.0.0.1";
    uint16_t port = 3333;

    try {
        asio::ip::tcp::endpoint ep(
            asio::ip::address::from_string(ip), port);

        asio::io_service ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        Communicate(sock);
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code()
            << ". Message: " << e.what() << std::endl;
        return e.code().value();
    }
    return 0;
}