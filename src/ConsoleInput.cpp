#include "ConsoleInput.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

// Repeats until a valid whole number is entered.
int ConsoleInput::readInt(const std::string& prompt,
                          int minimum, int maximum)
{
    if (minimum > maximum)
    {
        throw std::invalid_argument("Invalid input range.");
    }

    while (true)
    {
        std::cout << prompt;

        std::string line;
        if (!std::getline(std::cin, line))
        {
            throw std::runtime_error("Console input ended.");
        }

        std::istringstream input(line);
        int value;

        if (input >> value)
        {
            input >> std::ws;

            if (input.eof() && value >= minimum && value <= maximum)
            {
                return value;
            }
        }

        std::cout << "Enter a whole number from "
                  << minimum << " to " << maximum << ".\n";
    }
}

// Repeats until non-empty text is entered.
std::string ConsoleInput::readText(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt;

        std::string text;
        if (!std::getline(std::cin, text))
        {
            throw std::runtime_error("Console input ended.");
        }

        const auto first = text.find_first_not_of(" \t\r");

        if (first != std::string::npos)
        {
            const auto last = text.find_last_not_of(" \t\r");
            return text.substr(first, last - first + 1);
        }

        std::cout << "Input cannot be empty.\n";
    }
}