#include "converter.h"
#include "producer.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <vector>

TEST_CASE("Converter creates pipeline from valid descriptors")
{
    char ampl[] = "ampl";
    char factor[] = "2.0";
    char generator[] = "generator";
    char sin[] = "sin";
    char freq[] = "440";
    char duration[] = "10";

    FilterDescriptor generatorDescriptor;
    generatorDescriptor.filterName = generator;
    generatorDescriptor.params = {sin, freq, duration};

    FilterDescriptor amplDescriptor;
    amplDescriptor.filterName = ampl;
    amplDescriptor.params = {factor};

    CmdLineArgs2PipelineConverter converter;
    FilterProducers::addStandardFilterProducers(converter);

    Pipeline pipeline = converter.createPipeline({generatorDescriptor, amplDescriptor});

    REQUIRE(converter.getResult() == CmdLineArgs2PipelineConverter::Result::ok);
    REQUIRE(pipeline.getFilterNumber() == 2);
    REQUIRE(dynamic_cast<SinGenFilter*>(pipeline.getFilter(0)) != nullptr);
    REQUIRE(dynamic_cast<AmplFilter*>(pipeline.getFilter(1)) != nullptr);
}

TEST_CASE("Converter reports unknown filter name")
{
    char name[] = "unknown";
    FilterDescriptor descriptor;
    descriptor.filterName = name;

    CmdLineArgs2PipelineConverter converter;
    FilterProducers::addStandardFilterProducers(converter);

    Pipeline pipeline = converter.createPipeline({descriptor});

    REQUIRE(pipeline.getFilterNumber() == 0);
    REQUIRE(converter.getResult() == CmdLineArgs2PipelineConverter::Result::badFilterName);
}

TEST_CASE("Converter reports bad filter parameters")
{
    char name[] = "ampl";
    char badFactor[] = "-1";
    FilterDescriptor descriptor;
    descriptor.filterName = name;
    descriptor.params = {badFactor};

    CmdLineArgs2PipelineConverter converter;
    FilterProducers::addStandardFilterProducers(converter);

    Pipeline pipeline = converter.createPipeline({descriptor});

    REQUIRE(pipeline.getFilterNumber() == 0);
    REQUIRE(converter.getResult() == CmdLineArgs2PipelineConverter::Result::badParams);
}
