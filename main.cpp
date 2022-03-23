#include <iostream>
#include <filesystem>
#include "strands.h"
#include "encode.h"
#include "settings.h"
#include "huffmanCode.h"

bool doOutput = true;

int main() {
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    auto strands = xorCode::createStrands("../test.txt");
    std::cout << "Strands: " << strands.size() << "\n";
    auto results = new std::vector<std::string>;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < strands.size(); i++) {
        auto assembled = *strands[i]->assemble();
        auto nucleotides = *xorCode::convertBinaryToNucleotides('A', strands[i]->payload, getPayloadByteLength());
        if (doOutput) {
            std::cout << "Strand #" << i << " (" << strands[i]->payloadLength << ") with address "
                      << strands[i]->lowAddress << ":\n";
            std::cout << assembled << " (" << assembled.length() << ')' << '\n';
            std::cout << "\"" << nucleotides << "\"\n";
        }
        results->push_back(nucleotides);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time: " << duration.count() << '\n';


    std::cout << "Original File: \n";
    for (auto &result: *results) {
        auto original = *xorCode::convertNucleotidesToBinary('A', result);
        if (doOutput)
            std::cout << original;
    }
    std::cout << "Done" << '\n';
}
