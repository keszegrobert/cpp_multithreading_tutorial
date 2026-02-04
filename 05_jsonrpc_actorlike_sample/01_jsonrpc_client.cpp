#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <optional>
#include <random>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Request {
    double x;
    double y;
    std::promise<std::optional<int>> result_promise;
};

class Client {
public:
    Client(const std::string& host, int port) {
        connectToServer(host, port);
        running_ = true;
        worker_ = std::thread(&Client::processRequests, this);
    }

    ~Client() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            running_ = false;
        }
        queue_cv_.notify_one();
        
        if (worker_.joinable()) {
            worker_.join();
        }

        if (sockfd_ >= 0) {
            close(sockfd_);
        }
    }

    // Blocking request
    std::optional<int> request(double x, double y) {
        std::promise<std::optional<int>> prom;
        std::future<std::optional<int>> fut = prom.get_future();

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            requests_.push(Request{x, y, std::move(prom)});
        }
        queue_cv_.notify_one();

        // Wait for the result
        return fut.get();
    }

private:
    int sockfd_ = -1;
    bool running_ = false;
    std::queue<Request> requests_;
    std::thread worker_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;

    void connectToServer(const std::string& host, int port) {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            std::cerr << "Error creating socket" << std::endl;
            return;
        }

        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            close(sockfd_);
            sockfd_ = -1;
            return;
        }

        if (connect(sockfd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Connection Failed (Is the server running at " << host << ":" << port << "?)" << std::endl;
            close(sockfd_);
            sockfd_ = -1;
            return;
        }
        std::cout << "Connected to server at " << host << ":" << port << std::endl;
    }

    void processRequests() {
        while (true) {
            Request req;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_cv_.wait(lock, [this] { return !requests_.empty() || !running_; });

                if (!running_ && requests_.empty()) {
                    return;
                }

                if (requests_.empty()) continue;

                req = std::move(requests_.front());
                requests_.pop();
            }

            if (sockfd_ < 0) {
                req.result_promise.set_value(std::nullopt);
                continue;
            }

            // Construct JSON-RPC 2.0 message using nlohmann/json
            json j_req;
            j_req["jsonrpc"] = "2.0";
            j_req["method"] = "calculate";
            j_req["params"] = { {"x", req.x}, {"y", req.y} };
            j_req["id"] = 1;
            std::string payload = j_req.dump();

            // Send
            ssize_t sent = send(sockfd_, payload.c_str(), payload.size(), 0);
            if (sent != (ssize_t)payload.size()) {
                 std::cerr << "Send failed" << std::endl;
                 req.result_promise.set_value(std::nullopt);
                 continue; 
            }

            // Receive
            // Note: This assumes the server sends a response and closes connection OR sends a newline OR sends a valid frame.
            // A simple read might get partial data or merge multiple responses if not careful.
            // For this example, we assume one Recv per Send (synchronous request/response on socket).
            char buffer[4096] = {0};
            ssize_t valread = read(sockfd_, buffer, 4096);
            if (valread <= 0) {
                 std::cerr << "Read failed or connection closed" << std::endl;
                 req.result_promise.set_value(std::nullopt);
                 // If connection broken, practically we should stop or reconnect. 
                 // Here we just fail the current request.
                 continue;
            }

            std::string response(buffer, valread);
            
            std::optional<int> output = std::nullopt;
            try {
                // Parse using nlohmann/json
                json j_resp = json::parse(response);
                if (j_resp.contains("result") && !j_resp["result"].is_null()) {
                    int val = j_resp["result"].get<int>();
                    if (val >= 0 && val <= 1000) {
                        output = val;
                    }
                }
            } catch (const std::exception& e) {
                 std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
            
            req.result_promise.set_value(output);
        }
    }
};

void client_thread_func(Client& client, int id) {
    // Generate random coordinates
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    double x = dis(gen);
    double y = dis(gen);

    std::optional<int> result = client.request(x, y);

    std::stringstream log;
    log << "[Thread " << id << "] Request(" << x << ", " << y << ") => ";
    if (result.has_value()) {
        log << "Success: " << result.value();
    } else {
        log << "Failed";
    }
    log << "\n";
    std::cout << log.str();
}

int main() {
    // Create 1 client
    Client client("127.0.0.1", 3210);

    // Create 10 threads
    std::vector<std::thread> threads;
    threads.reserve(10);
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(client_thread_func, std::ref(client), i + 1);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
