#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <memory>
#include <ctime>

#include "media_scanner/config.hpp"
#include "media_scanner/scanner.hpp"
#include "media_scanner/json_builder.hpp"
#include "media_scanner/file_writer.hpp"
#include "media_scanner/http_server.hpp"

namespace {

std::atomic<bool> running{true};

void signal_handler(int /*signum*/) {
    running = false;
}

std::string current_timestamp() {
    std::time_t now = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return buf;
}

} // namespace

int main(int argc, char* argv[]) {
    media_scanner::Config cfg;
    try {
        cfg = media_scanner::parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cout << "[error] " << e.what() << "\n"
                  << "Usage: media_scanner [--path <dir>] [--interval <sec>] [--http]\n";
        return 1;
    }

    std::cout << "=== Media Scanner ===\n"
              << "  Path     : " << cfg.path << "\n"
              << "  Interval : " << cfg.interval << " second(s)\n"
              << "  HTTP     : " << (cfg.http_enabled
                                        ? "enabled (port " + std::to_string(cfg.http_port) + ")"
                                        : "disabled") << "\n"
              << "=====================\n";

    std::signal(SIGINT,  signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::unique_ptr<media_scanner::HttpServer> server;
    if (cfg.http_enabled) {
        try {
            server = std::make_unique<media_scanner::HttpServer>(cfg.http_port);
            server->start();
            std::cout << "[info] HTTP server started on port " << cfg.http_port << "\n";
        } catch (const std::exception& e) {
            std::cout << "[error] Failed to start HTTP server: " << e.what() << "\n";
            return 1;
        }
    }

    while (running) {
        media_scanner::Scanner    scanner{cfg.path};
        media_scanner::MediaFiles files = scanner.scan();

        std::string json = media_scanner::JsonBuilder::build(files);

        try {
            media_scanner::FileWriter::write(cfg.path, json);
        } catch (const std::exception& e) {
            std::cout << "[warn] FileWriter: " << e.what() << "\n";
        }

        if (server)
            server->update_json(json);

        std::cout << "[" << current_timestamp() << "] "
                  << "Scan complete — "
                  << "audio: "  << files.audio.size()  << ", "
                  << "video: "  << files.video.size()  << ", "
                  << "images: " << files.images.size() << "\n";

        for (int elapsed = 0; elapsed < cfg.interval && running; ++elapsed)
            std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Shutting down...\n";
    if (server)
        server->stop();

    return 0;
}
