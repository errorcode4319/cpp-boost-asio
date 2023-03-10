#include "http_request.hpp"

using namespace std::placeholders;

void HttpRequest::Execute() {
    
    if (m_port <= 0) 
        throw std::runtime_error("Failed to execute HttpRequest => Invalid Port");
    if (m_host.length() <= 0) 
        throw std::runtime_error("Failed to execute HttpRequest => Invalid Host");
    if (m_uri.length() <= 0) 
        throw std::runtime_error("Failed to execute HttpRequest => Invalid Uri");
    if (m_callback == nullptr) 
        throw std::runtime_error("Failed to execute HttpRequest => Required to set callback"); 

    asio::ip::tcp::resolver::query resolver_query(
        m_host, std::to_string(m_port), 
        asio::ip::tcp::resolver::query::numeric_service);

    if (IsCancelled()) return; 

    m_resolver.async_resolve(resolver_query, 
        std::bind(&HttpRequest::OnHostNameResolved, this, _1, _2)); 
}

void HttpRequest::Cancel() {
    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
    m_was_cancelled = true;
    m_resolver.cancel();
    if (m_sock.is_open()) {
        m_sock.cancel();
    }
}

bool HttpRequest::IsCancelled() {
    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
    if (m_was_cancelled) {
        cancel_lock.unlock();
        OnFinish(boost::system::error_code(asio::error::operation_aborted));
        return true;
    } 
    return false; 
}

bool HttpRequest::OnError(const boost::system::error_code& ec) {
    if (ec.value() != 0) {
        OnFinish(ec);
        return true;
    }
    return false; 
}


void HttpRequest::OnHostNameResolved(
    const boost::system::error_code& ec,
    asio::ip::tcp::resolver::iterator iterator
) {
    if (OnError(ec)) return;
    if (IsCancelled()) return; 

    asio::async_connect(m_sock, iterator, 
        std::bind(&HttpRequest::OnConnectionEstablished, this, _1, _2));
} 

void HttpRequest::OnConnectionEstablished(
    const boost::system::error_code& ec,
    asio::ip::tcp::resolver::iterator iterator
) {
    if (OnError(ec)) return;
    if (IsCancelled()) return; 

    m_request_buf += "GET " + m_uri + " HTTP/1.1\r\n";
    m_request_buf += "Host: " + m_host + "\r\n";
    m_request_buf += "\r\n";

    asio::async_write(m_sock, asio::buffer(m_request_buf), 
        std::bind(&HttpRequest::OnRequestSent, this, _1, _2));
}

void HttpRequest::OnRequestSent(
    const boost::system::error_code& ec,
    std::size_t bytes_transferred
) {
    if (OnError(ec)) return;
    if (IsCancelled()) return; 
    
    m_sock.shutdown(asio::ip::tcp::socket::shutdown_send);

    asio::async_read_until(
        m_sock, m_response.GetMessageBuf(), "\r\n",
        std::bind(&HttpRequest::OnStatusLineReceived, this, _1, _2));
}

void HttpRequest::OnStatusLineReceived(
    const boost::system::error_code& ec,
    std::size_t bytes_transferred
) {
    if (OnError(ec)) return;
    if (IsCancelled()) return;
    
    std::string http_version;
    std::string status_code_s;
    std::string status_message; 

    std::istream response_stream(
        &m_response.GetMessageBuf());
    response_stream >> http_version;

    uint32_t status_code = 200;

    try {
        status_code = std::stoul(status_code_s);
    }
    catch (std::logic_error&) {
        OnFinish(http_errors::invalid_response);
        return; 
    }

    std::getline(response_stream, status_message, '\r');
    response_stream.get();  // "Remove '\n'
    
    m_response.SetStatusCode(status_code);
    m_response.SetStatusMessage(status_message);

    asio::async_read_until(m_sock, 
        m_response.GetMessageBuf(), "\r\n\r\n",
        std::bind(&HttpRequest::OnHeadersReceived, this, _1, _2));
}

void HttpRequest::OnHeadersReceived(
    const boost::system::error_code& ec,
    std::size_t bytes_transferred
) {
    if (OnError(ec)) return;
    if (IsCancelled()) return; 

    std::string header, header_name, header_value;
    std::istream response_stream(&m_response.GetMessageBuf());

    while(true) {
        std::getline(response_stream, header, '\r');
        response_stream.get();
        if (header == "") 
            break;

        size_t separator_pos = header.find(':');
        if (separator_pos != std::string::npos) {
            header_name = header.substr(0, separator_pos);
        }

        if (separator_pos < header.length() - 1) {
            header_value = header.substr(separator_pos + 1);
        }
        else {
            header_value = "";
        }

        m_response.AddHeader(header_name, header_value);
    }

    asio::async_read(m_sock, m_response.GetMessageBuf(), 
        std::bind(&HttpRequest::OnResponseBodyReceived, this, _1, _2)); 
}

void HttpRequest::OnResponseBodyReceived(
    const boost::system::error_code& ec,
    std::size_t bytes_transferred
) {
    if (ec == asio::error::eof)
        OnFinish(boost::system::error_code());
    else 
        OnFinish(ec);
}

void HttpRequest::OnFinish(const boost::system::error_code& ec) {
    if (ec.value() != 0) {
        std::cerr << "Error Code: " << ec.value() << ". Message: " << ec.message() << std::endl;
        return; 
    }
    m_callback(*this, m_response, ec);
    return;
}