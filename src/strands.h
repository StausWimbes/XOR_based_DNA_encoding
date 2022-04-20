#ifndef XOR_CODE_STRANDS_H
#define XOR_CODE_STRANDS_H

#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <list>

namespace xorCode {

    struct strand {
        std::list<char> payload;
        uint64_t lowAddress;
        bool xOr;
        std::shared_ptr<std::string> primer = std::make_shared<std::string>();
        std::shared_ptr<std::string> primerInv = std::make_shared<std::string>();

        std::list<char> assemble();
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

    std::shared_ptr<std::vector<strand *>> createXorStrands(std::vector<strand *> *strands);

}

#endif //XOR_CODE_STRANDS_H
