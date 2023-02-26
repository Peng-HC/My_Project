//
// Created by calt on 18-7-2.
//

#ifndef ROI_8B10B_H
#define ROI_8B10B_H

#include <string>
#include <vector>

struct Encode8b10bFlag {
    int rd;
    bool inv;
    bool y7pos;
    bool y7neg;
};

static const int x_tables[] = {0x27, 0x1D, 0x2D, 0x31, 0x35, 0x29, 0x19, 0x38, 0x39, 0x25, 0x15, 0x34, 0xD, 0x2C, 0x1C, 0x17, 0x1B, 0x23, 0x13, 0x32, 0xB, 0x2A, 0x1A, 0x3A, 0x33, 0x26, 0x16, 0x36, 0xE, 0x2E, 0x1E, 0x2B};
static const int y_tables[] = { 0xB, 0x9, 0x5, 0xC, 0xD, 0xA, 0x6, 0xE, 0x7 };

bool calculate_disparity (long data, int length);
std::vector<std::string> encode_8b_to_10b (const std::vector<std::string> &origins);
std::vector<std::string> encode_8b_to_10b (const std::string &text);


char five2six(char data, Encode8b10bFlag &flags);
char three2four(char data, Encode8b10bFlag &flags);

#endif //ROI_8B10B_H
