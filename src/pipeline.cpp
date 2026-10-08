#include "pipeline.h"

#include <utility>

Pipeline::~Pipeline()
{
    clear();
}

Pipeline::Pipeline(Pipeline&& other) noexcept
    : _filters(std::move(other._filters))
{
    other._filters.clear();
}

Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
{
    // идиома move-and-swap
    std::swap(_filters, other._filters);
    return *this;
}

IFilter::State Pipeline::apply(Waveform* sound)
{
    if (sound == nullptr)
        return IFilter::State::badArgs;

    for (IFilter* filter : _filters)
    {
        if (filter == nullptr)
            return IFilter::State::badArgs;

        IFilter::State state = filter->apply(sound);

        if (state != IFilter::State::ok)
            return state;
    }

    return IFilter::State::ok;
}

IFilter* Pipeline::addFilter(IFilter* filter)
{
    if (filter == nullptr)
        return nullptr;

    _filters.push_back(filter);

    return filter;
}

std::size_t Pipeline::getFilterNumber() const
{
    return _filters.size();
}

IFilter* Pipeline::getFilter(std::size_t index) const
{
    return _filters[index];
}

IFilter* Pipeline::operator[](std::size_t index) const
{
    return getFilter(index);
}

void Pipeline::clear()
{
    for (IFilter* filter : _filters)
    {
        delete filter;
    }

    _filters.clear();
}