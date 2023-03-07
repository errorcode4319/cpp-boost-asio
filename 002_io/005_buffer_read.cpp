#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

std::string ReadFromSocket(asio::ip::tcp::socket& sock) {

    constexpr size_t BUF_SIZE = 32;
    char buf[BUF_SIZE];
    size_t offset = 0;

    while(offset != BUF_SIZE) {
        offset += sock.read_some(
            asio::buffer(buf + offset, BUF_SIZE - offset)
        );
    }

    return std::string(buf, BUF_SIZE);
}

std::string ReadFromSocketEnhanced(asio::ip::tcp::socket& sock) {
    constexpr size_t BUF_SIZE = 32;
    char buf[BUF_SIZE];
    asio::read(sock, asio::buffer(buf, BUF_SIZE));
    return std::string(buf, BUF_SIZE);
}

std::string ReadFromSocketDelim(asio::ip::tcp::socket& sock) {
    asio::streambuf buf;

    // Stream Include Delimiter('\n')
    asio::read_until(sock, buf, '\n');

    // Remove delimeter 
    std::string msg;
    std::istream input_stream(&buf);
    std::getline(input_stream, msg);

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

        //ReadFromSocket(sock);
        //ReadFromSocketEnhanced(sock);
        ReadFromSocketDelim(sock);
    }
    catch (system::system_error& e) {
        std::cerr<< "Error Code: " << e.code()
            << ". Message: " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}