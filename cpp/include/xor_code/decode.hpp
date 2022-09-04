#ifndef XOR_CODE_DECODE_H
#define XOR_CODE_DECODE_H

#include "settings.hpp"

#include <cstdint>
#include <vector>
#include <list>
#include <string>

namespace xor_code {

    // TODO: add some documentation LEON

    types::bytes decode(std::string fileName, std::vector<types::nucleotides> &strands);

}

#endif //XOR_CODE_DECODE_H
