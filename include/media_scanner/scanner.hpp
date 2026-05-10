#pragma once

#include <string>
#include "media_files.hpp"

namespace media_scanner {

class Scanner final {
public:
    explicit Scanner(const std::string& path);

    MediaFiles scan() const;

private:
    std::string path_;
};

}
