#include <iostream>
#include <filesystem>
#include "strands.h"

int main() {
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    auto strands = xorCode::createStrands("../test.txt", 5);
    std::cout << "Strands: " << strands.size() << "\n";

    for (int i = 0; i < strands.size(); i++) {
        std::cout << "Strand #" << i << " (" << strands[i]->payloadLength << ") with key " << *strands[i]->highAddress
                  << " and address " << strands[i]->lowAddress << ":\n";
        std::cout << "\"" << strands[i]->payload << "\"\n";
    }
}
