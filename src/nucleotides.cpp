#include "nucleotides.h"
#include "huffmanCode.h"

#include <cstdint>

namespace xorCode {

    bool isNucleotide(char nucleotide) {
        return nucleotide == 'A' || nucleotide == 'C' || nucleotide == 'G' || nucleotide == 'T';
    }

    char getNucleotide(const uint8_t index) {
        switch (index) {
            case 0:
                return 'A';
            case 1:
                return 'C';
            case 2:
                return 'G';
            case 3:
                return 'T';
            default:
                return 'A';
        }
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

    void invertNucleotides(std::list<char> &nucleotides) {
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
                prevIndex += c - '0';
                prevIndex %= 4;
                curPrev = getNucleotide(prevIndex);
                output.push_back(curPrev);
            }
        }
    }

    void
    convertNucleotidesToBinary(char prev, iter itBegin, iter itEnd, uint32_t numOfBytes, std::list<uint8_t> &output) {
        // decode to base 3
        std::list<char> base3;
        auto curPrev = prev;
        while (itBegin != itEnd) {
            char t = *itBegin;
            uint8_t prevIndex = getIndex(curPrev);
            uint8_t curIndex = getIndex(t);
            if (curIndex <= prevIndex) {
                curIndex += 4;
            }
            base3.push_back(char(curIndex - (prevIndex + 1) + '0'));
            curPrev = t;
            ++itBegin;
        }
        // decode to binary
        auto decoder = HuffmanCode::getInstance();
        int bytesRead = 0;
        auto itBeginBase3 = base3.begin();
        auto itEndBase3 = base3.end();
        while (itBeginBase3 != itEndBase3 && bytesRead < numOfBytes) {
            decoder->decodeToByte(itBeginBase3, itEndBase3, output);
            ++bytesRead;
        }
    }

}
