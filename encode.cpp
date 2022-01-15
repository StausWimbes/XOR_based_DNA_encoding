#include "encode.h"

#include <string>
#include <cstdint>

namespace xorCode {

    std::string &convertToNucleotides(uint8_t *buffer, uint32_t size) {
        return convertHuffmanToNucleotides(convertToHuffmanBase3(buffer, size));
    }

    std::string &convertToHuffmanBase3(uint8_t *buffer, uint32_t size) {
        return *new std::string();
    }

    std::string &convertHuffmanToNucleotides(std::string &intermediateCode) {
        return intermediateCode;
    }

}
