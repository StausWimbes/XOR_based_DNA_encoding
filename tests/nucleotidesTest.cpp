#include "gtest/gtest.h"
#include "nucleotideUtils.h"

TEST(NucleotideTest, IsNucleotide) {
    char nuc;
    ASSERT_EQ(true, xorCode::isNucleotide(nuc = 'A'));
    ASSERT_EQ(true, xorCode::isNucleotide(nuc = 'C'));
    ASSERT_EQ(true, xorCode::isNucleotide(nuc = 'G'));
    ASSERT_EQ(true, xorCode::isNucleotide(nuc = 'T'));

    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'a'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'c'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'g'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 't'));


    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 0));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 255));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'B'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'D'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'F'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'H'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'S'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'U'));
    ASSERT_EQ(false, xorCode::isNucleotide(nuc = 'Z'));
}

TEST(NucleotidesTest, GetXorNucleotide) {
    ASSERT_EQ('A', xorCode::getXorNucleotide('A', 'A'));
    ASSERT_EQ('C', xorCode::getXorNucleotide('A', 'C'));
    ASSERT_EQ('G', xorCode::getXorNucleotide('A', 'G'));
    ASSERT_EQ('T', xorCode::getXorNucleotide('A', 'T'));

    ASSERT_EQ('A', xorCode::getXorNucleotide('A', 'A'));
    ASSERT_EQ('C', xorCode::getXorNucleotide('C', 'A'));
    ASSERT_EQ('G', xorCode::getXorNucleotide('G', 'A'));
    ASSERT_EQ('T', xorCode::getXorNucleotide('T', 'A'));


    ASSERT_EQ('C', xorCode::getXorNucleotide('C', 'A'));
    ASSERT_EQ('A', xorCode::getXorNucleotide('C', 'C'));
    ASSERT_EQ('T', xorCode::getXorNucleotide('C', 'G'));
    ASSERT_EQ('G', xorCode::getXorNucleotide('C', 'T'));

    ASSERT_EQ('C', xorCode::getXorNucleotide('A', 'C'));
    ASSERT_EQ('A', xorCode::getXorNucleotide('C', 'C'));
    ASSERT_EQ('T', xorCode::getXorNucleotide('G', 'C'));
    ASSERT_EQ('G', xorCode::getXorNucleotide('T', 'C'));


    ASSERT_EQ('G', xorCode::getXorNucleotide('G', 'A'));
    ASSERT_EQ('T', xorCode::getXorNucleotide('G', 'C'));
    ASSERT_EQ('A', xorCode::getXorNucleotide('G', 'G'));
    ASSERT_EQ('C', xorCode::getXorNucleotide('G', 'T'));

    ASSERT_EQ('G', xorCode::getXorNucleotide('A', 'G'));
    ASSERT_EQ('T', xorCode::getXorNucleotide('C', 'G'));
    ASSERT_EQ('A', xorCode::getXorNucleotide('G', 'G'));
    ASSERT_EQ('C', xorCode::getXorNucleotide('T', 'G'));


    ASSERT_EQ('T', xorCode::getXorNucleotide('T', 'A'));
    ASSERT_EQ('G', xorCode::getXorNucleotide('T', 'C'));
    ASSERT_EQ('C', xorCode::getXorNucleotide('T', 'G'));
    ASSERT_EQ('A', xorCode::getXorNucleotide('T', 'T'));

    ASSERT_EQ('T', xorCode::getXorNucleotide('A', 'T'));
    ASSERT_EQ('G', xorCode::getXorNucleotide('C', 'T'));
    ASSERT_EQ('C', xorCode::getXorNucleotide('G', 'T'));
    ASSERT_EQ('A', xorCode::getXorNucleotide('T', 'T'));
}
