#ifndef ZUC_H
#define ZUC_H



/*
* 针对codec2中的2.4 kbps压缩率的每分钟密钥生成
* 用于生成1分钟长密钥
	init_k : 初始密钥（量子密钥）
	char_key: 生成的 6 个char字符类型密钥的二维数组 char_key[3000][6]

	3000 * 20 ms = 60,000 ms
	3000 * 6 * 8 =144,000 bit    char_key[3000][6]
	144000 / 32 = 4500    int_key
*/
void Gen7Char_copa2400(unsigned char* init_k, unsigned char char_key[][6]);

void Gen7Char_copa1200(unsigned char* init_k, unsigned char char_key[][6]);



#endif
