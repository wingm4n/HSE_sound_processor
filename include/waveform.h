#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "riffstruct.h"
#include <cstdint>
#include <vector>

class Waveform
{
public:
    // clang-format off
    Waveform() :  riffHdr{{'R', 'I', 'F', 'F'}, 0, {'W', 'A', 'V', 'E'}}
    , fmtHdr{{'f', 'm', 't', ' '}, 16}
                , fmtChunk{1, 1, 44100, 88200, 2, 16}
                , dataSize(0) {};
    // clang-format on

    uint16_t getAudioFormat() const { return fmtChunk.audioFormat; };
    uint16_t getNbrChannels() const { return fmtChunk.nbrChannels; };
    uint32_t getFrequency() const { return fmtChunk.frequency; };
    uint32_t getBytePerSec() const { return fmtChunk.bytePerSec; };
    uint16_t getBytePerBlock() const { return fmtChunk.bytePerBlock; };
    uint16_t getBitsPerSample() const { return fmtChunk.bitsPerSample; };
    uint32_t getDataSizeByte() const
    {
        return samples.size() * sizeof(samples[0]);
    };
    uint32_t getDataSizeBlocks() const { return samples.size(); };
    double getDataSizeSec() const
    {
        return static_cast<double>(getDataSizeByte()) / fmtChunk.bitsPerSample;
    };
    double getDataSizeMilSec() const { return getDataSizeSec() * 1000; };

    const RiffHeader& getRiffHeader() const { return riffHdr; };
    const FmtHeader& getFmtHeader() const { return fmtHdr; };
    const FmtChunk& getFmtChunk() const { return fmtChunk; };

    RiffHeader& setRiffHeader(const RiffHeader& other)
    {
        return riffHdr = other;
    };
    FmtHeader& setFmtHeader(const FmtHeader& other) { return fmtHdr = other; };
    FmtChunk& setFmtChunk(const FmtChunk& other) { return fmtChunk = other; };

public:
    std::vector<int16_t>
        samples;  // int16_t hardcoded due to BitsPerSample limitations

public:
    RiffHeader riffHdr;
    FmtHeader fmtHdr;
    FmtChunk fmtChunk;

    uint32_t dataSize;  // Byte length of SampledData
};

#endif  // #ifndef WAVEFORM_H
