#include <boost/predef.h>

#ifdef BOOST_OS_WINDOWS 
#define _WIN32_WINNT 0x0501 

#if _WIN32_WINNT <= 0x0502
    #define BOOST_ASIO_DISABLE_IOCP 
    #define BOOST_ASIO_ENABLE_CANCLEIO 
#endif
#endif 

#include <boost/asio.hpp> 
#include <boost/noncopyable.hpp>

#include <thread>
#include <mutex>
#include <memory>
#include <list>
#include <map>
#include <iostream>

using namespace boost;

typedef void(*Callback) (
    uint32_t request_id, 
    const std::string& response, 
    const system::error_code& ec);

struct Session {
    Session(
        asio::io_service& ios,
        const std::string& ip,
        uint16_t port,
        const std::string& request,
        uint32_t id,
        Callback callback
    ): m_sock(ios), m_ep(asio::ip::address::from_string(ip), port),
    m_request(request), m_id(id), m_callback(callback), m_was_cancelled(false) {}

    asio::ip::tcp::socket   m_sock;
    asio::ip::tcp::endpoint m_ep;
    std::string             m_request;
    asio::streambuf         m_response_buf;
    std::string             m_response;
    system::error_code      m_ec;
    uint32_t                m_id;
    Callback                m_callback;
    bool                    m_was_cancelled;
    std::mutex              m_cancel_guard;
};

class AsyncTcpClient : public boost::noncopyable {

public:
    AsyncTcpClient(size_t num_of_threads) {
        m_work.reset(new asio::io_service::work(m_ios));

        for (size_t i = 0;i < num_of_threads; i++) {
            std::unique_ptr<std::thread> t(
                new std::thread([this](){m_ios.run();})
            );
            m_threads.push_back(std::move(t));
        }
    }

    void EmulateLongComputationOp(
        uint32_t duration_sec,
        const std::string& ip,
        uint16_t port,
        Callback callback,
        uint32_t request_id
    ) {
        std::string request = "EMULATE_LONG_CALC_OP "
            + std::to_string(duration_sec) + "\n";

        std::shared_ptr<Session> session = 
            std::shared_ptr<Session>(new Session(m_ios, ip, port, request, request_id, callback));

        session->m_sock.open(session->m_ep.protocol());
        
        std::unique_lock<std::mutex> lock(m_active_sessions_guard);
        m_active_sessions[request_id] = session;
        lock.unlock();

        session->m_sock.async_connect(
            session->m_ep,
            [this, session](const system::error_code& ec) {
                if (ec.value() != 0) {
                    session->m_ec = ec;
                    OnRequestComplete(session);
                    return;
                }

                std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);

                if (session->m_was_cancelled) {
                    OnRequestComplete(session);
                    return;
                }

                asio::async_write(
                    session->m_sock, 
                    asio::buffer(session->m_request),
                    [this, session] (const boost::system::error_code& ec, size_t bytes_transferred){
                        if (ec.value() != 0) {
                            session->m_ec = ec;
                            OnRequestComplete(session);
                            return;
                        }

                        std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);
                        if (session->m_was_cancelled) {
                            OnRequestComplete(session);
                            return;
                        }

                        asio::async_read_until(
                            session->m_sock, session->m_response_buf, '\n',
                            [this, session] (const boost::system::error_code& ec, size_t bytes_transferred) {
                                if (ec.value() != 0) {
                                    session->m_ec = ec;
                                } else {
                                    std::istream in(&session->m_response_buf);
                                    std::getline(in, session->m_response);
                                }
                                OnRequestComplete(session);
                            }
                        );
                    }
                );
            }
        );
    }

    void CancelRequest(uint32_t request_id) {
        std::unique_lock<std::mutex> lock(m_active_sessions_guard);

        auto it = m_active_sessions.find(request_id);
        if (it != m_active_sessions.end()) {
            std::unique_lock<std::mutex> cancel_lock(it->second->m_cancel_guard);
            it->second->m_was_cancelled = true;
            it->second->m_sock.cancel();
        }
    }

    void Close() {
        m_work.reset(NULL);

        for(auto& t: m_threads) {
            t->join(); 
        }
    }

private:
    void OnRequestComplete(std::shared_ptr<Session> session) {
        system::error_code ignored_ec;

        session->m_sock.shutdown(
            asio::ip::tcp::socket::shutdown_both, ignored_ec);
        
        std::unique_lock<std::mutex> lock(m_active_sessions_guard);

        auto it = m_active_sessions.find(session->m_id);
        if (it != m_active_sessions.end()) {
            m_active_sessions.erase(it);
        }
        lock.unlock();

        system::error_code ec;

        if (session->m_ec.value() == 0 && session->m_was_cancelled)
            ec = asio::error::operation_aborted;
        else 
            ec = session->m_ec;

        session->m_callback(session->m_id, session->m_response, ec);
    }



private:
    asio::io_service    m_ios;
    std::map<uint32_t, std::shared_ptr<Session>> m_active_sessions;
    std::mutex          m_active_sessions_guard;
    std::unique_ptr<asio::io_service::work>     m_work;
    std::unique_ptr<std::thread>    m_ios_thread;
    std::list<std::unique_ptr<std::thread>> m_threads; 
};

void handler(
    uint32_t request_id, 
    const std::string& response, 
    const system::error_code& ec
) {
    if (ec.value() != 0) {
        std::cerr << "Request ID: " << request_id << " Failed" << std::endl;
    }
    else {
        std::cout << "Request ID: " << request_id << " Success" << std::endl;
    }
}

int main() {
    try {
        AsyncTcpClient client(8);

        client.EmulateLongComputationOp(10, "127.0.0.1", 3333, handler, 1);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        client.EmulateLongComputationOp(11, "127.0.0.1", 3334, handler, 2);
        client.CancelRequest(1);
        std::this_thread::sleep_for(std::chrono::seconds(6));
        
        client.EmulateLongComputationOp(12, "127.0.0.1", 3335, handler, 3);
        std::this_thread::sleep_for(std::chrono::seconds(15));
        client.Close();
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code()
            << ". Message: " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}
