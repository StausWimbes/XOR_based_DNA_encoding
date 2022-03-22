#ifndef XOR_BASED_DNA_ENCODING_ENCODE_H
#define XOR_BASED_DNA_ENCODING_ENCODE_H

#include <string>
#include <cstdint>
#include <fstream>

namespace xorCode {

    std::string &convertBinaryToNucleotides(char prev, uint8_t *buffer, uint32_t size);

    std::string &convertBinaryToBase3(uint8_t *buffer, uint32_t size);

    std::string &convertBase3ToNucleotides(char prev, const std::string &intermediateCode);

    std::string &convertNucleotidesToBinary(char prev, const std::string &nucleotides);

    std::string &convertNucleotidesToBase3(char prev, const std::string &nucleotides);

    std::string &convertBase3ToBinary(const std::string &intermediateCode);

}

#endif //XOR_BASED_DNA_ENCODING_ENCODE_H
