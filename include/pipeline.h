#ifndef PIPELINE_H
#define PIPELINE_H

#include "descriptor.h"
#include "filters.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Pipeline
{
public:
    // Zero principle
    Pipeline() = default;
    ~Pipeline() = default;

    Pipeline(Pipeline&&) = default;
    Pipeline& operator=(Pipeline&&) = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void addFilter(std::unique_ptr<IFilter> filter)
    {
        _filters.push_back(std::move(filter));
    }
    bool apply(Waveform* sound);

private:
    // Zero principle
    std::vector<std::unique_ptr<IFilter>> _filters;
};

using FilterProducer = IFilter* (*)(const FilterDescriptor&);

class CmdLineArgs2PipelineConverter
{
public:
    Pipeline createPipeline(const std::vector<FilterDescriptor>& descriptors);
    void addFilterProducer(const std::string& filterName,
                           FilterProducer producer);
    FilterProducer getFilterProducer(const std::string& filterName);

private:
    std::map<std::string, FilterProducer> _producers;
};

namespace FilterProducers {
IFilter* amplFilterCreator(const FilterDescriptor& filtdesc);
IFilter* normalizeFilterCreator(const FilterDescriptor& filtdesc);
IFilter* silenceFilterCreator(const FilterDescriptor& filtdesc);
IFilter* timestretchFilterCreator(const FilterDescriptor& filtdesc);
IFilter* lowpassFilterCreator(const FilterDescriptor& filtdesc);
IFilter* generatorFilterCreator(const FilterDescriptor& filtdesc);
}  // namespace FilterProducers

#endif  // #ifndef PIPELINE_H