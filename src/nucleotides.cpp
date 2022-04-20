#include "nucleotides.h"
#include "huffmanCode.h"

#include <string>
#include <cstdint>
#include <memory>

namespace xorCode {

    char *order = new char[]{'A', 'C', 'G', 'T'};

    bool isNucleotide(char nucleotide) {
        return nucleotide == 'A' || nucleotide == 'C' || nucleotide == 'G' || nucleotide == 'T';
    }

    char getNucleotide(const uint8_t index) {
        return order[index];
    }

    uint8_t getIndex(const char nucleotide) {
        switch (nucleotide) {
            case 'A':
                return 0;
            case 'C':
                return 1;
            case 'G':
                return 2;
            case 'T':
                return 3;
            default:
                return 0;
        }
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

    char getXorNucleotide(const char nucleotide1, const char nucleotide2) {
        uint32_t index1 = getIndex(nucleotide1);
        uint32_t index2 = getIndex(nucleotide2);
        return getNucleotide(index1 xor index2);
    }

    void convertBinaryToNucleotides(char prev, std::list<uint8_t> &buffer, std::list<char> &output) {
        auto curPrev = prev;
        auto encoder = HuffmanCode::getInstance();
        for (uint8_t byte: buffer) {
            // encode byte as base 3
            for (char c: encoder->encodeByte(byte)) {
                // convert base 3 digit c into nucleotide
                uint32_t prevIndex = getIndex(curPrev) + 1;
                prevIndex += std::stoi(&c);
                prevIndex %= 4;
                curPrev = getNucleotide(prevIndex);
                output.push_back(curPrev);
            }
        }
    }

    std::shared_ptr<std::string>
    convertNucleotidesToBase3(char prev, const std::string &nucleotides, uint32_t startIndex) {
        std::shared_ptr<std::string> result(new std::string);
        auto curPrev = prev;
        for (uint32_t i = startIndex; i < nucleotides.length(); ++i) {
            char t = nucleotides[i];
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

    std::list<uint8_t> convertBase3ToBinary(const std::shared_ptr<std::string> &intermediateCode, uint32_t numOfBytes) {
        auto decoder = HuffmanCode::getInstance();
        std::list<uint8_t> result;
        int pos = 0;
        int bytesRead = 0;
        while (pos < intermediateCode->length() && bytesRead < numOfBytes) {
            uint8_t byte = decoder->decodeToByte(*intermediateCode, pos);
            if (pos > intermediateCode->length()) {
                break;
            }
            result.push_back(byte);
            ++bytesRead;
        }
        return result;
    }

    std::list<uint8_t>
    convertNucleotidesToBinary(char prev, const std::string &nucleotides, uint32_t startIndex, uint32_t numOfBytes) {
        return convertBase3ToBinary(convertNucleotidesToBase3(prev, nucleotides, startIndex), numOfBytes);
    }

    std::list<uint8_t>
    convertNucleotidesToBinary(char prev, const std::string &nucleotides, uint32_t numOfBytes) {
        return convertNucleotidesToBinary(prev, nucleotides, 0, numOfBytes);
    }

}
