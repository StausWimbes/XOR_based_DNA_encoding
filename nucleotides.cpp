#include "nucleotides.h"
#include "huffmanCode.h"

#include <string>
#include <cstdint>
#include <iostream>
#include <memory>

namespace xorCode {

    char *order = new char[]{'A', 'C', 'G', 'T'};

    char getNucleotide(const uint32_t index) {
        return order[index];
    }

    uint32_t getIndex(const char &nucleotide) {
        int result = 0;
        switch (nucleotide) {
            case 'A':
                result = 0;
                break;
            case 'C':
                result = 1;
                break;
            case 'G':
                result = 2;
                break;
            case 'T':
                result = 3;
                break;
        }
        return result;
    }

    void invertNucleotides(std::string &nucleotides) {
        for (char &c: nucleotides) {
            switch (c) {
                case 'A':
                    c = 'T';
                    break;
                case 'C':
                    c = 'G';
                    break;
                case 'G':
                    c = 'C';
                    break;
                case 'T':
                    c = 'A';
                    break;
            }
        }
    }

    char getXOrNucleotide(const char &n1, const char &n2) {
        uint32_t i1 = getIndex(n1);
        uint32_t i2 = getIndex(n2);
        return getNucleotide(n1 xor n2);
    }

    std::shared_ptr<std::string> convertBinaryToBase3(uint8_t *buffer, uint32_t size) {
        auto encoder = HuffmanCode::getInstance();
        std::shared_ptr<std::string> result(new std::string);
        for (int i = 0; i < size; ++i) {
            (*result) += encoder->encodeByte(buffer[i]);
        }
        return result;
    }

    std::shared_ptr<std::string>
    convertBase3ToNucleotides(const char prev, const std::shared_ptr<std::string> &intermediateCode) {
        std::shared_ptr<std::string> result(new std::string);
        auto curPrev = prev;
        for (char t: *intermediateCode) {
            uint32_t prevIndex = getIndex(curPrev) + 1;
            prevIndex += std::stoi(&t);
            prevIndex %= 4;
            curPrev = getNucleotide(prevIndex);
            result->append(sizeof(char), curPrev);
        }
        return result;
    }

    std::shared_ptr<std::string> convertBinaryToNucleotides(char prev, uint8_t *buffer, uint32_t size) {
        return convertBase3ToNucleotides(prev, convertBinaryToBase3(buffer, size));
    }

    std::shared_ptr<std::string> convertNucleotidesToBase3(char prev, const std::string &nucleotides) {
        std::shared_ptr<std::string> result(new std::string);
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
        return result;
    }

    std::shared_ptr<std::string>
    convertBase3ToBinary(const std::shared_ptr<std::string> &intermediateCode, uint32_t numOfBytes) {
        auto decoder = HuffmanCode::getInstance();
        std::shared_ptr<std::string> result(new std::string);
        int p = 0;
        int *pos = &p;
        int bytesRead = 0;
        while (p < intermediateCode->length() && bytesRead < numOfBytes) {
            result->append(sizeof(char), decoder->decodeToByte(*intermediateCode, pos));
            ++bytesRead;
        }
        if (p > intermediateCode->length()) {
            return nullptr;
        }
        return result;
    }

    std::shared_ptr<std::string>
    convertNucleotidesToBinary(char prev, const std::string &nucleotides, uint32_t numOfBytes) {
        return convertBase3ToBinary(convertNucleotidesToBase3(prev, nucleotides), numOfBytes);
    }

}
