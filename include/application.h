#ifndef APPLICATION_H
#define APPLICATION_H

#include "argsparser.h"
#include "descriptor.h"
#include "pipeline.h"

class Application
{
public:
    void configure();
    int start(int argc, char* argv[]);

protected:
    CmdLineArgs2PipelineConverter _cla2PipelineConverter;
};

#endif  // #ifndef APPLICATION_H
