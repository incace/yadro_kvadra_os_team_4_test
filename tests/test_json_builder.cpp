#include <gtest/gtest.h>

#include "media_scanner/json_builder.hpp"
#include "media_scanner/media_files.hpp"

using namespace media_scanner;


static std::string makeExpected(const std::string& audioArr,
                                const std::string& videoArr,
                                const std::string& imagesArr)
{
    return "{\n"
           "    \"audio\": "  + audioArr  + ",\n"
           "    \"video\": "  + videoArr  + ",\n"
           "    \"images\": " + imagesArr + "\n"
           "}";
}

TEST(JsonBuilderTest, EmptyMediaFiles)
{
    MediaFiles files;

    const std::string result   = JsonBuilder::build(files);
    const std::string expected = makeExpected("[]", "[]", "[]");

    EXPECT_EQ(result, expected);

    EXPECT_NE(result.find("\"audio\": []"),  std::string::npos);
    EXPECT_NE(result.find("\"video\": []"),  std::string::npos);
    EXPECT_NE(result.find("\"images\": []"), std::string::npos);
}


TEST(JsonBuilderTest, OnlyAudio)
{
    MediaFiles files;
    files.audio = {"song.mp3"};

    const std::string result   = JsonBuilder::build(files);
    const std::string expected = makeExpected("[\"song.mp3\"]", "[]", "[]");

    EXPECT_EQ(result, expected);
}


TEST(JsonBuilderTest, AllCategories)
{
    MediaFiles files;
    files.audio  = {"track1.mp3", "track2.flac"};
    files.video  = {"movie.mkv",  "clip.mp4"};
    files.images = {"photo.jpg",  "icon.png", "anim.gif"};

    const std::string result = JsonBuilder::build(files);

    const std::string expectedAudio  = "[\"track1.mp3\", \"track2.flac\"]";
    const std::string expectedVideo  = "[\"movie.mkv\", \"clip.mp4\"]";
    const std::string expectedImages = "[\"photo.jpg\", \"icon.png\", \"anim.gif\"]";

    const std::string expected = makeExpected(expectedAudio, expectedVideo, expectedImages);

    EXPECT_EQ(result, expected);
}


TEST(JsonBuilderTest, EscapingQuotes)
{
    MediaFiles files;
    files.audio = {"say \"hello\".mp3"};

    const std::string result = JsonBuilder::build(files);

    EXPECT_NE(result.find("\"say \\\"hello\\\".mp3\""), std::string::npos)
        << "Double-quote inside filename must be escaped as \\\" in JSON output.\n"
        << "Actual output:\n" << result;
}


TEST(JsonBuilderTest, EscapingBackslash)
{
    MediaFiles files;
    files.audio = {"back\\slash.mp3"};

    const std::string result = JsonBuilder::build(files);


    const std::string expectedToken = "\"back\\\\slash.mp3\"";
    EXPECT_NE(result.find(expectedToken), std::string::npos)
        << "Backslash inside filename must be escaped as double-backslash in JSON output."
        << "  Actual output: " << result;
}
