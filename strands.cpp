
#include "strands.h"
#include "nucleotides.h"
#include "settings.h"
#include <iostream>
#include <memory>

namespace xorCode {

    std::vector<strand *> createStrands(const std::string &filePath) {
        std::ifstream input(filePath, std::ios::binary);
        if (input.is_open()) {
            std::string key = filePath.substr(filePath.find_last_of("/\\") + 1);
            auto strands = createStrands(key, input);
            input.close();
            return strands;
        } else {
            std::cout << "Unable to open file!\n";
            return *new std::vector<strand *>();
        }
    }

    std::vector<strand *> createStrands(const std::string &key, std::ifstream &input) {
        auto length = getPayloadByteLength();

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
                //newStrand->payloadLength = readChars;
                newStrand->payload = convertBinaryToNucleotides('A', payload, getPayloadByteLength());
                newStrand->xOr = false;
                newStrand->lowAddress = address;
                // TODO: create and set primer and primerInv newStrand->primer = ;

                strands->push_back(newStrand);

                address += readChars;
            }
        }

        // create and add XOR strands
        auto xorStrands = createXOrStrands(strands);
        strands->insert(strands->end(), xorStrands->begin(), xorStrands->end());

        return *strands;
    }

    std::shared_ptr<std::vector<strand *>> createXOrStrands(std::vector<strand *> *strands) {
        auto size = getXOrSize();
        auto step = getXOrStep();

        uint64_t strandsCount = strands->size();
        uint64_t xOrStrandsCount = strandsCount - (size - 1); // number of strands with step size 1
        xOrStrandsCount = (xOrStrandsCount / step) +
                          (xOrStrandsCount % step == 0 ? 0 : 1); // number of strands with correct step size

        std::shared_ptr<std::vector<strand *>> xOrStrands(new std::vector<strand *>(xOrStrandsCount));

        for (uint64_t i = 0; i < xOrStrandsCount; i++) {
            auto start = i * step; // inclusive
            auto end = start + size; // exclusive
            auto length = strands->at(i)->payload->length();

            std::shared_ptr<std::string> payload = std::make_shared<std::string>(length, 'A');
            //auto *payload = new unsigned char[length];
            for (uint64_t b = 0; b < length; b++) {
                payload->at(b) = strands->at(start)->payload->at(b);
                for (uint64_t s = start + 1; s < end; s++) {
                    //payload[b] = payload[b] xor strands->at(s)->payload[b]; // TODO: create xOr based on nucleotides
                }
            }

            auto newStrand = new strand();
            //newStrand->payloadLength = length;
            newStrand->payload = payload;
            newStrand->xOr = true;
            newStrand->lowAddress = start;
            newStrand->primer = strands->at(start)->primer;
            newStrand->primerInv = strands->at(start)->primerInv;
            xOrStrands->at(i) = newStrand;
        }

        return xOrStrands;
    }

    std::shared_ptr<std::string> strand::assemble() {
        std::shared_ptr<std::string> result(new std::string);
        // add primer
        result->append(*primer);
        // add S nucleotide for detecting if strand is inverted
        result->append(sizeof(char), 'A');
        // add payload nucleotides
        result->append(*payload);
        // add address
        char last = result->at(result->size() - 1);
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
        auto addressNucleotides = convertBinaryToNucleotides(last, address, 9);
        result->append(*addressNucleotides);
        // pad address to always have a length of 9 * MAX_NUCLEOTIDES_PER_BYTE
        int remaining = (9 * MAX_NUCLEOTIDES_PER_BYTE) - addressNucleotides->length();
        last = result->at(result->length() - 1);
        char next = 'C';
        if (last == 'C') {
            next = 'T';
        }
        for (int i = 0; i < remaining; ++i) {
            result->append(sizeof(char), next);
            if (next == 'C') {
                next = 'T';
            } else {
                next = 'C';
            }
        }
        // add another S nucleotide
        result->append(sizeof(char), 'A');
        // add inverted primer
        result->append(*primerInv);
        return result;
    }
}
