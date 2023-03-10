#pragma once 

#include "http_common.hpp"
#include "http_message.hpp"

class HttpRequest;

using Callback = std::function<void(const HttpRequest& request, const HttpMessage& response, const system::error_code& ec)>;

class HttpRequest {

public:
    static const uint16_t DEFAULT_PORT = 80;

    HttpRequest(asio::io_service& ios, uint32_t id) : 
        m_port(DEFAULT_PORT), 
        m_id(id), 
        m_callback(nullptr), 
        m_sock(ios), 
        m_resolver(ios), 
        m_was_cancelled(false), 
        m_ios(ios),
        m_method("GET")
    {}

public:
    void SetHost(const std::string& host) { m_host = host; }
    void SetPort(uint16_t port) { m_port = port; }
    void SetUri(const std::string& uri) { m_uri = uri; }
    void SetMethod(const std::string& method) { m_method = method; }
    void SetCallback(Callback callback) { m_callback = callback; }
    const std::string& GetHost() const { return m_host; }
    uint16_t GetPort() const { return m_port; }
    const std::string& GetUri() const { return m_uri; }
    uint32_t GetId() const { return m_id; }

    void Execute();
    void Cancel();

private:
    bool IsCancelled();
    bool OnError(const boost::system::error_code& ec);

    void OnHostNameResolved(
        const boost::system::error_code& ec,
        asio::ip::tcp::resolver::iterator iterator);

    void OnConnectionEstablished(
        const boost::system::error_code& ec,
        asio::ip::tcp::resolver::iterator iterator);

    void OnRequestSent(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void OnStatusLineReceived(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void OnHeadersReceived(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void OnResponseBodyReceived(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void OnFinish(const boost::system::error_code& ec);

private:
    std::string m_host;
    uint16_t    m_port;
    std::string m_uri;  
    std::string m_method; 
    uint32_t    m_id; 
    Callback    m_callback; 
    std::string m_request_buf;
    HttpMessage m_response; 
    bool        m_was_cancelled; 
    std::mutex  m_cancel_mux;

    asio::io_service&       m_ios;
    asio::ip::tcp::socket   m_sock;
    asio::ip::tcp::resolver m_resolver; 
};