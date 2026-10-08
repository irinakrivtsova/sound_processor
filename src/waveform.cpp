#include "waveform.h"

Waveform::Waveform(std::uint16_t formatTag, std::uint16_t channels,
                   std::uint32_t sampleRate, std::uint16_t bitsPerSample,
                   std::vector<std::int16_t> samples)
    : _formatTag(formatTag), _channels(channels), _sampleRate(sampleRate),
      _bitsPerSample(bitsPerSample), _samples(std::move(samples))
{
    if (_formatTag != 1)
        throw std::runtime_error("Only PCM WAV is supported");

    if (_channels != 1)
        throw std::runtime_error("Only mono WAV is supported");

    if (_sampleRate != 44100)
        throw std::runtime_error("Only 44100 Hz WAV is supported");

    if (_bitsPerSample != 16)
        throw std::runtime_error("Only 16-bit WAV is supported");
}

std::size_t Waveform::getSampleCount() const { return _samples.size(); }

std::uint32_t Waveform::getDataSizeBytes() const
{
    return static_cast<std::uint32_t>(_samples.size() * sizeof(std::int16_t));
}

const std::vector<std::int16_t>& Waveform::getSamples() const
{
    return _samples;
}

std::vector<std::int16_t>& Waveform::getSamples()
{
    return _samples;
}

uint16_t Waveform::getFormatTag() const { return _formatTag; }

uint16_t Waveform::getChannels() const { return _channels; }

uint32_t Waveform::getSampleRate() const { return _sampleRate; }

std::uint16_t Waveform::getBitsPerSample() const { return _bitsPerSample; }

std::uint32_t Waveform::getAvgBytesPerSec() const
{
    return _sampleRate * _channels * _bitsPerSample / 8;
}

std::uint16_t Waveform::getBlockAlign() const
{
    return _channels * _bitsPerSample / 8;
}

void Waveform::setFormatTag(std::uint16_t formatTag)
{
    if (formatTag != 1)
        throw std::runtime_error("Only PCM WAV is supported");

    _formatTag = formatTag;
}

void Waveform::setChannels(std::uint16_t channels)
{
    if (channels != 1)
        throw std::runtime_error("Only mono WAV is supported");

    _channels = channels;
}

void Waveform::setSampleRate(std::uint32_t sampleRate)
{
    if (sampleRate != 44100)
        throw std::runtime_error("Only 44100 Hz WAV is supported");

    _sampleRate = sampleRate;
}

void Waveform::setBitsPerSample(std::uint16_t bitsPerSample)
{
    if (bitsPerSample != 16)
        throw std::runtime_error("Only 16-bit WAV is supported");

    _bitsPerSample = bitsPerSample;
}

void Waveform::setSamples(const std::vector<std::int16_t>& samples)
{
    _samples = samples;
}

void Waveform::setSamples(std::vector<std::int16_t>&& samples)
{
    _samples = std::move(samples);
}