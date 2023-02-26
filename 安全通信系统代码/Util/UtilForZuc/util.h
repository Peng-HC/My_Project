#include <string>
#include <vector>
#include <algorithm>
using namespace std;
// util.cpp�д�Ÿ��ֹ��ߺ���

//������Կ�����������ʹ��SM3��ϣ�㷨�õ�Ma
string generateMaHexStr(string Key_Hex, int randomLen);
//ʮ������ -> ������
string HexStrToBinStr(string strHex);
//������ -> ʮ������
string BinStrToHexStr(string str);
//ʮ����->ʮ������
string DecNumToHexStr(std::vector<uint32_t> nums);
string DecNumToHexStr(int num);
//������->ʮ����
int BinStrToDecNum(string strBin);
// 根据zuc初始密钥生成zuc密钥流
string generate_zuc_key(string zuc_init_key);

std::string dec2bin(int n);
long binstr2dec(std::string binary_str);

//std::string str2binstr (std::string str);