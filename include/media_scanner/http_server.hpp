#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <atomic>

namespace media_scanner {

class HttpServer final {
public:
    explicit HttpServer(int port);
    ~HttpServer();

    void start();
    void update_json(const std::string& json);
    void stop();

private:
    void run();
    void handleClient(int clientFd);

    int               m_port;
    int               m_serverFd{-1};
    std::thread       m_thread;
    std::atomic<bool> m_running{false};
    std::mutex        m_jsonMutex;
    std::string       m_json;
};

}
