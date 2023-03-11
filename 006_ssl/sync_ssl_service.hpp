#pragma once 

#include "asio_ssl_common.hpp"

class SyncSSLService {

public:
    void HandleClient(asio::ssl::stream<asio::ip::tcp::socket>& ssl_stream);


};