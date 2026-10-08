#include "converter.h"

Pipeline CmdLineArgs2PipelineConverter::createPipeline(
    const std::vector<FilterDescriptor>& descriptors)
{
    _result = Result::ok;

    Pipeline pipeline;

    for (const FilterDescriptor& descriptor : descriptors)
    {
        if (descriptor.filterName == nullptr)
        {
            _result = Result::badFilterName;
            return pipeline;
        }

        FilterProducer producer = getFilterProducer(descriptor.filterName);

        if (producer == nullptr)
        {
            _result = Result::badFilterName;
            return pipeline;
        }

        IFilter* filter = producer(descriptor);

        if (filter == nullptr)
        {
            _result = Result::badParams;
            return pipeline;
        }

        pipeline.addFilter(filter);
    }

    return pipeline;
}

void CmdLineArgs2PipelineConverter::addFilterProducer(
    const char* filterName,
    FilterProducer producer)
{
    if (filterName == nullptr || producer == nullptr)
        return;

    _producers[filterName] = producer;
}

FilterProducer CmdLineArgs2PipelineConverter::getFilterProducer(
    const char* filterName) const
{
    if (filterName == nullptr)
        return nullptr;

    auto it = _producers.find(filterName);

    if (it == _producers.end())
        return nullptr;

    return it->second;
}

CmdLineArgs2PipelineConverter::Result
CmdLineArgs2PipelineConverter::getResult() const
{
    return _result;
}