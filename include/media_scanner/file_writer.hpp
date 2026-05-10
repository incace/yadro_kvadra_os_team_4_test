#pragma once

#include <string>

namespace media_scanner {

class FileWriter final {
public:
    static void write(const std::string& directory, const std::string& json);
};

}
