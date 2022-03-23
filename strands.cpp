
#include "strands.h"
#include "encode.h"
#include "settings.h"
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
                newStrand->xOr = false;
                newStrand->lowAddress = address;
                // TODO: create and set primer and primerInv newStrand->primer = ;

                strands->push_back(newStrand);

                address += readChars;
            }
        }

        return *strands;
    }

    std::vector<strand *> createXOrStrands(std::vector<strand *> strands) {
        // TODO: implement
        return std::vector<strand *>();
    }

    std::string strand::assemble() {
        auto result = std::string();
        result.append(*primer);
        result.append(sizeof(char), 'A');
        result.append(convertBinaryToNucleotides('A', payload, getPayloadByteLength()));
        char last = result[result.size() - 1];
        auto address = new uint8_t[9];
        if (xOr) {
            address[0] = 255;
        } else {
            address[0] = 0;
        }
        auto low = reinterpret_cast<uint8_t *>(&lowAddress);
        for (int i = 0; i < 8; ++i) {
            address[i + 1] = low[i];
        }
        result.append(convertBinaryToNucleotides(last, address, 9));
        result.append(sizeof(char), 'A');
        result.append(*primerInv);
        return result;
    }
}
