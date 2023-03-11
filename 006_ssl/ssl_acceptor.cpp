#include "ssl_acceptor.hpp"
#include "sync_ssl_service.hpp"

using namespace std::placeholders; 

Acceptor::Acceptor(asio::io_service& ios, uint16_t port) 
: m_ios(ios) 
, m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port))
, m_ssl_context(asio::ssl::context::sslv23_server) {

    m_ssl_context.set_options(
        asio::ssl::context::default_workarounds
        | asio::ssl::context::no_sslv2
        | asio::ssl::context::single_dh_use);

    m_ssl_context.set_password_callback(
        std::bind(&Acceptor::GetPassword, this, _1, _2)
    );

    m_ssl_context.use_certificate_chain_file("server.crt");
    m_ssl_context.use_private_key_file("server.key", asio::ssl::context::pem);
    m_ssl_context.use_tmp_dh_file("dhparams.pem");

    m_acceptor.listen();
}

void Acceptor::Accept() {
    asio::ssl::stream<asio::ip::tcp::socket> ssl_stream(m_ios, m_ssl_context);

    m_acceptor.accept(ssl_stream.lowest_layer());

    SyncSSLService service;
    service.HandleClient(ssl_stream);
}