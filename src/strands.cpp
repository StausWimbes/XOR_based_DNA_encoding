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
        }

        std::cout << "Unable to open file!\n";
        return *new std::vector<strand *>();
    }

    std::vector<strand *> createStrands(const std::string &key, std::ifstream &input) {
        auto length = getPayloadByteLength();

        auto *strands = new std::vector<strand *>();

        if (sizeof(uint8_t) != sizeof(char)) {
            std::cerr << "Unsupported platform!\n";
            return *strands;
        }

        uint64_t index = 0;
        bool end = false;
        while (!end) {
            auto *payload = new unsigned char[length];
            auto readChars = input.readsome(reinterpret_cast<char *>(payload), length);

            if (readChars == 0) {
                delete[](payload);
                end = true;
            } else {
                if (readChars != length) { // buffer might have been exhausted without reaching EOF
                    input.readsome(reinterpret_cast<char *>(payload + readChars), length - readChars);
                }

                auto *newStrand = new strand();
                newStrand->payload = convertBinaryToNucleotides('A', payload, getPayloadByteLength());
                newStrand->xOr = false;
                newStrand->lowAddress = index++;
                // TODO: create and set primer and primerInv newStrand->primer = ;

                strands->push_back(newStrand);
            }
        }

        // create and add XOR strands
        auto xorStrands = createXorStrands(strands);
        strands->insert(strands->end(), xorStrands->begin(), xorStrands->end());

        return *strands;
    }

    std::shared_ptr<std::vector<strand *>> createXorStrands(std::vector<strand *> *strands) {
        auto size = getXOrSize();
        auto step = getXOrStep();
        auto xorStrandsCount = getXorStrandsCount(strands->size());

        // xor strands
        std::shared_ptr<std::vector<strand *>> xOrStrands(new std::vector<strand *>(xorStrandsCount));

        for (uint64_t i = 0; i < xorStrandsCount; i++) {
            auto start = i * step; // inclusive
            auto end = start + size; // exclusive

            // determine max payload length
            auto maxLength = strands->at(start)->payload->length();
            for (uint64_t strand = start + 1; strand < end; strand++) {
                if (strands->at(strand)->payload->length() > maxLength) {
                    maxLength = strands->at(strand)->payload->length();
                }
            }

            // create xor payload
            std::shared_ptr<std::string> xorPayload = std::make_shared<std::string>(maxLength, 'A');
            for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                auto payload = strands->at(strandIndex)->payload;
                for (uint64_t byte = 0; byte < payload->length(); byte++) {
                    xorPayload->at(byte) = getXorNucleotide(xorPayload->at(byte), payload->at(byte));
                }
            }

            auto *newStrand = new strand();
            newStrand->payload = xorPayload;
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
        auto *address = new uint8_t[9];
        if (xOr) {
            address[0] = 255;
        } else {
            address[0] = 0;
        }
        auto *low = reinterpret_cast<uint8_t *>(&lowAddress);
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
