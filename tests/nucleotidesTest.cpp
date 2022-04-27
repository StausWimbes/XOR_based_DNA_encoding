#include "gtest/gtest.h"
#include "nucleotideUtils.h"

using namespace xorCode;

TEST(NucleotideTest, IsNucleotide) {
    ASSERT_EQ(true, isNucleotide(nuc_A));
    ASSERT_EQ(true, isNucleotide(nuc_C));
    ASSERT_EQ(true, isNucleotide(nuc_G));
    ASSERT_EQ(true, isNucleotide(nuc_T));

    ASSERT_EQ(false, isNucleotide(255));
    ASSERT_EQ(false, isNucleotide(4));
    ASSERT_EQ(false, isNucleotide(10));
    ASSERT_EQ(false, isNucleotide(27));
    ASSERT_EQ(false, isNucleotide(100));
    ASSERT_EQ(false, isNucleotide(111));
    ASSERT_EQ(false, isNucleotide(125));
    ASSERT_EQ(false, isNucleotide(200));
}

TEST(NucleotidesTest, GetXorNucleotide) {
    ASSERT_EQ(nuc_A, getXorNucleotide(nuc_A, nuc_A));
    ASSERT_EQ(nuc_C, getXorNucleotide(nuc_A, nuc_C));
    ASSERT_EQ(nuc_G, getXorNucleotide(nuc_A, nuc_G));
    ASSERT_EQ(nuc_T, getXorNucleotide(nuc_A, nuc_T));

    ASSERT_EQ(nuc_A, getXorNucleotide(nuc_A, nuc_A));
    ASSERT_EQ(nuc_C, getXorNucleotide(nuc_C, nuc_A));
    ASSERT_EQ(nuc_G, getXorNucleotide(nuc_G, nuc_A));
    ASSERT_EQ(nuc_T, getXorNucleotide(nuc_T, nuc_A));


    ASSERT_EQ(nuc_C, getXorNucleotide(nuc_C, nuc_A));
    ASSERT_EQ(nuc_A, getXorNucleotide(nuc_C, nuc_C));
    ASSERT_EQ(nuc_T, getXorNucleotide(nuc_C, nuc_G));
    ASSERT_EQ(nuc_G, getXorNucleotide(nuc_C, nuc_T));

    ASSERT_EQ(nuc_C, getXorNucleotide(nuc_A, nuc_C));
    ASSERT_EQ(nuc_A, getXorNucleotide(nuc_C, nuc_C));
    ASSERT_EQ(nuc_T, getXorNucleotide(nuc_G, nuc_C));
    ASSERT_EQ(nuc_G, getXorNucleotide(nuc_T, nuc_C));


    ASSERT_EQ(nuc_G, getXorNucleotide(nuc_G, nuc_A));
    ASSERT_EQ(nuc_T, getXorNucleotide(nuc_G, nuc_C));
    ASSERT_EQ(nuc_A, getXorNucleotide(nuc_G, nuc_G));
    ASSERT_EQ(nuc_C, getXorNucleotide(nuc_G, nuc_T));

    ASSERT_EQ(nuc_G, getXorNucleotide(nuc_A, nuc_G));
    ASSERT_EQ(nuc_T, getXorNucleotide(nuc_C, nuc_G));
    ASSERT_EQ(nuc_A, getXorNucleotide(nuc_G, nuc_G));
    ASSERT_EQ(nuc_C, getXorNucleotide(nuc_T, nuc_G));


    ASSERT_EQ(nuc_T, getXorNucleotide(nuc_T, nuc_A));
    ASSERT_EQ(nuc_G, getXorNucleotide(nuc_T, nuc_C));
    ASSERT_EQ(nuc_C, getXorNucleotide(nuc_T, nuc_G));
    ASSERT_EQ(nuc_A, getXorNucleotide(nuc_T, nuc_T));

    ASSERT_EQ(nuc_T, getXorNucleotide(nuc_A, nuc_T));
    ASSERT_EQ(nuc_G, getXorNucleotide(nuc_C, nuc_T));
    ASSERT_EQ(nuc_C, getXorNucleotide(nuc_G, nuc_T));
    ASSERT_EQ(nuc_A, getXorNucleotide(nuc_T, nuc_T));
}
