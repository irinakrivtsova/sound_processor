#include "wavio.h"

Waveform WavReader::read(const char* filename)
{
    std::ifstream strm(filename, std::ios::binary);

    if(!strm.is_open())
    {
        throw std::runtime_error(
            "Cannot open input WAV file");  // куда выкинуло ошибку и что делать
                                      // дальше
    }

    RiffHeader riffHdr;
    strm.read(reinterpret_cast<char*>(&riffHdr), sizeof(riffHdr));

    if (std::memcmp(riffHdr.chunkid, "RIFF", 4) != 0 ||
    std::memcmp(riffHdr.waveid, "WAVE", 4) != 0)
    {
        throw std::runtime_error("Invalid WAV file");
    }

    FmtHeader fmtHdr;
    strm.read(reinterpret_cast<char*>(&fmtHdr), sizeof(fmtHdr));

    if (std::memcmp(fmtHdr.chunkId, "fmt ", 4) != 0)
        throw std::runtime_error("Missing fmt chunk");

    DataHeader dataHdr;
    strm.read(reinterpret_cast<char*>(&dataHdr), sizeof(dataHdr));

    if (std::memcmp(dataHdr.chunkId, "data", 4) != 0)
        throw std::runtime_error("Missing data chunk");

    if(!strm)
    {
        throw std::runtime_error(
            "Error reading WAV header");  // куда выкинуло ошибку и что делать
                                          // дальше
    }

    if (dataHdr.chunkSize % sizeof(std::int16_t) != 0)
        throw std::runtime_error("Bad WAV data size");

    std::size_t sampleCount = dataHdr.chunkSize / sizeof(std::int16_t);

    std::vector<std::int16_t> samples(sampleCount);

    strm.read(reinterpret_cast<char*>(samples.data()), dataHdr.chunkSize);

    if(!strm)
        throw std::runtime_error("Error reading WAV samples");

    return Waveform(fmtHdr.wFormatTag, fmtHdr.wChannels, fmtHdr.dwSamplePerSec,
                    fmtHdr.wBitsPerSample, std::move(samples));
}

void WavWriter::write(const char* filename, const Waveform& sound)
{
    std::ofstream strm(filename, std::ios::binary);
    if (!strm.is_open())
        throw std::runtime_error("Cannot open output WAV file");

    RiffHeader riffHdr;
    riffHdr.size = 36 + sound.getDataSizeBytes();   
    strm.write(reinterpret_cast<char*>(&riffHdr), sizeof(riffHdr));

    FmtHeader fmtHdr;
    fmtHdr.wFormatTag = sound.getFormatTag();
    fmtHdr.wChannels = sound.getChannels();
    fmtHdr.dwSamplePerSec = sound.getSampleRate();
    fmtHdr.dwAvgBytesPerSec = sound.getAvgBytesPerSec();
    fmtHdr.wBlockAlign = sound.getBlockAlign();
    fmtHdr.wBitsPerSample = sound.getBitsPerSample();
    strm.write(reinterpret_cast<char*>(&fmtHdr), sizeof(fmtHdr));

    DataHeader dataHdr;
    dataHdr.chunkSize = sound.getDataSizeBytes();
    strm.write(reinterpret_cast<char*>(&dataHdr), sizeof(dataHdr));

    const auto& samples = sound.getSamples();
    strm.write(reinterpret_cast<const char*>(samples.data()),
            sound.getDataSizeBytes());
}
