#include "UniversityConsole.h"
#include <filesystem>
#include <iostream>
int main(int argc, char* argv[])
{
    try
    {
        if (argc == 3 && std::string(argv[1]) == "--init-demo")
        {
            if (std::filesystem::exists(argv[2])) throw std::runtime_error("Demo destination must be a new folder.");
            UniversitySystem system; system.initialiseDemo(); system.save(argv[2]);
            std::cout << "Demo created. Run with that folder as the argument.\n"; return 0;
        }
        UniversityConsole console(argc > 1 ? argv[1] : "data"); console.run();
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n'; return 1;
    }
}
