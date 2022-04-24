#include "nucleotideUtils.h"
#include "huffmanCode.h"
#include "settings.h"

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

    uint8_t getIndex(char nucleotide) {
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

    void invertNucleotides(types::nucleotides &seq) {
        for (char &c: seq) {
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

    void convertBinaryToNucleotides(char prev, types::bytes &buffer, uint8_t extraBytes, types::nucleotides &out) {
        auto curPrev = prev;
        auto encoder = HuffmanCode::getInstance();
        for (uint8_t byte: buffer) {
            // encode byte as base 3
            for (char c: encoder->encodeToTernaries(byte)) {
                // convert base 3 digit c into nucleotide
                uint32_t prevIndex = getIndex(curPrev) + 1;
                prevIndex += c - '0';
                prevIndex %= 4;
                curPrev = getNucleotide(prevIndex);
                out.push_back(curPrev);
            }
        }
        // add nucleotides for the extra bytes to out
        for (int i = 0; i < extraBytes; ++i) {
            // encode byte as base 3
            for (char c: encoder->getExtraTernaries()) {
                // convert base 3 digit c into nucleotide
                uint32_t prevIndex = getIndex(curPrev) + 1;
                prevIndex += c - '0';
                prevIndex %= 4;
                curPrev = getNucleotide(prevIndex);
                out.push_back(curPrev);
            }
        }
    }

    void
    convertNucleotidesToBinary(char prev, types::iter itBegin, types::iter itEnd, uint32_t numOfBytes,
                               types::bytes &out) {
        // decode to base 3
        types::ternaries base3;
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
            decoder->decodeToByte(itBeginBase3, itEndBase3, out);
            ++bytesRead;
        }
    }

}
