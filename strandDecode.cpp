
#include "strandDecode.h"
#include "strands.h"

namespace xorCode {

    class StrandDecoder {

    private:

        std::string fileName;
        std::vector<std::string> strands;

        std::vector<std::string> *cleanedStrands;

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
            cleanedStrands = new std::vector<std::string>;
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
            for (auto strand: strands) {
                strand.starts_with(primer);

            }
        }

        /*
         * 4. Addressen in Binär umwandeln
	     * 4.1 Bei Fehlern wird Strand weggeschmissen
         * 5. Aufteilen in xOr und Payload Strands
         * 6. Payload Strands sortieren nach index
         */
        void decodeStrands() {

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