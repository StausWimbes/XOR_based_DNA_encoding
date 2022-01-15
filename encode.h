//
// Created by lucas on 15.01.2022.
//

#ifndef XOR_BASED_DNA_ENCODING_ENCODE_H
#define XOR_BASED_DNA_ENCODING_ENCODE_H

#include <string>
#include <cstdint>
#include <fstream>

std::string convertToNucleotides(uint8_t* buffer, uint32_t size);

std::string convertToNucleotides(std::ifstream buffer, uint32_t size);

#endif //XOR_BASED_DNA_ENCODING_ENCODE_H
