#include "parser.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>

TEST_CASE("ArgsParser returns noArgs for empty command line")
{
    char app[] = "sound_processor";
    char* argv[] = {app};

    ArgsParser parser;

    REQUIRE(parser.parse(1, argv) == ArgsParser::Result::noArgs);
}

TEST_CASE("ArgsParser parses input output and filter descriptors")
{
    char app[] = "sound_processor";
    char iFlag[] = "-i";
    char inFile[] = "input.wav";
    char oFlag[] = "-o";
    char outFile[] = "output.wav";
    char fFlag1[] = "-f";
    char ampl[] = "ampl";
    char amplFactor[] = "0.8";
    char fFlag2[] = "-f";
    char silence[] = "silence";
    char unit[] = "sec";
    char start[] = "0.2";
    char end[] = "0.4";

    char* argv[] = {
        app, iFlag, inFile, oFlag, outFile,
        fFlag1, ampl, amplFactor,
        fFlag2, silence, unit, start, end
    };

    ArgsParser parser;

    REQUIRE(parser.parse(13, argv) == ArgsParser::Result::ok);
    REQUIRE(std::strcmp(parser.getInFileName(), "input.wav") == 0);
    REQUIRE(std::strcmp(parser.getOutFileName(), "output.wav") == 0);

    const auto& descriptors = parser.getDescriptors();
    REQUIRE(descriptors.size() == 2);

    REQUIRE(std::strcmp(descriptors[0].filterName, "ampl") == 0);
    REQUIRE(descriptors[0].params.size() == 1);
    REQUIRE(std::strcmp(descriptors[0].params[0], "0.8") == 0);

    REQUIRE(std::strcmp(descriptors[1].filterName, "silence") == 0);
    REQUIRE(descriptors[1].params.size() == 3);
    REQUIRE(std::strcmp(descriptors[1].params[0], "sec") == 0);
    REQUIRE(std::strcmp(descriptors[1].params[1], "0.2") == 0);
    REQUIRE(std::strcmp(descriptors[1].params[2], "0.4") == 0);
}

TEST_CASE("ArgsParser rejects duplicated input flag")
{
    char app[] = "sound_processor";
    char iFlag1[] = "-i";
    char inFile1[] = "first.wav";
    char iFlag2[] = "-i";
    char inFile2[] = "second.wav";
    char* argv[] = {app, iFlag1, inFile1, iFlag2, inFile2};

    ArgsParser parser;

    REQUIRE(parser.parse(5, argv) == ArgsParser::Result::badArgs);
}
