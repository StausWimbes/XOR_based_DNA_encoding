
#include "strands.h"
#include <iostream>

namespace xorCode {

    std::vector<strand *> createStrands(std::string filePath, uint32_t length) {
        std::ifstream input(filePath, std::ios::binary);
        if (input.is_open()) {
            std::string key = filePath.substr(filePath.find_last_of("/\\") + 1);
            auto strands = createStrands(key, input, length);
            input.close();
            return strands;
        } else {
            std::cout << "Unable to open file!\n";
            return *new std::vector<strand *>();
        }
    }

    std::vector<strand *> createStrands(std::string key, std::ifstream &input, uint32_t length) {
        auto *strands = new std::vector<strand *>();
        uint64_t address = 0;

        bool end = false;
        while (!end) {
            auto *payload = new unsigned char[length];
            auto readChars = input.readsome(reinterpret_cast<char *>(payload), length);
            if (readChars != length) {
                end = true;
            }
            if (readChars == 0) {
                delete[](payload);
            } else {
                auto newStrand = new strand();
                newStrand->payloadLength = readChars;
                newStrand->payload = payload;
                auto *highAddress = new std::string(key); // TODO use one pointer for all?
                newStrand->highAddress = highAddress;
                newStrand->lowAddress = address;

                strands->push_back(newStrand);

                address += readChars;
            }
        }

        return *strands;
    }
}
