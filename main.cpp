#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>

#include "Block.hpp"
#include "PermutedChoice.hpp"

/**
 *
 * Stephen Eckels
 *
**/

Block permuteKey(const std::string& s) {
    Block b(s, 64, true);
    return PC_1(b);
}

int main() {
    std::cout << "Enter a file path to open as input: ";
    std::string fileInPath;
    std::cin >> fileInPath;
    std::ifstream        in(fileInPath, std::ios::binary | std::ios::in);

    std::string mode = "";
    std::cout << "Encrypt Or Decrypt (D/E): ";
    std::cin >> mode;

    std::vector<Block> blocks;
    if(mode == "E") {
        std::vector<uint8_t> filtered;
        filtered.reserve(128);

        char c;
        while (in.get(c)) {
            if (c <= 0x20 || c > 0x7E)
                continue;

            filtered.push_back((uint8_t)c);
        }
        blocks = Block::buildBlocks(filtered, 64);
    } else if(mode == "D") {
        std::vector<uint8_t> buf;
        buf.reserve(128);

        char c;
        while (in.get(c)) { buf.push_back((uint8_t)c); }
        blocks = Block::buildBlocks(buf, 64);
    } else {
        std::cout << "Unsupported Mode, Exiting" << std::endl;
        return 1;
    }

    // Key Permutation
    std::cout << "Enter the password: ";
    std::string password;
    std::cin >> password;
    Block              PC_1             = permuteKey(password);
    password = "";

    // 16-Round Key Generation
    std::vector<Block> keys;
    Block              Ci               = leftHalf(PC_1);
    Block              Di               = rightHalf(PC_1);
    uint8_t            round_shifts[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
    for (size_t round = 0; round < 16; round++) {
        Ci = circularLS(Ci, round_shifts[round]);
        Di = circularLS(Di, round_shifts[round]);
        keys.push_back(PC_2(join(Ci, Di)));

        std::cout << "Round " << round + 1 << " key:" << std::endl;
        keys.back().str();
    }

    // feed backwards through network in decrypt
    if(mode == "D")
        std::reverse(keys.begin(), keys.end());

    // Feistel Network Model
    std::vector<Block> cipherBlocks;
    for(const auto& data : blocks) {
        std::cout << "Data Block: " << std::endl;
        data.str();

        // Initial Permutation
        Block I = IP(data);
        Block L_i = leftHalf(I);
        Block R_i = rightHalf(I);

        // 16 Rounds
        for(size_t round = 0; round < 16; round++) {
            std::cout << "Round " << round + 1 << std::endl << std::endl;
            std::cout << "L_i:" << std::endl;
            L_i.str();

            std::cout << "R_i:" << std::endl;
            R_i.str();

            Block oldLi = L_i;
            L_i = R_i;
            R_i = XOR(oldLi, F(R_i, keys[round]));

            std::cout << "Xor w/ L_i - 1" << std::endl;
            R_i.str();
        }

        // 32 bit Swap + Inverse Initial Permutation
        cipherBlocks.push_back(IP_1(join(R_i,L_i)));

        std::cout << std::endl << "Final Permutation:" << std::endl;
        cipherBlocks.back().str();
    }

    std::cout << "Enter file path to output to: ";
    std::string fileOut;
    std::cin >> fileOut;

    std::ofstream out(fileOut, std::ios::binary | std::ios::out);
    for(const Block& block : cipherBlocks) {
        out.write((char*)block.m_data.data(), block.m_data.size());
    }
    out.close();
    return 0;
}
