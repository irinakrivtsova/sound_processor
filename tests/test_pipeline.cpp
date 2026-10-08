#include "pipeline.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <vector>

namespace
{
    Waveform makeSound(std::vector<std::int16_t> samples)
    {
        return Waveform(1, 1, 44100, 16, std::move(samples));
    }
}

TEST_CASE("Pipeline applies filters in order")
{
    Pipeline pipeline;
    pipeline.addFilter(new AmplFilter(2.0));
    pipeline.addFilter(new Normalize(1.0));

    Waveform sound = makeSound({1000, -2000});

    REQUIRE(pipeline.getFilterNumber() == 2);
    REQUIRE(pipeline.apply(&sound) == IFilter::State::ok);
    REQUIRE(sound.getSamples()[1] == -32767);
}

TEST_CASE("Pipeline rejects nullptr sound")
{
    Pipeline pipeline;
    pipeline.addFilter(new AmplFilter(2.0));

    REQUIRE(pipeline.apply(nullptr) == IFilter::State::badArgs);
}
