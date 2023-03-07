#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

    const std::string buf = "Hello";
    
    asio::const_buffers_1 output_buf = asio::buffer(buf);

    return 0; 
}