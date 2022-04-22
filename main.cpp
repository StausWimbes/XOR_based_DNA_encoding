#include <iostream>
#include <filesystem>
#include "encode.h"
#include "decode.h"
#include "settings.h"

bool doOutput = true;

int main() {
    using namespace xorCode;
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    auto strands = encode("../data/test.txt");
    std::cout << "Strands: " << strands.size() << "\n";
    auto *results = new std::vector<types::nucleotides>;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto &assembled: strands) {
        if (doOutput) {
            for (char c: assembled) {
                std::cout << c;
            }
            std::cout << " (" << assembled.size() << ')' << '\n';
        }
        results->push_back(assembled);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time: " << duration.count() << '\n';


    std::cout << "Original File: \n";
    auto original = decode("test.txt", *results);
    for (unsigned char character: original) {
        std::cout << character;
    }
    std::cout << "\n";

    std::cout << "Done" << '\n';
}
