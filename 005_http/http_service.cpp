#include "http_service.hpp"

using namespace std::placeholders;

const std::map<uint32_t, std::string> HttpService::http_status_table = {
    {200, "200 OK"},
    {404, "404 Not Found"},
    {413, "431 Request Entity Too Large"},
    {500, "500 Server Error"},
    {501, "501 Not Implemented"},
    {505, "505 HTTP Version Not Supported"}
};

void HttpService::StartHandling() {
    asio::async_read_until(*m_sock.get(), m_request, "\r\n", 
        std::bind(&HttpService::OnRequestLineReceived, this, _1, _2));
}

bool HttpService::OnErrorWithRes (
    const boost::system::error_code& ec,  
    boost::asio::error::misc_errors e_res, 
    uint32_t e_status_code
) {
    if (ec.value() != 0) {

        PrintErrorCode(ec);

        if (ec == e_res) {
            m_response_status_code = e_status_code;
            SendResponse();
        }
        else {
            OnFinish();
        }
        return true;
    }
    return false; 
}

void HttpService::PrintErrorCode(const boost::system::error_code& ec) {
    std::cerr << "Error Code: " << ec.value() 
            << ". Message: " << ec.message() << std::endl;
}

void HttpService::OnRequestLineReceived(
    const boost::system::error_code& ec,
    std::size_t bytes_transferred
) {
    if (OnErrorWithRes(ec, asio::error::not_found, 413)) return; 
    
    std::string request_line;
    std::istream request_stream(&m_request);
    std::getline(request_stream, request_line, '\r');
    request_stream.get();

    std::string request_method;
    std::istringstream request_line_stream(request_line);
    request_line_stream >> request_method;

    if (request_method != "GET") {  // Only Support GET Method 
        m_response_status_code = 501;
        SendResponse();
        return;
    }

    request_line_stream >> m_requested_resource;

    std::string request_http_version;
    request_line_stream >> m_requested_resource;

    if (request_http_version != "HTTP/1.1") {
        m_response_status_code = 505;
        SendResponse();
        return;
    }

    asio::async_read_until(*m_sock.get(), m_request, "\r\n\r\n",
        std::bind(&HttpService::OnHeadersReceived, this, _1, _2));
}



void HttpService::OnHeadersReceived(
    const boost::system::error_code& ec,
    std::size_t bytes_transferred
) {
    if (OnErrorWithRes(ec, asio::error::not_found, 413)) return; 

    std::istream request_stream(&m_request);
    std::string header_name, header_value;

    while (!request_stream.eof()) {
        std::getline(request_stream, header_name, ':');
        if (!request_stream.eof()) {
            std::getline(request_stream, header_value, '\r');
            request_stream.get();
            m_request_headers[header_name] = header_value; 
        }
    }

    ProcessRequest();
    SendResponse();

    return; 
}

void HttpService::ProcessRequest() {
    std::string resource_file_path = std::string("./") + m_requested_resource;

    if (std::filesystem::exists(resource_file_path)) {
        m_response_status_code = 404;
        return;
    }

    std::ifstream resource_fstream(resource_file_path, std::ifstream::binary);

    if (!resource_fstream.is_open()) {
        m_response_status_code = 500;
        return; 
    }

    resource_fstream.seekg(0, std::ifstream::end);
    m_resource_size_bytes = static_cast<std::size_t>(resource_fstream.tellg());
    m_resource_buffer.reset(new char[m_resource_size_bytes]);

    resource_fstream.seekg(std::ifstream::beg);
    resource_fstream.read(m_resource_buffer.get(), m_resource_size_bytes);

    m_response_headers += std::string("content-length") + ": " 
        + std::to_string(m_resource_size_bytes) + "\r\n";
}

void HttpService::SendResponse() {
    m_sock->shutdown(asio::ip::tcp::socket::shutdown_receive);

    auto status_line = http_status_table.at(m_response_status_code);

    m_response_status_line = std::string("HTTP/1.1 ") + status_line + "\r\n";
    m_response_headers += "\r\n";

    std::vector<asio::const_buffer> response_buffers;
    response_buffers.push_back(asio::buffer(m_response_status_line));

    if (m_response_headers.length() > 0) {
        response_buffers.push_back(asio::buffer(m_response_headers));
    }

    if (m_resource_size_bytes > 0) {
        response_buffers.push_back(asio::buffer(m_resource_buffer.get(), m_resource_size_bytes));
    }

    asio::async_write(*m_sock.get(), response_buffers,
        std::bind(&HttpService::OnResponseSent, this, _1, _2));
}

void HttpService::OnResponseSent(
    const boost::system::error_code& ec,
    std::size_t bytes_transferred
) {
    if (ec.value() != 0) {
        PrintErrorCode(ec);
    }
    m_sock->shutdown(asio::ip::tcp::socket::shutdown_both);
    OnFinish();
}

void HttpService::OnFinish() {
    delete this;
}