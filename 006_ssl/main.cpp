#include "sync_ssl_client.hpp"
#include "ssl_server.hpp"

void RunSSLServer(const std::string& ip, uint16_t port);

int main(){
    const std::string ip = "127.0.0.1";
    uint16_t port = 3333;

    std::thread t(RunSSLServer, ip, port);

    try {
        SyncSSLClient client(ip, port);
        client.Connect();

        std::cout << "Sending request to the server... " << std::endl;
    
        std::string response = client.EmulateLongComputationOp(10);
        client.Close();
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << ". Message: " << e.what() << std::endl;
        return e.code().value();
    }

    t.join();
    return 0;
}

void RunSSLServer(const std::string& ip, uint16_t port) {
    try {
        SSLServer server;
        server.Start(port);

        std::this_thread::sleep_for(std::chrono::seconds(60));

        server.Stop();
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code()
            << ". Message: " << e.what() << std::endl;
    }
}