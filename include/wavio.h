#ifndef WAVIO_H
#define WAVIO_H

#include "waveform.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <vector>

struct __attribute__((packed)) RiffHeader
{
    char chunkid[4] = {'R', 'I', 'F', 'F'};
    uint32_t size;
    char waveid[4] = {'W', 'A', 'V', 'E'};
};

struct __attribute__((packed)) FmtHeader
{
    char chunkId[4] = {'f', 'm', 't', ' '};
    uint32_t chunkSize = 16;
    uint16_t wFormatTag;
    uint16_t wChannels;
    uint32_t dwSamplePerSec;
    uint32_t dwAvgBytesPerSec;
    uint16_t wBlockAlign;
    uint16_t wBitsPerSample;
};

struct __attribute__((packed)) DataHeader
{
    char chunkId[4] = {'d', 'a', 't', 'a'};
    uint32_t chunkSize;
};

class WavReader
{
public:
    Waveform read(const char* filename);
};

class WavWriter
{
public:
    void write(const char* filename, const Waveform& sound);
};

#endif
