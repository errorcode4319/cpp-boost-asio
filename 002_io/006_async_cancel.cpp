#include <boost/predef.h>

#ifdef BOOST_OS_WINDOWS
#define _WIN32_WINNT 0x0501 

#if _WIN32_WINNT <= 0x0502  // <= Win Server 2003
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO
#endif 
#endif 

#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using namespace boost;

int main() {
    std::string ip = "127.0.0.1";
    uint16_t port = 3333;

    try {
        asio::ip::tcp::endpoint ep(
            asio::ip::address::from_string(ip), port
        );

        asio::io_service ios;

        std::shared_ptr<asio::ip::tcp::socket> sock(
            new asio::ip::tcp::socket(ios, ep.protocol())
        );

        sock->async_connect(ep,
            [sock](const system::error_code& ec) {
                if (ec.value() != 0) {
                    if (ec == asio::error::operation_aborted) {
                        std::cout << "Operation Cancelled!" << std::endl;
                    }
                    else {
                        std::cout << "Error Code: " << ec.value() 
                            << ". Message: " << ec.message();
                    }
                    return;
                }
            }
        );
        std::thread worker_thread([&ios](){
            try {
                ios.run();
            }
            catch(system::system_error& e) {
                std::cout << "Error Code: " << e.code() 
                    << ". Message: " << e.what();
            }
        });

        std::this_thread::sleep_for(std::chrono::seconds(2));

        sock->cancel();
        worker_thread.join();
    }
    catch(system::system_error& e) {
        std::cout << "Error Code: " << e.code() 
            << ". Message: " << e.what();
    }

    return 0;
}