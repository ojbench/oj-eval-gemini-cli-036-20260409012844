#ifndef DYNAMIC_BITSET_HPP
#define DYNAMIC_BITSET_HPP

#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <string>

struct dynamic_bitset {
    dynamic_bitset() = default;
    ~dynamic_bitset() = default;
    dynamic_bitset(const dynamic_bitset &) = default;
    dynamic_bitset &operator = (const dynamic_bitset &) = default;
    dynamic_bitset(std::size_t n) {}
    dynamic_bitset(const std::string &str) {}
    bool operator [] (std::size_t n) const { return false; }
    dynamic_bitset &set(std::size_t n, bool val = true) { return *this; }
    dynamic_bitset &push_back(bool val) { return *this; }
    bool none() const { return true; }
    bool all()  const { return false; }
    std::size_t size() const { return 0; }
    dynamic_bitset &operator |= (const dynamic_bitset &) { return *this; }
    dynamic_bitset &operator &= (const dynamic_bitset &) { return *this; }
    dynamic_bitset &operator ^= (const dynamic_bitset &) { return *this; }
    dynamic_bitset &operator <<= (std::size_t n) { return *this; }
    dynamic_bitset &operator >>= (std::size_t n) { return *this; }
    dynamic_bitset &set     () { return *this; }
    dynamic_bitset &flip    () { return *this; }
    dynamic_bitset &reset   () { return *this; }
};

#endif
