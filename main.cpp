#include <iostream>
#include <filesystem>
#include "strands.h"
#include "encode.h"
#include "huffmanCode.h"

int main() {
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    auto strands = xorCode::createStrands("../test.txt", 5);
    std::cout << "Strands: " << strands.size() << "\n";

    for (int i = 0; i < strands.size(); i++) {
        std::cout << "Strand #" << i << " (" << strands[i]->payloadLength << ") with key " << *strands[i]->highAddress
                  << " and address " << strands[i]->lowAddress << ":\n";
        auto nucleotides = xorCode::convertBinaryToNucleotides(strands[i]->payload, 5);
        std::cout << "\"" << nucleotides << "\"\n";
        auto original = xorCode::convertNucleotidesToBinary(nucleotides);
        std::cout << "\"" << original << "\"\n";
    }

    /*
    auto data = new uint8_t[256];
    for (int i = 0; i < 256; ++i) {
        data[i] = i;
    }
    auto result = xorCode::convertBinaryToBase3(data, 256);
    std::cout << result << '\n';
    std::cout << xorCode::convertBase3ToNucleotides('A', result) << '\n';

    int pos = 0;
    auto decoder = xorCode::HuffmanCode::getInstance();
    std::string result2;
    while (pos < result.length()) {
        result2.append(sizeof(char), decoder->decodeToByte(result, &pos));
    }
    std::cout << result2 << '\n';
     */
}
