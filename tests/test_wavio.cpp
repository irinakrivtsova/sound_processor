#include "wavio.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

TEST_CASE("WavWriter and WavReader roundtrip valid mono PCM WAV")
{
    const std::filesystem::path filename =
        std::filesystem::temp_directory_path() / "sound_processor_roundtrip_test.wav";

    std::vector<std::int16_t> samples = {0, 1000, -1000, 32767, -32768};
    Waveform sound(1, 1, 44100, 16, samples);

    WavWriter writer;
    writer.write(filename.string().c_str(), sound);

    WavReader reader;
    Waveform restored = reader.read(filename.string().c_str());

    REQUIRE(restored.getFormatTag() == 1);
    REQUIRE(restored.getChannels() == 1);
    REQUIRE(restored.getSampleRate() == 44100);
    REQUIRE(restored.getBitsPerSample() == 16);
    REQUIRE(restored.getSamples() == samples);

    std::filesystem::remove(filename);
}

TEST_CASE("WavReader rejects invalid file")
{
    const std::filesystem::path filename =
        std::filesystem::temp_directory_path() / "sound_processor_invalid_test.wav";

    {
        std::ofstream out(filename, std::ios::binary);
        out << "not a wav";
    }

    bool thrown = false;

    try
    {
        WavReader reader;
        reader.read(filename.string().c_str());
    }
    catch (const std::exception&)
    {
        thrown = true;
    }

    REQUIRE(thrown);

    std::filesystem::remove(filename);
}
