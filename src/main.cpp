#include <iostream>
#include <filesystem>
#include "strands.h"
#include "nucleotides.h"
#include "settings.h"
#include "huffmanCode.h"
#include "strandDecode.h"

bool doOutput = true;

int main() {
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    auto strands = xorCode::createStrands("../../data/test.txt");
    std::cout << "Strands: " << strands.size() << "\n";
    auto results = new std::vector<std::string>;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < strands.size(); i++) {
        auto assembled = *strands[i]->assemble();
        auto nucleotides = *(strands[i]->payload);
        if (doOutput) {
            std::cout << "Strand #" << i << " (" << strands[i]->payload->length() << ") with address "
                      << strands[i]->lowAddress << ":\n";
            std::cout << assembled << " (" << assembled.length() << ')' << '\n';
            std::cout << "\"" << nucleotides << "\"\n";
        }
        results->push_back(assembled);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time: " << duration.count() << '\n';


    std::cout << "Original File: \n";
    auto original = xorCode::decode("test.txt", *results);
    for (unsigned char i: original) {
        std::cout << i;
    }
    std::cout << "\n";

    /*
    for (auto &result: *results) {
        auto original = xorCode::convertNucleotidesToBinary('A', result, xorCode::getPayloadByteLength());
        if (doOutput) {
            for (int i = 0; i < xorCode::getPayloadByteLength(); ++i) {
                std::cout << original[i];
            }
        }
    }
     */
    std::cout << "Done" << '\n';
}
