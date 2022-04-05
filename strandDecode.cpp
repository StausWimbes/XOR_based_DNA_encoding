
#include "strandDecode.h"
#include "strands.h"
#include "settings.h"
#include "nucleotides.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace xorCode {

    class StrandDecoder {

    private:

        std::string fileName;
        std::vector<std::string> strands;

        std::unordered_set<std::string> *cleanedStrands;

        // map of all payload strands. key is the index of the strand
        std::unordered_map<uint64_t, strand> *payLoadStrands;
        // the highest index of the strands in the map
        std::uint64_t highestPayloadIndex = 0;
        // map of all xOr strands. key is the index of the strand
        std::unordered_map<uint64_t, strand> *xOrStrands;

    public:

        StrandDecoder(std::string &fileName, std::vector<std::string> &strands) {
            this->fileName = fileName;
            this->strands = strands;
            cleanedStrands = new std::unordered_set<std::string>;
            payLoadStrands = new std::unordered_map<uint64_t, strand>;
            xOrStrands = new std::unordered_map<uint64_t, strand>;
        }

        ~StrandDecoder() {
            delete cleanedStrands;
            delete payLoadStrands;
            delete xOrStrands;
        }

        /*
         * 1. Überprüfen, dass die Primer richtig sind
         * 2. Überprüfen, dass der Strand nicht invertiert wurde
         * 3. Doppelte Strands entfernen
         */
        void filterStrands() {
            // TODO: Create Primers
            std::string primer = "";
            std::string invPrimer = "";
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
                auto cur = new xorCode::strand;
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
                    xOrStrands->insert({cur->lowAddress, *cur});
                } else {
                    if (cur->lowAddress > highestPayloadIndex) {
                        highestPayloadIndex = cur->lowAddress;
                    }
                    payLoadStrands->insert({cur->lowAddress, *cur});
                }
            }

        }

        /*
         * 7. Bestehende Strands mit xOr überprüfen
         * 8. Gucken welche Indizes fehlen und diese versuchen mit xOr Strands herzustellen
         */
        void checkXOrs() {

        }

        /*
         * 9. Payloads in binär umwandeln
         * 10. Payloads zusammenfassen
         */
        std::vector<uint8_t> assemble() {
            // temporary code for testing
            // TODO: replace this code
            for (int i = 0; i <= highestPayloadIndex; ++i) {
                if (!payLoadStrands->contains(i)) {
                    continue;
                }
                auto original = convertNucleotidesToBinary('A', *(payLoadStrands->at(i).payload),
                                                           xorCode::getPayloadByteLength());
                for (int j = 0; j < xorCode::getPayloadByteLength(); ++j) {
                    std::cout << original[j];
                }
            }
            return {};
        }


    };

    std::vector<uint8_t> decode(std::string fileName, std::vector<std::string> strands) {
        auto d = StrandDecoder(fileName, strands);
        d.filterStrands();
        d.decodeStrands();
        d.checkXOrs();
        return d.assemble();
    }
}