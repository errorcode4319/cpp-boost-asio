#include <iostream>
#include <boost/asio.hpp>

using namespace boost;

int main() {

    asio::io_service ios;

    asio::ip::tcp protocol = asio::ip::tcp::v6();

    asio::ip::tcp::acceptor acceptor(ios);

    boost::system::error_code ec;
    acceptor.open(protocol, ec);

    if (ec.value() != 0) {
        std::cout << "Failed to open acceptor socket, Error code = " 
        << ec.value() << ". Message: " << ec.message();
        return 1;
    }

    return 0;
}