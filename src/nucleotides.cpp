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

    std::list<char> convertBinaryToBase3(uint8_t *buffer, uint32_t size) {
        auto encoder = HuffmanCode::getInstance();
        std::list<char> result;
        for (int i = 0; i < size; ++i) {
            for (char c: encoder->encodeByte(buffer[i])) {
                result.push_back(c);
            }
        }
        return result;
    }

    std::list<char> convertBase3ToNucleotides(const char prev, const std::list<char> &intermediateCode) {
        std::list<char> result;
        auto curPrev = prev;
        for (char t: intermediateCode) {
            uint32_t prevIndex = getIndex(curPrev) + 1;
            prevIndex += std::stoi(&t);
            prevIndex %= 4;
            curPrev = getNucleotide(prevIndex);
            result.push_back(curPrev);
        }
        return result;
    }

    std::list<char> convertBinaryToNucleotides(char prev, uint8_t *buffer, uint32_t size) {
        return convertBase3ToNucleotides(prev, convertBinaryToBase3(buffer, size));
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
