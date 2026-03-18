#pragma once
#include <array>
#include <bitset>
#include <stdexcept>

namespace VEOEngine {

template<typename T, size_t N>
class ObjectPool {
public:
    T* acquire() {
        for (size_t i = 0; i < N; ++i) {
            if (!used_[i]) {
                used_[i] = true;
                return &pool_[i];
            }
        }
        throw std::bad_alloc();
    }
    
    void release(T* ptr) {
        size_t idx = ptr - pool_.data();
        if (idx < N) used_[idx] = false;
    }
    
private:
    std::array<T, N> pool_;
    std::bitset<N> used_;
};

} // namespace VEOEngine
