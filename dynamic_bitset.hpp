#ifndef DYNAMIC_BITSET_HPP
#define DYNAMIC_BITSET_HPP

#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <string>

namespace sjtu {

struct dynamic_bitset {
    std::vector<uint64_t> blocks;
    std::size_t num_bits;

    dynamic_bitset() : num_bits(0) {}
    ~dynamic_bitset() = default;
    dynamic_bitset(const dynamic_bitset &) = default;
    dynamic_bitset &operator = (const dynamic_bitset &) = default;

    dynamic_bitset(std::size_t n) : num_bits(n) {
        blocks.resize((n + 63) / 64, 0);
    }

    dynamic_bitset(const std::string &str) {
        num_bits = str.length();
        blocks.resize((num_bits + 63) / 64, 0);
        for (std::size_t i = 0; i < num_bits; ++i) {
            if (str[i] == '1') {
                blocks[i / 64] |= (1ULL << (i % 64));
            }
        }
    }

    bool operator [] (std::size_t n) const {
        if (n >= num_bits) return false;
        return (blocks[n / 64] >> (n % 64)) & 1;
    }

    dynamic_bitset &set(std::size_t n, bool val = true) {
        if (n < num_bits) {
            if (val) {
                blocks[n / 64] |= (1ULL << (n % 64));
            } else {
                blocks[n / 64] &= ~(1ULL << (n % 64));
            }
        }
        return *this;
    }

    dynamic_bitset &push_back(bool val) {
        if (num_bits % 64 == 0) {
            blocks.push_back(0);
        }
        if (val) {
            blocks[num_bits / 64] |= (1ULL << (num_bits % 64));
        }
        num_bits++;
        return *this;
    }

    bool none() const {
        if (num_bits == 0) return true;
        std::size_t full_blocks = num_bits / 64;
        for (std::size_t i = 0; i < full_blocks; ++i) {
            if (blocks[i] != 0) return false;
        }
        std::size_t rem = num_bits % 64;
        if (rem > 0) {
            uint64_t mask = (1ULL << rem) - 1;
            if ((blocks.back() & mask) != 0) return false;
        }
        return true;
    }

    bool all() const {
        if (num_bits == 0) return true;
        std::size_t full_blocks = num_bits / 64;
        for (std::size_t i = 0; i < full_blocks; ++i) {
            if (blocks[i] != ~0ULL) return false;
        }
        std::size_t rem = num_bits % 64;
        if (rem > 0) {
            uint64_t mask = (1ULL << rem) - 1;
            if ((blocks.back() & mask) != mask) return false;
        }
        return true;
    }

    std::size_t size() const {
        return num_bits;
    }

    dynamic_bitset &operator |= (const dynamic_bitset &other) {
        std::size_t min_blocks = std::min(blocks.size(), other.blocks.size());
        for (std::size_t i = 0; i < min_blocks; ++i) {
            blocks[i] |= other.blocks[i];
        }
        _clean_unused_bits();
        return *this;
    }

    dynamic_bitset &operator &= (const dynamic_bitset &other) {
        std::size_t min_blocks = std::min(blocks.size(), other.blocks.size());
        for (std::size_t i = 0; i < min_blocks; ++i) {
            blocks[i] &= other.blocks[i];
        }
        for (std::size_t i = min_blocks; i < blocks.size(); ++i) {
            blocks[i] = 0;
        }
        _clean_unused_bits();
        return *this;
    }

    dynamic_bitset &operator ^= (const dynamic_bitset &other) {
        std::size_t min_blocks = std::min(blocks.size(), other.blocks.size());
        for (std::size_t i = 0; i < min_blocks; ++i) {
            blocks[i] ^= other.blocks[i];
        }
        _clean_unused_bits();
        return *this;
    }

    dynamic_bitset &operator <<= (std::size_t n) {
        if (n == 0) return *this;
        if (n >= num_bits) {
            reset();
            return *this;
        }
        std::size_t block_shift = n / 64;
        std::size_t bit_shift = n % 64;
        if (bit_shift == 0) {
            for (std::size_t i = blocks.size(); i > block_shift; --i) {
                blocks[i - 1] = blocks[i - 1 - block_shift];
            }
        } else {
            for (std::size_t i = blocks.size(); i > block_shift; --i) {
                blocks[i - 1] = blocks[i - 1 - block_shift] << bit_shift;
                if (i - 1 - block_shift > 0) {
                    blocks[i - 1] |= blocks[i - 2 - block_shift] >> (64 - bit_shift);
                }
            }
        }
        for (std::size_t i = 0; i < block_shift; ++i) {
            blocks[i] = 0;
        }
        _clean_unused_bits();
        return *this;
    }

    dynamic_bitset &operator >>= (std::size_t n) {
        if (n == 0) return *this;
        if (n >= num_bits) {
            reset();
            return *this;
        }
        std::size_t block_shift = n / 64;
        std::size_t bit_shift = n % 64;
        if (bit_shift == 0) {
            for (std::size_t i = 0; i < blocks.size() - block_shift; ++i) {
                blocks[i] = blocks[i + block_shift];
            }
        } else {
            for (std::size_t i = 0; i < blocks.size() - block_shift; ++i) {
                blocks[i] = blocks[i + block_shift] >> bit_shift;
                if (i + block_shift + 1 < blocks.size()) {
                    blocks[i] |= blocks[i + block_shift + 1] << (64 - bit_shift);
                }
            }
        }
        for (std::size_t i = blocks.size() - block_shift; i < blocks.size(); ++i) {
            blocks[i] = 0;
        }
        _clean_unused_bits();
        return *this;
    }

    dynamic_bitset &set() {
        for (auto &block : blocks) {
            block = ~0ULL;
        }
        _clean_unused_bits();
        return *this;
    }

    dynamic_bitset &flip() {
        for (auto &block : blocks) {
            block = ~block;
        }
        _clean_unused_bits();
        return *this;
    }

    dynamic_bitset &reset() {
        for (auto &block : blocks) {
            block = 0;
        }
        return *this;
    }

private:
    void _clean_unused_bits() {
        std::size_t rem = num_bits % 64;
        if (rem > 0 && !blocks.empty()) {
            blocks.back() &= (1ULL << rem) - 1;
        }
    }
};

} // namespace sjtu

#endif