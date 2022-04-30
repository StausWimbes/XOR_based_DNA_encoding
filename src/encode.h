#ifndef XOR_CODE_STRANDS_H
#define XOR_CODE_STRANDS_H

#include "settings.h"
#include "nucleotideUtils.h"

#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <list>

namespace xorCode {

    struct strand {
        types::nucleotides payload;
        union {
            uint64_t lowAddress;
            std::array<uint8_t, 8> lowAddressArray;
        };
        bool xOr;
        types::nucleotides *primerBegin;
        types::nucleotides *primerEnd;
    };

    std::vector<types::nucleotides> encode(const std::string &key, std::ifstream &input);

    std::vector<types::nucleotides> encode(const std::string &filePath);

}

#endif //XOR_CODE_STRANDS_H
