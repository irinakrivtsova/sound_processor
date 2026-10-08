#include "application.h"
#include "wavio.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <string>
#include <vector>

TEST_CASE("Application returns help code without arguments")
{
    char appName[] = "sound_processor";
    char* argv[] = {appName};

    Application app;
    app.configure();

    REQUIRE(app.start(1, argv) == 1);
}

TEST_CASE("Application can generate output WAV")
{
    const std::filesystem::path filename =
        std::filesystem::temp_directory_path() / "sound_processor_application_test.wav";

    std::vector<std::string> args = {
        "sound_processor",
        "-o",
        filename.string(),
        "-f",
        "generator",
        "sin",
        "440",
        "10"
    };

    std::vector<char*> argv;
    for (std::string& arg : args)
    {
        argv.push_back(arg.data());
    }

    Application app;
    app.configure();

    REQUIRE(app.start(static_cast<int>(argv.size()), argv.data()) == 0);
    REQUIRE(std::filesystem::exists(filename));

    WavReader reader;
    Waveform sound = reader.read(filename.string().c_str());
    REQUIRE(sound.getSampleCount() == 441);

    std::filesystem::remove(filename);
}
