#pragma once 

#include "asio_ssl_common.hpp"

class Acceptor {

public:
    Acceptor(asio::io_service& ios, uint16_t port);

    void Accept();

private:
    std::string GetPassword(
        std::size_t max_length,
        asio::ssl::context::password_purpose purpose
    ) const { return "pass"; }

private:
    asio::io_service& m_ios;
    asio::ip::tcp::acceptor m_acceptor; 
    asio::ssl::context m_ssl_context; 
};