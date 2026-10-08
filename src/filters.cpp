#include "filters.h"

static constexpr double minSample = -32768.0;
static constexpr double maxSample = 32767.0;

IFilter::State AmplFilter::apply(Waveform* sound)
{
    if(sound == nullptr)
        return State::badArgs;

    std::vector<std::int16_t>& samples = sound->getSamples();
    for(int16_t& sample: samples)
        sample = static_cast<int16_t>(
            std::clamp(sample * getAmpl(), minSample, maxSample));

    return State::ok;
}

double AmplFilter::getAmpl() const { return _ampl; }

IFilter::State Normalize::apply(Waveform* sound)
{
    if(sound == nullptr)
        return State::badArgs;

    // добавить проверку
    // if (_peak < 0.0 || _peak > 1.0)
    //     return State::badArgs;

    std::vector<std::int16_t>& samples = sound->getSamples();
    int currentPeak = 0;  // int, а не int16_t, так как abs(-32768) = 32768

    for(int16_t sample: samples)
        currentPeak = std::max(currentPeak, std::abs(static_cast<int>(sample)));

    if(sound->getSampleCount() == 0 || currentPeak == 0)
        return State::ok;

    double scale = getPeak() * maxSample / currentPeak;

    for(int16_t& sample: samples)
        sample = static_cast<int16_t>(
            std::clamp(sample * scale, minSample, maxSample));

    return State::ok;
}

double Normalize::getPeak() const { return _peak; }

IFilter::State Silence::apply(Waveform* sound)
{
    if(sound == nullptr)
        return State::badArgs;

    double startSec;
    double endSec;

    if(getUnit() == TimeUnit::sec)
    {
        startSec = getStart();
        endSec = getEnd();
    }
    else
    {
        // можно добавить перевод в секунды и в миллисекунды как функции члены
        startSec = getStart() / 1000.0;
        endSec = getEnd() / 1000.0;
    }

    // if (startSec < 0 || endSec < startSec)
    //     return State::badArgs;

    double lenSec = endSec - startSec;

    int len = static_cast<int>(lenSec * sound->getSampleRate());

    int startIndex = static_cast<int>(startSec * sound->getSampleRate());

    auto& samples = sound->getSamples();

    if(startIndex > static_cast<int>(samples.size()))
        startIndex = static_cast<int>(samples.size());

    samples.insert(samples.begin() + startIndex, len, 0);

    return State::ok;
}

Silence::TimeUnit Silence::getUnit() const { return _unit; }

double Silence::getStart() const { return _start; }

double Silence::getEnd() const { return _end; }

IFilter::State Timestretch::apply(Waveform* sound)
{
    if(sound == nullptr)
        return State::badArgs;

    if(sound->getSampleCount() == 0)
        return State::ok;

    // проверка на factor > 0

    double factor = getFactor();

    int newSize = std::round(sound->getSampleCount() * factor);

    std::vector<std::int16_t> newSamples(newSize);
    const std::vector<std::int16_t>& oldSamples = sound->getSamples();
    for(int i = 0; i < newSize; i++)
    {
        double pos = i / factor;
        double wholeDouble, frac;
        frac = std::modf(pos, &wholeDouble);

        int whole = static_cast<int>(wholeDouble);

        // s'[i] = s[l] * (1 - frac) + s[l + 1] * frac
        if(whole + 1 < static_cast<int>(sound->getSampleCount()))
            newSamples[i] = static_cast<int16_t>(
                oldSamples[whole] * (1 - frac) + oldSamples[whole + 1] * frac);
        else
            newSamples[i] = oldSamples[whole];
    }

    sound->setSamples(std::move(newSamples));

    return State::ok;
}

double Timestretch::getFactor() const { return _factor; }

IFilter::State Lowpass::apply(Waveform* sound)
{
    if(sound == nullptr)
        return State::badArgs;

    // проверка что windowSize нечётное, windowSize >= 1

    std::vector<std::int16_t> newSamples(sound->getSampleCount());

    std::vector<std::int16_t>& samples = sound->getSamples();
    int samplesCount = sound->getSampleCount();
    int windowSize = getWindowSize();
    for(int i = 0; i < samplesCount; i++)
    {
        int left = 0;
        int right = 0;
        int countOneSide = windowSize / 2;

        if(i < countOneSide)
            left = countOneSide - i;

        if((i + countOneSide) >= samplesCount)
            right = i + countOneSide - samplesCount + 1;

        int currLeft = countOneSide - left;
        int currRight = countOneSide - right;

        long long sum =
            left * samples[i - currLeft] +
            std::accumulate(samples.begin() + i - currLeft,
                            samples.begin() + i + currRight + 1, 0LL) +
            right * samples[i + currRight];

        newSamples[i] = static_cast<int16_t>(sum / windowSize);
    }

    sound->setSamples(std::move(newSamples));
    return State::ok;
}

int Lowpass::getWindowSize() const { return _windowSize; }

IFilter::State SinGenFilter::apply(Waveform* sound)
{
    int sampleCount = static_cast<int>(std::round(getDuration() / 1000.0 * sampleRate));

    std::vector<std::int16_t> samples(sampleCount);

    for (int i = 0; i < sampleCount; i++)
    {
        double time = static_cast<double>(i) / sampleRate;

        double value = 32767.0 * std::sin(2.0 * M_PI * getFreq() * time);

        samples[i] = static_cast<std::int16_t>(std::clamp(value, minSample, maxSample));
    }

    sound->setSamples(std::move(samples));
    sound->setSampleRate(sampleRate);
    sound->setChannels(1);
    sound->setBitsPerSample(16);

    return State::ok;
}

double SinGenFilter::getFreq() const
{
    return _freq;
}

double SinGenFilter::getDuration() const
{
    return _duration;
}

IFilter::State AmGenFilter::apply(Waveform* sound)
{
    int sampleCount = static_cast<int>(
        std::round(getDurationMs() / 1000.0 * sampleRate)
    );

    std::vector<std::int16_t> samples(sampleCount);

    for (int i = 0; i < sampleCount; i++)
    {
        double time = static_cast<double>(i) / sampleRate;

        double envelope =
            1.0 + getDepth() * std::sin(2.0 * M_PI * getModulationHz() * time);

        double carrier =
            std::sin(2.0 * M_PI * getCarrierHz() * time);

        double value =
            getAmplitude() * 32767.0 * envelope * carrier;

        samples[i] = static_cast<std::int16_t>(
            std::clamp(value, minSample, maxSample)
        );
    }

    sound->setSamples(std::move(samples));
    sound->setSampleRate(sampleRate);
    sound->setChannels(1);
    sound->setBitsPerSample(16);

    return State::ok;
}

double AmGenFilter::getAmplitude() const
{
    return _amplitude;
}

double AmGenFilter::getCarrierHz() const
{
    return _carrierHz;
}

double AmGenFilter::getModulationHz() const
{
    return _modulationHz;
}

double AmGenFilter::getDepth() const
{
    return _depth;
}

double AmGenFilter::getDurationMs() const
{
    return _durationMs;
}

IFilter::State FmGenFilter::apply(Waveform* sound)
{
    int sampleCount = static_cast<int>(
        std::round(getDurationMs() / 1000.0 * sampleRate)
    );

    std::vector<std::int16_t> samples(sampleCount);

    for (int i = 0; i < sampleCount; i++)
    {
        double time = static_cast<double>(i) / sampleRate;

        double phase =
            2.0 * M_PI * getCarrierHz() * time
            + (getDeviationHz() / getModulationHz())
            * std::sin(2.0 * M_PI * getModulationHz() * time);

        double value =
            getAmplitude() * maxSample * std::sin(phase);

        samples[i] = static_cast<std::int16_t>(
            std::clamp(value, minSample, maxSample)
        );
    }

    sound->setSamples(std::move(samples));
    sound->setSampleRate(sampleRate);
    sound->setChannels(1);
    sound->setBitsPerSample(16);

    return State::ok;
}

double FmGenFilter::getAmplitude() const
{
    return _amplitude;
}

double FmGenFilter::getCarrierHz() const
{
    return _carrierHz;
}

double FmGenFilter::getModulationHz() const
{
    return _modulationHz;
}

double FmGenFilter::getDeviationHz() const
{
    return _deviationHz;
}

double FmGenFilter::getDurationMs() const
{
    return _durationMs;
}