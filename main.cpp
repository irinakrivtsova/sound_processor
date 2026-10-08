#include "application.h"

#include <exception>
#include <iostream>

namespace
{
    constexpr int unhandledExceptionCode = -1;
    constexpr int unknownExceptionCode = -2;
}

int main(int argc, char* argv[])
{
    Application app;

    try
    {
        app.configure();
        return app.start(argc, argv);
    }
    catch (const std::exception& e)
    {
        std::cerr << "An exception handled: " << e.what() << '\n';
        return unhandledExceptionCode;
    }
    catch (...)
    {
        std::cerr << "An unknown exception\n";
        return unknownExceptionCode;
    }
}