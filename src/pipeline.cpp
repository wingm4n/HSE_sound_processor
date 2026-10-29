#include "pipeline.h"
#include <stdexcept>
#include <string>

bool Pipeline::apply(Waveform* sound)
{
    for(auto& filt: _filters)
        if(!filt->apply(sound))
            return false;
    return true;
}

Pipeline CmdLineArgs2PipelineConverter::createPipeline(
    const std::vector<FilterDescriptor>& descriptors)
{
    Pipeline pipeline;
    for(const auto& filtdesc: descriptors)
    {
        std::string name = filtdesc.filterName;
        FilterProducer prod = getFilterProducer(name);
        if(prod)
        {
            IFilter* rawFilter = prod(filtdesc);
            if(rawFilter)
                pipeline.addFilter(std::unique_ptr<IFilter>(rawFilter));
        }
        else
        {
            throw std::invalid_argument("Unknown filter type: " + name);
        }
    }
    return pipeline;
}

void CmdLineArgs2PipelineConverter::addFilterProducer(
    const std::string& filterName, FilterProducer producer)
{
    _producers[filterName] = producer;
}

FilterProducer
CmdLineArgs2PipelineConverter::getFilterProducer(const std::string& filterName)
{
    auto it = _producers.find(filterName);
    return (it != _producers.end()) ? it->second : nullptr;
}

namespace FilterProducers {

IFilter* amplFilterCreator(const FilterDescriptor& filtdesc)
{
    if(!filtdesc.filterName || std::string(filtdesc.filterName) != "ampl")
        throw std::logic_error("Wrong filtdesc for ampl filter producer.");

    if(filtdesc.params.size() < 1)
        throw std::logic_error("Wrong params number for ampl filter producer.");

    try
    {
        double ampl = std::stod(filtdesc.params[0]);
        return new AmplFilter(ampl);
    }
    catch(const std::invalid_argument& e)
    {
        throw std::logic_error(
            "Error converting the 1st argument of ampl filter.");
    }
    catch(const std::out_of_range& e)
    {
        throw std::logic_error(
            "Out of range error converting the 1st argument of ampl filter.");
    }
}

IFilter* normalizeFilterCreator(const FilterDescriptor& filtdesc)
{
    if(!filtdesc.filterName || std::string(filtdesc.filterName) != "normalize")
        throw std::logic_error("Wrong filtdesc for normalize filter producer.");

    double peak = 1.0;
    if(!filtdesc.params.empty())
    {
        try
        {
            peak = std::stod(filtdesc.params[0]);
        }
        catch(const std::invalid_argument& e)
        {
            throw std::logic_error(
                "Error converting the argument of normalize filter.");
        }
        catch(const std::out_of_range& e)
        {
            throw std::logic_error("Out of range error converting the argument "
                                   "of normalize filter.");
        }
    }
    return new NormalizeFilter(peak);
}

IFilter* silenceFilterCreator(const FilterDescriptor& filtdesc)
{
    if(!filtdesc.filterName || std::string(filtdesc.filterName) != "silence")
        throw std::logic_error("Wrong filtdesc for silence filter producer.");

    if(filtdesc.params.size() < 3)
        throw std::logic_error(
            "Wrong params number for silence filter producer.");

    try
    {
        std::string unit = filtdesc.params[0];
        double start = std::stod(filtdesc.params[1]);
        double end = std::stod(filtdesc.params[2]);
        return new SilenceFilter(unit, start, end);
    }
    catch(const std::invalid_argument& e)
    {
        throw std::logic_error("Error converting arguments of silence filter.");
    }
    catch(const std::out_of_range& e)
    {
        throw std::logic_error(
            "Out of range error converting arguments of silence filter.");
    }
}

IFilter* timestretchFilterCreator(const FilterDescriptor& filtdesc)
{
    if(!filtdesc.filterName ||
       std::string(filtdesc.filterName) != "timestretch")
        throw std::logic_error(
            "Wrong filtdesc for timestretch filter producer.");

    if(filtdesc.params.size() < 1)
        throw std::logic_error(
            "Wrong params number for timestretch filter producer.");

    try
    {
        double factor = std::stod(filtdesc.params[0]);
        return new TimestretchFilter(factor);
    }
    catch(const std::invalid_argument& e)
    {
        throw std::logic_error(
            "Error converting the 1st argument of timestretch filter.");
    }
    catch(const std::out_of_range& e)
    {
        throw std::logic_error("Out of range error converting the 1st argument "
                               "of timestretch filter.");
    }
}

IFilter* lowpassFilterCreator(const FilterDescriptor& filtdesc)
{
    if(!filtdesc.filterName || std::string(filtdesc.filterName) != "lowpass")
        throw std::logic_error("Wrong filtdesc for lowpass filter producer.");

    if(filtdesc.params.size() < 1)
        throw std::logic_error(
            "Wrong params number for lowpass filter producer.");

    try
    {
        int windowSize = std::stoi(filtdesc.params[0]);
        return new LowpassFilter(windowSize);
    }
    catch(const std::invalid_argument& e)
    {
        throw std::logic_error(
            "Error converting the 1st argument of lowpass filter.");
    }
    catch(const std::out_of_range& e)
    {
        throw std::logic_error("Out of range error converting the 1st argument "
                               "of lowpass filter.");
    }
}

IFilter* generatorFilterCreator(const FilterDescriptor& filtdesc)
{
    if(!filtdesc.filterName || std::string(filtdesc.filterName) != "generator")
        throw std::logic_error("Wrong filtdesc for generator filter producer.");

    if(filtdesc.params.empty())
        throw std::logic_error(
            "Wrong params number for generator filter producer.");

    std::string type = filtdesc.params[0];

    try
    {
        if(type == "sin")
        {
            if(filtdesc.params.size() < 3)
                throw std::logic_error(
                    "Wrong params number for sin generator filter.");

            double freq = std::stod(filtdesc.params[1]);
            double duration = std::stod(filtdesc.params[2]);
            return new SinGenFilter(freq, duration);
        }
        else if(type == "am")
        {
            if(filtdesc.params.size() < 6)
                throw std::logic_error(
                    "Wrong params number for am generator filter.");

            double amp = std::stod(filtdesc.params[1]);
            double carrier = std::stod(filtdesc.params[2]);
            double mod = std::stod(filtdesc.params[3]);
            double depth = std::stod(filtdesc.params[4]);
            double duration = std::stod(filtdesc.params[5]);
            return new AmGenFilter(amp, carrier, mod, depth, duration);
        }
        else
        {
            throw std::logic_error("Unknown generator type: " + type);
        }
    }
    catch(const std::invalid_argument& e)
    {
        throw std::logic_error(
            "Error converting arguments of generator filter.");
    }
    catch(const std::out_of_range& e)
    {
        throw std::logic_error(
            "Out of range error converting arguments of generator filter.");
    }
}

}  // namespace FilterProducers