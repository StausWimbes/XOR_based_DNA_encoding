#include <iostream>
#include <filesystem>
#include "encode.h"
#include "decode.h"
#include "settings.h"

bool doConsoleOutput = false;
std::string inputDirectory = "../data/";
std::string fileName = "example.jpg";
bool doFileOutput = true;
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

    if (doFileOutput) {
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
