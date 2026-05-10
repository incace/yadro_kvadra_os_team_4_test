#pragma once

#include <string>
#include <vector>

namespace media_scanner {
struct MediaFiles final {
    std::vector<std::string> audio;
    std::vector<std::string> video;
    std::vector<std::string> images;
};
}
