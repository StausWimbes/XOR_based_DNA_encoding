#ifndef XOR_CODE_SETTINGS_H
#define XOR_CODE_SETTINGS_H

#include <cstdint>

namespace xorCode {

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
