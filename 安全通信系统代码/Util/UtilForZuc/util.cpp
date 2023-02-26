#include "util.h"
#include "Util/Digest/Sm3/SM3.h"
#include "Util/Zuc/zuc256_1min.h"
#include "Util/Math/Convert/Convert.h"
#include <time.h>
using namespace std;

const int HashMapLen=256;

// 根据zuc初始密钥生成72000bit的2进制的zuc密钥流
std::string generate_zuc_key(std::string zuc_init_key) {
    Convert convert;
//    unsigned char* Init_Key =  (unsigned char *)convert.binstr2str(zuc_init_key).c_str();
    const int charLen = HashMapLen / (sizeof(unsigned char)*8);
    //cout << "charLen=" << charLen << endl;
    unsigned char Init_Key[charLen];
    for (int i = 0; i < charLen; i++) {
        string temp = "";
        for (int j = i*8; j < 8*(i+1); j++) {
            temp += zuc_init_key[j];
        }

        Init_Key[i] = BinStrToDecNum(temp);
    }
    const int row = 1500;
    unsigned char Char_Key[row][6];

//    for (int i = 0; i < row; i++) {
//        for (int j = 0; j < 6; j++) {
//            Char_Key[i][j]='\0';
//        }
//    }
    Gen7Char_copa1200(Init_Key, Char_Key);
    // test
//    std::cout<<"@test"<<std::endl;
//    for(int i=0;i<row;i++) {
//        for(int j=0;j<6;j++) {
//            std::cout<<Char_Key[i][j]<<" "<<(int)Char_Key[i][j]<<" "<<DecNumToHexStr((int)Char_Key[i][j])<<" ";
//        }
//        std::cout<<endl;
//    }
//    std::string zuc_key_hex="";
    std::string zuc_key_bin="";
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < 6; j++) {
            // char字符转10进制
            int temp_dec = int(Char_Key[i][j]);
            // 10进制转16进制
//            zuc_key_hex += DecNumToHexStr(temp_dec);
//            // 10进制转2进制
            zuc_key_bin += convert.dec2bin(temp_dec, 8);
        }
    }
//    cout << "生成的密钥流长度为:" << zuc_key_hex.length()*4<<"bit"<< endl;
    cout << "生成的密钥流长度为:" << zuc_key_bin.length()<<"bit"<< endl;
    cout << "生成的密钥流长度为:" << zuc_key_bin.length()<<"bit"<< endl;
    cout << "生成的密钥流长度为:" << zuc_key_bin.length()<<"bit"<< endl;
//    return zuc_key_hex;
    return zuc_key_bin;
}

#define TOLOWER(p) {transform(p.begin(),p.end(),p.begin(),::tolower);}

std::string dec2bin(int n) {
    std::string res;
    while (n) {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }
    if (res.empty()) {
        res = "0";
    } else{
        std::reverse(res.begin(), res.end());
    }
    return res;
};

long binstr2dec(std::string binary_str) {
    long res = 0;
    for (int i=0;i<binary_str.size();i++) {
        if (binary_str[i] == '1') {
            res += std::pow(2, binary_str.size() - i - 1);
        }
    }
    return res;
}

string HexStrToBinStr(string strHex)
{
	string sReturn = "";
	unsigned int len = strHex.length();
	TOLOWER(strHex);
	for (unsigned int i = 0; i < len; i++)
	{
		switch (strHex[i])
		{
		case '0': sReturn.append("0000"); break;
		case '1': sReturn.append("0001"); break;
		case '2': sReturn.append("0010"); break;
		case '3': sReturn.append("0011"); break;
		case '4': sReturn.append("0100"); break;
		case '5': sReturn.append("0101"); break;
		case '6': sReturn.append("0110"); break;
		case '7': sReturn.append("0111"); break;
		case '8': sReturn.append("1000"); break;
		case '9': sReturn.append("1001"); break;
		case 'a': sReturn.append("1010"); break;
		case 'b': sReturn.append("1011"); break;
		case 'c': sReturn.append("1100"); break;
		case 'd': sReturn.append("1101"); break;
		case 'e': sReturn.append("1110"); break;
		case 'f': sReturn.append("1111"); break;
		}
	}
	return sReturn;
}

string BinStrToHexStr(string str)
{
	string str_Hex = "";
	string temp_Hex = "";
	int iHex = 0;
	if (str.size() % 4 != 0)
	{
		int num = 4 - (str.size() % 4);
		for (int i = 0; i < num; i++)
		{
			str = "0" + str;
		}
	}
	for (int i = 0; i < str.size(); i += 4)
	{
		iHex = (str[i] - '0') * 8 + (str[i + 1] - '0') * 4 + (str[i + 2] - '0') * 2 + (str[i + 3] - '0') * 1;
		if (iHex >= 10)
		{
			int left = iHex % 16;
			temp_Hex = 'a' + left - 10;
		}
		else
		{
			temp_Hex = to_string(iHex);
		}
		str_Hex += temp_Hex;
	}
	return str_Hex;
}

string DecNumToHexStr(std::vector<uint32_t> nums) {
	char hex[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	string ans = "";
	for (int i = 0; i < nums.size(); i++) {
		int j = 0, temp[100];
		uint32_t n = nums[i];
		while (n > 0) {
			temp[j++] = n % 16;
			n = n / 16;
		}
		if (j == 1) {
			ans += "0";
		}
		for (j = j - 1; j >= 0; j--) {
			int m = temp[j];
			ans += hex[m];
		}
	}
	return ans;
}

string DecNumToHexStr(int num) {
	char hex[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	string ans = "";
	int j = 0, temp[100];
	while (num > 0) {
		temp[j++] = num % 16;
		num = num / 16;
	}
	if (j == 1) {
		ans += "0";
	}
	for (j = j - 1; j >= 0; j--) {
		int m = temp[j];
		ans += hex[m];
	}
	return ans;
}

int BinStrToDecNum(string strBin) {
	int sum=0;
	for (int i = 0; i < strBin.size(); i++)
	{
		if (strBin[i] > '1')
		{
			return 0;
		}
		if (strBin[i] == '1')
		{
			int j = pow(2, strBin.size() - i - 1);
			sum += j;
		}
	}
	return sum;
}
