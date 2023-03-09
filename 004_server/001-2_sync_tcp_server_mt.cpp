#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

class Service {

public:
    Service() {}

    void StartHandlingClient(
        std::shared_ptr<asio::ip::tcp::socket> sock
    ) {
        std::thread t(([this, sock]() {
            HandleClient(sock);
        }));

        t.detach(); 
    }

private:
    void HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock) {
        try {
            asio::streambuf request;
            asio::read_until(*sock.get(), request, '\n');

            for(volatile int i=0;i < 1000000; i++);

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            std::string response = "Response\n";
            asio::write(*sock.get(), asio::buffer(response));
        }
        catch(system::system_error& e) {
            std::cerr << "Error Code: " << e.code() 
                << ". Message: " << e.what() << std::endl;
            
            delete this; 
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
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));
        m_acceptor.accept(*sock.get());
        (new Service)->StartHandlingClient(sock);
    }

private:
    asio::io_service& m_ios;
    asio::ip::tcp::acceptor m_acceptor;
};

class Server {

public:
    Server() :m_stop(false) {}

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

        while (!m_stop.load()) {
            acceptor.Accept();
        }
    }

    std::unique_ptr<std::thread> m_thread;
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