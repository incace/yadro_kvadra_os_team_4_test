#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "media_scanner/file_writer.hpp"

namespace fs = std::filesystem;
using namespace media_scanner;


static std::string readFile(const fs::path& p)
{
    std::ifstream ifs(p);
    EXPECT_TRUE(ifs.is_open()) << "Cannot open file: " << p;
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}


class FileWriterTest : public ::testing::Test
{
protected:
    fs::path tempDir_;

    void SetUp() override
    {
        const ::testing::TestInfo* info =
            ::testing::UnitTest::GetInstance()->current_test_info();

        std::string subdir = std::string("media_scanner_fw_test_")
                           + info->test_suite_name()
                           + "_"
                           + info->name();

        tempDir_ = fs::temp_directory_path() / subdir;

        fs::remove_all(tempDir_);
        fs::create_directories(tempDir_);
    }

    void TearDown() override
    {
        std::error_code ec;
        fs::remove_all(tempDir_, ec);
    }
};

TEST_F(FileWriterTest, WritesFile)
{
    const std::string json =
        "{\n"
        "    \"audio\": [],\n"
        "    \"video\": [],\n"
        "    \"images\": []\n"
        "}";

    ASSERT_NO_THROW(FileWriter::write(tempDir_.string(), json));

    const fs::path target = tempDir_ / ".media_files";

    ASSERT_TRUE(fs::exists(target))
        << ".media_files was not created in: " << tempDir_;

    EXPECT_EQ(readFile(target), json);
}


TEST_F(FileWriterTest, AtomicOverwrite)
{
    const std::string firstJson  = "{\"audio\":[\"first.mp3\"],\"video\":[],\"images\":[]}";
    const std::string secondJson = "{\"audio\":[\"second.mp3\"],\"video\":[],\"images\":[]}";

    ASSERT_NO_THROW(FileWriter::write(tempDir_.string(), firstJson));
    ASSERT_NO_THROW(FileWriter::write(tempDir_.string(), secondJson));

    const fs::path target = tempDir_ / ".media_files";

    ASSERT_TRUE(fs::exists(target));
    EXPECT_EQ(readFile(target), secondJson)
        << "Expected second write to completely overwrite the first.";

    EXPECT_NE(readFile(target), firstJson);
}

TEST_F(FileWriterTest, ThrowsOnBadDirectory)
{
    const fs::path nonExistent = tempDir_ / "does_not_exist" / "subdir";

    EXPECT_THROW(
        FileWriter::write(nonExistent.string(), "{}"),
        std::runtime_error
    ) << "FileWriter::write() should throw std::runtime_error when the "
         "target directory does not exist.";
}
