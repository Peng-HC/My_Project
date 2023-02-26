//
// Created by louis on 22-5-21.
//

#ifndef ROI_LDPCCODE_H
#define ROI_LDPCCODE_H

#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <utility>
#include <immintrin.h>
#include <iostream>
#include <chrono>

#define maxiter 20

class LdpcCode {
public:
    struct SMatrix {
        int col;				/* index in base braph  			*/
        int shift;				/* shift value					    */
        int index;				/* block index, ==col*zc		    */
        SMatrix() : col(), shift(), index() {};
        SMatrix(int x, int y, int z) : col(x), shift(y), index(z) {};
    };

    // 构造函数
//    LdpcCode(int bg,int zc,double rate) {
//        this->InitMatrix(bg,zc,rate);
//        this->InitConfig();
//    }

    std::vector<std::vector<SMatrix>> PCM;	 /* parity check matrix */

    uint16_t m, n;				/* cols and rows of bg				*/
    uint16_t k, zc;				/* infobits and lifting size		*/
    float* llr;				    /* llrs for hard decision			*/
    float* b;					/* layer llr buffer					*/
    float** r;					/* llrs from last iteration			*/
    int* vec_shift;				/* index buffer of CyclicShiftTable	*/
    uint8_t* hard_decision;		/* hard decision buffer				*/
    uint8_t* layer_len;			/* PCM[layer].size()-1				*/
    uint16_t** Table;			/* cyclic shift table				*/
    SMatrix** PCM_layer_header;	/* header ptr for every layer	    */
    uint32_t* index_nums;
    uint8_t** info_prod;
    uint8_t* block_buf;
    int itercounter = 0;
    bool initflag = false; uint16_t nonzero_pos;
    int16_t nonzero_shift, firstrow_shift;
    int bound[4];
    int enc_reg_nums;
    int dec_reg_nums;

    void InitMatrix(int, int, double);
    void InitConfig();
    std::string LdpcEncoder(std::string&);
    std::string LdpcDecoder(std::string&, int&);
    void SubtractLLR(int, float*, float*, float*);
    void UpdateLLR(int, float*, float*, float*);
    std::string bin2str(uint8_t*, int);
    void str2bin(std::string&, uint8_t*);
    bool isvalid(uint8_t*);

    ~LdpcCode() {
        if (initflag) {
            delete [] this->llr;
            delete [] this->hard_decision;
            delete [] this->b;
            delete [] this->layer_len;
            delete [] this->PCM_layer_header;
            delete [] this->block_buf;
            delete [] this->vec_shift;
            delete [] this->index_nums;

            for (int i = 0; i < this->m; i++)
                delete [] this->r[i];
            delete [] this->r;
            for (int i = 0; i < this->m; i++)
                delete[] this->Table[i];
            delete [] this->Table;
            for (int i = 0; i < 4; i++) {
                delete[] this->info_prod[i];
            }
            delete[] this->info_prod;
        }
    }
};
#endif //ROI_LDPCCODE_H
