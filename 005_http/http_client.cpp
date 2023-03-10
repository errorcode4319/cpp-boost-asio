#include "http_client.hpp"

HttpClient::HttpClient() {
    m_work.reset(new asio::io_service::work(m_ios));
    m_thread.reset(new std::thread([this]() { m_ios.run(); }));
}

std::shared_ptr<HttpRequest> HttpClient::CreateRequest(uint32_t id) {
    return std::shared_ptr<HttpRequest>(new HttpRequest(m_ios, id)); 
}

void HttpClient::Close() {
    m_work.reset(nullptr);
    m_thread->join(); 
}