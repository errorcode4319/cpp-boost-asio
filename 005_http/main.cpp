#include "http_client.hpp"

void Handler(const HttpRequest& request, 
    const HttpMessage& response, const system::error_code& ec
) {
    if (ec.value() == 0) {
        std::cout << "Request #" << request.GetId() 
            << " has completed. Response: "
            << response.GetMessage().rdbuf();
    }
    else if (ec == asio::error::operation_aborted) {
        std::cout << "Request #" << request.GetId()
            << " has been cancelled by the user." << std::endl;
    }
    else {
        std::cerr << "Request #" << request.GetId()
            << " failed! Error Code: " << ec.value()
            << ". Message: " << ec.message() << std::endl;
    }
    return;
}

void SendHttpRequest(HttpClient& client);

int main(int argc, char* argv[]) {
    HttpClient client;
    try {
        auto request = client.CreateRequest(1);
        request->SetHost("localhost");
        request->SetUri("/test");
        request->SetPort(3333);
        request->SetCallback(Handler);
        request->Execute(); 
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        client.Close();
    }
    catch (system::system_error& e) {
        std::cerr << "Error Code: " << e.code() 
            << ". Message: " << e.what() << std::endl;
        return e.code().value(); 
    }
    return 0;
}

void SendHttpRequest(HttpClient& client) {

    
}