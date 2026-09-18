#include "SafeFile.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

std::string readFile(const std::string& filename)
{
    std::ifstream input(filename);

    if (!input)
        throw std::runtime_error("Cannot read test file.");

    return std::string(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>());
}

void check(bool condition, const std::string& message)
{
    if (!condition)
        throw std::runtime_error(message);
}

int main()
{
    namespace fs = std::filesystem;

    try
    {
        const std::string directory = "data/safe_save_test/";

        if (fs::exists(directory))
        {
            throw std::runtime_error(
                "Use a fresh data/safe_save_test folder.");
        }

        fs::create_directories(directory);

        const std::string first = directory + "first.txt";
        const std::string second = directory + "second.txt";

        SafeFile::writeAll({
            {first, "OLD FIRST\n"},
            {second, "OLD SECOND\n"}
        });

        SafeFile::writeAll({
            {first, "NEW FIRST\n"},
            {second, "NEW SECOND\n"}
        });

        check(
            readFile(first) == "NEW FIRST\n" &&
            readFile(second) == "NEW SECOND\n" &&
            readFile(first + ".bak") == "OLD FIRST\n" &&
            readFile(second + ".bak") == "OLD SECOND\n",
            "Saved files or backups are incorrect.");

        std::cout
            << "PASS: new files saved; previous contents backed up.\n";

        bool rejected = false;

        try
        {
            SafeFile::writeAll({
                {first, "SHOULD NOT REPLACE\n"},
                {directory + "missing/second.txt", "FAIL\n"}
            });
        }
        catch (const std::exception&)
        {
            rejected = true;
        }

        check(
            rejected &&
            readFile(first) == "NEW FIRST\n" &&
            readFile(second) == "NEW SECOND\n" &&
            !fs::exists(first + ".tmp"),
            "Failed staging changed data or left its temporary file.");

        std::cout
            << "PASS: failed staging preserved existing data.\n";

        // Simulates a temporary file left by an earlier save.
        {
            std::ofstream leftover(first + ".tmp");
            leftover << "UNFINISHED\n";
        }

        rejected = false;

        try
        {
            SafeFile::writeAll({
                {first, "SHOULD NOT REPLACE\n"}
            });
        }
        catch (const std::exception&)
        {
            rejected = true;
        }

        check(
            rejected &&
            readFile(first) == "NEW FIRST\n" &&
            readFile(first + ".tmp") == "UNFINISHED\n",
            "An unfinished save was overwritten.");

        fs::remove(first + ".tmp");

        std::cout << "PASS: unfinished save blocked.\n";

        // Restores the previous versions from their backups.
        fs::copy_file(
            first + ".bak", first,
            fs::copy_options::overwrite_existing);

        fs::copy_file(
            second + ".bak", second,
            fs::copy_options::overwrite_existing);

        check(
            readFile(first) == "OLD FIRST\n" &&
            readFile(second) == "OLD SECOND\n",
            "Backup recovery failed.");

        std::cout << "PASS: both backups restored.\n";
        std::cout << "All safe-save checks passed.\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }

    return 0;
}