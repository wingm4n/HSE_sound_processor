#include "application.h"
#include <exception>
#include <iostream>

enum
{
    exceptionErrCode = 1,
    badExceptionErrCode = 2
};

int main(int argc, char* argv[])
{
    Application app;
    int res = 0;

    try
    {
        app.configure();
        res = app.start(argc, argv);
    }
    catch(const std::exception& e)
    {
        std::cerr << "An exception handled: " << e.what() << "\n";
        return exceptionErrCode;
    }
    catch(...)
    {
        std::cerr << "An unknown exception occurred.\n";
        return badExceptionErrCode;
    }

    return res;
}
