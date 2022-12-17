#include <iostream>
#include <cstdint>
#include <boost/asio.hpp>

using namespace boost;

int main() {

    const int BACKLOG_SIZE = 30;

    uint16_t port_num = 3333;

    asio::ip::tcp::endpoint ep(
        asio::ip::address_v4::any(), port_num 
    );        

    asio::io_service ios;

    try {

        asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
        acceptor.bind(ep);
        acceptor.listen(BACKLOG_SIZE);
        asio::ip::tcp::socket sock(ios);
        acceptor.accept(sock);

    } 
    catch (const system::system_error& e) {
        std::cout << e.what();
        return e.code().value();
    }

    return 0;
}