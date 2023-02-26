#ifndef ZUC_H
#define ZUC_H

void Initialization(unsigned char* init_k, unsigned char* init_iv);
void GenerateKeystream(unsigned int* pKeystream, int KeystreamLen);

#endif