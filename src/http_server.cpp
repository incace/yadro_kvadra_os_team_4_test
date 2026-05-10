#include "media_scanner/http_server.hpp"

#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace media_scanner {

namespace {

class UniqueSocket final {
public:
    explicit UniqueSocket(int fd) : m_fd{fd} {}

    ~UniqueSocket() {
        if (m_fd >= 0)
            ::close(m_fd);
    }

    UniqueSocket(const UniqueSocket&)            = delete;
    UniqueSocket& operator=(const UniqueSocket&) = delete;

    UniqueSocket(UniqueSocket&& other) noexcept
        : m_fd{other.m_fd} { other.m_fd = -1; }

    UniqueSocket& operator=(UniqueSocket&& other) noexcept {
        if (this != &other) {
            if (m_fd >= 0) ::close(m_fd);
            m_fd       = other.m_fd;
            other.m_fd = -1;
        }
        return *this;
    }

    int get()     const { return m_fd; }

    int release() {
        int fd = m_fd;
        m_fd   = -1;
        return fd;
    }

private:
    int m_fd;
};

void sendAll(int fd, const std::string& data) {
    std::size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = ::send(fd, data.data() + sent, data.size() - sent, 0);
        if (n <= 0) break;
        sent += static_cast<std::size_t>(n);
    }
}

} // namespace

HttpServer::HttpServer(int port) : m_port(port) {}

HttpServer::~HttpServer() {
    stop();
    if (m_thread.joinable())
        m_thread.join();
}

void HttpServer::start() {
    if (m_running.exchange(true))
        return;

    UniqueSocket sock{::socket(AF_INET, SOCK_STREAM, 0)};
    if (sock.get() < 0) {
        m_running = false;
        throw std::runtime_error("HttpServer: socket() failed");
    }

    int opt = 1;
    ::setsockopt(sock.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(static_cast<uint16_t>(m_port));

    if (::bind(sock.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        m_running = false;
        throw std::runtime_error("HttpServer: bind() failed on port " + std::to_string(m_port));
    }

    if (::listen(sock.get(), 16) < 0) {
        m_running = false;
        throw std::runtime_error("HttpServer: listen() failed");
    }

    m_serverFd = sock.release();

    m_thread = std::thread{&HttpServer::run, this};
}

void HttpServer::update_json(const std::string& json) {
    std::lock_guard<std::mutex> lock(m_jsonMutex);
    m_json = json;
}

void HttpServer::stop() {
    if (!m_running.exchange(false))
        return;

    if (m_serverFd >= 0) {
        ::shutdown(m_serverFd, SHUT_RDWR);
        ::close(m_serverFd);
        m_serverFd = -1;
    }
}

void HttpServer::run() {
    while (m_running.load()) {
        sockaddr_in clientAddr{};
        socklen_t   clientLen = sizeof(clientAddr);

        int clientFd = ::accept(m_serverFd,
                                reinterpret_cast<sockaddr*>(&clientAddr),
                                &clientLen);
        if (clientFd < 0)
            break;

        std::thread t{&HttpServer::handleClient, this, clientFd};
        t.detach();
    }
}

void HttpServer::handleClient(int clientFd) {
    UniqueSocket sock{clientFd};

    std::string requestBuf;
    requestBuf.reserve(1024);

    char buf[512];
    while (true) {
        ssize_t n = ::recv(sock.get(), buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;

        buf[n] = '\0';
        requestBuf.append(buf, static_cast<std::size_t>(n));

        if (requestBuf.find("\r\n") != std::string::npos ||
            requestBuf.find('\n')  != std::string::npos)
            break;

        if (requestBuf.size() > 8192) break;
    }

    std::string firstLine;
    {
        const auto crlf = requestBuf.find("\r\n");
        if (crlf != std::string::npos) {
            firstLine = requestBuf.substr(0, crlf);
        } else {
            const auto lf = requestBuf.find('\n');
            firstLine = (lf != std::string::npos)
                            ? requestBuf.substr(0, lf)
                            : requestBuf;
        }
    }

    std::string method;
    std::string path;
    {
        const std::size_t s1 = firstLine.find(' ');
        if (s1 != std::string::npos) {
            method = firstLine.substr(0, s1);
            const std::size_t s2 = firstLine.find(' ', s1 + 1);
            path = (s2 != std::string::npos)
                       ? firstLine.substr(s1 + 1, s2 - s1 - 1)
                       : firstLine.substr(s1 + 1);
        }
    }

    std::string response;

    if (method == "GET" && path == "/media_files") {
        std::string body;
        {
            std::lock_guard<std::mutex> lock(m_jsonMutex);
            body = m_json;
        }

        response  = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: application/json\r\n";
        response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += body;
    } else {
        const std::string body = "404 Not Found";
        response  = "HTTP/1.1 404 Not Found\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += body;
    }

    sendAll(sock.get(), response);
    ::shutdown(sock.get(), SHUT_RDWR);
}

} // namespace media_scanner
