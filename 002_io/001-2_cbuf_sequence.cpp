#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

    std::string buf = "Mutable Data";   
    asio::const_buffer cbuf(buf.c_str(), buf.length());
    std::vector<asio::const_buffer> cbuf_seq;
    cbuf_seq.push_back(cbuf);

    // asio::const_buffers_1 cbufs = asio::buffer(buf);
    // Compile Error 
}