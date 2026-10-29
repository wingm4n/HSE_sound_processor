#include "filters.h"
#include <algorithm>
#include <cmath>

constexpr double SEC_UNIT = 1000.0;
constexpr double FREQ_HZ = 44100.0;
constexpr double MAX_AMPL = 32767.0;
constexpr double MIN_AMPL = -32768.0;

static int16_t doubleToInt16(double val)
{
    if(std::isnan(val))
        return 0;
    if(val > MAX_AMPL)
        return 32767;
    if(val < MIN_AMPL)
        return -32768;
    return static_cast<int16_t>(val);
}

AmplFilter::AmplFilter(double factor): _factor(factor) {}

bool AmplFilter::apply(Waveform* sound)
{
    if(_factor < 0)
        throw FilterBadArgs();

    for(auto& sample: sound->samples)
        sample = doubleToInt16(sample * _factor);
    return true;
}

NormalizeFilter::NormalizeFilter(double peak): _peak(peak) {}

bool NormalizeFilter::apply(Waveform* sound)
{
    // Required limits
    if(_peak > 1 || _peak < 0)
        throw FilterBadArgs();
    if(sound->samples.empty())
        return true;

    int16_t currentPeak = 0;
    for(auto smpl: sound->samples)
        if(std::abs(smpl) > currentPeak)
            currentPeak = std::abs(smpl);

    if(currentPeak == 0)
        return true;

    double scale = (_peak * MAX_AMPL) / currentPeak;
    for(auto& smpl: sound->samples)
        smpl = doubleToInt16(smpl * scale);
    return true;
}

SilenceFilter::SilenceFilter(const std::string& unit, double start, double end)
    : _unit(unit), _start(start), _end(end)
{
}

bool SilenceFilter::apply(Waveform* sound)
{
    // Required limits
    if((_unit != "sec" && _unit != "ms") || _start < 0 || _end < _start)
        throw FilterBadArgs();

    double freq = sound->getFrequency();
    double mult = (_unit == "sec") ? freq : (freq / SEC_UNIT);

    size_t startS = std::round(_start * mult);
    size_t endS = std::round(_end * mult);

    if(endS < startS)
        return false;
    size_t count = endS - startS + 1;

    if(startS >= sound->samples.size())
    {
        sound->samples.resize(startS, 0);
        sound->samples.resize(startS + count, 0);
    }
    else
    {
        sound->samples.insert(sound->samples.begin() + startS, count, 0);
    }
    return true;
}

TimestretchFilter::TimestretchFilter(double factor): _factor(factor) {}

bool TimestretchFilter::apply(Waveform* sound)
{
    // Required limits
    if(_factor <= 0)
        throw FilterBadArgs();

    if(sound->samples.empty())
        return true;

    size_t oldSize = sound->samples.size();
    size_t newSize = std::round(oldSize * _factor);
    std::vector<int16_t> newSamples(newSize);

    for(size_t i = 0; i < newSize; ++i)
    {
        double pos = i / _factor;
        auto lpart = static_cast<size_t>(pos);
        double frac = pos - lpart;

        if(lpart + 1 < oldSize)
        {
            newSamples[i] = doubleToInt16(sound->samples[lpart] * (1.0 - frac) +
                                          sound->samples[lpart + 1] * frac);
        }
        else
        {
            newSamples[i] = sound->samples[lpart];
        }
    }
    sound->samples = std::move(newSamples);
    return true;
}

LowpassFilter::LowpassFilter(int windowSize): _windowSize(windowSize) {}

bool LowpassFilter::apply(Waveform* sound)
{
    // Required limits
    if(_windowSize < 1 || _windowSize % 2 == 0)
        throw FilterBadArgs();

    if(sound->samples.empty() || _windowSize <= 1)
        return true;

    int nsize = sound->samples.size();
    std::vector<int16_t> res(nsize);
    int half = _windowSize / 2;
    long long sum = 0;

    for(int j = -half; j <= half; ++j)
        sum += sound->samples[std::clamp(j, 0, nsize - 1)];
    res[0] = sum / _windowSize;

    for(int i = 1; i < nsize; ++i)
    {
        int dropInd = std::clamp(i - 1 - half, 0, nsize - 1);
        int addInd = std::clamp(i + half, 0, nsize - 1);
        sum = sum - sound->samples[dropInd] + sound->samples[addInd];
        res[i] = sum / _windowSize;
    }

    sound->samples = std::move(res);
    return true;
}

AbstractGeneratorFilter::AbstractGeneratorFilter(double duration)
    : _duration(duration)
{
}

void AbstractGeneratorFilter::prepareWaveform(Waveform* sound,
                                              size_t samplesNum)
{
    sound->samples.clear();
    sound->samples.resize(samplesNum, 0);
    sound->fmtChunk.frequency = FREQ_HZ;
}

SinGenFilter::SinGenFilter(double freq, double duration)
    : AbstractGeneratorFilter(duration), _freq(freq)
{
}

bool SinGenFilter::apply(Waveform* sound)
{
    // Required limits
    if(_freq < 0 || +_duration < 0)
        throw FilterBadArgs();

    size_t samplesNum = std::round((_duration / SEC_UNIT) * FREQ_HZ);
    prepareWaveform(sound, samplesNum);
    for(size_t i = 0; i < samplesNum; ++i)
    {
        double time = i / FREQ_HZ;
        sound->samples[i] =
            doubleToInt16(MAX_AMPL * std::sin(2 * M_PI * _freq * time));
    }
    return true;
}

AmGenFilter::AmGenFilter(double amp, double carrier, double mod, double depth,
                         double duration)
    : AbstractGeneratorFilter(duration), _amp(amp), _carrier(carrier),
      _mod(mod), _depth(depth)
{
}

bool AmGenFilter::apply(Waveform* sound)
{
    // Required limits
    if(_amp < 0 || _amp > 1 || _carrier < 0 || _mod < 0 || _depth < 0 ||
       _depth > 1 || _duration < 0)
        throw FilterBadArgs();

    size_t samplesNum = std::round((_duration / SEC_UNIT) * FREQ_HZ);
    prepareWaveform(sound, samplesNum);
    for(size_t i = 0; i < samplesNum; ++i)
    {
        double time = i / FREQ_HZ;
        double envelope = 1.0 + _depth * std::sin(2 * M_PI * _mod * time);
        double carrier = std::sin(2 * M_PI * _carrier * time);
        sound->samples[i] = doubleToInt16(_amp * MAX_AMPL * envelope * carrier);
    }
    return true;
}
