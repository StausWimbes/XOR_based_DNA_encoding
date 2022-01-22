#include "encode.h"
#include "huffmanCode.h"

#include <string>
#include <cstdint>

namespace xorCode {

    char *order = new char[4];
    bool init = false;

    std::string &convertBinaryToNucleotides(uint8_t *buffer, uint32_t size) {
        return convertBase3ToNucleotides('A', convertBinaryToBase3(buffer, size));
    }

    std::string &convertBinaryToBase3(uint8_t *buffer, uint32_t size) {
        auto encoder = HuffmanCode::getInstance();
        auto result = new std::string;
        for (int i = 0; i < size; ++i) {
            (*result) += encoder->encodeByte(buffer[i]);
        }
        return *result;
    }

    char getNucleotide(const char &ternary, const char &prev) {
        int prevIndex;
        switch (prev) {
            case 'A':
                prevIndex = 1;
                break;
            case 'C':
                prevIndex = 2;
                break;
            case 'G':
                prevIndex = 3;
                break;
            case 'T':
                prevIndex = 0;
                break;
            default:
                prevIndex = 1;
        }
        prevIndex += std::stoi(&ternary);
        prevIndex %= 4;
        return order[prevIndex];
    }

    std::string &convertBase3ToNucleotides(const char prev, const std::string &intermediateCode) {
        if (!init) {
            init = true;
            order[0] = 'A';
            order[1] = 'C';
            order[2] = 'G';
            order[3] = 'T';
        }
        auto result = new std::string();
        auto curPrev = prev;
        for (char t: intermediateCode) {
            curPrev = getNucleotide(t, curPrev);
            result->append(sizeof(char), curPrev);
        }
        return *result;
    }

}
