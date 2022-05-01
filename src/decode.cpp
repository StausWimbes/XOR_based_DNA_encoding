#include "decode.h"
#include "encode.h"
#include "settings.h"
#include "nucleotideUtils.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>

namespace xorCode {

    class StrandDecoder {

    private:

        std::string fileName;
        std::vector<types::nucleotides> strands;

        std::list<types::nucleotides> *cleanedStrands;

        // map of all payload strands. key is the index (low address) of the strand
        std::unordered_map<uint64_t, strand> *payLoadStrands;
        // the highest index of the strands in the map
        uint64_t highestPayloadIndex = 0;
        // map of all xor strands. key is the index (low address) of the strand
        std::unordered_map<uint64_t, strand> *xorStrands;

    public:

        StrandDecoder(std::string &fileName, std::vector<types::nucleotides> &strands) {
            this->fileName = fileName;
            this->strands = strands;
            cleanedStrands = new std::list<types::nucleotides>;
            payLoadStrands = new std::unordered_map<uint64_t, strand>;
            xorStrands = new std::unordered_map<uint64_t, strand>;
        }

        ~StrandDecoder() {
            delete cleanedStrands;
            delete payLoadStrands;
            delete xorStrands;
        }

        /*
         * 1. Überprüfen, dass die Primer richtig sind
         * 2. Überprüfen, dass der Strand nicht invertiert wurde
         * 3. Doppelte Strands entfernen
         */
        void filterStrands() {
            // create primers
            auto primerBegin = *getPrimer(fileName);
            auto primerEnd = types::nucleotides(12, 0);
            std::copy(primerBegin.begin(), primerBegin.end(), primerEnd.begin());
            invertNucleotides(primerEnd);
            auto revPrimerBegin = types::nucleotides(12, 0);
            std::reverse_copy(primerBegin.begin(), primerBegin.end(), revPrimerBegin.begin());
            // start filtering the strands
            for (auto strand: strands) {
                // check if strand is inverted
                if (strand[primerBegin.size()] == nuc_T) {
                    invertNucleotides(strand);
                } else if (strand[primerBegin.size()] != nuc_A) {
                    continue;
                }
                // check if strand is reversed
                if (std::equal(revPrimerBegin.begin(), revPrimerBegin.end(), strand.end() - revPrimerBegin.size())) {
                    std::reverse(strand.begin(), strand.end());
                }
                // check if primers are correct
                bool correct = true;
                auto itStart = strand.begin();
                for (auto c: primerBegin) {
                    correct = c == *itStart;
                    ++itStart;
                }
                ++itStart;
                auto itEnd = strand.end();
                for (int i = primerEnd.size() - 1; i >= 0; --i) {
                    --itEnd;
                    correct = primerEnd[i] == *itEnd;
                }
                --itEnd;
                if (correct) {
                    // remove primers and 'A' nucleotides
                    auto cleaned = types::nucleotides(itStart, itEnd);
                    cleanedStrands->push_back(cleaned);
                }
            }
        }

        /*
         * 4. Addressen in Binär umwandeln
	     * 4.1 Bei Fehlern wird Strand weggeschmissen
         * 5. Aufteilen in xOr und Payload Strands
         * 6. Payload Strands sortieren nach index
         */
        void decodeStrands() {
            int addressLengthBytes = 9;
            int addressLength = addressLengthBytes * MAX_NUCLEOTIDES_PER_BYTE + 1;
            for (auto &strand: *cleanedStrands) {
                // itBegin now points to the char right before the address
                auto itBegin = strand.end() - addressLength - 1;
                auto *cur = new xorCode::strand;
                uint8_t prev = *itBegin;
                ++itBegin; // itBegin now points to the first char of the address
                // check parity nucleotide
                auto itEnd = strand.end() - 1;
                auto parity = getParityNucleotide(itBegin, itEnd);
                if (parity != strand.back()) {
                    continue;
                }
                types::bytes address;
                itBegin = strand.end() - addressLength;
                itEnd = strand.end() - 1;
                convertNucleotidesToBinary(prev, itBegin, itEnd, addressLengthBytes, address);
                if (address.size() != addressLengthBytes) {
                    continue;
                }
                // get xOr byte from address
                uint8_t xOr = address.at(0);
                if (xOr != 0 && xOr != 255) {
                    continue;
                }
                cur->xOr = xOr == 255;
                // get lowAddress from address
                for (int i = 0; i < 8; ++i) {
                    cur->lowAddressArray[i] = address.at(i + 1);
                }
                // extract payload from strand
                int length = strand.size() - addressLength;
                for (int i = 0; i < length; ++i) {
                    cur->payload.push_back(strand[i]);
                }
                // TODO: figure out a way to store duplicates
                if (cur->xOr) {
                    if (cur->lowAddress + getXOrSize() - 1 > highestPayloadIndex) {
                        highestPayloadIndex = cur->lowAddress + getXOrSize() - 1;
                    }
                    xorStrands->insert({cur->lowAddress, *cur});
                } else {
                    if (cur->lowAddress > highestPayloadIndex) {
                        highestPayloadIndex = cur->lowAddress;
                    }
                    payLoadStrands->insert({cur->lowAddress, *cur});
                }
            }

        }

        /**
         * Validates existing strands and attempts to correct possible mutations as well as restore lost strands
         * 1. Check all possibilities when searching for correct strands.
         * 2. Recreate missing strands.
         *  2.1 If a missing strand has been recreated and the xors overlap check all possibilities for the other
         *      xors involving this strand again.
         * 3. If a strand cant be recreated but a strand with that address is available then just use that strands
         *    payload
         * 4. If even that fails just choose random bytes.
         */
        void checkAndRestore() {
            auto size = getXOrSize();
            auto step = getXOrStep();
            auto xorStrandsCount = getXorStrandsCount(highestPayloadIndex + 1);
            if (xorStrands->size() > xorStrandsCount) {
                xorStrandsCount = xorStrands->size();
            }

            std::unordered_set<uint64_t> correctStrands{};

            // first check for correct strands
            for (uint64_t i = 0; i < xorStrandsCount; i++) {
                auto start = i * step; // inclusive
                auto end = start + size; // exclusive

                if (!xorStrands->contains(start)) { // no xor strand -> can't check
                    continue;
                }
                auto xorStrand = xorStrands->at(start);

                // determine max payload length
                uint64_t maxLength = 0;
                for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                    if (payLoadStrands->contains(strandIndex)) {
                        if (payLoadStrands->at(strandIndex).payload.size() > maxLength) {
                            maxLength = payLoadStrands->at(strandIndex).payload.size();
                        }
                    } else { // at least one strandIndex is missing -> can't check
                        goto firstCheckLoop; // continue outerLoop
                    }
                }
                if (xorStrand.payload.size() > maxLength) {
                    maxLength = xorStrand.payload.size();
                }

                // recreate xor payload and compare
                {
                    auto xorPayload = types::nucleotides(maxLength, nuc_A);
                    for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                        auto payload = payLoadStrands->at(strandIndex).payload;
                        for (int j = 0; j < payload.size(); ++j) {
                            xorPayload[j] = getXorNucleotide(xorPayload[j], payload[j]);
                        }
                    }
                    if (std::equal(xorStrand.payload.begin(), xorStrand.payload.end(), xorPayload.begin())) {
                        // all strands that were involved in the xOr are correct
                        for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                            correctStrands.insert(strandIndex);
                        }
                    }
                }

                firstCheckLoop:;
            }

            // find incorrect or missing strands
            std::queue<uint64_t> incorrectStrands{};
            for (uint64_t strandIndex = 0; strandIndex < highestPayloadIndex; strandIndex++) {
                if (!correctStrands.contains(strandIndex)) {
                    incorrectStrands.push(strandIndex);
                }
            }
            // restore incorrect strands
            uint64_t lastCount = 0;
            while (lastCount != incorrectStrands.size()) {
                lastCount = incorrectStrands.size();
                // go through all strands in the queue exactly once
                for (uint64_t lookedAt = 0; lookedAt < lastCount; lookedAt++) {
                    auto restoreIndex = incorrectStrands.front();
                    incorrectStrands.pop();
                    // restore this strand if possible
                    for (uint64_t i = 0; i < xorStrandsCount; i++) {
                        auto start = i * step; // inclusive
                        auto end = start + size; // exclusive

                        if (restoreIndex < start || end <= restoreIndex || !xorStrands->contains(start)) {
                            continue;
                        }

                        uint32_t missingStrandsCount = 0;
                        for (auto j = start; j < end; j++) {
                            if (!correctStrands.contains(j)) {
                                missingStrandsCount++;
                            }
                        }
                        if (missingStrandsCount > 1) { // more strands than only the current strand are incorrect
                            continue;
                        }

                        // found fitting xor batch -> restore strand
                        auto xorStrand = xorStrands->at(start);

                        // recreate payload
                        std::string restoredPayloadString = std::string(xorStrand.payload.size(), nuc_A);
                        int index = 0;
                        for (uint8_t c: xorStrand.payload) {
                            restoredPayloadString.at(index) = c;
                            ++index;
                        }
                        for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                            if (strandIndex == restoreIndex) {
                                continue;
                            }
                            auto payload = payLoadStrands->at(strandIndex).payload;
                            uint64_t byte = 0;
                            for (uint8_t c: payload) {
                                restoredPayloadString.at(byte) = getXorNucleotide(restoredPayloadString.at(byte), c);
                                ++byte;
                            }
                        }

                        types::nucleotides restoredPayload;
                        for (uint8_t c: restoredPayloadString) {
                            restoredPayload.push_back(c);
                        }

                        auto *restoredStrand = new strand();
                        restoredStrand->payload = restoredPayload;
                        restoredStrand->xOr = false;
                        restoredStrand->lowAddress = restoreIndex;
                        restoredStrand->primerBegin = xorStrand.primerBegin;
                        restoredStrand->primerEnd = xorStrand.primerEnd;
                        auto const result = payLoadStrands->insert({restoreIndex, *restoredStrand});
                        if (not result.second) { result.first->second = *restoredStrand; }

                        correctStrands.insert(restoreIndex);

                        goto nextTry;
                    }

                    incorrectStrands.push(restoreIndex);
                }

                nextTry:;
            }
        }

        /**
         * Converts the payloads to binary and stores the resulting bytes in a vector.
         */
        types::bytes assemble() {
            types::bytes result;

            for (int i = 0; i <= highestPayloadIndex; ++i) {
                if (!payLoadStrands->contains(i)) {
                    // TODO: stop decoding?
                    continue;
                }

                // decode and append payload
                auto payload = payLoadStrands->at(i).payload;
                convertNucleotidesToBinary(nuc_A, payload.begin(), payload.end(), getPayloadByteLength(), result);
            }
            return result;
        }

    };

    types::bytes decode(std::string fileName, std::vector<types::nucleotides> &strands) {
        auto decoder = StrandDecoder(fileName, strands);
        decoder.filterStrands();
        decoder.decodeStrands();
        decoder.checkAndRestore();
        return decoder.assemble();
    }
}
