#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "media_scanner/scanner.hpp"
#include "media_scanner/media_files.hpp"

namespace fs = std::filesystem;
using namespace media_scanner;


static void touch(const fs::path& p)
{
    std::ofstream ofs(p);
    ASSERT_TRUE(ofs.is_open()) << "Could not create file: " << p;
}

static bool contains(const std::vector<std::string>& vec, const std::string& name)
{
    return std::find(vec.begin(), vec.end(), name) != vec.end();
}


class ScannerTest : public ::testing::Test
{
protected:
    fs::path tempDir_;

    void SetUp() override
    {

        const ::testing::TestInfo* info =
            ::testing::UnitTest::GetInstance()->current_test_info();

        std::string subdir = std::string("media_scanner_test_")
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


TEST_F(ScannerTest, EmptyDirectory)
{
    Scanner scanner(tempDir_.string());
    MediaFiles files = scanner.scan();

    EXPECT_TRUE(files.audio.empty());
    EXPECT_TRUE(files.video.empty());
    EXPECT_TRUE(files.images.empty());
}


TEST_F(ScannerTest, AudioFiles)
{
    touch(tempDir_ / "song.mp3");
    touch(tempDir_ / "recording.wav");
    touch(tempDir_ / "lossless.flac");

    Scanner scanner(tempDir_.string());
    MediaFiles files = scanner.scan();

    EXPECT_EQ(files.audio.size(), 3u);
    EXPECT_TRUE(contains(files.audio, "song.mp3"));
    EXPECT_TRUE(contains(files.audio, "recording.wav"));
    EXPECT_TRUE(contains(files.audio, "lossless.flac"));

    EXPECT_TRUE(files.video.empty());
    EXPECT_TRUE(files.images.empty());
}


TEST_F(ScannerTest, VideoFiles)
{
    touch(tempDir_ / "movie.mp4");
    touch(tempDir_ / "series.mkv");
    touch(tempDir_ / "old.avi");

    Scanner scanner(tempDir_.string());
    MediaFiles files = scanner.scan();

    EXPECT_EQ(files.video.size(), 3u);
    EXPECT_TRUE(contains(files.video, "movie.mp4"));
    EXPECT_TRUE(contains(files.video, "series.mkv"));
    EXPECT_TRUE(contains(files.video, "old.avi"));

    EXPECT_TRUE(files.audio.empty());
    EXPECT_TRUE(files.images.empty());
}


TEST_F(ScannerTest, ImageFiles)
{
    touch(tempDir_ / "photo.jpg");
    touch(tempDir_ / "logo.png");
    touch(tempDir_ / "anim.gif");

    Scanner scanner(tempDir_.string());
    MediaFiles files = scanner.scan();

    EXPECT_EQ(files.images.size(), 3u);
    EXPECT_TRUE(contains(files.images, "photo.jpg"));
    EXPECT_TRUE(contains(files.images, "logo.png"));
    EXPECT_TRUE(contains(files.images, "anim.gif"));

    EXPECT_TRUE(files.audio.empty());
    EXPECT_TRUE(files.video.empty());
}

TEST_F(ScannerTest, MixedFiles)
{
    touch(tempDir_ / "track.mp3");
    touch(tempDir_ / "clip.mp4");
    touch(tempDir_ / "shot.png");
    touch(tempDir_ / "data.xyz");
    touch(tempDir_ / "readme.txt");

    Scanner scanner(tempDir_.string());
    MediaFiles files = scanner.scan();

    EXPECT_EQ(files.audio.size(),  1u);
    EXPECT_EQ(files.video.size(),  1u);
    EXPECT_EQ(files.images.size(), 1u);

    EXPECT_TRUE(contains(files.audio,  "track.mp3"));
    EXPECT_TRUE(contains(files.video,  "clip.mp4"));
    EXPECT_TRUE(contains(files.images, "shot.png"));

    EXPECT_FALSE(contains(files.audio,  "data.xyz"));
    EXPECT_FALSE(contains(files.video,  "data.xyz"));
    EXPECT_FALSE(contains(files.images, "data.xyz"));

    EXPECT_FALSE(contains(files.audio,  "readme.txt"));
    EXPECT_FALSE(contains(files.video,  "readme.txt"));
    EXPECT_FALSE(contains(files.images, "readme.txt"));
}


TEST_F(ScannerTest, CaseInsensitive)
{
    touch(tempDir_ / "SONG.MP3");
    touch(tempDir_ / "PHOTO.JPEG");
    touch(tempDir_ / "VIDEO.MKV");

    Scanner scanner(tempDir_.string());
    MediaFiles files = scanner.scan();

    EXPECT_TRUE(contains(files.audio,  "SONG.MP3"));
    EXPECT_TRUE(contains(files.images, "PHOTO.JPEG"));
    EXPECT_TRUE(contains(files.video,  "VIDEO.MKV"));
}
TEST_F(ScannerTest, Recursive)
{
    const fs::path subDir = tempDir_ / "level1" / "level2";
    fs::create_directories(subDir);

    touch(subDir / "deep.flac");
    touch(subDir / "deep.mp4");
    touch(subDir / "deep.gif");

    Scanner scanner(tempDir_.string());
    MediaFiles files = scanner.scan();

    EXPECT_TRUE(contains(files.audio,  "deep.flac"));
    EXPECT_TRUE(contains(files.video,  "deep.mp4"));
    EXPECT_TRUE(contains(files.images, "deep.gif"));
}
