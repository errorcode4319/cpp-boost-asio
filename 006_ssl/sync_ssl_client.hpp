#pragma once 

#include "asio_ssl_common.hpp"

class SyncSSLClient {

public:
    SyncSSLClient(const std::string& ip, uint16_t port);


    void Connect();
    
    void Close();

    std::string EmulateLongComputationOp(uint32_t duration_sec);

private:
    bool OnPeerVerify(bool preferified, asio::ssl::verify_context& context);

    void SendRequest(const std::string& request);

    std::string ReceiveResponse();

private:
    asio::io_service m_ios;
    asio::ip::tcp::endpoint m_ep;

    asio::ssl::context m_ssl_context;
    asio::ssl::stream<asio::ip::tcp::socket> m_ssl_stream;

};