
#pragma once 

#include "http_request.hpp"

class HttpClient {

public:
    HttpClient();

    std::shared_ptr<HttpRequest> CreateRequest(uint32_t id);

    void Close();

private:
    asio::io_service m_ios; 
    std::unique_ptr<boost::asio::io_service::work> m_work; 
    std::unique_ptr<std::thread> m_thread;
};