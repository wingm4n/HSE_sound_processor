#ifndef FILTERS_H
#define FILTERS_H

#include "waveform.h"
#include <stdexcept>
#include <string>
#include <vector>

class IFilter
{
public:
    virtual ~IFilter() = default;
    virtual bool apply(Waveform* sound) = 0;
};

class AmplFilter: public IFilter
{
public:
    AmplFilter(double factor);
    bool apply(Waveform* sound) override;

private:
    double _factor;
};

class NormalizeFilter: public IFilter
{
public:
    NormalizeFilter(double peak = 1.0);
    bool apply(Waveform* sound) override;

private:
    double _peak;
};

class SilenceFilter: public IFilter
{
public:
    SilenceFilter(const std::string& unit, double start, double end);
    bool apply(Waveform* sound) override;

private:
    std::string _unit;
    double _start;
    double _end;
};

class TimestretchFilter: public IFilter
{
public:
    TimestretchFilter(double factor);
    bool apply(Waveform* sound) override;

private:
    double _factor;
};

class LowpassFilter: public IFilter
{
public:
    LowpassFilter(int windowSize);
    bool apply(Waveform* sound) override;

private:
    int _windowSize;
};

class AbstractGeneratorFilter: public IFilter
{
public:
    AbstractGeneratorFilter(double duration);

protected:
    void prepareWaveform(Waveform* sound, size_t samplesNum);
    double _duration;
};

class SinGenFilter: public AbstractGeneratorFilter
{
public:
    SinGenFilter(double freq, double duration);
    bool apply(Waveform* sound) override;

private:
    double _freq;
};

class AmGenFilter: public AbstractGeneratorFilter
{
public:
    AmGenFilter(double amp, double carrier, double mod, double depth,
                double duration);
    bool apply(Waveform* sound) override;

private:
    double _amp, _carrier, _mod, _depth;
};

class FilterBadArgs: public std::runtime_error
{
public:
    FilterBadArgs(): std::runtime_error("Bad arguments passed to filter") {}
};

#endif  // #ifndef FILTERS_H
