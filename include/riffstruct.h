#ifndef RIFFSTRUCT_H
#define RIFFSTRUCT_H

#include <cstdint>

struct __attribute__((packed)) RiffHeader
{
    char chunkId[4];     // "RIFF"
    uint32_t chunkSize;  // Size of the remaining file
    char format[4];      // "WAVE"
};

struct __attribute__((packed)) FmtHeader
{
    char chunkId[4];  // "fmt " or "data"
    uint32_t chunkSize;
};

struct __attribute__((packed)) FmtChunk
{
    uint16_t
        audioFormat;  // Compr. code equals 1 (PCM) due to the task limitations
    uint16_t nbrChannels;  // Equals 1 due to the task limitations
    uint32_t frequency;    // Sample rate equals 44100 (herz) due to the task
    uint32_t bytePerSec;   // Bytes to read per second (Frequency * BytePerBloc)
    uint16_t bytePerBlock;  // Bytes per block (NbrChannels * BitsPerSample / 8)
    uint16_t bitsPerSample;  // Equals 16 due to the task limitations
};

using DataHeader = FmtHeader;

#endif  // #ifndef RIFFSTRUCT_H
