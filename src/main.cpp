#include "AttendanceConsole.h"

#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr << "Usage: mainApp.exe [data-directory]\n";
        return 1;
    }

    try
    {
        const std::string directory =
            argc == 2 ? argv[1] : "data";

        AttendanceConsole app(directory);
        app.run();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Application stopped: "
                  << error.what() << '\n';

        return 1;
    }

    return 0;
}