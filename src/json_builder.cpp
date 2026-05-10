#include "media_scanner/json_builder.hpp"

#include <sstream>

namespace media_scanner {

std::string JsonBuilder::escape(const std::string& s) {
    std::string out;
    out.reserve(s.size());

    for (char c : s) {
        if (c == '"')
            out += "\\\"";
        else if (c == '\\')
            out += "\\\\";
        else
            out += c;
    }
    return out;
}

std::string JsonBuilder::buildArray(const std::vector<std::string>& items) {
    std::ostringstream oss;
    oss << '[';

    for (std::size_t i = 0; i < items.size(); ++i) {
        if (i != 0)
            oss << ", ";
        oss << '"' << escape(items[i]) << '"';
    }

    oss << ']';
    return oss.str();
}

std::string JsonBuilder::build(const MediaFiles& files) {
    std::ostringstream oss;

    oss << "{\n";
    oss << "    \"audio\": "  << buildArray(files.audio)  << ",\n";
    oss << "    \"video\": "  << buildArray(files.video)  << ",\n";
    oss << "    \"images\": " << buildArray(files.images) << "\n";
    oss << '}';

    return oss.str();
}

}
