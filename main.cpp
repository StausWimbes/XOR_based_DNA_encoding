#include <iostream>
#include <filesystem>
#include "strands.h"
#include "encode.h"

int main() {
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    auto strands = xorCode::createStrands("../test.txt", 5);
    std::cout << "Strands: " << strands.size() << "\n";

    for (int i = 0; i < strands.size(); i++) {
        std::cout << "Strand #" << i << " (" << strands[i]->payloadLength << ") with key " << *strands[i]->highAddress
                  << " and address " << strands[i]->lowAddress << ":\n";
        std::cout << "\"" << strands[i]->payload << "\"\n";
    }

    auto data = new uint8_t[5];
    for (int i = 0; i < 5; ++i) {
        data[i] = 23;
    }
    auto result = xorCode::convertBinaryToBase3(data, 5);
    std::cout << result << '\n';
    std::cout << xorCode::convertBase3ToNucleotides('A', result) << '\n';
}
