#ifndef PIPELINE_H
#define PIPELINE_H

#include "filters.h"
#include "waveform.h"

#include <cstddef>
#include <vector>

class Pipeline
{
public:
    Pipeline() = default;

    ~Pipeline();

    Pipeline(const Pipeline& other) = delete;
    Pipeline& operator=(const Pipeline& other) = delete;

    Pipeline(Pipeline&& other) noexcept;
    Pipeline& operator=(Pipeline&& other) noexcept;

public:
    IFilter::State apply(Waveform* sound);

    IFilter* addFilter(IFilter* filter);

    std::size_t getFilterNumber() const;
    IFilter* getFilter(std::size_t index) const;

    IFilter* operator[](std::size_t index) const;

private:
    void clear();

private:
    std::vector<IFilter*> _filters;
};

#endif