
#ifndef XOR_BASED_DNA_ENCODING_STRANDS_H
#define XOR_BASED_DNA_ENCODING_STRANDS_H

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

namespace xorCode {

    struct strand {
        uint32_t payloadLength;
        char *payload;
        uint64_t lowAddress;
        std::string *highAddress;
    };

    /**
     * Uses the file located at `filePath` to create a vector of strands with a payload length of `length` and the name of the file as the high address.
     * @param filePath location of the file
     * @param length the length of each payload
     * @return the vector of strands
     */
    std::vector<strand *> createStrands(std::string filePath, uint32_t length);

    /**
     * Uses `input` to create a vector of strands with a payload length of `length` and high address `key`.
     * @param key the high address for the strands
     * @param input the input stream
     * @param length the length of each payload
     * @return the vector of strands
     */
    std::vector<strand *> createStrands(std::string key, std::ifstream &input, uint32_t length);

}

#endif //XOR_BASED_DNA_ENCODING_STRANDS_H
