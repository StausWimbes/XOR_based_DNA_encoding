#include "gtest/gtest.h"
#include "nucleotides.h"

TEST(GetXorNucleotideTest, SingleOperation) {
    ASSERT_EQ('A', xorCode::getXOrNucleotide('A', 'A'));
}
