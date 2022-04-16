
#include "strandDecode.h"
#include "strands.h"
#include "settings.h"
#include "nucleotides.h"
#include <unordered_map>
#include <unordered_set>

namespace xorCode {

    class StrandDecoder {

    private:

        std::string fileName;
        std::vector<std::string> strands;

        std::unordered_set<std::string> *cleanedStrands;

        // map of all payload strands. key is the index (low address) of the strand
        std::unordered_map<uint64_t, strand> *payLoadStrands;
        // the highest index of the strands in the map
        uint64_t highestPayloadIndex = 0;
        // map of all xor strands. key is the index (low address) of the strand
        std::unordered_map<uint64_t, strand> *xorStrands;

    public:

        StrandDecoder(std::string &fileName, std::vector<std::string> &strands) {
            this->fileName = fileName;
            this->strands = strands;
            cleanedStrands = new std::unordered_set<std::string>;
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
            // TODO: Create Primers
            std::string primer;
            std::string invPrimer;
            std::string revPrimer = std::string(primer.rbegin(), primer.rend());
            for (auto strand: strands) {
                // check if strand is inverted
                if (strand[primer.size()] == 'T') {
                    invertNucleotides(strand);
                } else if (strand[primer.size()] != 'A') {
                    continue;
                }
                // check if strand is reversed
                if (strand.ends_with(revPrimer)) {
                    //std::reverse(strand.begin(), strand.end());
                }
                // check if primers are correct
                if (strand.starts_with(primer) && strand.ends_with(invPrimer)) {
                    // remove primers and S nucleotides
                    cleanedStrands->insert(std::string(
                            strand.begin() + primer.length() + 1,
                            strand.end() - invPrimer.length() - 1
                    ));
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
            int addressLength = 9 * MAX_NUCLEOTIDES_PER_BYTE;
            for (auto strand: *cleanedStrands) {
                auto *cur = new xorCode::strand;
                char prev = strand[strand.length() - addressLength - 1];
                auto address = convertNucleotidesToBinary(prev, strand, strand.length() - addressLength, 9);
                if (address == nullptr) {
                    continue;
                }
                cur->lowAddress = *reinterpret_cast<uint64_t *>(address.get() + 1);
                uint8_t xOr = address[0];
                if (xOr != 0 && xOr != 255) {
                    continue;
                }
                cur->xOr = xOr == 255;
                cur->payload = std::make_shared<std::string>(strand.begin(), strand.end() - addressLength);
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
                        if (payLoadStrands->at(strandIndex).payload->length() > maxLength) {
                            maxLength = payLoadStrands->at(strandIndex).payload->length();
                        }
                    } else { // at least one strandIndex is missing -> can't check
                        goto firstCheckLoop; // continue outerLoop
                    }
                }
                if (xorStrand.payload->length() > maxLength) {
                    maxLength = xorStrand.payload->length();
                }

                // recreate xor payload and compare
                {
                    std::string xorPayload(maxLength, 'A');
                    for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                        auto payload = payLoadStrands->at(strandIndex).payload;
                        for (uint64_t byte = 0; byte < payload->length(); byte++) {
                            xorPayload.at(byte) = getXorNucleotide(xorPayload.at(byte), payload->at(byte));
                        }
                    }

                    if (*xorStrand.payload == xorPayload) {
                        for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                            correctStrands.insert(strandIndex);
                        }
                    }
                }

                firstCheckLoop:;
            }

            // find incorrect or missing strands
            std::unordered_set<uint64_t> incorrectStrands{};
            for (uint64_t strandIndex = 0; strandIndex < highestPayloadIndex; strandIndex++) {
                if (!correctStrands.contains(strandIndex)) {
                    incorrectStrands.insert(strandIndex);
                }
            }

            // restore incorrect strands
            uint64_t lastCount = 0;
            while (lastCount != incorrectStrands.size()) {
                lastCount = incorrectStrands.size();

                for (auto restoreIndex: incorrectStrands) {

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
                        auto maxLength = xorStrand.payload->length();

                        // recreate payload
                        std::shared_ptr<std::string> restoredPayload = std::make_shared<std::string>(
                                *xorStrand.payload);
                        for (uint64_t strandIndex = start; strandIndex < end; strandIndex++) {
                            if (strandIndex == restoreIndex) {
                                continue;
                            }
                            auto payload = payLoadStrands->at(strandIndex).payload;
                            for (uint64_t byte = 0; byte < payload->length(); byte++) {
                                restoredPayload->at(byte) = getXorNucleotide(restoredPayload->at(byte),
                                                                             payload->at(byte));
                            }
                        }


                        auto *restoredStrand = new strand();
                        restoredStrand->payload = restoredPayload;
                        restoredStrand->xOr = false;
                        restoredStrand->lowAddress = restoreIndex;
                        restoredStrand->primer = xorStrand.primer;
                        restoredStrand->primerInv = xorStrand.primerInv;
                        auto const result = payLoadStrands->insert({restoreIndex, *restoredStrand});
                        if (not result.second) { result.first->second = *restoredStrand; }

                        incorrectStrands.erase(restoreIndex);
                        // TODO consider adding new strands to correct strands set

                        goto nextTry;
                    }
                }

                nextTry:;
            }
        }

        /**
         * Converts the payloads to binary and stores the resulting bytes in a vector.
         */
        std::vector<uint8_t> assemble() {
            auto *result = new std::vector<uint8_t>();

            for (int i = 0; i <= highestPayloadIndex; ++i) {
                if (!payLoadStrands->contains(i)) {
                    // TODO: stop decoding?
                    continue;
                }

                // decode payload
                auto original = convertNucleotidesToBinary('A', *(payLoadStrands->at(i).payload),
                                                           xorCode::getPayloadByteLength());
                // append payload
                result->insert(result->end(), original.get(), original.get() + xorCode::getPayloadByteLength());
            }
            return *result;
        }

    };

    std::vector<uint8_t> decode(std::string fileName, std::vector<std::string> strands) {
        auto decoder = StrandDecoder(fileName, strands);
        decoder.filterStrands();
        decoder.decodeStrands();
        decoder.checkAndRestore();
        return decoder.assemble();
    }
}