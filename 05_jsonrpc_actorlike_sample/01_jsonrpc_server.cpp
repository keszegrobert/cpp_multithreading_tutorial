#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <complex>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <optional>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Mandelbrot calculation
// Maps (x, y) coordinate to iteration count (0-1000)
// The Mandelbrot set is traditionally framed within x [-2.5, 1], y [-1, 1] usually.
// The client sends random coordinates from [-100, 100]. Most will be outside the set (iterations = 0 or 1).
// To make it interesting, we interpret the client's large coordinates directly, 
// or maybe we should just calc as is. The user asked for "number of iteration for the given pixel of the mandelbrot set".
int calculateMandelbrot(double x, double y, int maxAllowedIterations = 1000) {
    std::complex<double> c(x, y);
    std::complex<double> z(0, 0);
    int iterations = 0;
    
    while (std::abs(z) <= 2.0 && iterations < maxAllowedIterations) {
        z = z * z + c;
        iterations++;
    }
    
    return iterations;
}

void handleClient(int clientSocket) {
    std::cout << "Client connected (Socket: " << clientSocket << ")" << std::endl;
    
    // Buffer for reading
    char buffer[4096];
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        
        if (bytesRead <= 0) {
            // Client closed connection or error
            break;
        }
        
        std::string request(buffer);
        std::cout << "Received: " << request << std::endl;

        std::string responsePayload;
        try {
            auto j_req = json::parse(request);

            if (!j_req.contains("method") || j_req["method"] != "calculate") {
                continue;
            }

            if (j_req.contains("params") && j_req["params"].contains("x") && j_req["params"].contains("y")) {
                double x = j_req["params"]["x"];
                double y = j_req["params"]["y"];
                auto id = j_req.value("id", 1);
                
                int result = calculateMandelbrot(x, y);

                json j_resp;
                j_resp["jsonrpc"] = "2.0";
                j_resp["result"] = result;
                j_resp["id"] = id;
                
                responsePayload = j_resp.dump();
            } else {
                 json j_err = {
                     {"jsonrpc", "2.0"},
                     {"error", {{"code", -32602}, {"message", "Invalid params"}}},
                     {"id", nullptr}
                 };
                 responsePayload = j_err.dump();
            }
        } catch (const std::exception& e) {
             std::cerr << "JSON parsing error: " << e.what() << std::endl;
             continue;
        }
        
        // Send response
        ssize_t bytesSent = send(clientSocket, responsePayload.c_str(), responsePayload.length(), 0);
        if (bytesSent == -1) {
            std::cerr << "Failed to send response" << std::endl;
            break;
        }
    }
    
    close(clientSocket);
    std::cout << "Client disconnected (Socket: " << clientSocket << ")" << std::endl;
}

int main() {
    int port = 3210;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    
    // Reuse address option
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed. Is port " << port << " already in use?" << std::endl;
        return 1;
    }
    
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }
    
    std::cout << "Server listening on port " << port << "..." << std::endl;
    
    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }
        
        // Handle each client in a new thread
        std::thread(handleClient, clientSocket).detach();
    }
    
    close(serverSocket);
    return 0;
}
