#pragma once

#include <vector>
#include <cstdint> 
#include <algorithm>

template<typename T>
class SparseSet {
private:
    std::vector<uint32_t> sparse;
    std::vector<T> packed;

public:
    void insert(uint32_t id, const T& value) {
        if (id >= sparse.size()) {
            sparse.resize(id + 1, UINT32_MAX); // Using UINT32_MAX as a sentinel for non-existent entries
        }

        sparse[id] = static_cast<uint32_t>(packed.size());
        packed.push_back(value);
    }

    void erase(uint32_t id) {
        if (id < sparse.size() && sparse[id] != UINT32_MAX) {
            uint32_t last = static_cast<uint32_t>(packed.size()) - 1;
            uint32_t idx_to_remove = sparse[id];

            if (idx_to_remove != last) {
                std::swap(packed[idx_to_remove], packed[last]);
                sparse[packed[idx_to_remove]] = idx_to_remove;
            }

            packed.pop_back();
            sparse[id] = UINT32_MAX;
        }
    }

    bool contains(uint32_t id) const {
        return id < sparse.size() && sparse[id] != UINT32_MAX;
    }

    T& get(uint32_t id) {
        return packed[sparse[id]];
    }

    const T& get(uint32_t id) const {
        return packed[sparse[id]];
    }

    // Iteration support
    typename std::vector<T>::iterator begin() {
        return packed.begin();
    }

    typename std::vector<T>::iterator end() {
        return packed.end();
    }

    typename std::vector<T>::const_iterator begin() const {
        return packed.begin();
    }

    typename std::vector<T>::const_iterator end() const {
        return packed.end();
    }

    // You can also add cbegin, cend, etc. for more comprehensive iterator support.
};

    // Other utility functions like size(), begin(), end(), etc. can be added
};