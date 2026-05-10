#pragma once

#include <string>
#include <vector>
#include "media_files.hpp"

namespace media_scanner {

class JsonBuilder final {
public:
    static std::string build(const MediaFiles& files);

private:
    static std::string escape(const std::string& s);
    static std::string buildArray(const std::vector<std::string>& items);
};

}
