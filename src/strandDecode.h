
#ifndef XOR_BASED_DNA_ENCODING_STRANDDECODE_H
#define XOR_BASED_DNA_ENCODING_STRANDDECODE_H

#include <cstdint>
#include <vector>
#include <string>

namespace xorCode {

    std::vector<uint8_t> decode(std::string fileName, std::vector<std::string> strands);

}

#endif //XOR_BASED_DNA_ENCODING_STRANDDECODE_H
