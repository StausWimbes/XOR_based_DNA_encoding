#include "settings.h"

namespace xorCode {

    // 9 * MAX_NUCLEOTIDES_PER_BYTE for address + 2 for S Nucleotides + 24 for Primers + MAX_NUCLEOTIDES_PER_BYTE for at least one byte in payload
    const int MIN_STRAND_LENGTH = 26 + (10 * MAX_NUCLEOTIDES_PER_BYTE);

    int maxStrandLength = 110; // in nucleotides
    int payloadByteLength = 5; // in bytes

    const int MIN_XOR_SIZE = 2;
    const int MIN_XOR_STEP = 1;

    int xOrSize = 3; // the amount of strands involved in each xOr
    int xOrStep = 3; // the difference between the indices of the starting strands for each xOr

    int getMaxStrandLength() {
        return maxStrandLength;
    }

    void setMaxStrandLength(int length) {
        if (length >= MIN_STRAND_LENGTH) {
            maxStrandLength = length;
            payloadByteLength = (length - MIN_STRAND_LENGTH + MAX_NUCLEOTIDES_PER_BYTE) / MAX_NUCLEOTIDES_PER_BYTE;
        }
    }

    int getPayloadByteLength() {
        return payloadByteLength;
    }

    int getXOrSize() {
        return xOrSize;
    }

    void setXOrSize(int size) {
        if (size >= MIN_XOR_SIZE) {
            xOrSize = size;
        }
    }

    int getXOrStep() {
        return xOrStep;
    }

    void setXOrStep(int step) {
        if (step >= MIN_XOR_STEP) {
            xOrStep = step;
        }
    }

    uint64_t getXorStrandsCount(uint64_t strandsCount) {
        auto size = getXOrSize();
        auto step = getXOrStep();

        // number of strands with step size 1
        uint64_t xOrStrandsCount = strandsCount - (size - 1);

        // number of strands with correct step size
        return (xOrStrandsCount / step) + (xOrStrandsCount % step == 0 ? 0 : 1);
    }

}
