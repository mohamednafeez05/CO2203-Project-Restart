#ifndef SYSTEM_STORAGE_H
#define SYSTEM_STORAGE_H
#include <string>
class UniversitySystem;
// Stores one complete snapshot, rebuilt in isolation before replacing live objects.
class SystemStorage
{
public:
    static void save(const UniversitySystem& system, const std::string& directory);
    static void load(UniversitySystem& system, const std::string& directory);
};
#endif
