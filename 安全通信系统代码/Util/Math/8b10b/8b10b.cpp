//
// Created by calt on 18-7-2.
//

#include "8b10b.h"
#include <Util/Math/Convert/Convert.h>

std::vector<std::string> encode_8b_to_10b (const std::vector<std::string> &origins) {
    std::string plain;
    for (const auto& str: origins) {
        plain += str;
    }
    return encode_8b_to_10b(plain);
}

std::vector<std::string> encode_8b_to_10b (const std::string &plain) {
    Convert converter;
    std::vector<std::string> vector_8b;
    for (int i=0;i+8<=plain.size();i+=8) {
        vector_8b.push_back(plain.substr(i, 8));
    }

    std::vector<std::string> vector_10b;
    int rd = -1;
    for (int i=0;i<vector_8b.size();i++) {
        long byte = converter.binstr2dec(vector_8b[i]);
        Encode8b10bFlag flags;
        flags.rd = rd;
        char byte5B = byte & 0x1F;
        char byte3B = (byte & 0xE0) >> 5;
        char enc6B = five2six(byte5B, flags);
        char enc4B = three2four(byte3B, flags);
        long symbol = (enc6B << 4) | enc4B;

        if (calculate_disparity(symbol, 10)) rd = -rd;

        vector_10b.push_back(converter.dec2bin(symbol, 10));
    }
    return vector_10b;
}

bool calculate_disparity (long data, int length) {
    int ones = 0, zeros = 0, i = 0x1;
    while (length > 0) {
        (data & i)? ones++: zeros++;
        i = i << 1;
        length--;
    }
    return (ones != zeros);
}

char five2six(char data, Encode8b10bFlag &flags){
    char enc = x_tables[data];
    bool disparity = calculate_disparity(enc, 6);
    // Flags used for 3b4b encoding
    flags.inv   = (disparity && flags.rd < 0) || !(disparity || flags.rd < 0); // (disparity) XOR (rd < 0)
    flags.y7pos = (data == 11 || data == 13 || data == 14);        // special cases for y = 7 e rd > 0
    flags.y7neg = (data == 17 || data == 18 || data == 20);        // special cases for y = 7 e rd < 0
    if (flags.rd > 0 && (enc == 0x38 || disparity))
        enc = ~enc & 0x3F; // complement of enc (6 bits -> 0x3F = 0011 1111)
    return enc;
}

char three2four(char data, Encode8b10bFlag &flags){
    char enc;
    if (data == 7 && flags.y7neg && flags.rd < 0){
        enc = y_tables[8];
    } else if (data == 7 && flags.y7pos && flags.rd > 0) {
        enc = ~y_tables[8] & 0xF;
    } else {
        enc = y_tables[data];
        if (flags.inv && (enc == 0xC || calculate_disparity(enc, 4)))
            enc = ~enc & 0xF; // complement of enc (4 bits -> 0xF = 0000 1111)
    }
    return enc;
}
