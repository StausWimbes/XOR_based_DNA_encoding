#ifndef XOR_CODE_NUCLEOTIDES_H
#define XOR_CODE_NUCLEOTIDES_H

#include <list>
#include <cstdint>
#include "huffmanCode.h"

namespace xorCode {

    /**
     * Returns whether the given character is a nucleotide.
     *
     * @param nucleotide the character to be checked
     * @return true if the given character is one of 'A', 'C', 'G', 'T'; false otherwise
     */
    bool isNucleotide(char nucleotide);

    /**
     * Returns the nucleotide associated with the given number according to the following mapping:
     * 0 -> 'A', 1 -> 'C', 2 -> 'G', 3 -> 'T'.
     * This is the exact inverse function to getIndex().
     * @param index A number between 0 and 3.
     * @return A char representation of a nucleotide.
     */
    char getNucleotide(uint8_t index);

    /**
     * Returns the number associated with the given nucleotide according to the following mapping:
     * 'A' -> 0, 'C' -> 1, 'G' -> 2, 'T' -> 3.
     * This is the exact inverse function to getNucleotide().
     * @param nucleotide A char representation of a nucleotide.
     * @return A number between 0 and 3.
     */
    uint8_t getIndex(char nucleotide);

    /**
     * Inverts every nucleotide in the given string according to the following pairs:
     * ('A', 'T'), ('C', 'G').
     * For example the string "ACTAG" would be turned into "TGATC".
     * @param nucleotides The string that should be inverted.
     */
    void invertNucleotides(std::list<char> &nucleotides);

    /**
     * Performs an xor operation on the given nucleotides. The xor operation is based on the nucleotides indices:
     *
     * xor    | A (00)   C (01)   G (10)   T (11)
     * ------- ----------------------------------
     * A (00) | A (00)   C (01)   G (10)   T (11)
     * C (01) | C (01)   A (00)   T (11)   G (10)
     * G (10) | G (10)   T (11)   A (00)   C (01)
     * T (11) | T (11)   G (10)   C (01)   A (00)
     *
     * @param nucleotide1 A char representation of the first nucleotide.
     * @param nucleotide2 A char representation of the second nucleotide.
     * @return A char representation of the nucleotide that is the result of the xor operation.
     */
    char getXorNucleotide(char nucleotide1, char nucleotide2);

    /**
     * Takes an array of uint8_t and turns it into a string of nucleotides. This is done by first turning each uint8_t
     * into a string of 5 or 6 ternary digits using a prefix encoding. Each of those digits is then turned into a
     * nucleotide using a rotating encoding that makes sure that no two consecutive nucleotides are the same.
     * This is the exact inverse function of convertNucleotidesToBinary().
     * @param prev A char representation of a nucleotide. This is used as the previous nucleotide
     *             for the rotating encoding.
     * @param buffer A pointer to the first uint8_t in the input array.
     * @param size The size of the input array.
     * @return The resulting string of nucleotides.
     */
    void convertBinaryToNucleotides(char prev, std::list<uint8_t> &buffer, std::list<char> &output);

    /**
     * Takes a string of nucleotides and turns a part of it starting at 'startIndex' into an array of uint8_t of size
     * 'numOfBytes'. This is done by first turning each nucleotide into a ternary digit according to a rotary encoding.
     * Then each string of 5 or 6 ternary digits is turned into one uint8_t using a prefix encoding. If the string
     * can be turned into more than 'numOfBytes' uint8_t then only the first 'numOfBytes' uint8_t will be returned.
     * This is the exact inverse function of convertBinaryToNucleotides().
     * @param prev A char representation of a nucleotide. This is used as the previous nucleotide
     *             for the rotating encoding.
     * @param nucleotides A string of nucleotides.
     * @param startIndex The index at which the conversion should start.
     * @param numOfBytes The number of uint8_t that should be converted.
     * @return An array of uint8_t with length 'numOfBytes'. If the string cannot be converted into at least 'numOfBytes'
     * many uint8_t then the nullptr is returned instead.
     */
    void
    convertNucleotidesToBinary(char prev, iter itBegin, iter itEnd, uint32_t numOfBytes, std::list<uint8_t> &output);


}

#endif //XOR_CODE_NUCLEOTIDES_H
