#include "sync_ssl_service.hpp"

void SyncSSLService::HandleClient(
    asio::ssl::stream<asio::ip::tcp::socket>& ssl_stream
) {

    try {
        ssl_stream.handshake(asio::ssl::stream_base::server);

        asio::streambuf request;
        asio::read_until(ssl_stream, request, '\n');

        // Dummy
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::string response = "Response\n";
        asio::write(ssl_stream, asio::buffer(response));
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << ". Message: " << e.what() << std::endl;
    }

}