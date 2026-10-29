#ifndef WAVESTREAM_H
#define WAVESTREAM_H

#include "riffstruct.h"
#include "waveform.h"
#include <stdexcept>

class WaveReader
{
public:
    Waveform read(const char* filename);
};

class WaveWriter
{
public:
    void write(const char* filename, const Waveform& content);
};

// clang-format off
class WavestreamWrongFile: public std::runtime_error
{
public:
    WavestreamWrongFile()
        : std::runtime_error("WavestreamWrongFile")
    {
    }
};

class WavestreamReadError: public std::runtime_error
{
public:
    WavestreamReadError()
        : std::runtime_error("WavestreamReadError")
    {
    }
};

class WavestreamWriteError: public std::runtime_error
{
public:
    WavestreamWriteError()
        : std::runtime_error("WavestreamWriteError")
    {
    }
};

class WavestreamBadContent: public std::runtime_error
{
public:
    WavestreamBadContent()
        : std::runtime_error("WavestreamBadContent")
    {
    }
};
//clang-format on

#endif  // #ifndef WAVESTREAM_H
