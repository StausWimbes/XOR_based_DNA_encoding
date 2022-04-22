#include "encode.h"
#include "nucleotideUtils.h"
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
        while (!input.eof()) {
            std::list<uint8_t> payload;
            for (int i = 0; i < length && !input.eof(); ++i) {
                payload.push_back(input.get());
            }
            auto *newStrand = new strand();
            convertBinaryToNucleotides('A', payload, newStrand->payload);
            newStrand->xOr = false;
            newStrand->lowAddress = index++;
            // TODO: create and set primer and primerInv newStrand->primer = ;

            strands->push_back(newStrand);
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
            auto maxLength = strands->at(start)->payload.size();
            for (uint64_t strand = start + 1; strand < end; strand++) {
                if (strands->at(strand)->payload.size() > maxLength) {
                    maxLength = strands->at(strand)->payload.size();
                }
            }

            // create xor payload
            auto xorPayloadString = std::string(maxLength, 'A');
            for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                auto payload = strands->at(strandIndex)->payload;
                uint64_t byte = 0;
                for (char c: payload) {
                    xorPayloadString.at(byte) = getXorNucleotide(xorPayloadString.at(byte), c);
                    ++byte;
                }
            }
            std::list<char> xorPayload;
            for (char c: xorPayloadString) {
                xorPayload.push_back(c);
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

    types::nucleotides strand::assemble() {
        std::list<char> result;
        // add primer
        result.insert(result.end(), primer->begin(), primer->end());
        // add S nucleotide for detecting if strand is inverted
        result.push_back('A');
        // add payload nucleotides
        for (char c: payload) {
            result.push_back(c);
        }
        // add address
        char last = result.back();
        std::list<uint8_t> address;
        if (xOr) {
            address.push_back(255);
        } else {
            address.push_back(0);
        }
        auto *low = reinterpret_cast<uint8_t *>(&lowAddress);
        for (int i = 0; i < 8; ++i) {
            address.push_back(low[i]);
        }
        auto prev = result.size();
        convertBinaryToNucleotides(last, address, result);
        // pad address to always have a length of 9 * MAX_NUCLEOTIDES_PER_BYTE
        int remaining = (9 * MAX_NUCLEOTIDES_PER_BYTE) - (result.size() - prev);
        last = result.back();
        char next = 'C';
        if (last == 'C') {
            next = 'T';
        }
        for (int i = 0; i < remaining; ++i) {
            result.push_back(next);
            if (next == 'C') {
                next = 'T';
            } else {
                next = 'C';
            }
        }
        // add another S nucleotide
        result.push_back('A');
        // add inverted primer
        result.insert(result.end(), primerInv->begin(), primerInv->end());
        return result;
    }
}
