#ifndef APPLICATION_H
#define APPLICATION_H

#include "converter.h"
#include "parser.h"
#include "pipeline.h"
#include "waveform.h"
#include "wavio.h"

class Application
{
    enum class Result
    {
        ok = 0,
        help = 1,
        badArgs = 2,
        badPipeline = 3,
        filterError = 4,
        fileError = 5
    };

public:
    Application() = default;

    void configure();
    int start(int argc, char* argv[]);

private:
    void printHelp() const;
    void printParserError() const;
    void printConverterError(
        CmdLineArgs2PipelineConverter::Result result
    ) const;
    void printFilterError(IFilter::State state) const;

private:
    ArgsParser _parser;
    CmdLineArgs2PipelineConverter _converter;
    WavReader _reader;
    WavWriter _writer;
};

#endif