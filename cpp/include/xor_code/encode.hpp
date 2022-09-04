#ifndef XOR_CODE_ENCODE_H
#define XOR_CODE_ENCODE_H

#include "settings.hpp"
#include "nucleotideUtils.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <list>
#include <array>

namespace xor_code {

    // TODO: add some documentation LEON

    struct strand {
        types::nucleotides payload;
        union {
            uint64_t lowAddress;
            std::array<uint8_t, 8> lowAddressArray;
        };
        bool isXor;
        types::nucleotides *primerBegin;
        types::nucleotides *primerEnd;
    };

    std::vector<types::nucleotides> encode(const std::string &key, types::bytes &bytes);

    std::vector<types::nucleotides> encode(const std::string &key, std::istream &input);

    std::vector<types::nucleotides> encode(const std::string &filePath);

}

#endif //XOR_CODE_ENCODE_H
