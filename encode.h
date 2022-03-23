#ifndef XOR_BASED_DNA_ENCODING_ENCODE_H
#define XOR_BASED_DNA_ENCODING_ENCODE_H

#include <string>
#include <cstdint>
#include <fstream>
#include <memory>

namespace xorCode {

    std::shared_ptr<std::string> convertBinaryToNucleotides(char prev, uint8_t *buffer, uint32_t size);

    std::shared_ptr<std::string> convertNucleotidesToBinary(char prev, const std::string &nucleotides);

}

#endif //XOR_BASED_DNA_ENCODING_ENCODE_H
