#ifndef XOR_CODE_STRANDS_H
#define XOR_CODE_STRANDS_H

#include "settings.h"
#include "nucleotideUtils.h"

#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <list>

namespace xorCode {

    struct strand {
        types::nucleotides payload;
        uint64_t lowAddress;
        bool xOr;
        std::shared_ptr<std::string> primer = std::make_shared<std::string>();
        std::shared_ptr<std::string> primerInv = std::make_shared<std::string>();
    };

    std::vector<types::nucleotides> encode(const std::string &key, std::ifstream &input);

    std::vector<types::nucleotides> encode(const std::string &filePath);

}

#endif //XOR_CODE_STRANDS_H
