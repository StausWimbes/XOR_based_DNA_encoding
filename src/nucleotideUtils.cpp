#include "nucleotideUtils.h"
#include "huffmanCode.h"
#include "settings.h"

#include <cstdint>

namespace xorCode {

    const uint8_t nuc_A = 0;
    const uint8_t nuc_C = 1;
    const uint8_t nuc_G = 2;
    const uint8_t nuc_T = 3;

    const char charRepresentations[] = {'A', 'C', 'G', 'T'};

    bool isNucleotide(uint8_t nucleotide) {
        return nucleotide <= 3;
    }

    char getCharNucleotide(uint8_t nucleotide) {
        return charRepresentations[nucleotide % 4];
    }

    uint8_t getIntegerNucleotide(char nucleotide) {
        switch (nucleotide) {
            case 'A':
                return nuc_A;
            case 'C':
                return nuc_C;
            case 'G':
                return nuc_G;
            case 'T':
                return nuc_T;
            default:
                return nuc_A;
        }
    }

    void invertNucleotides(types::nucleotides &seq) {
        uint8_t mask = 3; // preserve only the 2 lowest bits
        for (uint8_t &nucleotide: seq) {
            nucleotide = ~nucleotide & mask;
        }
    }

    uint8_t getXorNucleotide(uint8_t nucleotide1, uint8_t nucleotide2) {
        return nucleotide1 ^ nucleotide2;
    }

    uint8_t getParityNucleotide(types::iter itBegin, types::iter itEnd) {
        uint8_t sum = 0;
        while (itBegin < itEnd) {
            sum = (sum + *itBegin) % 4;
            ++itBegin;
        }
        return sum % 4;
    }

    void convertBinaryToNucleotides(uint8_t prev, types::bytes &buffer, uint8_t extraBytes, types::nucleotides &out) {
        auto curPrev = prev;
        auto *encoder = HuffmanCode::getInstance();
        for (uint8_t byte: buffer) {
            // encode byte as base 3
            for (uint8_t c: encoder->encodeToTernaries(byte)) {
                // convert base 3 digit c into nucleotide
                uint8_t prevIndex = curPrev + 1;
                prevIndex += c;
                prevIndex %= 4;
                curPrev = prevIndex;
                out.push_back(curPrev);
            }
        }
        // add nucleotides for the extra bytes to out
        for (int i = 0; i < extraBytes; ++i) {
            // encode byte as base 3
            for (uint8_t c: encoder->getExtraTernaries()) {
                // convert base 3 digit c into nucleotide
                uint8_t prevIndex = curPrev + 1;
                prevIndex += c;
                prevIndex %= 4;
                curPrev = prevIndex;
                out.push_back(curPrev);
            }
        }
    }

    void convertNucleotidesToBinary(uint8_t prev, types::iter itBegin, types::iter itEnd, uint32_t numOfBytes,
                                    types::bytes &out) {
        // decode to base 3
        types::ternaries base3;
        auto curPrev = prev;
        while (itBegin != itEnd) {
            uint8_t t = *itBegin;
            uint8_t prevIndex = curPrev;
            uint8_t curIndex = t;
            if (curIndex <= prevIndex) {
                curIndex += 4;
            }
            base3.push_back(curIndex - (prevIndex + 1));
            curPrev = t;
            ++itBegin;
        }
        // decode to binary
        auto *decoder = HuffmanCode::getInstance();
        int bytesRead = 0;
        auto itBeginBase3 = base3.begin();
        auto itEndBase3 = base3.end();
        while (itBeginBase3 != itEndBase3 && bytesRead < numOfBytes) {
            decoder->decodeToByte(itBeginBase3, itEndBase3, out);
            ++bytesRead;
        }
    }

}
