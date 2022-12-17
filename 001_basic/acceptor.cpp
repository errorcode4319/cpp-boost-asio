#include <iostream>
#include <cstdint>
#include <boost/asio.hpp>

using namespace boost;

int main() {

    uint16_t port_num = 3333;
    asio::ip::tcp::endpoint ep(
        asio::ip::address_v4::any(), port_num 
    );

    asio::io_service ios;
    asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

    boost::system::error_code ec;

    acceptor.bind(ep, ec);

    if (ec.value() != 0) {
        std::cout << "Failed to bind acceptor socket, Error code = " 
        << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }



    return 0;
}