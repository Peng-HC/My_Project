#ifndef ZUC_H
#define ZUC_H



/*
* ���codec2�е�2.4 kbpsѹ���ʵ�ÿ������Կ����
* ��������1���ӳ���Կ
	init_k : ��ʼ��Կ��������Կ��
	char_key: ���ɵ� 6 ��char�ַ�������Կ�Ķ�ά���� char_key[3000][6]

	3000 * 20 ms = 60,000 ms
	3000 * 6 * 8 =144,000 bit    char_key[3000][6]
	144000 / 32 = 4500    int_key
*/
void Gen7Char_copa2400(unsigned char* init_k, unsigned char char_key[][6]);

void Gen7Char_copa1200(unsigned char* init_k, unsigned char char_key[][6]);



#endif
