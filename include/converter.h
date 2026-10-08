#ifndef CONVERTER_H
#define CONVERTER_H

#include "parser.h"
#include "pipeline.h"
#include "filters.h"

#include <map>
#include <string>
#include <vector>

using FilterProducer = IFilter* (*)(const FilterDescriptor&);

class CmdLineArgs2PipelineConverter
{
public:
    enum class Result
    {
        ok,
        badFilterName,
        badParams
    };

public:
    CmdLineArgs2PipelineConverter() = default;

    Pipeline createPipeline(const std::vector<FilterDescriptor>& descriptors);

    void addFilterProducer(const char* filterName, FilterProducer producer);
    FilterProducer getFilterProducer(const char* filterName) const;

    Result getResult() const;

private:
    std::map<std::string, FilterProducer> _producers;
    Result _result = Result::ok;
};

#endif