//
// Created by steve on 1/26/19.
//

#ifndef CS465_DES_PERMUTEDCHOICE_HPP
#define CS465_DES_PERMUTEDCHOICE_HPP

#include "Block.hpp"

/**
 *
 * Stephen Eckels
 *
**/

/* Map a block of size N to a block of size M with the relation sources given
 * via a choice vector and sinks their index in that vector*/
Block permute(const Block& data, const std::vector<uint8_t>& choiceTable) {
    // output block is size of choice table (bit count)
    Block out(choiceTable.size());
    out.zero();

    std::vector<uint8_t> vec;
    for (int             i = 0; i < choiceTable.size(); i++) {
        uint8_t b = data.bitAt_OneIdx(choiceTable.at(i));
        out.setBitAt(i, b);
    }
    return out;
}

Block leftHalf(const Block& data) {
    std::vector<uint8_t> indices;
    indices.reserve(data.blockSize());

    for (size_t i = 0; i < (data.blockSize() / 2); i++) {
        indices.push_back(i + 1);
    }

    return permute(data, indices);
}

Block rightHalf(const Block& data) {
    std::vector<uint8_t> indices;
    indices.reserve(data.blockSize());

    for (size_t i = data.blockSize() / 2; i < data.blockSize(); i++) {
        indices.push_back(i + 1);
    }

    return permute(data, indices);
}

Block circularLS(const Block& data, int amount) {
    std::vector<uint8_t> indices;
    indices.reserve(data.blockSize());

    // all indices that dont fall off
    for (size_t i = amount; i < data.blockSize(); i++) {
        indices.push_back(i + 1);
    }

    // append the ones that fell
    for (size_t i = 0; i < amount; i++) {
        indices.push_back(i + 1);
    }

    return permute(data, indices);
}

Block join(const Block& lhs, const Block& rhs) {
    Block out(lhs.blockSize() + rhs.blockSize());
    out.zero();

    size_t      cursor = 0;
    for (size_t i      = 0; i < lhs.blockSize(); i++) {
        out.setBitAt(cursor++, lhs.bitAt(i));
    }

    for (size_t i = 0; i < rhs.blockSize(); i++) {
        out.setBitAt(cursor++, rhs.bitAt(i));
    }
    return out;
}

Block PC_1(const Block& data) {
    return permute(data, {57, 49, 41, 33, 25, 17, 9,
                          1, 58, 50, 42, 34, 26, 18,
                          10, 2, 59, 51, 43, 35, 27,
                          19, 11, 3, 60, 52, 44, 36,
                          63, 55, 47, 39, 31, 23, 15,
                          7, 62, 54, 46, 38, 30, 22,
                          14, 6, 61, 53, 45, 37, 29,
                          21, 13, 5, 28, 20, 21, 5
    });
}

Block PC_2(const Block& data) {
    return permute(data, {14, 17, 11, 24, 1, 5, 3, 28,
                          15, 6, 21, 10, 23, 19, 12, 4,
                          26, 8, 16, 7, 27, 20, 13, 2,
                          41, 52, 31, 37, 47, 55, 30, 40,
                          51, 45, 33, 48, 44, 49, 39, 56,
                          34, 53, 46, 42, 50, 36, 29, 32
    });
}

uint8_t sbox_lut[8][4][16] = {
        {
                {14, 4,  13, 1,  2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0,  7},
                {0,  15, 7,  4,  14, 2,  13, 1,  10, 6, 12, 11, 9,  5,  3,  8},
                {4,  1,  14, 8,  13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5,  0},
                {15, 12, 8,  2,  4,  9,  1,  7,  5,  11, 3,  14, 10, 0, 6,  13},
        },

        {
                {15, 1,  8,  14, 6,  11, 3,  4,  9,  7,  2,  13, 12, 0,  5,  10},
                {3,  13, 4,  7,  15, 2,  8,  14, 12, 0, 1,  10, 6,  9,  11, 5},
                {0,  14, 7,  11, 10, 4,  13, 1,  5,  8,  12, 6,  9,  3,  2,  15},
                {13, 8,  10, 1,  3,  15, 4,  2,  11, 6,  7,  12, 0,  5, 14, 9},
        },

        {
                {10, 0,  9,  14, 6,  3,  15, 5,  1,  13, 12, 7,  11, 4,  2,  8},
                {13, 7,  0,  9,  3,  4,  6,  10, 2,  8, 5,  14, 12, 11, 15, 1},
                {13, 6,  4,  9,  8,  15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7},
                {1,  10, 13, 0,  6,  9,  8,  7,  4,  15, 14, 3,  11, 5, 2,  12},
        },

        {
                {7,  13, 14, 3,  0,  6,  9,  10, 1,  2,  8,  5,  11, 12, 4,  15},
                {13, 8,  11, 5,  6,  15, 0,  3,  4,  7, 2,  12, 1,  10, 14, 9},
                {10, 6,  9,  0,  12, 11, 7,  13, 15, 1,  3,  14, 5,  2,  8,  4},
                {3,  15, 0,  6,  10, 1,  13, 8,  9,  4,  5,  11, 12, 7, 2,  14},
        },

        {
                {2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0,  14, 9},
                {14, 11, 2,  12, 4,  7,  13, 1,  5,  0, 15, 10, 3,  9,  8,  6},
                {4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3,  0,  14},
                {11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4, 5,  3},
        },

        {
                {12, 1,  10, 15, 9,  2,  6,  8,  0,  13, 3,  4,  14, 7,  5,  11},
                {10, 15, 4,  2,  7,  12, 9,  5,  6,  1, 13, 14, 0,  11, 3,  8},
                {9,  14, 15, 5,  2,  8,  12, 3,  7,  0,  4,  10, 1,  13, 11, 6},
                {4,  3,  2,  12, 9,  5,  15, 10, 11, 14, 1,  7,  6,  0, 8,  13},
        },

        {
                {4,  11, 2,  14, 15, 0,  8,  13, 3,  12, 9,  7,  5,  10, 6,  1},
                {13, 0,  11, 7,  4,  9,  1,  10, 14, 3, 5,  12, 2,  15, 8,  6},
                {1,  4,  11, 13, 12, 3,  7,  14, 10, 15, 6,  8,  0,  5,  9,  2},
                {6,  11, 13, 8,  1,  4,  10, 7,  9,  5,  0,  15, 14, 2, 3,  12},
        },

        {
                {13, 2,  8,  4,  6,  15, 11, 1,  10, 9,  3,  14, 5,  0,  12, 7},
                {1,  15, 13, 8,  10, 3,  7,  4,  12, 5, 6,  11, 0,  14, 9,  2},
                {7,  11, 4,  1,  9,  12, 14, 2,  0,  6,  10, 13, 15, 3,  5,  8},
                {2,  1,  14, 7,  4,  10, 8,  13, 15, 12, 9,  0,  3,  5, 6,  11},
        },
};

uint8_t SBOX_6Bits(const uint8_t tableNum, uint8_t selector) {
    uint8_t row = (uint8_t)((0b100000 & selector) >> 4 | (1UL & selector));
    uint8_t col = (uint8_t)(0b011110 & selector) >> 1;

    assert(row < 4 && row >= 0);
    assert(col < 16 && col >= 0);
    return sbox_lut[tableNum][row][col];
}

// walk a data block in 6 bit chunks, passing each chunk to SBOX_i
Block SBOX(const Block& data) {
    assert(data.blockSize() == 48);
    Block out(32);
    out.zero();

    uint8_t cursor = 0;
    uint8_t tmp    = 0;
    uint8_t table  = 0;

    // Loop up-to and include 32bits
    for (size_t i = 0; i <= data.blockSize(); i++) {
        // every 6-th bit after the first 6 get substituted
        if (i % 6 == 0 && i != 0) {

            // 6Bits In -> 8bits Out (4 bit val) -> 4 bits Final
            uint8_t sub = SBOX_6Bits(table, tmp);

            // shift to 8 bit -> 4 bit value, then copy bits
            for (size_t bit = 0; bit < 4; bit++) {
                out.setBitAt(cursor++, GET_BIT(sub << 4, bit));
            }

            // each 6 bits uses next table
            table++;
            tmp = 0;
        }

        // build up bit cache one at a time, recheck bounds since inclusive loop
        if (i < data.blockSize()) {
            tmp <<= 1;
            tmp |= data.bitAt(i);
        }
    }
    return out;
}

// expand
Block E(const Block& data) {
    return permute(data, {
            32, 1, 2, 3, 4, 5,
            4, 5, 6, 7, 8, 9,
            8, 9, 10, 11, 12, 13,
            12, 13, 14, 15, 16, 17,
            16, 17, 18, 19, 20, 21,
            20, 21, 22, 23, 24, 25,
            24, 25, 26, 27, 28, 29,
            28, 29, 30, 31, 32, 1
    });
}

// Initial data permuation
Block IP(const Block& data) {
    return permute(data, {
            58, 50, 42, 34, 26, 18, 10, 2,
            60, 52, 44, 36, 28, 20, 12, 4,
            62, 54, 46, 38, 30, 22, 14, 6,
            64, 56, 48, 40, 32, 24, 16, 8,
            57, 49, 41, 33, 25, 17, 9, 1,
            59, 51, 43, 35, 27, 19, 11, 3,
            61, 53, 45, 37, 29, 21, 13, 5,
            63, 55, 47, 39, 31, 23, 15, 7
    });
}

Block IP_1(const Block& data) {
    return permute(data, {
            40, 8, 48, 16, 56, 24, 64, 32,
            39, 7, 47, 15, 55, 23, 63, 31,
            38, 6, 46, 14, 54, 22, 62, 30,
            37, 5, 45, 13, 53, 21, 61, 29,
            36, 4, 44, 12, 52, 20, 60, 28,
            35, 3, 43, 11, 51, 19, 59, 27,
            34, 2, 42, 10, 50, 18, 58, 26,
            33, 1, 41, 9, 49, 17, 57, 25
    });
}

Block XOR(const Block& lhs, const Block& rhs) {
    assert(lhs.blockSize() == rhs.blockSize());
    Block out(lhs.blockSize());
    out.zero();

    for (size_t i = 0; i < lhs.blockSize(); i++) {
        out.setBitAt(i, lhs.bitAt(i) ^ rhs.bitAt(i));
    }
    return out;
}

// Final permutation for F
Block P(const Block& data) {
    return permute(data, {
            16, 7, 20, 21, 29, 12, 28, 17,
            1, 15, 23, 26, 5, 18, 31, 10,
            2, 8, 24, 14, 32, 27, 3, 9,
            19, 13, 30, 6, 22, 11, 4, 25
    });
}

// data should be the right of initial permutation
Block F(const Block& data, const Block& key) {
    assert(data.blockSize() == 32);
    assert(key.blockSize() == 48);
    Block expanded = E(data);
    std::cout << "E:" << std::endl;
    expanded.str();

    assert(expanded.blockSize() == 48);
    Block x = XOR(expanded, key);
    std::cout << "Xor w/ key:" << std::endl;
    x.str();

    Block s = SBOX(x);
    assert(s.blockSize() == 32);
    std::cout << "S-Box:" << std::endl;
    s.str();

    Block p = P(s);
    std::cout << "P: " << std::endl;
    p.str();
    return p;
}

#endif //CS465_DES_PERMUTEDCHOICE_HPP
