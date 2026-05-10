#include "media_scanner/file_writer.hpp"

#include <fstream>
#include <stdexcept>
#include <cstdio>

namespace media_scanner {

void FileWriter::write(const std::string& directory, const std::string& json) {
    const std::string target  = directory + "/.media_files";
    const std::string tmpPath = directory + "/.media_files.tmp";

    {
        std::ofstream ofs(tmpPath, std::ios::out | std::ios::trunc);

        if (!ofs.is_open())
            throw std::runtime_error("FileWriter: cannot open temp file: " + tmpPath);

        ofs << json;

        if (!ofs)
            throw std::runtime_error("FileWriter: write error for temp file: " + tmpPath);
    }

    if (std::rename(tmpPath.c_str(), target.c_str()) != 0)
        throw std::runtime_error("FileWriter: rename failed: " + tmpPath + " -> " + target);
}

}
