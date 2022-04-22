#include "encode.h"
#include "nucleotideUtils.h"
#include "settings.h"
#include <iostream>
#include <memory>

namespace xorCode {

    class Encoder {

    private:

        std::string key;
        types::bytes *inputBytes = new types::bytes;

        std::vector<strand *> *strands = new std::vector<strand *>;

    public:

        Encoder(const std::string &key, std::ifstream &input) {
            this->key = key;
            while (!input.eof()) {
                inputBytes->push_back(input.get());
            }
        }

        ~Encoder() {
            delete inputBytes;
            for (strand *s: *strands) {
                delete s;
            }
            delete strands;
        }

        void createPayloadStrands() {
            auto length = getPayloadByteLength();

            uint64_t index = 0;
            while (!inputBytes->empty()) {
                std::list<uint8_t> payload;
                for (int i = 0; i < length && !inputBytes->empty(); ++i) {
                    payload.push_back(inputBytes->front());
                    inputBytes->pop_front();
                }
                auto *newStrand = new strand();
                convertBinaryToNucleotides('A', payload, newStrand->payload);
                newStrand->xOr = false;
                newStrand->lowAddress = index++;
                // TODO: create and set primer and primerInv newStrand->primer = ;

                strands->push_back(newStrand);
            }
        }

        void createXOrStrands() {
            auto size = getXOrSize();
            auto step = getXOrStep();
            auto xorStrandsCount = getXorStrandsCount(strands->size());

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
                strands->push_back(newStrand);
            }
        }

        std::vector<types::nucleotides> assembleStrands() {
            std::vector<types::nucleotides> output;
            for (strand *s: *strands) {
                types::nucleotides result;
                // add primer
                result.insert(result.end(), s->primer->begin(), s->primer->end());
                // add S nucleotide for detecting if strand is inverted
                result.push_back('A');
                // add payload nucleotides
                for (char c: s->payload) {
                    result.push_back(c);
                }
                // add address
                char last = result.back();
                std::list<uint8_t> address;
                if (s->xOr) {
                    address.push_back(255);
                } else {
                    address.push_back(0);
                }
                auto *low = reinterpret_cast<uint8_t *>(&(s->lowAddress));
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
                result.insert(result.end(), s->primerInv->begin(), s->primerInv->end());
                output.push_back(result);
            }
            return output;
        }

    };

    std::vector<types::nucleotides> encode(const std::string &key, std::ifstream &input) {
        auto encoder = Encoder(key, input);
        encoder.createPayloadStrands();
        encoder.createXOrStrands();
        return encoder.assembleStrands();
    }

    std::vector<types::nucleotides> encode(const std::string &filePath) {
        std::ifstream input(filePath, std::ios::binary);
        if (!input.is_open()) {
            std::cout << "Unable to open file!\n";
            return {};
        }
        std::string key = filePath.substr(filePath.find_last_of("/\\") + 1);
        auto strands = encode(key, input);
        input.close();
        return strands;
    }
}
