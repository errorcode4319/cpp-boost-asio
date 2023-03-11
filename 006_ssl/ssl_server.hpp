#pragma once 

#include "asio_ssl_common.hpp"
#include "ssl_acceptor.hpp"

class SSLServer {

public:
    SSLServer(): m_stop(false) {}

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
        Acceptor acc(m_ios, port);

        while (!m_stop.load()) {
            acc.Accept();
        }
    }

    std::unique_ptr<std::thread> m_thread;
    std::atomic<bool> m_stop;
    asio::io_service m_ios;

};