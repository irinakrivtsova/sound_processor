#ifndef STD_FILTER_PRODUCERS_H
#define STD_FILTER_PRODUCERS_H

#include "filters.h"
#include "parser.h"
#include "converter.h"

namespace FilterProducers
{
    IFilter* amplFilterCreator(const FilterDescriptor& fd);
    IFilter* normalizeFilterCreator(const FilterDescriptor& fd);
    IFilter* silenceFilterCreator(const FilterDescriptor& fd);
    IFilter* timestretchFilterCreator(const FilterDescriptor& fd);
    IFilter* lowpassFilterCreator(const FilterDescriptor& fd);

    IFilter* generatorFilterCreator(const FilterDescriptor& fd);

    void addStandardFilterProducers(CmdLineArgs2PipelineConverter& converter);
}

#endif