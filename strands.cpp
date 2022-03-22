
#include "strands.h"
#include <iostream>

namespace xorCode {

    std::vector<strand *> createStrands(const std::string &filePath, uint32_t length) {
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

    std::vector<strand *> createStrands(const std::string &key, std::ifstream &input, uint32_t length) {
        auto *strands = new std::vector<strand *>();

        if (sizeof(uint8_t) != sizeof(char)) {
            std::cerr << "Unsupported platform!\n";
            return *strands;
        }

        uint64_t address = 0;
        auto *highAddress = new std::string(key);
        bool end = false;
        while (!end) {
            auto *payload = new unsigned char[length];
            auto readChars = input.readsome(reinterpret_cast<char *>(payload), length);

            if (readChars == 0) {
                delete[](payload);
                end = true;
            } else {
                if (readChars != length) {
                    readChars += input.readsome(reinterpret_cast<char *>(payload + readChars), length - readChars);
                }

                auto newStrand = new strand();
                newStrand->payloadLength = readChars;
                newStrand->payload = payload;
                newStrand->highAddress = highAddress;
                newStrand->lowAddress = address;

                strands->push_back(newStrand);

                address += readChars;
            }
        }

        return *strands;
    }
}
