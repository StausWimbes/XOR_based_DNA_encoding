
#ifndef XOR_BASED_DNA_ENCODING_STRANDS_H
#define XOR_BASED_DNA_ENCODING_STRANDS_H

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <memory>

namespace xorCode {

    struct strand {
        uint32_t payloadLength;
        uint8_t *payload; // TODO: change to nucleotide string
        uint64_t lowAddress;
        bool xOr;
        std::string *primer = new std::string;
        std::string *primerInv = new std::string;

        std::shared_ptr<std::string> assemble();
    };

    /**
     * Uses the file located at `filePath` to create a vector of strands with a payload length of `length` and the name of the file as the high address.
     * @param filePath location of the file
     * @param length the length of each payload
     * @return the vector of strands
     */
    std::vector<strand *> createStrands(const std::string &filePath);

    /**
     * Uses `input` to create a vector of strands with a payload length of `length` and high address `key`.
     * @param key the high address for the strands
     * @param input the input stream
     * @param length the length of each payload
     * @return the vector of strands
     */
    std::vector<strand *> createStrands(const std::string &key, std::ifstream &input);

    std::shared_ptr<std::vector<strand *>> createXOrStrands(std::vector<strand *> *strands);

    void invertNucleotides(std::string &nucleotides);

}

#endif //XOR_BASED_DNA_ENCODING_STRANDS_H
