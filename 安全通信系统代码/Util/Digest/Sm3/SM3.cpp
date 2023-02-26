//
// Created by ttai on 2021/12/16.
//

#include "SM3.h"
int get_l(string m) {
	return m.size() * 8;
}

int get_k(int l) {
	int tmp = (l + 1) % 512;
	int k = 448 - tmp;
	if (k < 0)return 512 + k;
	return k;
}


string string_bin(string m) {

	string m_hex = string_hex(m);
	string str = "";
	for (int i = 0; i < m_hex.size(); i++) {
		string s(1, m_hex[i]);
		int r = stoi(s, nullptr, 16);
		bitset<4> bit(r);
		str += bit.to_string();
	}
	return str;
}


string string_hex(string m) {
	string strSha1 = "";

	unsigned int c = 0;

	int nLen = m.size();

	for (int i = 0; i < nLen; i++) {
		// 查看md一个字节里的信息
		unsigned int x = 0;
		c = (m[i] >> 4) & 0x0f;
		strSha1 += hex_chars[c];
		x = m[i] & 0x0f;
		strSha1 += hex_chars[x];
	}

	return strSha1;
}


string padding(string m) {
	//cout << "padding run " << endl;
	int l = get_l(m);
	int k = get_k(l);
	string m_bin = string_bin(m);
	m_bin += '1';
	m_bin.insert(m_bin.size(), k, '0');
	//长度转换为64bit字符串
	bitset<64> bit(l);
	m_bin += bit.to_string();
	return m_bin;
}

string CF(string iv, string b) {
	// cout << "CF run" << endl;
	vector<string> word_list;
	vector<string> word_list_o;
	mess_expand(b, word_list, word_list_o);
	bitset<32> A, B, C, D, E, F, G, H, SS1, SS2, TT1, TT2;
	bitset<32> T;
	//iv初始化
	string iv_bin = iv;
	A = bitset<32>(iv_bin.substr(0, 32));

	B = bitset<32>(iv_bin.substr(32, 32));

	C = bitset<32>(iv_bin.substr(64, 32));

	D = bitset<32>(iv_bin.substr(96, 32));

	E = bitset<32>(iv_bin.substr(128, 32));

	F = bitset<32>(iv_bin.substr(160, 32));

	G = bitset<32>(iv_bin.substr(192, 32));

	H = bitset<32>(iv_bin.substr(224, 32));

	for (int j = 0; j <= 63; j++) {
		if (j >= 0 && j <= 15)T = bitset<32>(hex_bin(T_0_15));
		else if (j >= 16 && j <= 63)T = bitset<32>(hex_bin(T_16_63));
		SS1 = CROL32(add_32(add_32(CROL32(A, 12), E), CROL32(T, j)), 7);
		SS2 = SS1 ^ (CROL32(A, 12));
		TT1 = add_32(add_32(add_32(bitset<32>(FF(A.to_string(), B.to_string(), C.to_string(), j)), D), SS2),
			bitset<32>(word_list_o[j]));
		TT2 = add_32(add_32(add_32(bitset<32>(GG(E.to_string(), F.to_string(), G.to_string(), j)), H), SS1),
			bitset<32>(word_list[j]));
		D = C;
		C = CROL32(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = CROL32(F, 19);
		F = E;
		E = bitset<32>(P0(TT2.to_string()));
	}
	string iv_tmp = A.to_string() + B.to_string() + C.to_string() + D.to_string() + E.to_string() + F.to_string() +
		G.to_string() + H.to_string();
	bitset<256> iv1(iv_bin);
	bitset<256> new_iv(iv_tmp);
	new_iv ^= iv1;
	return new_iv.to_string();
}

string bin_hex(string bin) {
	string hex = "";
	for (int i = 0; i < bin.size(); i += 4) {
		string tmp = bin.substr(i, 4);
		int r = stoi(tmp, nullptr, 2);
		hex += hex_chars[r];
	}
	return hex;
}

string hex_bin(string hex) {
	string bin = "";
	for (int i = 0; i < hex.size(); i++) {
		string tmp(1, hex[i]);
		bitset<4> bit(stoi(tmp, nullptr, 16));
		bin += bit.to_string();
	}
	return bin;
}

int mess_expand(string b, vector<string> &word_list, vector<string> &word_list_o) {
	//cout << "mess_expand run" << endl;
	//将512bitb分成16个字，1个字32bit
	for (int i = 0; i < b.size(); i += 32) {
		string tmp = b.substr(i, 32);
		word_list.push_back(tmp);
	}
	for (int j = 16; j <= 67; j++) {
		bitset<32> j_16(word_list[j - 16]);
		bitset<32> j_9(word_list[j - 9]);
		bitset<32> j_3(word_list[j - 3]);
		j_3 = CROL32(j_3, 15);
		j_16 ^= j_9;
		j_16 ^= j_3;
		string p1 = P1(j_16.to_string());
		bitset<32> j_13(word_list[j - 13]);
		j_13 = CROL32(j_13, 7);
		bitset<32> j_6(word_list[j - 6]);
		bitset<32> j_p1(p1);
		j_p1 ^= j_13;
		j_p1 ^= j_6;
		word_list.push_back(j_p1.to_string());
	}
	for (int j = 0; j <= 63; j++) {
		bitset<32> bit_j(word_list[j]);
		bitset<32> bit_j4(word_list[j + 4]);
		bitset<32> ret = bit_j ^ bit_j4;
		word_list_o.push_back(ret.to_string());
	}
	return 1;
}

string P0(string s) {
	bitset<32> bit(s);
	bitset<32> bit1 = CROL32(bit, 9);
	bitset<32> bit2 = CROL32(bit, 17);
	bitset<32> bit3 = bit1 ^ bit2;
	bit ^= bit3;
	return bit.to_string();
}

string P1(string s) {
	bitset<32> bit(s);
	bitset<32> bit1 = CROL32(bit, 15);
	bitset<32> bit2 = CROL32(bit, 23);
	bitset<32> bit3 = bit1 ^ bit2;
	bit ^= bit3;
	return bit.to_string();
}

string FF(string x, string y, string z, int j) {

	bitset<32> X(x);
	bitset<32> Y(y);
	bitset<32> Z(z);
	if (j >= 0 && j <= 15) {
		bitset<32> ret = X ^ Y ^ Z;
		return ret.to_string();
	}
	else if (j >= 16 && j <= 63) {
		bitset<32> tmp1 = X & Y;
		bitset<32> tmp2 = X & Z;
		bitset<32> tmp3 = Y & Z;
		bitset<32> ret = tmp1 | tmp2 | tmp3;
		return ret.to_string();
	}

}

string GG(string x, string y, string z, int j) {
	bitset<32> X(x);
	bitset<32> Y(y);
	bitset<32> Z(z);
	if (j >= 0 && j <= 15) {
		bitset<32> ret = X ^ Y ^ Z;
		return ret.to_string();
	}
	else if (j >= 16 && j <= 63) {
		bitset<32> tmp1 = X & Y;
		bitset<32> tmp2 = ~X & Z;
		bitset<32> ret = tmp1 | tmp2;
		return ret.to_string();
	}
}

string itera_CF(string iv, vector<string> b) {
	//cout << "itera_CF run" << endl;
	string tmp_iv;
	for (int i = 0; i < b.size(); i++) {
		tmp_iv = iv;
		iv = CF(tmp_iv, b[i]);
	}
	return iv;
}

string sm3_hash(string m) {
	string m_hash = padding(m);
	vector<string> b;
	for (int i = 0; i < m_hash.size(); i += 512) {
		string tmp = m_hash.substr(i, 512);
		b.push_back(tmp);
	}
	string iv = hex_bin(iv_f);
	string iv_final = itera_CF(iv, b);
	return iv_final;
}

bitset<32> CROL32(bitset<32> bit, int num) {
	if (num >= 32)num -= 32;
	bitset<32> tmp1 = bit << num;
	bitset<32> tmp2 = bit >> (32 - num);
	return tmp1 ^ tmp2;
}

bitset<32> add_32(bitset<32> bit1, bitset<32> bit2) {
	unsigned long long tmp1 = bit1.to_ullong();
	unsigned long long tmp2 = bit2.to_ullong();
	bitset<32> b("11111111111111111111111111111111");
	unsigned long long t = b.to_ullong() + 1;
	unsigned long long ret = (tmp1 + tmp2) % t;
	bitset<32> result(ret);
	return result;
}
