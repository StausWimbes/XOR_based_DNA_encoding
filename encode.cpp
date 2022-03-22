#include "encode.h"
#include "huffmanCode.h"

#include <string>
#include <cstdint>
#include <iostream>

namespace xorCode {

    char *order = new char[]{'A', 'C', 'G', 'T'};

    std::string &convertBinaryToNucleotides(char prev, uint8_t *buffer, uint32_t size) {
        return convertBase3ToNucleotides(prev, convertBinaryToBase3(buffer, size));
    }

    std::string &convertBinaryToBase3(uint8_t *buffer, uint32_t size) {
        auto encoder = HuffmanCode::getInstance();
        auto result = new std::string;
        for (int i = 0; i < size; ++i) {
            (*result) += encoder->encodeByte(buffer[i]);
        }
        return *result;
    }

    uint32_t getIndex(const char &nucleotide) {
        int result;
        switch (nucleotide) {
            case 'A':
                result = 1;
                break;
            case 'C':
                result = 2;
                break;
            case 'G':
                result = 3;
                break;
            case 'T':
                result = 0;
                break;
            default:
                result = 1;
        }
        return result;
    }

    std::string &convertBase3ToNucleotides(const char prev, const std::string &intermediateCode) {
        auto result = new std::string();
        auto curPrev = prev;
        for (char t: intermediateCode) {
            uint32_t prevIndex = getIndex(curPrev);
            prevIndex += std::stoi(&t);
            prevIndex %= 4;
            curPrev = order[prevIndex];
            result->append(sizeof(char), curPrev);
        }
        return *result;
    }

    std::string &convertNucleotidesToBinary(char prev, const std::string &nucleotides) {
        return convertBase3ToBinary(convertNucleotidesToBase3(prev, nucleotides));
    }

    std::string &convertNucleotidesToBase3(char prev, const std::string &nucleotides) {
        auto result = new std::string();
        auto curPrev = prev;
        for (char t: nucleotides) {
            uint8_t prevIndex = getIndex(curPrev);
            uint8_t curIndex = getIndex(t);
            if (curIndex <= prevIndex) {
                curIndex += 4;
            }
            uint8_t res = curIndex - (prevIndex + 1) + '0';
            result->append(sizeof(char), res);
            curPrev = t;
        }
        return *result;
    }

    std::string &convertBase3ToBinary(const std::string &intermediateCode) {
        auto decoder = HuffmanCode::getInstance();
        auto result = new std::string;
        int p = 0;
        int *pos = &p;
        while (p < intermediateCode.length()) {
            result->append(sizeof(char), decoder->decodeToByte(intermediateCode, pos));
        }
        return *result;
    }

}
