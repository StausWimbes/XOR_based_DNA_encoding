#include <iostream>
#include <filesystem>
#include "encode.h"
#include "decode.h"
#include "settings.h"

bool doConsoleOutput = true;
std::string inputDirectory = "../data/";
std::string fileName = "test.txt";
bool doFileOutput = false;
std::string outputDirectory = "../data/out/";

int main() {
    using namespace xorCode;
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";

    auto start = std::chrono::high_resolution_clock::now();

    auto strands = encode(inputDirectory + fileName);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time for encoding: " << duration.count() << '\n';

    if (doConsoleOutput) {
        std::cout << "Strands: " << strands.size() << "\n";
        for (const auto &assembled: strands) {
            for (uint8_t nucleotide: assembled) {
                std::cout << getCharNucleotide(nucleotide);
            }
            std::cout << " (" << assembled.size() << ')' << '\n';
        }
    }

    start = std::chrono::high_resolution_clock::now();

    auto original = decode(fileName, strands);

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time for decoding: " << duration.count() << '\n';

    if (doFileOutput) {
        std::filesystem::create_directories(outputDirectory);
        auto out = std::ofstream(outputDirectory + fileName, std::ios::binary);
        for (uint8_t character: original) {
            out << character;
        }
        out.close();
    }

    if (doConsoleOutput) {
        std::cout << "Original File: \n";
        for (uint8_t character: original) {
            std::cout << character;
        }
        std::cout << "\n";
    }

    std::cout << "Done" << '\n';
}
