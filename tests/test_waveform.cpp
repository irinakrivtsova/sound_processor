#include "waveform.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <stdexcept>
#include <vector>

TEST_CASE("Waveform stores metadata and samples")
{
    std::vector<std::int16_t> samples = {1, -2, 3};
    Waveform sound(1, 1, 44100, 16, samples);

    REQUIRE(sound.getFormatTag() == 1);
    REQUIRE(sound.getChannels() == 1);
    REQUIRE(sound.getSampleRate() == 44100);
    REQUIRE(sound.getBitsPerSample() == 16);
    REQUIRE(sound.getSampleCount() == 3);
    REQUIRE(sound.getDataSizeBytes() == 6);
    REQUIRE(sound.getAvgBytesPerSec() == 88200);
    REQUIRE(sound.getBlockAlign() == 2);
    REQUIRE(sound.getSamples() == samples);
}

TEST_CASE("Waveform rejects unsupported metadata")
{
    bool thrown = false;

    try
    {
        Waveform sound(1, 2, 44100, 16, {});
    }
    catch (const std::runtime_error&)
    {
        thrown = true;
    }

    REQUIRE(thrown);
}

TEST_CASE("Waveform replaces samples")
{
    Waveform sound(1, 1, 44100, 16, {1, 2, 3});
    std::vector<std::int16_t> expected = {10, 20};

    sound.setSamples(expected);

    REQUIRE(sound.getSamples() == expected);
}
