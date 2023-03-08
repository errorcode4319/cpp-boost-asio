#include <boost/asio.hpp>
#include <iostream>

using namespace boost; 

class SyncTcpClient{

public:
    SyncTcpClient(const std::string& ip, uint16_t port)
    : m_ep(asio::ip::address::from_string(ip), port), m_sock(m_ios) {
        m_sock.open(m_ep.protocol());
    }

    void Connect() {
        m_sock.connect(m_ep);
    }

    void Close() {
        m_sock.shutdown(asio::ip::tcp::socket::shutdown_both);
        m_sock.close();
    }

    std::string EmulateLongComputationOp(uint32_t duration_sec) {
        std::string request = "EMULATE_LONG_COMP_OP " 
            + std::to_string(duration_sec) + "\n";
        SendRequest(request);
        return ReceiveResponse();
    }

private:
    void SendRequest(const std::string& request) {
        asio::write(m_sock, asio::buffer(request));
    }

    std::string ReceiveResponse() {
        asio::streambuf buf;
        asio::read_until(m_sock, buf, '\n');
        std::istream in(&buf);
        std::string response;
        std::getline(in, response);
        return response;
    }

private:
    asio::io_service   m_ios; 
    asio::ip::tcp::endpoint     m_ep;
    asio::ip::tcp::socket       m_sock; 

};

int main() {

    const std::string ip = "127.0.0.1";
    uint16_t port = 3333;

    try {
        SyncTcpClient client(ip, port);

        client.Connect();

        std::cout << "Send request to server" << std::endl;

        auto response = client.EmulateLongComputationOp(10);

        std::cout << "Response: " << response << std::endl;
        client.Close();
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << "Message: " << e.what() << std::endl; 
        return e.code().value();
    }
    
    return 0; 
}