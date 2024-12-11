#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <vector>
#include <string>

class Hash {
public:
    Hash(std::string& str, uint32_t base = 131, uint32_t mod = 1e9 + 7): base(base), mod(mod) {
        pre_hash.resize(str.size() + 1);
        pre_base.resize(str.size() + 1);
        hash_table.resize(str.size() + 1);

        pre_hash[0] = 0;
        pre_base[0] = 1;
        hash_table[0] = 0;

        for (size_t i = 0; i < str.size(); ++i) {
            pre_hash[i + 1] = (pre_hash[i] * base + str[i]) % mod;
            pre_base[i + 1] = pre_base[i] * base % mod;
        }
    }

    bool subHash(size_t l, size_t r) {
        return ((pre_hash[r] - pre_hash[l - 1] * pre_base[r - l]) % mod + mod) % mod;
    }
private:
    std::vector<uint32_t> hash_table;
    std::vector<uint32_t> pre_hash;
    std::vector<uint32_t> pre_base;
    uint32_t base;
    uint32_t mod;
};

#endif
