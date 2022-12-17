#include <iostream>
#include <boost/asio.hpp>

using namespace boost;

int main() {
    std::string host = "www.google.com";
    std::string port_num = "80";

    asio::io_service ios;

    asio::ip::tcp::resolver::query resolver_query(
        host, port_num, asio::ip::tcp::resolver::query::numeric_service
    );

    asio::ip::tcp::resolver resolver(ios);
    boost::system::error_code ec;
    asio::ip::tcp::resolver::iterator it = 
        resolver.resolve(resolver_query, ec);
    
    asio::ip::tcp::resolver::iterator it_end;

    if (ec.value() != 0) {
        std::cout << "Failed to resolve a DNS name." 
            << "Error code: " << ec.value()
            << ". Message: " << ec.message();
        return ec.value();
    }

    for (; it != it_end; it++) {
        asio::ip::tcp::endpoint ep = it->endpoint();
        std::cout << ep << std::endl;
    }

    return 0;

}