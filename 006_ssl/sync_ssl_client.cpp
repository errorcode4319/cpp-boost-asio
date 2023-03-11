#include "sync_ssl_client.hpp"

using namespace std::placeholders; 

SyncSSLClient::SyncSSLClient(const std::string& ip, uint16_t port)
: m_ep(asio::ip::address::from_string(ip), port),
m_ssl_context(asio::ssl::context::sslv3_client),
m_ssl_stream(m_ios, m_ssl_context) {

    m_ssl_stream.set_verify_mode(asio::ssl::verify_peer);

    m_ssl_stream.set_verify_callback(
        std::bind(&SyncSSLClient::OnPeerVerify, this, _1, _2)
    );

}

void SyncSSLClient::Connect() {
    m_ssl_stream.lowest_layer().connect(m_ep);

    m_ssl_stream.handshake(asio::ssl::stream_base::client);
}

void SyncSSLClient::Close() {
    boost::system::error_code ec;

    m_ssl_stream.shutdown(ec);  // Shutdown SSL 
}

std::string SyncSSLClient::EmulateLongComputationOp(uint32_t duration_sec) {
    std::string request = "EMULATE_LONG_COMP_OP "
        + std::to_string(duration_sec) + "\n";

    SendRequest(request);
    return ReceiveResponse();
}

bool SyncSSLClient::OnPeerVerify(
    bool preverified, asio::ssl::verify_context& context
) {
    /*
        Verification
    */
    return true;
}

void SyncSSLClient::SendRequest(const std::string& request) {
    asio::write(m_ssl_stream, asio::buffer(request));
}

std::string SyncSSLClient::ReceiveResponse() {
    asio::streambuf buf;
    asio::read_until(m_ssl_stream, buf, '\n');

    std::string response;
    std::istream input(&buf);
    std::getline(input, response);

    return response;
}