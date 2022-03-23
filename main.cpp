#include <iostream>
#include <filesystem>
#include "strands.h"
#include "encode.h"
#include "huffmanCode.h"

bool doOutput = false;
uint32_t length = 5;

int main() {
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    auto strands = xorCode::createStrands("../test2.txt");
    std::cout << "Strands: " << strands.size() << "\n";
    auto results = new std::vector<std::string>;

    for (int i = 0; i < strands.size(); i++) {
        if (doOutput) {
            std::cout << "Strand #" << i << " (" << strands[i]->payloadLength << ") with address "
                      << strands[i]->lowAddress << ":\n";
        }
        auto assembled = strands[i]->assemble();
        auto nucleotides = xorCode::convertBinaryToNucleotides('A', strands[i]->payload, length);
        if (doOutput) {
            std::cout << assembled << " (" << assembled.length() << ')' << '\n';
            std::cout << "\"" << nucleotides << "\"\n";
        }
        results->push_back(nucleotides);
    }

    std::cout << "Original File: \n";
    for (auto &result: *results) {
        auto original = xorCode::convertNucleotidesToBinary('A', result);
        if (doOutput)
            std::cout << original;
    }
    std::cout << "Done" << '\n';

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
