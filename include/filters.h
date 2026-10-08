#ifndef FILTERS_H
#define FILTERS_H

#include "waveform.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>  // для lowpass
#include <utility>
#include <vector>

class IFilter
{
public:
    virtual ~IFilter() = default;

    enum class State
    {
        ok,
        badArgs  // дополнить какие могут быть ошибки по ходу выполнения
    };

    virtual State apply(Waveform* sound) = 0;
};

class AmplFilter: public IFilter
{
public:
    AmplFilter(double ampl): _ampl{ampl} {}
    ~AmplFilter() override = default;

public:
    State apply(Waveform* sound) override;
    double getAmpl() const;

private:
    double _ampl;
};

class Normalize: public IFilter
{
public:
    Normalize(double peak = 1.0): _peak{peak} {};
    ~Normalize() override = default;

public:
    State apply(Waveform* sound) override;
    double getPeak() const;

private:
    double _peak;
};

class Silence: public IFilter
{
public:
    enum class TimeUnit
    {
        sec,
        ms
    };

public:
    Silence(TimeUnit unit, double start, double end)
        : _unit{unit}, _start{start}, _end{end} {};
    ~Silence() override = default;
    
public:
    State apply(Waveform* sound) override;
    TimeUnit getUnit() const;
    double getStart() const;
    double getEnd() const;

private:
    TimeUnit _unit;
    double _start;
    double _end;
};

class Timestretch: public IFilter
{
public:
    Timestretch(double factor): _factor{factor} {};
    ~Timestretch() override = default;

public:
    State apply(Waveform* sound) override;
    double getFactor() const;

private:
    double _factor;
};

class Lowpass: public IFilter
{
public:
    Lowpass(int windowSize): _windowSize{windowSize} {};
    ~Lowpass() override = default;

public:
    State apply(Waveform* sound) override;
    int getWindowSize() const;

private:
    int _windowSize;
};

class AbstractGeneratorFilter: public IFilter
{
public:
    virtual State apply(Waveform* sound) = 0;

protected:
    static constexpr int sampleRate = 44100;
};

class SinGenFilter: public AbstractGeneratorFilter
{
public:
    SinGenFilter(double freqHz, double durationMs)
        : _freq{freqHz}, _duration{durationMs} {};
    ~SinGenFilter() override = default;

public:
    State apply(Waveform* sound) override;

    double getFreq() const;
    double getDuration() const;

private:
    double _freq;
    double _duration;
};

class AmGenFilter : public AbstractGeneratorFilter
{
public:
    AmGenFilter(double amplitude,
                double carrierHz,
                double modulationHz,
                double depth,
                double durationMs)
        : _amplitude{amplitude},
          _carrierHz{carrierHz},
          _modulationHz{modulationHz},
          _depth{depth},
          _durationMs{durationMs}
    {}

    ~AmGenFilter() override = default;

public:
    State apply(Waveform* sound) override;

    double getAmplitude() const;
    double getCarrierHz() const;
    double getModulationHz() const;
    double getDepth() const;
    double getDurationMs() const;

private:
    double _amplitude;
    double _carrierHz;
    double _modulationHz;
    double _depth;
    double _durationMs;
};

class FmGenFilter : public AbstractGeneratorFilter
{
public:
    FmGenFilter(double amplitude,
                double carrierHz,
                double modulationHz,
                double deviationHz,
                double durationMs)
        : _amplitude{amplitude},
          _carrierHz{carrierHz},
          _modulationHz{modulationHz},
          _deviationHz{deviationHz},
          _durationMs{durationMs}
    {}

    ~FmGenFilter() override = default;

public:
    State apply(Waveform* sound) override;

    double getAmplitude() const;
    double getCarrierHz() const;
    double getModulationHz() const;
    double getDeviationHz() const;
    double getDurationMs() const;

private:
    double _amplitude;
    double _carrierHz;
    double _modulationHz;
    double _deviationHz;
    double _durationMs;
};

#endif