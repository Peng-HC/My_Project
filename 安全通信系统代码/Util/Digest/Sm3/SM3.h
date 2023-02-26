//
// Created by ttai on 2021/12/16.
//

#ifndef SM3_ALGORITHM_SM3_H
#define SM3_ALGORITHM_SM3_H

#include "string"
#include "algorithm"
#include "bitset"
#include "vector"
#include "iostream"
using namespace std;
static const char hex_chars[] = "0123456789abcdef";  //ʮ�������ַ�
const static string iv_f = "7380166f4914b2b9172442d7da8a0600a96f30bc163138aae38dee4db0fb0e4e";
const static string T_0_15 = "79cc4519";
const static string T_16_63 = "7a879d8a";
//ѭ������
bitset<32>CROL32(bitset<32>bit, int num);
//mod2^32������
bitset<32>add_32(bitset<32>bit1, bitset<32>bit2);
string bin_hex(string bin);

string hex_bin(string hex);

int get_l(string m);  //���ַ����ı��س���

int get_k(int l); //����l+k+1=448mod512
string string_bin(string m); //���ض������ַ���
string string_hex(string m); //����ʮ�������ַ���
string padding(string m);  //������512��������

//�û�����
string P0(string s);

string P1(string s);

//��������
string FF(string x, string y, string z, int j);

string GG(string x, string y, string z, int j);

string CF(string iv, string b);  //ѹ������
//����ѹ��
string itera_CF(string iv, vector<string> b);
int mess_expand(string b, vector<string>& word_list, vector<string>& word_list_o);  //��Ϣ��չ  ����132����
string sm3_hash(string m); //sm3��ϣ

#endif //SM3_ALGORITHM_SM3_H
