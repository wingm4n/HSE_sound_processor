#include "argsparser.h"
#include <cctype>
#include <getopt.h>

// For getopt_long
extern char* optarg;
extern int optind, opterr, optopt;

ArgsParser::Result ArgsParser::parse(int argc, char* argv[])
{
    if(argc == 1)
        return ArgsParser::Result::noArgs;

    // Supported options
    static struct option longopts[] = {{"help", no_argument, NULL, 'h'},
                                       {"input", required_argument, NULL, 'i'},
                                       {"output", required_argument, NULL, 'o'},
                                       {"filter", required_argument, NULL, 'f'},
                                       {NULL, 0, NULL, 0}};

    ArgsParser::Result res = ArgsParser::Result::ok;
    int opt;
    while((opt = getopt_long(argc, argv, "hi:o:f:", longopts, NULL)) != -1)
    {
        switch(opt)
        {
        case 'h':
            _printHelp = true;
            break;
        case 'i':
            _inFileName = optarg;
            break;
        case 'o':
            _outFileName = optarg;
            break;
        case 'f':
        {
            // Parse filter info
            FilterDescriptor curDescriptor;
            curDescriptor.filterName = optarg;

            while(optind < argc &&
                  (argv[optind][0] != '-' ||
                   (argv[optind][0] == '-' && argv[optind][1] != '\0' &&
                    std::isdigit(argv[optind][1]))))
            {
                curDescriptor.params.push_back(argv[optind]);
                ++optind;
            }
            _filterDescriptors.push_back(curDescriptor);
            break;
        }
        default:
            res = ArgsParser::Result::badArgs;
            break;
        }
    }
    return res;
};
