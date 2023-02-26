#include "SHA256.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::hex;
using std::uppercase;
using std::setfill;
using std::setw;


static constexpr uint32_t initial_hash_value[] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

static constexpr uint32_t round_constant[] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
        0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
        0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
        0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
        0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
        0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};


void print_binary(uint32_t value) {
	for (int8_t pos = 31; pos >= 0; pos--) {
		uint32_t mask = 1 << pos;

		if (value & mask) {
			cout << "1";
		}
		else {
			cout << "0";
		}

		if (pos % 8 == 0)
			cout << " ";
	}
	cout << endl;
}

void print_block(Block block) {
	cout << "Printing block:\n";
	for (int word = 0; word < 16; word++) {
		cout << "Word " << word << ": ";
		print_binary(block.words[word]);
	}
	cout << endl;
}

std::string SHA256::hash(std::string input) {
	message_blocks.clear();

	// Preprocessing stage.
	add_pading(input);
	parse_message(input);

	// Hash computation.
	return compute_hash();
}

// ======================================================
// Preprocessing.
// ======================================================

void SHA256::add_pading(std::string &input) {
	uint64_t message_bits = input.size() * 8;
	uint64_t bits_last_block = message_bits % 512;

	// The last block contains: [message, 1-bit delimiter, padding zeros, and 64 bits for message length]. 
	// If message size is greater than 447 bits there isn't enough space for the message length and delimiter,
	// therefore more padding needs to be added ti create a new block.
	int16_t padding_zeros;
	if (bits_last_block > 447) {
		padding_zeros = 1024 - bits_last_block - 65;
	}
	else {
		padding_zeros = 512 - bits_last_block - 65;
	}

	// Append delimiter and 7 zeros (there will always be at least 7 zeros).
	input.push_back(0x80);

	// Append padding.
	for (int16_t i = padding_zeros - 7; i > 0; i -= 8) {
		input.push_back(0);
	}

	// Append message length.
	for (int8_t shift = 56; shift >= 0; shift -= 8) {
		input.push_back((message_bits >> shift) & 0xFF);
	}
}

void SHA256::parse_message(std::string& input) {
	Block current_block;
	uint32_t word = 0;
	uint8_t processed_characters = 0;
	uint8_t processed_words = 0;

	for (const uint8_t& character : input) {
		word <<= 8;
		word += character;

		// Every 4 processed characters we have a new 32-bits word.
		if (++processed_characters == 4) {
			current_block.words[processed_words++] = word;
			word = processed_characters = 0;

			// Every 16 processed words we have a new 512-bits block.
			if (processed_words == 16) {
				message_blocks.push_back(current_block);
				processed_words = 0;
				current_block = {};
			}
		}
	}
}

// ======================================================
// Hash Computation.
// ======================================================

std::string SHA256::compute_hash() {
	uint32_t H[8] = { 0 };

	for (uint8_t i = 0; i < 8; i++) {
		H[i] = initial_hash_value[i];
	}

	for (const auto& block : message_blocks) {
		uint32_t a, b, c, d, e, f, g, h, T1, T2, W[64];

		// Prepare the message schedule.
		for (uint8_t t = 0; t <= 15; t++) {
			W[t] = block.words[t];
		}
		for (uint8_t t = 16; t <= 63; t++) {
			W[t] = lowerSigma1(W[t - 2]) + W[t - 7] + lowerSigma0(W[t - 15]) + W[t - 16];
		}

		// Initialize working variables.
		a = H[0];
		b = H[1];
		c = H[2];
		d = H[3];
		e = H[4];
		f = H[5];
		g = H[6];
		h = H[7];

		// Compression function.
		for (uint8_t t = 0; t < 64; t++) {
			T1 = h + upperSigma1(e) + choose(e, f, g) + round_constant[t] + W[t];
			T2 = upperSigma0(a) + majority(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		// Compute intermediate hash value.
		H[0] = a + H[0];
		H[1] = b + H[1];
		H[2] = c + H[2];
		H[3] = d + H[3];
		H[4] = e + H[4];
		H[5] = f + H[5];
		H[6] = g + H[6];
		H[7] = h + H[7];
	}

	std::stringstream hash_string;
	for (uint8_t i = 0; i < 8; ++i) {
		hash_string << setfill('0') << setw(8) << hex << H[i];
	}

	return hash_string.str();
}

// ======================================================
// Hashing functions and operators.
// ======================================================

uint32_t SHA256::rotr(uint32_t word, uint32_t positions) {
	return (word >> positions) | (word << (32 - positions));
}

uint32_t SHA256::choose(uint32_t x, uint32_t y, uint32_t z) {
	return (x & y) ^ (~x & z);
}

uint32_t SHA256::majority(uint32_t a, uint32_t b, uint32_t c) {
	return (a & b) ^ (a & c) ^ (b & c);
}

uint32_t SHA256::upperSigma0(uint32_t x) {
	return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

uint32_t SHA256::upperSigma1(uint32_t x) {
	return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

uint32_t SHA256::lowerSigma0(uint32_t x) {
	return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint32_t SHA256::lowerSigma1(uint32_t x) {
	return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}