#pragma once 

#include "http_common.hpp"
#include "http_message.hpp"

class HttpService {
    
    static const std::map<uint32_t, std::string> http_status_table; 

public:

    HttpService(std::shared_ptr<asio::ip::tcp::socket> sock)
    : m_sock(sock), m_request(4096), m_response_status_code(200), m_resource_size_bytes(0)
    {}

    void StartHandling();

private:
    bool OnErrorWithRes(const boost::system::error_code& ec, 
        boost::asio::error::misc_errors e_res, uint32_t e_status_code);

    void PrintErrorCode(const boost::system::error_code& ec);

    void OnRequestLineReceived(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void OnHeadersReceived(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void ProcessRequest();

    void SendResponse();

    void OnResponseSent(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void OnFinish();

private:
    std::shared_ptr<asio::ip::tcp::socket> m_sock;
    asio::streambuf m_request;
    std::map<std::string, std::string> m_request_headers;
    std::string m_requested_resource;

    std::unique_ptr<char[]> m_resource_buffer;
    uint32_t m_response_status_code;
    std::size_t m_resource_size_bytes;
    std::string m_response_headers;
    std::string m_response_status_line; 
};