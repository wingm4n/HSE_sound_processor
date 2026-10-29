#include "application.h"
#include "wavestream.h"
#include <iostream>

void Application::configure()
{
    _cla2PipelineConverter.addFilterProducer(
        "ampl", FilterProducers::amplFilterCreator);
    _cla2PipelineConverter.addFilterProducer(
        "normalize", FilterProducers::normalizeFilterCreator);
    _cla2PipelineConverter.addFilterProducer(
        "silence", FilterProducers::silenceFilterCreator);
    _cla2PipelineConverter.addFilterProducer(
        "timestretch", FilterProducers::timestretchFilterCreator);
    _cla2PipelineConverter.addFilterProducer(
        "lowpass", FilterProducers::lowpassFilterCreator);
    _cla2PipelineConverter.addFilterProducer(
        "generator", FilterProducers::generatorFilterCreator);
}

int Application::start(int argc, char* argv[])
{
    ArgsParser parser;
    ArgsParser::Result res = parser.parse(argc, argv);

    if(res == ArgsParser::Result::badArgs)
    {
        std::cerr << "Error: Incorrect arguments.\n";
        res = ArgsParser::Result::noArgs;
    }
    if(res == ArgsParser::Result::noArgs || parser.getHelpFlag())
    {
        std::cout << "Usage: " << argv[0]
                  << " -i <input.wav> -o <output.wav> -f "
                     "<filter> [params...]\n";
        return 0;
    }

    // Read sound info into this form
    Waveform audio;

    if(parser.getInFileName() != nullptr)
    {
        WaveReader reader;
        std::cout << "Reading input file: " << parser.getInFileName()
                  << "...\n";
        audio = reader.read(parser.getInFileName());
    }
    else
    {
        std::cout << "No input file. Creating empty waveform.\n";
    }

    const std::vector<FilterDescriptor>& fds = parser.getFilterDescriptors();
    if(!fds.empty())
    {
        std::cout << "Applying Pipeline...\n";

        Pipeline pipeline = _cla2PipelineConverter.createPipeline(fds);

        if(!pipeline.apply(&audio))
        {
            std::cerr << "Error occurred during pipeline application.\n";
            return -1;
        }
    }

    if(parser.getOutFileName() != nullptr)
    {
        WaveWriter writer;
        std::cout << "Writing output file: " << parser.getOutFileName()
                  << "...\n";
        writer.write(parser.getOutFileName(), audio);
    }
    else
    {
        std::cerr
            << "Warning: No output file specified. Result will not be saved.\n";
    }

    std::cout << "Processing completed.\n";
    return 0;
}
