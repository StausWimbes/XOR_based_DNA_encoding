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


        void deleteOtherStrands(strand *keepStrand, uint64_t xorIndex) {
            for (auto *otherStrand: payloadStrandMap[xorIndex]) {
                if (otherStrand != keepStrand) {
                    delete otherStrand;
                }
            }
            payloadStrandMap[xorIndex].clear();
            payloadStrandMap[xorIndex].push_back(keepStrand);
        }

        void determineIncorrectAndMissingStrands(std::unordered_set<uint64_t> &correctStrands,
                                                 std::queue<uint64_t> &incorrectStrands) const {
            for (uint64_t strandIndex = 0; strandIndex < highestPayloadIndex; strandIndex++) {
                if (!correctStrands.contains(strandIndex)) {
                    incorrectStrands.push(strandIndex);
                }
            }
        }

        bool canBeRestored(uint64_t start, uint64_t end, std::unordered_set<uint64_t> &correctStrands) {
            // no strand can be restored without the xor strand
            if (!xorStrandMap.contains(start)) {
                return false;
            }

            uint32_t missingStrandsCount = 0;
            for (auto j = start; j < end; j++) {
                if (!correctStrands.contains(j)) {

                    // only one strand can be restored using the xor strand
                    if (missingStrandsCount == 1) {
                        return false;
                    }

                    missingStrandsCount++;
                }
            }

            return true;
        }

        void restoreStrand(uint64_t start, uint64_t end, uint64_t restoreIndex) {
            auto *xorStrand = xorStrandMap[start].front(); // TODO choose different than front?


            // recreate payload
            std::string restoredPayloadString = std::string(xorStrand->payload.size(), (int8_t) nuc_A);

            // copy xor strand
            for (uint64_t nucIndex = 0; nucIndex < xorStrand->payload.size(); nucIndex++) {
                restoredPayloadString.at(nucIndex) = xorStrand->payload[nucIndex];
            }

            for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                if (strandIndex == restoreIndex) {
                    continue;
                }

                auto payload = payloadStrandMap[strandIndex].front()->payload; // should only contain one element (correct)
                for (uint64_t nucIndex = 0; nucIndex < payload.size(); nucIndex++) {
                    restoredPayloadString.at(nucIndex) =
                            getXorNucleotide(restoredPayloadString.at(nucIndex), payload[nucIndex]);
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
            restoredStrand->primerBegin = xorStrand->primerBegin;
            restoredStrand->primerEnd = xorStrand->primerEnd;
            payloadStrandMap[restoreIndex].push_back(restoredStrand);

            // remove all incorrect strands from the payloadStrandMap
            deleteOtherStrands(restoredStrand, restoreIndex);
        }

        void restoreIncorrectAndMissingStrands(std::unordered_set<uint64_t> &correctStrands,
                                               std::queue<uint64_t> &incorrectStrands,
                                               uint64_t xorStrandsCount) {
            auto size = getXorSize();
            auto step = getXorStep();

            uint64_t lastCount = 0;
            while (lastCount != incorrectStrands.size()) { // repeat until no incorrect strand can be restored/exists
                lastCount = incorrectStrands.size();

                // go through all strands in the queue exactly once
                for (uint64_t lookedAt = 0; lookedAt < incorrectStrands.size(); lookedAt++) {
                    auto restoreIndex = incorrectStrands.front();
                    incorrectStrands.pop();

                    // restore strand if possible
                    auto smallestXorIndex = getSmallestContainingXorAddress(restoreIndex);
                    for (auto start = smallestXorIndex; start <= restoreIndex; start += step) {
                        auto end = start + size; // exclusive

                        if (!canBeRestored(start, end, correctStrands)) {
                            continue;
                        }


                        // found fitting xor batch
                        restoreStrand(start, end, restoreIndex);
                        correctStrands.insert(restoreIndex);

                        // check all possibilities for all xor strands, this strand is involved with
                        for (auto start2 = smallestXorIndex; start2 <= restoreIndex; start2 += step) {
                            checkAllPossibilities(start2, correctStrands);
                        }

                        // update incorrectStrands
                        for (uint64_t lookedAt = 0; lookedAt < incorrectStrands.size(); lookedAt++) {
                            auto incorrectIndex = incorrectStrands.front();
                            incorrectStrands.pop();

                            if (!correctStrands.contains(incorrectIndex)) {
                                incorrectStrands.push(incorrectIndex);
                            }
                        }

                        goto nextTry;
                    }

                    incorrectStrands.push(restoreIndex);
                }

                nextTry:;
            }
        }

        void chooseFromAvailableStrands(std::unordered_set<uint64_t> &correctStrands,
                                        std::queue<uint64_t> &incorrectStrands) {
            // go through all strands in the queue exactly once
            auto incorrectCount = incorrectStrands.size(); // can't inline as size changes
            for (uint64_t lookedAt = 0; lookedAt < incorrectCount; lookedAt++) {
                auto restoreIndex = incorrectStrands.front();
                incorrectStrands.pop();

                if (payloadStrandMap[restoreIndex].empty()) {
                    incorrectStrands.push(restoreIndex);
                    continue;
                }

                auto *chosenStrand = payloadStrandMap[restoreIndex].front(); // TODO choose different than front?

                // remove all incorrect strands from the payloadStrandMap
                deleteOtherStrands(chosenStrand, restoreIndex);
                correctStrands.insert(restoreIndex);
            }
        }

        void generateRandomStrands(std::unordered_set<uint64_t> &correctStrands,
                                   std::queue<uint64_t> &incorrectStrands) {
            if (incorrectStrands.empty()) {
                return;
            }

            // go through all strands in the queue exactly once
            while (!incorrectStrands.empty()) {
                auto restoreIndex = incorrectStrands.front();
                incorrectStrands.pop();

                auto *restoredStrand = new strand();
                restoredStrand->payload = generateRandomPayload(getMaxStrandLength());
                restoredStrand->isXor = false;
                restoredStrand->lowAddress = restoreIndex;
                restoredStrand->primerBegin = nullptr;
                restoredStrand->primerEnd = nullptr;
                payloadStrandMap[restoreIndex].push_back(restoredStrand);

                correctStrands.insert(restoreIndex);
            }
        }

    public:

        StrandDecoder(std::string &fileName, std::vector<types::nucleotides> &strands) {
            this->fileName = fileName;
            this->strands = strands;
        }

        ~StrandDecoder() {
            for (auto *strand: cleanedStrands) {
                delete strand;
            }
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
                    auto *cleaned = new types::nucleotides(itStart, itEnd);
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
            }
        }

        void sortDecodedStrands() {
            // sort decodedPayloadStrands in ascending order by their lowAddress
            std::sort(decodedPayloadStrands.begin(), decodedPayloadStrands.end(), [](strand *s1, strand *s2) -> bool {
                return s1->lowAddress < s2->lowAddress;
            });
            // find the highest payload index
            auto prevIndex = decodedPayloadStrands.back()->lowAddress;
            int inARow = 0;
            for (auto i = decodedPayloadStrands.size(); i > 0 && inARow < MIN_BIGGER_PAYLOADS_AMOUNT; --i) {
                auto i1 = i - 1;
                auto *curStrand = decodedPayloadStrands[i1];
                if (curStrand->lowAddress + MAX_PAYLOAD_INDEX_GAP < prevIndex) {
                    inARow = 1; // already counting curStrand
                    // remove and delete strands with higher index than curStrand
                    for (auto j = decodedPayloadStrands.size() - 1; j > i1; --j) {
                        delete decodedPayloadStrands[j];
                    }
                    decodedPayloadStrands.resize(i);
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
            // otherwise add them to xorStrandMap
            while (!decodedXorStrands.empty()) {
                auto *s = decodedXorStrands.back();
                if (s->lowAddress > highestPayloadIndex + MAX_PAYLOAD_INDEX_GAP) {
                    delete s;
                } else {
                    if (!xorStrandMap.contains(s->lowAddress)) {
                        xorStrandMap[s->lowAddress] = std::vector<strand *>();
                    }
                    xorStrandMap[s->lowAddress].push_back(s);
                }
                decodedXorStrands.pop_back();
            }
            // put remaining strands into data structures for next step
            // TODO: use unordered_multiset to eliminate duplicates and sort by frequency
            for (auto *s: decodedPayloadStrands) {
                if (!payloadStrandMap.contains(s->lowAddress)) {
                    payloadStrandMap[s->lowAddress] = std::vector<strand *>();
                }
                payloadStrandMap[s->lowAddress].push_back(s);
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
                                deleteOtherStrands(curStrand, xorIndex + i);

                                // recursively call this function for all other xorStrands that involve this strand
                                auto smallestXorIndex = getSmallestContainingXorAddress(curStrand->lowAddress);
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
                        return;
                    }

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
            auto step = getXorStep();
            auto xorStrandsCount = getXorStrandsCount(highestPayloadIndex + 1);
            if (xorStrandMap.size() > xorStrandsCount) {
                xorStrandsCount = xorStrandMap.size();
            }

            std::unordered_set<uint64_t> correctStrands{};

            // first check for correct strands
            for (uint64_t xorIndex = 0; xorIndex < xorStrandsCount * step; xorIndex += step) {
                checkAllPossibilities(xorIndex, correctStrands);
            }

            // find incorrect/missing strands
            std::queue<uint64_t> incorrectStrands{};
            determineIncorrectAndMissingStrands(correctStrands, incorrectStrands);

            // restore incorrect/missing strands using correct strands and xor strands (not correct if the xor strands have problems)
            restoreIncorrectAndMissingStrands(correctStrands, incorrectStrands, xorStrandsCount);

            // choose available strand (probably not correct)
            chooseFromAvailableStrands(correctStrands, incorrectStrands);

            // restore incorrect/missing strands using correct strands, available strands and xor strands (not correct if the available or the xor strands have problems)
            restoreIncorrectAndMissingStrands(correctStrands, incorrectStrands, xorStrandsCount);

            // generate random strand (most likely not correct)
            generateRandomStrands(correctStrands, incorrectStrands);
        }

        /**
         * Converts the payloads to binary and stores the resulting bytes in a vector.
         */
        types::bytes assemble() {
            types::bytes result;

            for (int i = 0; i <= highestPayloadIndex; ++i) {
                if (!payloadStrandMap.contains(i)) {
                    // TODO: stop decoding?
                    continue;
                }

                // decode and append payload
                auto payload = payloadStrandMap[i].front()->payload;
                convertNucleotidesToBinary(nuc_A, payload.begin(), payload.end(), getPayloadByteLength(), result);
            }
            return result;
        }

    };

    types::bytes decode(std::string fileName, std::vector<types::nucleotides> &strands) {
        auto decoder = StrandDecoder(fileName, strands);
        decoder.filterStrands();
        decoder.decodeStrands();
        decoder.sortDecodedStrands();
        decoder.checkAndRestore();
        return decoder.assemble();
    }
}
