#include "gtest/gtest.h"
#include "nucleotideUtils.h"

TEST(NucleotideTest, IsNucleotide) {
    ASSERT_EQ(true, xorCode::isNucleotide(xorCode::nuc_A));
    ASSERT_EQ(true, xorCode::isNucleotide(xorCode::nuc_C));
    ASSERT_EQ(true, xorCode::isNucleotide(xorCode::nuc_G));
    ASSERT_EQ(true, xorCode::isNucleotide(xorCode::nuc_T));

    ASSERT_EQ(false, xorCode::isNucleotide(255));
    ASSERT_EQ(false, xorCode::isNucleotide(4));
    ASSERT_EQ(false, xorCode::isNucleotide(10));
    ASSERT_EQ(false, xorCode::isNucleotide(27));
    ASSERT_EQ(false, xorCode::isNucleotide(100));
    ASSERT_EQ(false, xorCode::isNucleotide(111));
    ASSERT_EQ(false, xorCode::isNucleotide(125));
    ASSERT_EQ(false, xorCode::isNucleotide(200));
}

TEST(NucleotidesTest, GetXorNucleotide) {
    ASSERT_EQ(xorCode::nuc_A, xorCode::getXorNucleotide(xorCode::nuc_A, xorCode::nuc_A));
    ASSERT_EQ(xorCode::nuc_C, xorCode::getXorNucleotide(xorCode::nuc_A, xorCode::nuc_C));
    ASSERT_EQ(xorCode::nuc_G, xorCode::getXorNucleotide(xorCode::nuc_A, xorCode::nuc_G));
    ASSERT_EQ(xorCode::nuc_T, xorCode::getXorNucleotide(xorCode::nuc_A, xorCode::nuc_T));

    ASSERT_EQ(xorCode::nuc_A, xorCode::getXorNucleotide(xorCode::nuc_A, xorCode::nuc_A));
    ASSERT_EQ(xorCode::nuc_C, xorCode::getXorNucleotide(xorCode::nuc_C, xorCode::nuc_A));
    ASSERT_EQ(xorCode::nuc_G, xorCode::getXorNucleotide(xorCode::nuc_G, xorCode::nuc_A));
    ASSERT_EQ(xorCode::nuc_T, xorCode::getXorNucleotide(xorCode::nuc_T, xorCode::nuc_A));


    ASSERT_EQ(xorCode::nuc_C, xorCode::getXorNucleotide(xorCode::nuc_C, xorCode::nuc_A));
    ASSERT_EQ(xorCode::nuc_A, xorCode::getXorNucleotide(xorCode::nuc_C, xorCode::nuc_C));
    ASSERT_EQ(xorCode::nuc_T, xorCode::getXorNucleotide(xorCode::nuc_C, xorCode::nuc_G));
    ASSERT_EQ(xorCode::nuc_G, xorCode::getXorNucleotide(xorCode::nuc_C, xorCode::nuc_T));

    ASSERT_EQ(xorCode::nuc_C, xorCode::getXorNucleotide(xorCode::nuc_A, xorCode::nuc_C));
    ASSERT_EQ(xorCode::nuc_A, xorCode::getXorNucleotide(xorCode::nuc_C, xorCode::nuc_C));
    ASSERT_EQ(xorCode::nuc_T, xorCode::getXorNucleotide(xorCode::nuc_G, xorCode::nuc_C));
    ASSERT_EQ(xorCode::nuc_G, xorCode::getXorNucleotide(xorCode::nuc_T, xorCode::nuc_C));


    ASSERT_EQ(xorCode::nuc_G, xorCode::getXorNucleotide(xorCode::nuc_G, xorCode::nuc_A));
    ASSERT_EQ(xorCode::nuc_T, xorCode::getXorNucleotide(xorCode::nuc_G, xorCode::nuc_C));
    ASSERT_EQ(xorCode::nuc_A, xorCode::getXorNucleotide(xorCode::nuc_G, xorCode::nuc_G));
    ASSERT_EQ(xorCode::nuc_C, xorCode::getXorNucleotide(xorCode::nuc_G, xorCode::nuc_T));

    ASSERT_EQ(xorCode::nuc_G, xorCode::getXorNucleotide(xorCode::nuc_A, xorCode::nuc_G));
    ASSERT_EQ(xorCode::nuc_T, xorCode::getXorNucleotide(xorCode::nuc_C, xorCode::nuc_G));
    ASSERT_EQ(xorCode::nuc_A, xorCode::getXorNucleotide(xorCode::nuc_G, xorCode::nuc_G));
    ASSERT_EQ(xorCode::nuc_C, xorCode::getXorNucleotide(xorCode::nuc_T, xorCode::nuc_G));


    ASSERT_EQ(xorCode::nuc_T, xorCode::getXorNucleotide(xorCode::nuc_T, xorCode::nuc_A));
    ASSERT_EQ(xorCode::nuc_G, xorCode::getXorNucleotide(xorCode::nuc_T, xorCode::nuc_C));
    ASSERT_EQ(xorCode::nuc_C, xorCode::getXorNucleotide(xorCode::nuc_T, xorCode::nuc_G));
    ASSERT_EQ(xorCode::nuc_A, xorCode::getXorNucleotide(xorCode::nuc_T, xorCode::nuc_T));

    ASSERT_EQ(xorCode::nuc_T, xorCode::getXorNucleotide(xorCode::nuc_A, xorCode::nuc_T));
    ASSERT_EQ(xorCode::nuc_G, xorCode::getXorNucleotide(xorCode::nuc_C, xorCode::nuc_T));
    ASSERT_EQ(xorCode::nuc_C, xorCode::getXorNucleotide(xorCode::nuc_G, xorCode::nuc_T));
    ASSERT_EQ(xorCode::nuc_A, xorCode::getXorNucleotide(xorCode::nuc_T, xorCode::nuc_T));
}
