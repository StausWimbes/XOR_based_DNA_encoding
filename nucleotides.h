#ifndef XOR_BASED_DNA_ENCODING_NUCLEOTIDES_H
#define XOR_BASED_DNA_ENCODING_NUCLEOTIDES_H

#include <string>
#include <cstdint>
#include <fstream>
#include <memory>

namespace xorCode {

    char getNucleotide(uint32_t index);

    uint32_t getIndex(const char &nucleotide);

    void invertNucleotides(std::string &nucleotides);

    char getXOrNucleotide(const char &n1, const char &n2);

    std::shared_ptr<std::string> convertBinaryToNucleotides(char prev, uint8_t *buffer, uint32_t size);

    std::shared_ptr<std::string>
    convertNucleotidesToBinary(char prev, const std::string &nucleotides, uint32_t numOfBytes);

}

#endif //XOR_BASED_DNA_ENCODING_NUCLEOTIDES_H
