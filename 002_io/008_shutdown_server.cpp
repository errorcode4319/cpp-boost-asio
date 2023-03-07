#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

void ProcessRequest(asio::ip::tcp::socket& sock) {

    asio::streambuf request_buf;

    system::error_code ec;
    asio::read(sock, request_buf, ec);
    std::cout << "Request received" << std::endl;
    if (ec != asio::error::eof) {
        throw system::system_error(ec);
    }

    const char response_buf[] = { 0x48, 0x69, 0x21};

    asio::write(sock, asio::buffer(response_buf));
    std::cout << "Response send" << std::endl;
    sock.shutdown(asio::socket_base::shutdown_send);
}

int main(){

    uint16_t port = 3333;

    try {
        asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port);

        asio::io_service ios;
        asio::ip::tcp::acceptor acceptor(ios, ep);

        asio::ip::tcp::socket sock(ios);
        acceptor.accept(sock);
        ProcessRequest(sock);
    }
    catch(system::system_error& e) {
        std::cerr << "Error Code: " << e.code()
            << ". Message: " << e.what() << std::endl;
        return e.code().value();
    }
    return 0;
}