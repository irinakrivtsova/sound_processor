#include "filters.h"

#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace
{
    Waveform makeSound(std::vector<std::int16_t> samples)
    {
        return Waveform(1, 1, 44100, 16, std::move(samples));
    }

    int maxAbsSample(const std::vector<std::int16_t>& samples)
    {
        int result = 0;
        for (std::int16_t sample : samples)
        {
            result = std::max(result, std::abs(static_cast<int>(sample)));
        }
        return result;
    }
}

TEST_CASE("AmplFilter multiplies samples and clamps int16 range")
{
    Waveform sound = makeSound({10000, -20000, 30000});
    AmplFilter filter(2.0);
    std::vector<std::int16_t> expected = {20000, -32768, 32767};

    REQUIRE(filter.apply(&sound) == IFilter::State::ok);
    REQUIRE(sound.getSamples() == expected);
}

TEST_CASE("Normalize scales peak to requested part of int16 maximum")
{
    Waveform sound = makeSound({1000, -2000});
    Normalize filter(0.5);

    REQUIRE(filter.apply(&sound) == IFilter::State::ok);
    REQUIRE(maxAbsSample(sound.getSamples()) == 16383);
}

TEST_CASE("Silence inserts zero samples")
{
    Waveform sound = makeSound({1, 2, 3});
    Silence filter(Silence::TimeUnit::ms, 0.0, 1.0);

    REQUIRE(filter.apply(&sound) == IFilter::State::ok);
    REQUIRE(sound.getSampleCount() == 47);

    for (std::size_t i = 0; i < 44; ++i)
    {
        REQUIRE(sound.getSamples()[i] == 0);
    }

    REQUIRE(sound.getSamples()[44] == 1);
    REQUIRE(sound.getSamples()[45] == 2);
    REQUIRE(sound.getSamples()[46] == 3);
}

TEST_CASE("Timestretch uses linear interpolation")
{
    Waveform sound = makeSound({0, 10});
    Timestretch filter(2.0);
    std::vector<std::int16_t> expected = {0, 5, 10, 10};

    REQUIRE(filter.apply(&sound) == IFilter::State::ok);
    REQUIRE(sound.getSamples() == expected);
}

TEST_CASE("Lowpass averages samples with border extension")
{
    Waveform sound = makeSound({0, 9, 0});
    Lowpass filter(3);
    std::vector<std::int16_t> expected = {3, 3, 3};

    REQUIRE(filter.apply(&sound) == IFilter::State::ok);
    REQUIRE(sound.getSamples() == expected);
}

TEST_CASE("SinGenFilter creates signal with requested duration")
{
    Waveform sound = makeSound({123, 456});
    SinGenFilter filter(440.0, 10.0);

    REQUIRE(filter.apply(&sound) == IFilter::State::ok);
    REQUIRE(sound.getSampleCount() == 441);
    REQUIRE(sound.getSamples()[0] == 0);
}

TEST_CASE("AmGenFilter creates signal with requested duration")
{
    Waveform sound = makeSound({});
    AmGenFilter filter(0.5, 440.0, 5.0, 0.5, 10.0);

    REQUIRE(filter.apply(&sound) == IFilter::State::ok);
    REQUIRE(sound.getSampleCount() == 441);
}

TEST_CASE("FmGenFilter creates signal with requested duration")
{
    Waveform sound = makeSound({});
    FmGenFilter filter(0.5, 440.0, 5.0, 20.0, 10.0);

    REQUIRE(filter.apply(&sound) == IFilter::State::ok);
    REQUIRE(sound.getSampleCount() == 441);
}
