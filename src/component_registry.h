#pragma once

#include "util/sparse_set.h"

template <typename T>
class ComponentRegistry {
private:
    static inline SparseSet<T> storage;
public:
    static SparseSet<T>& get() { return storage; }
};