#ifndef XOR_CODE_SETTINGS_H
#define XOR_CODE_SETTINGS_H

#include <cstdint>
#include <vector>

namespace xorCode {

    namespace types {

        /**
         * A Random Access Iterator for a vector containing uint8_t values.
         */
        typedef std::vector<uint8_t>::iterator iter;
        /**
         * A vector containing bytes.
         */
        typedef std::vector<uint8_t> bytes;
        /**
         * A vector containing ternary digits.
         * Possible values: 0, 1, 2.
         */
        typedef std::vector<uint8_t> ternaries;
        /**
         * A vector containing integer representations of nucleotides.
         * Possible values: 0, 1, 2, 3.
         */
        typedef std::vector<uint8_t> nucleotides;

    }

    const int MAX_NUCLEOTIDES_PER_BYTE = 6;

    int getMaxStrandLength();

    void setMaxStrandLength(int length);

    int getPayloadByteLength();

    int getXOrSize();

    void setXOrSize(int size);

    int getXOrStep();

    void setXOrStep(int step);

    uint64_t getXorStrandsCount(uint64_t strandsCount);

}

#endif //XOR_CODE_SETTINGS_H
