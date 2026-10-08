#include "application.h"

#include "producer.h"

#include <iostream>
#include <utility>
#include <vector>

void Application::configure()
{
    FilterProducers::addStandardFilterProducers(_converter);
}

int Application::start(int argc, char* argv[])
{
    ArgsParser::Result parseResult = _parser.parse(argc, argv);

    if (parseResult == ArgsParser::Result::noArgs)
    {
        printHelp();
        return static_cast<int>(Result::help);
    }

    if (parseResult == ArgsParser::Result::badArgs)
    {
        printParserError();
        return static_cast<int>(Result::badArgs);
    }

    Waveform sound(
        1,
        1,
        44100,
        16,
        std::vector<std::int16_t>{}
    );

    const char* inputFileName = _parser.getInFileName();

    if (inputFileName != nullptr)
    {
        try
        {
            sound = _reader.read(inputFileName);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Input file error: " << e.what() << '\n';
            return static_cast<int>(Result::fileError);
        }
    }

    Pipeline pipeline = _converter.createPipeline(_parser.getDescriptors());

    CmdLineArgs2PipelineConverter::Result converterResult =
        _converter.getResult();

    if (converterResult != CmdLineArgs2PipelineConverter::Result::ok)
    {
        printConverterError(converterResult);
        return static_cast<int>(Result::badPipeline);
    }

    try
    {
        IFilter::State filterState = pipeline.apply(&sound);

        if (filterState != IFilter::State::ok)
        {
            printFilterError(filterState);
            return static_cast<int>(Result::filterError);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Filter applying exception: " << e.what() << '\n';
        return static_cast<int>(Result::filterError);
    }

    const char* outputFileName = _parser.getOutFileName();

    if (outputFileName != nullptr)
    {
        try
        {
            _writer.write(outputFileName, sound);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Output file error: " << e.what() << '\n';
            return static_cast<int>(Result::fileError);
        }
    }

    return static_cast<int>(Result::ok);
}

void Application::printHelp() const
{
    std::cout
        << "Usage:\n"
        << "  sound_processor [-i input.wav] [-o output.wav]\n"
        << "                  [-f filter_name [filter_params...]]...\n"
        << '\n'
        << "Examples:\n"
        << "  sound_processor -i input.wav -o output.wav -f ampl 0.8\n"
        << "  sound_processor -o output.wav -f generator sin 440 2000\n"
        << '\n'
        << "Filters:\n"
        << "  -f ampl factor\n"
        << "  -f normalize [peak]\n"
        << "  -f silence sec start end\n"
        << "  -f silence ms start end\n"
        << "  -f timestretch factor\n"
        << "  -f lowpass window_size\n"
        << "  -f generator sin frequency_hz duration_ms\n"
        << "  -f generator am amplitude carrier_hz modulation_hz depth duration_ms\n"
        << "  -f generator fm amplitude carrier_hz modulation_hz deviation_hz duration_ms\n";
}

void Application::printParserError() const
{
    std::cerr << "Bad command line arguments\n";
}

void Application::printConverterError(
    CmdLineArgs2PipelineConverter::Result result
) const
{
    if (result == CmdLineArgs2PipelineConverter::Result::badFilterName)
    {
        std::cerr << "Unknown filter name\n";
    }
    else if (result == CmdLineArgs2PipelineConverter::Result::badParams)
    {
        std::cerr << "Bad filter parameters\n";
    }
    else
    {
        std::cerr << "Pipeline creation error\n";
    }
}

void Application::printFilterError(IFilter::State state) const
{
    if (state == IFilter::State::badArgs)
    {
        std::cerr << "Filter applying error: bad arguments\n";
    }
    else
    {
        std::cerr << "Filter applying error\n";
    }
}