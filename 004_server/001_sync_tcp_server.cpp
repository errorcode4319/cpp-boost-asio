#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <memory>

using namespace boost; 

class Service {

public:
    Service() {}

    void HandleClient(asio::ip::tcp::socket& sock) {
        try {
            asio::streambuf request_buf;
            asio::read_until(sock, request_buf, '\n');
            std::istream in(&request_buf);
            std::string request;
            std::getline(in, request); 

            std::cout << "Request: " << request << std::endl;
            for(volatile int  i=0;i < 1000000;i++); // Dummy 

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            std::string response = "Response\n";
            asio::write(sock, asio::buffer(response));
        }
        catch (system::system_error& e) {
            std::cerr << "Error Code: " << e.code() << ". Message: " << e.what() << std::endl;
        }
    }
};

class Acceptor {

public:
    Acceptor(asio::io_service& ios, uint16_t port)
    : m_ios(ios), m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port)) {
        m_acceptor.listen(); 
    }

    void Accept() {
        asio::ip::tcp::socket sock(m_ios);
        m_acceptor.accept(sock);
        auto service = Service();
        service.HandleClient(sock);
    }

private:
    asio::io_service&       m_ios;
    asio::ip::tcp::acceptor m_acceptor;
};

class Server {

public:
    Server() : m_stop(false) {}

    void Start(uint16_t port) {
        m_thread.reset(new std::thread([this, port]() {
            Run(port);
        }));
    }

    void Stop() {
        m_stop.store(true);
        m_thread->join(); 
    }


private:
    void Run(uint16_t port) {
        Acceptor acceptor(m_ios, port);
        while(!m_stop.load()) {
            acceptor.Accept(); 
        }
    }

private:
    std::unique_ptr<std::thread>    m_thread;
    std::atomic<bool>   m_stop;
    asio::io_service    m_ios;

};


int main() {

    uint16_t port = 3333;

    try {
        Server server;
        server.Start(port);

        std::this_thread::sleep_for(std::chrono::seconds(60));
        server.Stop();
    }
    catch (system::system_error &e) {
        std::cerr << "Error Code: " << e.code()
            << ". Message: " << e.what() << std::endl;
    }

    return 0;
}