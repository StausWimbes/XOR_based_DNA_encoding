#ifndef XOR_BASED_DNA_ENCODING_ENCODE_H
#define XOR_BASED_DNA_ENCODING_ENCODE_H

#include <string>
#include <cstdint>
#include <fstream>

namespace xorCode {

    std::string &convertToNucleotides(uint8_t *buffer, uint32_t size);

    std::string &convertToHuffmanBase3(uint8_t *buffer, uint32_t size);

    std::string &convertHuffmanToNucleotides(std::string &intermediateCode);

}

#endif //XOR_BASED_DNA_ENCODING_ENCODE_H
