#include "media_scanner/scanner.hpp"

#include <algorithm>
#include <filesystem>
#include <system_error>
#include <unordered_set>

namespace media_scanner {

namespace {

const std::unordered_set<std::string> AUDIO_EXTS = {
    ".mp3", ".wav", ".flac", ".ogg", ".aac", ".m4a", ".wma", ".opus",
    ".aiff", ".aif", ".ape", ".wv", ".mka", ".ra", ".ram", ".mid",
    ".midi", ".amr", ".dts", ".ac3"
};

const std::unordered_set<std::string> VIDEO_EXTS = {
    ".mp4", ".mpg", ".mpeg", ".avi", ".mkv", ".mov", ".webm", ".wmv",
    ".flv", ".f4v", ".m4v", ".3gp", ".3g2", ".ogv", ".ts", ".m2ts",
    ".mts", ".vob", ".rm", ".rmvb", ".divx", ".asf"
};

const std::unordered_set<std::string> IMAGE_EXTS = {
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".webp", ".tiff", ".tif",
    ".svg", ".ico", ".heic", ".heif", ".raw", ".cr2", ".nef", ".arw",
    ".dng", ".psd", ".xcf", ".avif", ".jxl", ".pbm", ".pgm", ".ppm"
};

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

}
Scanner::Scanner(const std::string& path)
    : path_{path} {}

MediaFiles Scanner::scan() const {
    MediaFiles result;

    std::error_code ec;
    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(
             path_,
             std::filesystem::directory_options::skip_permission_denied,
             ec))
    {
        if (!entry.is_regular_file(ec))
            continue;

        const std::string ext      = to_lower(entry.path().extension().string());
        const std::string filename = entry.path().filename().string();

        if (AUDIO_EXTS.count(ext))
            result.audio.push_back(filename);
        else if (VIDEO_EXTS.count(ext))
            result.video.push_back(filename);
        else if (IMAGE_EXTS.count(ext))
            result.images.push_back(filename);
    }

    return result;
}

}
