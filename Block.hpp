//
// Created by steve on 1/24/19.
//

#ifndef CS465_DES_BLOCK_HPP
#define CS465_DES_BLOCK_HPP

#include <stdint.h>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>
#include <bitset>

#define BITS_P_BYTE 8

// MSB is 0, LSB is 7
#define GET_BIT(num, n) (((num) >> (7 - (n))) & 1UL)
#define CLR_BIT(num, n) ((num) &= ~(1UL << (7 - (n))))
#define SET_BIT(num, n) ((num) |= 1UL << (7 - (n)))

/**
 *
 * Stephen Eckels
 *
**/

void printBinary(const char* buf, const size_t sz, const size_t bits) {
    size_t      totalBits = 0;
    for (size_t byte      = 0; byte < sz; byte++) {
        for (size_t bit = 0; bit < 8; bit++) {
            if (totalBits++ >= bits)
                break;

            std::cout << (uint16_t)(((buf[byte]) >> (7 - bit)) & 1);
        }
    }
    std::cout << std::endl;
};

class Block
{
public:
    Block(std::vector<uint8_t>& data, const size_t blockSz) {
        m_blockSize = blockSz;

        size_t dSize = data.size() * BITS_P_BYTE;
        assert(dSize <= blockSize());
        m_data = data;

        // pad if needed
        for (size_t i = dSize; i < blockSize(); i += BITS_P_BYTE) {
            m_data.push_back(0);
        }

        assert(m_data.size() * BITS_P_BYTE >= blockSize());
    }

    // assumes 7 bit ascii is desired from 8 bit input
    Block(const std::string& str, const size_t blockSz, bool parityRemove) {
        std::vector<uint8_t> data(str.begin(), str.end());

        if(parityRemove) {
            for (size_t          i = 0; i < data.size(); i++) {
                data.at(i) <<= 1;
            }
        }

        m_blockSize = blockSz;

        size_t dSize = data.size() * BITS_P_BYTE;
        assert(dSize <= blockSize());
        m_data = data;

        // pad if needed
        for (size_t i = dSize; i < blockSize(); i += BITS_P_BYTE) {
            m_data.push_back(0);
        }

        assert(m_data.size() * BITS_P_BYTE >= blockSize());
    }

    Block(const size_t blockSz) {
        m_blockSize = blockSz;
        m_data.reserve(bytesPerBlock());
    }

    void zero() {
        m_data.resize(bytesPerBlock(), 0);
    }

    static std::vector<Block> buildBlocks(std::vector<uint8_t>& data, const size_t blockSz) {
        std::vector<Block> blocks;

        std::vector<uint8_t> tmp;

        // walk all data
        size_t       count = 0;
        for (uint8_t b : data) {
            // build blocks byte by byte
            if (count >= (blockSz / BITS_P_BYTE)) {
                blocks.emplace_back(tmp, blockSz);
                count = 0;
                tmp.clear();
            }

            count++;
            tmp.push_back(b);
        }

        if (count != 0) {
            blocks.emplace_back(tmp, blockSz);
        }
        return blocks;
    }

    size_t blockSize() const {
        return m_blockSize;
    }

    size_t bytesPerBlock() const {
        return (size_t)std::ceil((double)m_blockSize / BITS_P_BYTE);
    }

    /* given some index to a bit in the block, return that bit.
     * MSB is bit 0, LSB is m_blockSz*/
    uint8_t bitAt(const size_t bit_idx) const {
        uint8_t byt = m_data.at(bit_idx / BITS_P_BYTE);
        uint8_t bit = (uint8_t)bit_idx % (uint8_t)BITS_P_BYTE;
        return (uint8_t)GET_BIT(byt, bit);
    }

    uint8_t bitAt_OneIdx(const size_t bit_idx) const {
        return bitAt(bit_idx - 1);
    }

    void setBitAt(const size_t bit_idx, const uint8_t val) {
        uint8_t byt = m_data.at(bit_idx / BITS_P_BYTE);
        uint8_t bit = (uint8_t)bit_idx % (uint8_t)BITS_P_BYTE;
        if (val) {
            SET_BIT(byt, bit);
        } else {
            CLR_BIT(byt, bit);
        }
        m_data.at(bit_idx / BITS_P_BYTE) = byt;
    }

    void setBitAt_OneIdx(const size_t bit_idx, const uint8_t val) {
        setBitAt(bit_idx - 1, val);
    }

    void str() const {
        printBinary((char*)m_data.data(), m_data.size(), m_blockSize);
    }

    std::vector<uint8_t> m_data;
private:
    size_t m_blockSize;
};

#endif //CS465_DES_BLOCK_HPP
