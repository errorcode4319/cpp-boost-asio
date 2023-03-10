#pragma once 

#include "http_common.hpp"

class HttpMessage {

public:
    HttpMessage() : m_message_stream(&m_message_buf) {}

public:
    uint32_t GetStatusCode() const { return m_status_code; }

    const std::string& GetStatusMessage() const {
        return m_status_message; 
    }

    const auto& GetHeaders() { return m_headers; }

    const std::istream& GetMessage() const {
        return m_message_stream;
    }

    asio::streambuf& GetMessageBuf() {
        return m_message_buf;
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
    asio::streambuf m_message_buf;
    std::istream m_message_stream;

};