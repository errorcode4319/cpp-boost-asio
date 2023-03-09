#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

class Service {

public:
    Service(std::shared_ptr<asio::ip::tcp::socket> sock)
    : m_sock(sock) {

    }

    void StartHandling() {
        asio::async_read_until(*m_sock.get(), m_request, '\n', 
            [this] (const system::error_code& ec, size_t bytes_transferred) {
                OnRequestReceived(ec, bytes_transferred);
            }
        );
    }

private:
    void OnRequestReceived(const system::error_code& ec, size_t bytes_transferred) {
        if (ec.value() != 0) {
            std::cout << "Error Code: " << ec.value() 
                << ". Message: " << ec.message() << std::endl;

            OnFinish();
            return;
        }
        m_response = ProcessRequest(m_request);

        asio::async_write(*m_sock.get(), asio::buffer(m_response),
            [this](const system::error_code& ec, size_t bytes_transferred) {
                OnResponseSent(ec, bytes_transferred);
            }
        );
    }

    void OnResponseSent(const system::error_code& ec, size_t bytes_transferred) {
        if (ec.value() != 0) {
            std::cerr << "Error Code: " << ec.value()
                << ". Message: " << ec.message() << std::endl; 
        }
        OnFinish(); 
    }

    void OnFinish() {
        delete this;
    }

    std::string ProcessRequest(asio::streambuf& request) {
        for (volatile int i=0;i<1000000;i++);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::string response = "Response\n";
        return response;
    }

private:
    std::shared_ptr<asio::ip::tcp::socket> m_sock;
    std::string m_response;
    asio::streambuf m_request; 

};

class Acceptor{

public:
    Acceptor(asio::io_service& ios, uint16_t port)
    : m_ios(ios)
    , m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port))
    , m_is_stopped(false) {

    }

    void Start() {
        m_acceptor.listen();
        InitAccept();
    }

    void Stop() {
        m_is_stopped.store(true);
    }

private:
    void InitAccept() {
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));

        m_acceptor.async_accept(*sock.get(),
            [this, sock] (const system::error_code& e) {
                OnAccept(e, sock);
            }
        );
    }

    void OnAccept(const system::error_code& ec, std::shared_ptr<asio::ip::tcp::socket> sock) {
        if (ec.value() == 0) {
            (new Service(sock))->StartHandling();
        }
        else {
            std::cerr << "Error Code: " << ec.value()
                << ". Message: " << ec.message() << std::endl;
        }

        if (!m_is_stopped.load()) {
            InitAccept();
        }
        else {
            m_acceptor.close();
        }
    }

private:
    asio::io_service& m_ios;
    asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool>   m_is_stopped;
};

class Server {

public:
    Server() {
        m_work.reset(new asio::io_service::work(m_ios));
    }

    void Start(uint16_t port, size_t thread_pool_size) {
        m_acceptor.reset(new Acceptor(m_ios, port));
        m_acceptor->Start();

        for (uint32_t i = 0;i < thread_pool_size; i++) {
            std::unique_ptr<std::thread> t(
                new std::thread([this]() {
                    m_ios.run();
                })
            );

            m_thread_pool.push_back(std::move(t));
        }
    }

    void Stop() {
        m_acceptor->Stop();
        m_ios.stop();

        for (auto& t: m_thread_pool) {
            t->join();
        }
    }

private:
    asio::io_service m_ios;
    std::unique_ptr<asio::io_service::work> m_work;
    std::unique_ptr<Acceptor> m_acceptor;
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;
};

int main() {

    uint16_t port = 3333;

    try {
        Server server;
        size_t thread_pool_size = std::thread::hardware_concurrency() * 2;
        if (thread_pool_size == 0) thread_pool_size = 2;    // Default 

        server.Start(port, thread_pool_size);
        std::this_thread::sleep_for(std::chrono::seconds(60));

        server.Stop();
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << ". Message: " << e.what() << std::endl;
    }

    return 0;
}