#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

void WriteToSocket(asio::ip::tcp::socket& sock) {
    std::string buf = "Message";

    size_t buf_offset = 0;

    while(buf_offset != buf.length()) {
        buf_offset += sock.write_some(
            asio::buffer(
                buf.c_str() + buf_offset,
                buf.length() - buf_offset            
            )
        );
    }
}

void WriteToSocketEnhanced(asio::ip::tcp::socket& sock) {
    std::string buf = "Message";
    asio::write(sock, asio::buffer(buf));
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

        WriteToSocketEnhanced(sock);
    }
    catch(system::system_error& e) {
        std::cerr << "Error Code: " << e.code()
            << ". Message: " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}