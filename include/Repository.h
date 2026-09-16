#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <string>

template <typename T>
class Repository {
private:
    std::vector<T> items;
    std::string filename;
public:
    // Initialises the repository's file path.
    Repository(const std::string& file) : filename(file) {
    }

    // Adds a copy of the item to the collection.
    void add(T& item) {
        items.push_back(item);
    }

    // Returns the collection by reference.
    std::vector<T>& getAll() {
        return items;
    }

};

#endif
