#ifndef XOR_CODE_STRANDDECODE_H
#define XOR_CODE_STRANDDECODE_H

#include <cstdint>
#include <vector>
#include <list>

namespace xorCode {

    std::list<uint8_t> decode(std::string fileName, std::vector<std::list<char>> &strands);

}

#endif //XOR_CODE_STRANDDECODE_H
