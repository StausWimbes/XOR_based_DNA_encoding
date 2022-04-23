#include <iostream>
#include <filesystem>
#include "encode.h"
#include "decode.h"
#include "settings.h"

bool doOutput = true;

int main() {
    using namespace xorCode;
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";

    auto start = std::chrono::high_resolution_clock::now();

    auto strands = encode("../data/test.txt");

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time for encoding: " << duration.count() << '\n';

    if (doOutput) {
        std::cout << "Strands: " << strands.size() << "\n";
        for (const auto &assembled: strands) {
            for (char c: assembled) {
                std::cout << c;
            }
            std::cout << " (" << assembled.size() << ')' << '\n';
        }
    }

    start = std::chrono::high_resolution_clock::now();

    auto original = decode("test.txt", strands);

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time for decoding: " << duration.count() << '\n';

    if (doOutput) {
        std::cout << "Original File: \n";
        for (uint8_t character: original) {
            std::cout << character;
        }
        std::cout << "\n";
    }

    std::cout << "Done" << '\n';
}
