#include <boost/asio.hpp>
#include <iostream>

using namespace boost; 

class SyncUdpClient {

public:
    SyncUdpClient() : m_sock(m_ios) {
        m_sock.open(asio::ip::udp::v4());
    }

    std::string EmulateLongComputationOp(
        uint32_t duration_sec, const std::string& ip, uint16_t port
    ) {
        std::string request = "EMULATE_LONG_COMP_OP "
            + std::to_string(duration_sec) + "\n";

        asio::ip::udp::endpoint ep(
            asio::ip::address::from_string(ip), port);
        
        SendRequest(ep, request);
        return ReceiveResponse(ep);
    }

private:
    void SendRequest(
        const asio::ip::udp::endpoint& ep, const std::string& request
    ) {
        m_sock.send_to(asio::buffer(request), ep);
    }

    std::string ReceiveResponse(asio::ip::udp::endpoint& ep) {
        char response[6];
        size_t bytes_received = m_sock.receive_from(asio::buffer(response), ep);
        m_sock.shutdown(asio::ip::udp::socket::shutdown_both);
        return std::string(response, bytes_received);
    }

private:
    asio::io_service        m_ios;
    asio::ip::udp::socket   m_sock;
};

int main() {

    try {
        SyncUdpClient client;

        std::cout << "Send Request to server #1" << std::endl; 
        auto response1 = client.EmulateLongComputationOp(10, "127.0.0.1", 3333);
        std::cout << "Response #1: " << response1;

        auto response2 = client.EmulateLongComputationOp(10, "192.168.1.10", 3334);
        std::cout << "Response #2: " << response2;
    }
    catch(system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << ". Message: " << e.what() << std::endl;
        return e.code().value();
    }

    return 0;
}