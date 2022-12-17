#include <iostream>
#include <cstdint>
#include <boost/asio.hpp>

using namespace boost;

int main() {

    
    std::string raw_ip_address = "127.0.0.1";
    uint16_t port_num = 3333;

    try {

        asio::ip::tcp::endpoint ep (
            asio::ip::address::from_string(raw_ip_address),
            port_num 
        );
        asio::io_service ios;

        asio::ip::tcp::socket sock(ios, ep.protocol());

        sock.connect(ep);

    }
    catch (const system::system_error& e) {
        std::cerr << e.what() << std::endl;
        return e.code().value(); 
    }

    return 0;

}