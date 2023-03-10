#pragma once 

#include "http_common.hpp"

class HttpResponse {

public:
    HttpResponse() : m_response_stream(&m_response_buf) {}

public:
    uint32_t GetStatusCode() const { return m_status_code; }

    const std::string& GetStatusMessage() const {
        return m_status_message; 
    }

    const auto& GetHeaders() { return m_headers; }

    const std::istream& GetResponse() const {
        return m_response_stream;
    }

    asio::streambuf& GetResponseBuf() {
        return m_response_buf;
    }

    void SetStatusCode(uint32_t status_code) {
        m_status_code = status_code;
    }

    void SetStatusMessage(const std::string& status_message) {
        m_status_message = status_message; 
    }

    void AddHeader(const std::string& name, const std::string& value) {
        m_headers[name] = value; 
    }

private:
    uint32_t m_status_code;  
    std::string m_status_message;
    std::unordered_map<std::string, std::string> m_headers; 
    asio::streambuf m_response_buf;
    std::istream m_response_stream;

};