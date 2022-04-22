#include <iostream>
#include <filesystem>
#include "strands.h"
#include "strandDecode.h"

bool doOutput = true;

int main() {
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    auto strands = xorCode::createStrands("../data/test.txt");
    std::cout << "Strands: " << strands.size() << "\n";
    auto *results = new std::vector<std::list<char>>;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < strands.size(); i++) {
        auto assembled = strands[i]->assemble();
        auto nucleotides = strands[i]->payload;
        if (doOutput) {
            std::cout << "Strand #" << i << " (" << strands[i]->payload.size() << ") with address "
                      << strands[i]->lowAddress << ":\n";
            for (char c: assembled) {
                std::cout << c;
            }
            std::cout << " (" << assembled.size() << ')' << '\n';
            std::cout << "\"";
            for (char c: nucleotides) {
                std::cout << c;
            }
            std::cout << "\"\n";
        }
        results->push_back(assembled);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time: " << duration.count() << '\n';


    std::cout << "Original File: \n";
    auto original = xorCode::decode("test.txt", *results);
    for (unsigned char character: original) {
        std::cout << character;
    }
    std::cout << "\n";

    std::cout << "Done" << '\n';
}
