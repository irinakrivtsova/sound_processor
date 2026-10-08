#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <stdexcept>

class Waveform
{
public:
    Waveform(std::uint16_t formatTag, std::uint16_t channels,
             std::uint32_t sampleRate, std::uint16_t bitsPerSample,
             std::vector<std::int16_t> samples);

    std::size_t getSampleCount() const;
    // double getDurationSec() const;
    // double getDurationMs() const;
    std::uint32_t getDataSizeBytes() const;

    const std::vector<std::int16_t>& getSamples() const;
    std::vector<std::int16_t>& getSamples();

    std::uint16_t getFormatTag() const;
    std::uint16_t getChannels() const;
    std::uint32_t getSampleRate() const;
    std::uint16_t getBitsPerSample() const;

    std::uint32_t getAvgBytesPerSec() const;
    std::uint16_t getBlockAlign() const;

    void setFormatTag(std::uint16_t formatTag);
    void setChannels(std::uint16_t channels);
    void setSampleRate(std::uint32_t sampleRate);
    void setBitsPerSample(std::uint16_t bitsPerSample);

    void setSamples(const std::vector<std::int16_t>& samples);
    void setSamples(std::vector<std::int16_t>&& samples);
        
    // std::size_t secToSamples(double sec) const;
    // std::size_t msToSamples(double ms) const;

    // double samplesToSec(std::size_t samples) const;
    // double samplesToMs(std::size_t samples) const;

private:
    uint16_t _formatTag = 1;  // соответствует PCM
    uint16_t _channels = 1;
    uint32_t _sampleRate = 44100;
    uint16_t _bitsPerSample = 16;
    std::vector<int16_t> _samples;
};

#endif