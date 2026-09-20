#ifndef OBJECT_LOOKUP_H
#define OBJECT_LOOKUP_H
#include <memory>
#include <string>
#include <vector>
template<class T, class Key>
T* findObject(const std::vector<std::unique_ptr<T>>& objects, const std::string& id, Key key)
{
    for (const auto& object : objects) if (key(*object) == id) return object.get();
    return nullptr;
}
#endif
