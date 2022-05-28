#include "decode.h"
#include "encode.h"
#include "settings.h"
#include "nucleotideUtils.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

namespace xorCode {

    class StrandDecoder {

    private:

        std::string fileName;
        std::vector<types::nucleotides> strands;

        std::vector<types::nucleotides *> cleanedStrands;
        std::vector<strand *> decodedPayloadStrands;
        std::vector<strand *> decodedXorStrands;

        // the highest index of the strands in the map
        uint64_t highestPayloadIndex = 0;
        std::unordered_map<uint64_t, std::vector<strand *>> payloadStrandMap;
        std::unordered_map<uint64_t, std::vector<strand *>> xorStrandMap;

        // map of all payload strands. key is the index (low address) of the strand
        std::unordered_map<uint64_t, strand> *payLoadStrands;
        // map of all xor strands. key is the index (low address) of the strand
        std::unordered_map<uint64_t, strand> *xorStrands;

    public:

        StrandDecoder(std::string &fileName, std::vector<types::nucleotides> &strands) {
            this->fileName = fileName;
            this->strands = strands;
            payLoadStrands = new std::unordered_map<uint64_t, strand>;
            xorStrands = new std::unordered_map<uint64_t, strand>;
        }

        ~StrandDecoder() {
            for (auto *strand: cleanedStrands) {
                delete strand;
            }
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
                    auto cleaned = new types::nucleotides(itStart, itEnd);
                    cleanedStrands.push_back(cleaned);
                }
            }
        }

        /*
         * 4. Addressen in Binär umwandeln
	     * 4.1 Bei Fehlern wird Strand weggeschmissen
         * 5. Aufteilen in xor und Payload Strands
         * 6. Payload Strands sortieren nach index
         */
        void decodeStrands() {
            int addressLengthBytes = 9;
            int addressLength = addressLengthBytes * MAX_NUCLEOTIDES_PER_BYTE + 1;
            for (auto *strand: cleanedStrands) {
                // itBegin now points to the char right before the address
                auto itBegin = strand->end() - addressLength - 1;
                auto *cur = new xorCode::strand;
                uint8_t prev = *itBegin;
                ++itBegin; // itBegin now points to the first char of the address
                // check parity nucleotide
                auto itEnd = strand->end() - 1;
                auto parity = getParityNucleotide(itBegin, itEnd);
                if (parity != strand->back()) {
                    continue;
                }
                types::bytes address;
                itBegin = strand->end() - addressLength;
                itEnd = strand->end() - 1;
                convertNucleotidesToBinary(prev, itBegin, itEnd, addressLengthBytes, address);
                if (address.size() != addressLengthBytes) {
                    continue;
                }
                // get xor byte from address
                uint8_t isXor = address.at(0);
                if (isXor != 0 && isXor != 255) {
                    continue;
                }
                cur->isXor = isXor == 255;
                // get lowAddress from address
                for (int i = 0; i < 8; ++i) {
                    cur->lowAddressArray[i] = address.at(i + 1);
                }
                // extract payload from strand
                cur->payload.assign(strand->begin(), strand->end() - addressLength);
                if (cur->isXor) {
                    decodedXorStrands.push_back(cur);
                } else {
                    decodedPayloadStrands.push_back(cur);
                }

                // TODO: remove the following code once checkAndRestore() has been finished
                if (cur->isXor) {
                    if (cur->lowAddress + getXorSize() - 1 > highestPayloadIndex) {
                        highestPayloadIndex = cur->lowAddress + getXorSize() - 1;
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

        void sortDecodedStrands() {
            // sort decodedPayloadStrands in ascending order by their lowAddress
            std::sort(decodedPayloadStrands.begin(), decodedPayloadStrands.end(), [](strand *s1, strand *s2) -> bool {
                return s1->lowAddress < s2->lowAddress;
            });
            // find the highest payload index
            uint64_t prevIndex = decodedPayloadStrands.back()->lowAddress;
            int inARow = 0;
            for (auto i = decodedPayloadStrands.size() - 1; i >= 0 && inARow < MIN_BIGGER_PAYLOADS_AMOUNT; --i) {
                auto *curStrand = decodedPayloadStrands[i];
                if (curStrand->lowAddress < prevIndex - MAX_PAYLOAD_INDEX_GAP) {
                    inARow = 1; // already counting curStrand
                    // remove and delete strands with higher index than curStrand
                    for (auto j = decodedPayloadStrands.size() - 1; j > i; --j) {
                        delete decodedPayloadStrands[j];
                    }
                    decodedPayloadStrands.resize(i + 1);
                } else {
                    ++inARow;
                }
                prevIndex = curStrand->lowAddress;
            }
            highestPayloadIndex = decodedPayloadStrands.back()->lowAddress;
            // sort decodedXorStrands in ascending order by their lowAddress
            std::sort(decodedXorStrands.begin(), decodedXorStrands.end(), [](strand *s1, strand *s2) -> bool {
                return s1->lowAddress < s2->lowAddress;
            });
            // remove and delete xor strands with index that is too high
            while (!decodedXorStrands.empty()) {
                if (decodedXorStrands.back()->lowAddress - MAX_PAYLOAD_INDEX_GAP > highestPayloadIndex) {
                    delete decodedXorStrands.back();
                    decodedXorStrands.pop_back();
                }
            }
            // put remaining strands into data structures for next step
            for (auto *s: decodedPayloadStrands) {
                if (!payloadStrandMap.contains(s->lowAddress)) {
                    payloadStrandMap[s->lowAddress] = std::vector<strand *>();
                }
                payloadStrandMap[s->lowAddress].push_back(s);
            }
            for (auto *s: decodedXorStrands) {
                if (!xorStrandMap.contains(s->lowAddress)) {
                    xorStrandMap[s->lowAddress] = std::vector<strand *>();
                }
                xorStrandMap[s->lowAddress].push_back(s);
            }
        }

        void checkAllPossibilities(uint64_t xorIndex, std::unordered_set<uint64_t> &correctStrands) {
            auto size = getXorSize();
            auto step = getXorStep();
            if (!xorStrandMap.contains(xorIndex) || xorIndex + size - 1 > highestPayloadIndex) {
                return;
            }
            auto payloadIterators = std::vector<std::vector<strand *>::iterator>();
            payloadIterators.resize(size);
            auto curPayloads = std::vector<types::nucleotides *>();
            curPayloads.resize(size);
            for (auto *curXorStrand: xorStrandMap[xorIndex]) {
                // initialize iterators for every payload index
                for (int i = 0; i < size; ++i) {
                    if (payloadStrandMap.contains(xorIndex + i)) {
                        payloadIterators[i] = payloadStrandMap[xorIndex + i].begin();
                    } else {
                        return;
                    }
                }
                // iterate over all combinations of payloads and check if they create the correct xor payload
                int nextIncrease = size - 1;
                while (nextIncrease >= 0) {
                    // check current payloads
                    for (int i = 0; i < size; ++i) {
                        curPayloads[i] = &((*payloadIterators[i])->payload);
                    }
                    auto newXorPayload = getXorPayload(curPayloads.data(), size);
                    if (newXorPayload.size() == curXorStrand->payload.size()
                        && std::equal(newXorPayload.begin(), newXorPayload.end(), curXorStrand->payload.begin())) {
                        auto recursiveCalls = std::unordered_set<unsigned long long>();
                        // the calculated xor payload is correct
                        // add current strands to correct strands
                        for (int i = 0; i < size; ++i) {
                            auto *curStrand = *payloadIterators[i];
                            if (!correctStrands.contains(curStrand->lowAddress)) {
                                correctStrands.insert(curStrand->lowAddress);
                                // remove all incorrect strands from the payloadStrandMap
                                for (auto *otherStrand: payloadStrandMap[xorIndex + i]) {
                                    if (otherStrand != curStrand) {
                                        delete otherStrand;
                                    }
                                }
                                payloadStrandMap[xorIndex + i].clear();
                                payloadStrandMap[xorIndex + i].push_back(curStrand);
                                // recursively call this function for all other xorStrands that involve this strand
                                auto smallestXorIndex = curStrand->lowAddress - size - (curStrand->lowAddress % step);
                                for (auto j = smallestXorIndex; j <= curStrand->lowAddress; j += step) {
                                    if (j >= 0 && j + size > curStrand->lowAddress && j != xorIndex) {
                                        recursiveCalls.insert(j);
                                        break;
                                    }
                                }
                            }
                        }
                        // execute the recursive calls
                        for (auto i: recursiveCalls) {
                            checkAllPossibilities(i, correctStrands);
                        }
                    } else {
                        // update iterators so that they point to the next combination of payloads
                        while (nextIncrease >= 0) {
                            payloadIterators[nextIncrease]++;
                            if (payloadIterators[nextIncrease] == payloadStrandMap[size + nextIncrease].end()) {
                                payloadIterators[nextIncrease] = payloadStrandMap[size + nextIncrease].begin();
                                nextIncrease--;
                            } else {
                                nextIncrease = size - 1;
                                break;
                            }
                        }
                    }
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
            auto size = getXorSize();
            auto step = getXorStep();
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
                        // all strands that were involved in the xor are correct
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
                        restoredStrand->isXor = false;
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
        // TODO: uncomment the next line once checkAndRestore() has been finished
        // decoder.sortDecodedStrands();
        decoder.checkAndRestore();
        return decoder.assemble();
    }
}
