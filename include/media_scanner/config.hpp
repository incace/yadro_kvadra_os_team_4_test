#pragma once

#include <string>
#include <cstdlib>
#include <stdexcept>

namespace media_scanner {

struct Config final {
    std::string path = []() -> std::string {
        const char* home = std::getenv("HOME");
        return home ? home : ".";
    }();

    int  interval     = 60;
    bool http_enabled = false;
    int  http_port    = 1234;
};

inline Config parse_args(int argc, char* argv[]) {
    Config cfg;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--path") {
            if (i + 1 >= argc)
                throw std::invalid_argument("--path requires a value");
            cfg.path = argv[++i];

        } else if (arg == "--interval") {
            if (i + 1 >= argc)
                throw std::invalid_argument("--interval requires a value");
            cfg.interval = std::stoi(argv[++i]);

        } else if (arg == "--http") {
            cfg.http_enabled = true;

        } else {
            throw std::invalid_argument("Unknown argument: " + arg
            );
        }
    }

    return cfg;
}

}
