
#ifndef XOR_BASED_DNA_ENCODING_SETTINGS_H
#define XOR_BASED_DNA_ENCODING_SETTINGS_H

namespace xorCode {

    const int MAX_NUCLEOTIDES_PER_BYTE = 6;

    int getMaxStrandLength();

    void setMaxStrandLength(int length);

    int getPayloadByteLength();

    int getXOrSize();

    void setXOrSize(int size);

    int getXOrStep();

    void setXOrStep(int step);

}

#endif //XOR_BASED_DNA_ENCODING_SETTINGS_H
