#include "Zuc256.h"
#include "malloc.h"

/*
* ���codec2�е�2.4 kbpsѹ���ʵ�ÿ������Կ����
* ��������1���ӳ���Կ
	init_k : ��ʼ��Կ��������Կ��
	char_key: ���ɵ� 6 ��char�ַ�������Կ�Ķ�ά���� char_key[3000][6]

	3000 * 20 ms = 60,000 ms
	3000 * 6 * 8 =144,000 bit    char_key[3000][6]
	144000 / 32 = 4500    int_key
*/
void Gen7Char_copa2400(unsigned char* init_k, unsigned char char_key[][6]) {
	unsigned int i, j, k = 0;
	unsigned int count;		//
	unsigned char init_iv[25] = { 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00 };	//��ʼ����	
	unsigned int* int_key = (unsigned int*)malloc(4500 * sizeof(unsigned int));//���ɵ�4500��int���͵���Կ
	Initialization(init_k, init_iv);
	GenerateKeystream(int_key, 4500);
	for (j = 0; j < 3000; j++)
	{
		for (i = 0; i < 6; ++i)
		{
			char_key[j][i] = int_key[k / 4] & 0xff;//ȡÿ��int�ĵͰ�λ
			int_key[k / 4] = int_key[k / 4] >> 8;
			k++;
		}
	}
	free(int_key);
}



/*
* ���codec2�е�1.2 kbpsѹ���ʵ�ÿ������Կ����
* �������ɷ��ӳ���Կ
	init_k : ��ʼ��Կ��������Կ��
	char_key: ���ɵ� 6 ��char�ַ�������Կ�Ķ�ά���� char_key[1500][6]

	1500 * 40 ms = 60,000 ms
	1500 * 6 * 8 =72,000 bit    char_key[1500][6]
	72000 / 32 = 2250    int_key
*/
void Gen7Char_copa1200(unsigned char* init_k, unsigned char char_key[][6]) {
	unsigned int i, j, k = 0;
	unsigned int count;
	unsigned char init_iv[25] = { 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00 };	//��ʼ����	
	unsigned int* int_key = (unsigned int*)malloc(2250 * sizeof(unsigned int));//���ɵ�2250��int���͵���Կ
	Initialization(init_k, init_iv);
	GenerateKeystream(int_key, 2250);
	for (j = 0; j < 1500; j++)
	{
		for (i = 0; i < 6; ++i)
		{
			char_key[j][i] = int_key[k / 4] & 0xff;//ȡÿ��int�ĵͰ�λ
			int_key[k / 4] = int_key[k / 4] >> 8;
			k++;
		}
	}
	free(int_key);
}

void Gen72000(unsigned char* init_k,unsigned int* int_key) {
    unsigned int i, j, k = 0;
    unsigned int count;
    unsigned char init_iv[25] = { 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00 };	//��ʼ����
//    unsigned int* int_key = (unsigned int*)malloc(2250 * sizeof(unsigned int));//���ɵ�2250��int���͵���Կ
    Initialization(init_k, init_iv);
    GenerateKeystream(int_key, 2250);
}
