#include "SafeFile.h"

#include <filesystem>
#include <fstream>
#include <set>
#include <stdexcept>

void SafeFile::writeAll(
    const std::vector<std::pair<std::string, std::string>>& files)
{
    namespace fs = std::filesystem;

    struct Entry
    {
        fs::path target, temporary, backup;
        bool existed = false;
        bool staged = false;
        bool touched = false;
    };

    std::vector<Entry> entries;
    std::set<fs::path> paths;

    // Check every path before creating temporary files.
    for (const auto& file : files)
    {
        Entry entry;
        entry.target = fs::absolute(file.first).lexically_normal();
        entry.temporary = entry.target.string() + ".tmp";
        entry.backup = entry.target.string() + ".bak";

        for (const fs::path& path :
             {entry.target, entry.temporary, entry.backup})
        {
            if (!paths.insert(path).second || fs::is_symlink(path))
            {
                throw std::runtime_error(
                    "Conflicting or linked save path: " + path.string());
            }

            if (fs::exists(path) && !fs::is_regular_file(path))
            {
                throw std::runtime_error(
                    "Save path is not a regular file: " + path.string());
            }
        }

        if (fs::exists(entry.temporary))
        {
            throw std::runtime_error(
                "Unfinished save file exists: " +
                entry.temporary.string());
        }

        entry.existed = fs::exists(entry.target);
        entries.push_back(entry);
    }

    try
    {
        // Finish all new files before touching existing data.
        for (std::size_t i = 0; i < entries.size(); ++i)
        {
            Entry& entry = entries[i];
            entry.staged = true;

            std::ofstream output(entry.temporary);

            if (!output)
                throw std::runtime_error(
                    "Cannot create temporary save file.");

            output << files[i].second;
            output.close();

            if (!output)
                throw std::runtime_error(
                    "Failed to finish temporary save file.");
        }

        // Back up every existing file before replacing any.
        for (const Entry& entry : entries)
        {
            if (entry.existed)
            {
                fs::copy_file(
                    entry.target,
                    entry.backup,
                    fs::copy_options::overwrite_existing);
            }
        }

        for (Entry& entry : entries)
        {
            entry.touched = true;

            if (entry.existed)
                fs::remove(entry.target);

            fs::rename(entry.temporary, entry.target);
        }
    }
    catch (...)
    {
        bool restored = true;

        // Restore any destination whose replacement began.
        for (const Entry& entry : entries)
        {
            if (!entry.touched)
                continue;

            std::error_code error;

            if (entry.existed)
            {
                fs::copy_file(
                    entry.backup,
                    entry.target,
                    fs::copy_options::overwrite_existing,
                    error);
            }
            else
            {
                fs::remove(entry.target, error);
            }

            if (error)
                restored = false;
        }

        if (!restored)
        {
            throw std::runtime_error(
                "Save and rollback failed. "
                "Stop the app and recover the .bak files.");
        }

        // Remove temporary files created by this attempt.
        for (const Entry& entry : entries)
        {
            if (entry.staged)
            {
                std::error_code ignored;
                fs::remove(entry.temporary, ignored);
            }
        }

        throw;
    }
}