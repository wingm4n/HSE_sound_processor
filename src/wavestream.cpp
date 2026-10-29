#include "wavestream.h"
#include <cstring>
#include <fstream>

Waveform WaveReader::read(const char* filename)
{
    std::ifstream file(filename, std::ios_base::binary);
    if(!file.is_open())
        throw WavestreamWrongFile();

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    Waveform result;

    try
    {
        file.read(reinterpret_cast<char*>(&result.riffHdr), sizeof(RiffHeader));
        if(std::strncmp(result.riffHdr.chunkId, "RIFF", 4) != 0 ||
           std::strncmp(result.riffHdr.format, "WAVE", 4) != 0)
        {
            throw WavestreamBadContent();
        }

        file.read(reinterpret_cast<char*>(&result.fmtHdr), sizeof(FmtHeader));
        if(std::strncmp(result.fmtHdr.chunkId, "fmt ", 4) != 0)
            throw WavestreamBadContent();

        file.read(reinterpret_cast<char*>(&result.fmtChunk), sizeof(FmtChunk));

        FmtHeader dataHdr;
        file.read(reinterpret_cast<char*>(&dataHdr), sizeof(FmtHeader));
        if(std::strncmp(dataHdr.chunkId, "data", 4) != 0)
            throw WavestreamBadContent();

        result.samples.resize(dataHdr.chunkSize / sizeof(int16_t));
        file.read(reinterpret_cast<char*>(result.samples.data()),
                  dataHdr.chunkSize);
    }
    catch(const WavestreamBadContent&)
    {
        throw;
    }
    catch(const std::ios_base::failure&)
    {
        throw WavestreamReadError();
    }
    return result;
}

void WaveWriter::write(const char* filename, const Waveform& content)
{
    std::ofstream file(filename, std::ios_base::binary);
    if(!file.is_open())
        throw WavestreamWrongFile();

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        uint32_t dataBytes = content.getDataSizeByte();
        RiffHeader riff = content.riffHdr;
        riff.chunkSize = 4 + sizeof(FmtHeader) + sizeof(FmtChunk) +
                         sizeof(FmtHeader) + dataBytes;

        FmtHeader fmtHdr = content.fmtHdr;
        fmtHdr.chunkSize = sizeof(FmtChunk);

        FmtHeader dataHdr;
        std::memcpy(dataHdr.chunkId, "data", 4);
        dataHdr.chunkSize = dataBytes;

        file.write(reinterpret_cast<const char*>(&riff), sizeof(RiffHeader));
        file.write(reinterpret_cast<const char*>(&fmtHdr), sizeof(FmtHeader));
        file.write(reinterpret_cast<const char*>(&content.fmtChunk),
                   sizeof(FmtChunk));
        file.write(reinterpret_cast<const char*>(&dataHdr), sizeof(FmtHeader));
        file.write(reinterpret_cast<const char*>(content.samples.data()),
                   dataBytes);
    }
    catch(const std::ios_base::failure&)
    {
        throw WavestreamWriteError();
    }
}
