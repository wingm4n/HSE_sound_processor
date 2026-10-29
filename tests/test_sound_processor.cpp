#include <catch2/catch_test_macros.hpp>

#include "argsparser.h"
#include "filters.h"
#include "pipeline.h"
#include "waveform.h"
#include "wavestream.h"

extern int optind;

TEST_CASE("ArgsParser correctly parses command line arguments", "argsparser") {
    optind = 1;
    ArgsParser parser;
    char arg0[] = "sound_processor";
    char arg1[] = "-i";
    char arg2[] = "input.wav";
    char arg3[] = "-o";
    char arg4[] = "output.wav";
    char arg5[] = "-f";
    char arg6[] = "ampl";
    char arg7[] = "0.8";
    char arg8[] = "-f";
    char arg9[] = "silence";
    char arg10[] = "sec";
    char arg11[] = "1";
    char arg12[] = "2";
    char* argv[] = {arg0, arg1, arg2, arg3,  arg4,  arg5, arg6,
                    arg7, arg8, arg9, arg10, arg11, arg12};
    int argc = 13;

    auto res = parser.parse(argc, argv);

    REQUIRE(res == ArgsParser::Result::ok);
    REQUIRE(std::string(parser.getInFileName()) == "input.wav");
    REQUIRE(std::string(parser.getOutFileName()) == "output.wav");

    auto descriptors = parser.getFilterDescriptors();
    REQUIRE(descriptors.size() == 2);
    REQUIRE(std::string(descriptors[0].filterName) == "ampl");
    REQUIRE(descriptors[0].params[0] == std::string("0.8"));
    REQUIRE(std::string(descriptors[1].filterName) == "silence");
    REQUIRE(descriptors[1].params.size() == 3);
}

TEST_CASE("Waveform correct size calculation", "waveform") {
    Waveform wave;
    wave.samples.assign(100, 0);

    REQUIRE(wave.getDataSizeBlocks() == 100);
    REQUIRE(wave.getDataSizeByte() == 200);
}

TEST_CASE("AmplFilter scale samples", "filters") {
    Waveform wave;
    wave.samples = {1000, -1000, 20000, -30000};

    AmplFilter filter(2.0);
    REQUIRE(filter.apply(&wave) == true);

    REQUIRE(wave.samples[0] == 2000);
    REQUIRE(wave.samples[1] == -2000);
    REQUIRE(wave.samples[2] == 32767);   // Cut max
    REQUIRE(wave.samples[3] == -32768);  // CUt min
}

TEST_CASE("NormalizeFilter scales max peak", "filters") {
    Waveform wave;
    wave.samples = {0, 10000, -20000};

    NormalizeFilter filter(1.0);
    REQUIRE(filter.apply(&wave) == true);

    // Scale = 32767 / 20000 = 1.63835
    // 10000 * 1.63835 = 16383.5 -> 16383
    REQUIRE(wave.samples[1] == 16383);
    REQUIRE(wave.samples[2] == -32767);
}

TEST_CASE("SilenceFilter correctly inserts zero samples", "filters") {
    Waveform wave;
    wave.samples = {100, 200, 300};
    wave.fmtChunk.frequency = 1000;

    SilenceFilter filter("ms", 1.0, 2.0);
    REQUIRE(filter.apply(&wave) == true);

    REQUIRE(wave.samples.size() == 5);
    REQUIRE(wave.samples[0] == 100);
    REQUIRE(wave.samples[1] == 0);
    REQUIRE(wave.samples[2] == 0);
    REQUIRE(wave.samples[3] == 200);
}

TEST_CASE("Pipeline applies multiple filters", "pipeline") {
    CmdLineArgs2PipelineConverter conv;
    conv.addFilterProducer("ampl", FilterProducers::amplFilterCreator);

    FilterDescriptor fd1, fd2;
    fd1.filterName = "ampl";
    char param1[] = "0.5";
    fd1.params.push_back(param1);
    fd2.filterName = "ampl";
    char param2[] = "0.5";
    fd2.params.push_back(param2);

    std::vector<FilterDescriptor> fds = {fd1, fd2};
    Pipeline pipeline = conv.createPipeline(fds);

    Waveform wave;
    wave.samples = {10000};

    REQUIRE(pipeline.apply(&wave) == true);
    // (10000 * 0.5 * 0.5) = 2500
    REQUIRE(wave.samples[0] == 2500);
}

TEST_CASE("CmdLineArgs2PipelineConverter throws exceptions", "converter") {
    CmdLineArgs2PipelineConverter conv;
    conv.addFilterProducer("ampl", FilterProducers::amplFilterCreator);

    // Unknown filter
    FilterDescriptor fdUnknown;
    fdUnknown.filterName = "non_existent_filter";
    REQUIRE_THROWS_AS(conv.createPipeline({fdUnknown}), std::invalid_argument);

    // Wrong params
    FilterDescriptor fdBadParam;
    fdBadParam.filterName = "ampl";
    fdBadParam.params.push_back("not_a_number");
    REQUIRE_THROWS_AS(conv.createPipeline({fdBadParam}), std::logic_error);

    // Wron number of params
    FilterDescriptor fdNoParam;
    fdNoParam.filterName = "ampl";
    // empty params
    REQUIRE_THROWS_AS(conv.createPipeline({fdNoParam}), std::logic_error);
}

TEST_CASE("WaveReader throws on missing file", "wavestream") {
    WaveReader reader;
    // Wrong input file
    REQUIRE_THROWS_AS(reader.read("definitely_non_existent_file.wav"), WavestreamWrongFile);
}

TEST_CASE("TimestretchFilter correctly processes samples", "filters") {
    Waveform wave;
    wave.samples = {100, 200};

    // round(2 * 2.0) = 4
    TimestretchFilter filter(2.0);
    REQUIRE(filter.apply(&wave) == true);

    REQUIRE(wave.samples.size() == 4);
    REQUIRE(wave.samples[0] == 100);  // pos=0.0 -> l=0, frac=0
    REQUIRE(wave.samples[1] == 150);  // pos=0.5 -> l=0, frac=0.5 -> 100*0.5 + 200*0.5
    REQUIRE(wave.samples[2] == 200);  // pos=1.0 -> l=1, frac=0
    REQUIRE(wave.samples[3] == 200);  // pos=1.5 -> l=1
}

TEST_CASE("LowpassFilter applies itself correctly", "filters") {
    Waveform wave;
    wave.samples = {30, 90, 30};

    LowpassFilter filter(3);
    REQUIRE(filter.apply(&wave) == true);

    // avg(30, 30, 90) = 150 / 3 = 50
    REQUIRE(wave.samples[0] == 50);

    // avg(30, 90, 30) = 150 / 3 = 50
    REQUIRE(wave.samples[1] == 50);

    // avg(90, 30, 30) = 150 / 3 = 50
    REQUIRE(wave.samples[2] == 50);
}

TEST_CASE("SinGenFilter overwrites waveform and builds sine wave", "filters") {
    Waveform wave;
    wave.samples = {1000, 2000, 3000};

    SinGenFilter filter(440.0, 10.0);
    REQUIRE(filter.apply(&wave) == true);

    REQUIRE(wave.samples.size() == 441);

    REQUIRE(wave.fmtChunk.frequency == 44100);

    REQUIRE(wave.samples[0] == 0);

    REQUIRE(wave.samples[1] != 0);
}