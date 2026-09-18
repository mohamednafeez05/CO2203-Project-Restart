#ifndef SAFE_FILE_H
#define SAFE_FILE_H

#include <string>
#include <utility>
#include <vector>

class SafeFile
{
public:
    // Each pair holds a destination filename and its new text.
    static void writeAll(
        const std::vector<std::pair<std::string, std::string>>& files);
};

#endif