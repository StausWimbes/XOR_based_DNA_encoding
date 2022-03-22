
#include "settings.h"


int maxStrandLength = 100; // in nucleotides
int payloadCharLength = 10; // in chars/bytes

int xOrSize = 3; // the amount of strands involved in each xOr
int xOrStep = 1;
//int xOrRepeat = 2; // the amount of xOrs a single strand is involved in

int getMaxStrandLength() {
    return maxStrandLength;
}

void setMaxStrandLength(int length) {
    // TODO: implement
}

int getPayloadCharLength() {
    return payloadCharLength;
}

int getXOrSize() {
    return xOrSize;
}

void setXOrSize(int size) {
    // TODO: implement
}

int getXOrStep() {
    return xOrStep;
}

void setXOrStep(int step) {
    // TODO: implement
}
