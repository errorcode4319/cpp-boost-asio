#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {

    asio::streambuf sbuf;

    std::ostream out(&sbuf);

    out << "Message\nBuffer";

    std::istream in(&sbuf);

    std::string msg;
    std::getline(in, msg);

    std::cout << msg << std::endl; // Message

}