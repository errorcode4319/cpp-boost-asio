#include <iostream>
#include <boost/asio.hpp>

using namespace boost;

int main() {

    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    boost::system::error_code ec;

    asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);

    if (ec.value() != 0) {
        std::cout << "Failed to parse IP address. Error Code = "
        << ec.value() << ". Message: " << ec.message();
        return 1;
    }

    asio::ip::tcp::endpoint ep(ip_address, port_num);
    return 0;
}