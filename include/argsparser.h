#ifndef ARGSPARSER_H
#define ARGSPARSER_H

#include "descriptor.h"
#include <vector>

class ArgsParser
{
public:
    enum class Result
    {
        ok,
        noArgs,  // empty arguments sequence
        badArgs  // incorrect arguments sequence
    };

    // clang-format off
    ArgsParser()
        : _inFileName(nullptr)
        , _outFileName(nullptr)
        , _printHelp(false) {};
    Result parse(int argc, char* argv[]);
    // clang-format on

public:
    const char* getInFileName() const { return _inFileName; }
    const char* getOutFileName() const { return _outFileName; }
    const std::vector<FilterDescriptor>& getFilterDescriptors() const
    {
        return _filterDescriptors;
    }
    bool getHelpFlag() const { return _printHelp; };

private:
    char* _inFileName;
    char* _outFileName;
    bool _printHelp;
    std::vector<FilterDescriptor> _filterDescriptors;
};

#endif  // #ifndef ARGSPARSER_H
