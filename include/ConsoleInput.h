#ifndef CONSOLE_INPUT_H
#define CONSOLE_INPUT_H

#include <string>

class ConsoleInput
{
public:
    // Reads a whole number within the given range.
    static int readInt(const std::string& prompt,
                       int minimum, int maximum);

    // Reads non-empty text and trims surrounding whitespace.
    static std::string readText(const std::string& prompt);
};

#endif