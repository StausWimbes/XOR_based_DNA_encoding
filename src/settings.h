#ifndef XOR_CODE_SETTINGS_H
#define XOR_CODE_SETTINGS_H

#include <cstdint>
#include <vector>

namespace xorCode {

    namespace types {

        /**
         * A Random Access Iterator for a vector containing uint8_t values.
         */
        typedef std::vector<uint8_t>::iterator iter;
        /**
         * A vector containing bytes.
         */
        typedef std::vector<uint8_t> bytes;
        /**
         * A vector containing ternary digits.
         * Possible values: 0, 1, 2.
         */
        typedef std::vector<uint8_t> ternaries;
        /**
         * A vector containing integer representations of nucleotides.
         * Possible values: 0, 1, 2, 3.
         */
        typedef std::vector<uint8_t> nucleotides;

    }

    const int MAX_NUCLEOTIDES_PER_BYTE = 6;

    /**
     * Returns the maximum length of a strand measured in nucleotides.
     */
    int getMaxStrandLength();

    /**
     * Sets the maximum length of a strand measured in nucleotides to the given parameter.
     * @param length The new maximum length measured in nucleotides.
     */
    void setMaxStrandLength(int length);

    /**
     * Returns the amount of bytes a single payload contains.
     */
    int getPayloadByteLength();

    /**
     * Returns the amount of strands that that are used to create a single xOr strand.
     */
    int getXorSize();

    /**
     * Sets the amount of strands that that are used to create a single xOr strand to the given parameter.
     * @param size The new amount of strands.
     */
    void setXorSize(int size);

    /**
     * Returns the step size for the creation of the xOr strands.
     * If the strand with index i is the first strand used to create a xOr strand then the first strand for the next
     * xOr strand will have index i + step size.
     */
    int getXorStep();

    /**
     * Sets the step size for the creation of the xOr strands to the given parameter.
     * If the strand with index i is the first strand used to create a xOr strand then the first strand for the next
     * xOr strand will have index i + step size.
     * @param step The new step size.
     */
    void setXorStep(int step);

    /**
     * Calculates the amount of xOr strands that will be created for a given amount of payload strands based on the
     * current size and step values.
     * @param strandsCount The amount of payload strands.
     * @return The calculated amount of xOr strands.
     */
    uint64_t getXorStrandsCount(uint64_t strandsCount);

}

#endif //XOR_CODE_SETTINGS_H
