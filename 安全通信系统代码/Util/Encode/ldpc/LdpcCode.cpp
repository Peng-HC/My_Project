//
// Created by louis on 22-5-21.
//
#include "LdpcCode.h"

void LdpcCode::InitMatrix(int bg, int Zc, double Rate) {
    this->zc = Zc;
    enc_reg_nums = zc / 32;
    dec_reg_nums = zc / 8;
    int iLS = zc;
    int i, j, c;
    std::string bgname = "BG1_iLS0.txt";
    std::string bgpath = "../Util/Encode/ldpc/";
    bgname[2] = bg + '0';
    while (!(iLS & 1) && iLS > 2) {
        iLS >>= 1;
    }
    iLS >>= 1;
    if (iLS == 1) iLS = 0;
    bgname[7] = iLS + '0';
    /*	for (i = 0; i != 13; i++)
            std::cout << bgname[i];*/
    FILE* fp = fopen((bgpath + bgname).c_str(), "r");
    if (fp == NULL) {
        std::cout << "Failed reading BG file.";
        exit(0);
    }
    if (bg == 1) {
        m = 46; n = 68; k = 22;
        /*	if (Rate < 0.3334) {
                std::cout << "Rate is not compatible with Base Graph." << std::endl;
                exit(0);
            }*/
    }
    else {
        m = 42; n = 52; k = 10;
        /*	if (Rate < 0.2001) {
                std::cout << "Rate is not compatible with Base Graph." << std::endl;
                exit(0);
            }*/
    }
    std::vector<std::vector<short>> mat_t(m, std::vector<short>(n));
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            fscanf(fp, "%d", &c);
            mat_t[i][j] = c;
        }
    }
    fclose(fp);
    // Get a sub-PCM if the Rate is not BG's mother Rate (BG1:1/3, BG2:1/5)
    n = (int)round(k / Rate) + 2;
    m = n - k;
    for (i = 0; i < m; i++) {
        std::vector<SMatrix> indexs;
        for (j = 0; j < n; j++)
            if (mat_t[i][j] != -1) {
                SMatrix temp(j, mat_t[i][j] % zc, j*zc);
                indexs.emplace_back(temp);
            }
        PCM.emplace_back(indexs);
        //	std::cout << indexs.size() << std::endl;
    }
    nonzero_pos = (PCM[1][PCM[1].size() - 3].col == k) ? 1 : 2;
    firstrow_shift = PCM[0][PCM[0].size() - 2].shift;
    nonzero_shift = PCM[nonzero_pos][PCM[nonzero_pos].size() - 3].shift;
}

void LdpcCode::InitConfig() {
    initflag = true;
    llr = new float[n*zc];
    hard_decision = new uint8_t[n*zc];
    b = new float[n*zc];
    layer_len = new uint8_t[m];
    block_buf = new uint8_t[zc];
    vec_shift = new int[n];
    r = new float*[m];
    index_nums = new uint32_t[zc];

    for (int i = 0; i < m; i++)
        r[i] = new float[n*zc];

    Table = new uint16_t*[m];
    for (int i = 0; i < m; i++)
        Table[i] = new uint16_t[PCM[i].size() * zc];

    PCM_layer_header = new SMatrix*[m];
    for (int i = 0; i < m; i++)
        PCM_layer_header[i] = &PCM[i][0];

    for (int i = 0; i < m; i++)
        layer_len[i] = PCM[i].size() - 1;

    for (int i = 0; i < 4; i++) {
        int j = PCM[i].size() - 1;
        while (PCM[i][j].col >= k) j--;
        bound[i] = j;
    }

    info_prod = new uint8_t*[4];
    for (int i = 0; i < 4; i++)
        info_prod[i] = new uint8_t[zc];

    // Init cyclic shift table
    int k, t, s, index;
    for (int i = 0; i < PCM.size(); i++) {
        k = 0;
        for (int j = 0; j < PCM[i].size(); j++) {
            index = zc * PCM[i][j].col;
            s = PCM[i][j].shift;
            for (t = s + index; t < zc + index; t++)
                Table[i][k++] = t;
            for (t = index; t < s + index; t++)
                Table[i][k++] = t;
        }
    }

    for (int i = 0; i < zc; i++)
        index_nums[i] = i;
}

void LdpcCode::str2bin(std::string& str, uint8_t* seq) {
    int len = str.length();
    memcpy(seq, str.c_str(), len);
    for (int i = 0; i < len; i++)
        seq[i] ^= '0';
}

std::string LdpcCode::bin2str(uint8_t* seq, int seqSize) {
    std::string str(seqSize, 0);
    char* start = &str[0];
    memcpy(start, seq, seqSize);
    for (int i = 0; i < seqSize; i++) {
        start[i] ^= '0';
    }
    return str;
}

bool LdpcCode::isvalid(uint8_t* x) {
    // codeword validity check, using SIMD commands
    int i, j, k, idx;
    int t, shift_t;
    bool flag = true;
    uint8_t* buf = new uint8_t[zc];
    __m256i prod[enc_reg_nums];

    for (i = 0; i < m; i++) {
        for (j = 0; j < enc_reg_nums; j++)
            prod[j] = _mm256_setzero_si256();
        auto ptr = PCM_layer_header[i];
        for (j = 0; j <= layer_len[i]; j++) {
            t = ptr[j].index;
            shift_t = ptr[j].shift;
            memcpy(buf, x + t + shift_t, zc - shift_t);
            memcpy(buf + zc - shift_t, x + t, shift_t);
            for (k = 0, idx = 0; k < enc_reg_nums; k++, idx += 32) {
                __m256i reg_t = _mm256_loadu_si256((__m256i*)(buf + idx));
                prod[k] = _mm256_xor_si256(prod[k], reg_t);
            }
        }
        for (j = 0; j < enc_reg_nums; j++) {
            for (k = 0; k < 4; k++)
                if (prod[j][k] != 0) {
                    flag = false;
                    break;
                }
            if (!flag) break;
        }
        if (!flag) break;
    }

    //	std::cout << "check time : " << time << "ms" << std::endl;
//	delete[] prod;
    delete[] buf;
    return flag;
}

std::string LdpcCode::LdpcEncoder(std::string& msg) {
    int i, j, l;
    int t, index, shift_t;
    int codeword_len;
    uint8_t *msg_enc = new uint8_t[n*zc];
    codeword_len = n * zc;

    if (msg.length() > k*zc) {
        std::cout << "Message is too long, needs to slice.";
        return "";
    }
    str2bin(msg, msg_enc);

    // Double diagonal encoding for 5G NR LDPC codes

    // Calculate p[1]-p[zc]
    __m256i prod[enc_reg_nums];
    for (i = 0; i < 4; i++) {
        __m256i r_0;
        auto ptr = PCM_layer_header[i];
        for (j = 0; j < enc_reg_nums; j++)
            prod[j] = _mm256_setzero_si256();
        for (j = 0; j <= bound[i]; j++) {
            t = ptr[j].index;
            shift_t = ptr[j].shift;
            uint16_t lp = zc - shift_t;
            memcpy(block_buf, msg_enc + t + shift_t, lp);
            memcpy(block_buf + lp, msg_enc + t, shift_t);
            for (int reg_i = 0, idx = 0; reg_i < enc_reg_nums; reg_i++, idx += 32) {
                r_0 = _mm256_loadu_si256((__m256i*)(block_buf + idx));
                prod[reg_i] = _mm256_xor_si256(r_0, prod[reg_i]);
            }
            for (int reg_i = 0, idx = 0; reg_i < enc_reg_nums; reg_i++, idx += 32)
                _mm256_storeu_si256((__m256i*)(info_prod[i] + idx), prod[reg_i]);
        }
    }
    index = k * zc;
    shift_t = nonzero_shift;
    auto ptr1 = msg_enc + index;
    for (i = 0; i < zc; i += 32) {
        __m256i r_prod = _mm256_setzero_si256();
        for (j = 0; j < 4; j++) {
            __m256i r_0 = _mm256_loadu_si256((__m256i*)(&info_prod[j][i]));
            r_prod = _mm256_xor_si256(r_prod, r_0);
        }
        _mm256_storeu_si256((__m256i*)(block_buf + i), r_prod);
    }
    memcpy(ptr1 + shift_t, block_buf, zc - shift_t);
    memcpy(ptr1, block_buf + zc - shift_t, shift_t);

    // Calculate p[zc+1]-p[2*zc]
    t = index;
    index += zc;
    shift_t = firstrow_shift;
    /*for (i = 0; i < zc; i++) {
        msg_enc[index + i] = info_prod[0][i] ^ msg_enc[t + (i + shift_t) % zc];
    }*/
    __m256i r_prod, r_0;
    auto ptr2 = msg_enc + t;
    memcpy(block_buf, ptr2 + shift_t, zc - shift_t);
    memcpy(block_buf + zc - shift_t, ptr2 , shift_t);
    ptr2 = msg_enc + index;
    for (i = 0; i < zc; i += 32) {
        r_prod = _mm256_loadu_si256((__m256i*)(info_prod[0]+i));
        r_0 = _mm256_loadu_si256((__m256i*)(block_buf + i));
        r_0 = _mm256_xor_si256(r_0, r_prod);
        _mm256_storeu_si256((__m256i*)(ptr2 + i), r_0);
    }

    // Calculate p[3*zc+1]-p[4*zc]
    t = index - zc;
    index += (zc << 1);
    shift_t = firstrow_shift;
    /*for (i = 0; i < zc; i++) {
        msg_enc[index + i] = info_prod[3][i] ^ msg_enc[t + (i + shift_t) % zc];
    }*/
    auto ptr4 = msg_enc + t;
    memcpy(block_buf, ptr4 + shift_t, zc - shift_t);
    memcpy(block_buf + zc - shift_t, ptr4, shift_t);
    ptr4 = msg_enc + index;
    for (i = 0; i < zc; i += 32) {
        r_prod = _mm256_loadu_si256((__m256i*)(info_prod[3] + i));
        r_0 = _mm256_loadu_si256((__m256i*)(block_buf + i));
        r_0 = _mm256_xor_si256(r_0, r_prod);
        _mm256_storeu_si256((__m256i*)(ptr4 + i), r_0);
    }


    // Calculate p[2*zc+1]-p[3*zc]
    index = (k + 2)*zc;
    t = index + (nonzero_pos == 1 ? zc : -zc);
/*	for (i = 0; i < zc; i++) {
		msg_enc[index + i] = info_prod[3 - nonzero_pos][i] ^ msg_enc[t + i];
	}*/
    ptr1 = msg_enc + index;
    ptr2 = msg_enc + t;
    int zeropos = 3 - nonzero_pos;
    for (i = 0; i < zc; i += 32) {
        r_prod = _mm256_loadu_si256((__m256i*)(info_prod[zeropos] + i));
        r_0 = _mm256_loadu_si256((__m256i*)(ptr2 + i));
        r_0 = _mm256_xor_si256(r_0, r_prod);
        _mm256_storeu_si256((__m256i*)(ptr1 + i), r_0);
    }

    // Calculate p[4*zc+1]-p[m*zc]
    index += (zc << 1);
    for (i = 4; i < m; i++) {
        __m256i r_0;
        for (j = 0; j < enc_reg_nums; j++)
            prod[j] = _mm256_setzero_si256();
        /*SMatrix* ptr = &PCM[i][0];
        for (j = 0; j < layer_len[i]; j++) {
            shift_t = ptr[j].shift;
            t = ptr[j].col * zc;
            for (l = 0; l < zc; l++) {
                msg_enc[index + l] ^= msg_enc[t + (l + shift_t) % zc];
            }*/
        SMatrix* block = PCM_layer_header[i];
        for (j = 0; j < layer_len[i]; j++) {
            shift_t = block[j].shift;
            t = block[j].col * zc;
            uint16_t lp = zc - shift_t;
            memcpy(block_buf, msg_enc + t + shift_t, lp);
            memcpy(block_buf + lp, msg_enc + t, shift_t);
            for (int reg_i = 0, idx = 0; reg_i < enc_reg_nums; reg_i++, idx += 32) {
                r_0 = _mm256_loadu_si256((__m256i*)(block_buf + idx));
                prod[reg_i] = _mm256_xor_si256(r_0, prod[reg_i]);
            }

        }
        for (int reg_i = 0, idx = index; reg_i < enc_reg_nums; reg_i++, idx += 32)
            _mm256_storeu_si256((__m256i*)(msg_enc + idx), prod[reg_i]);
        index += zc;
    }

//	delete[] prod;
    //	std::cout << nonzero_pos << "!";
    /*	for (i = 0; i < 4; i++) {
            for (j = 0; j < zc; j++) {
                std::cout << unsigned(info_prod[i][j]) << " ";
            }
            std::cout << std::endl;
        }*/

    std::string str_msg_enc = bin2str(msg_enc, codeword_len);

    delete[] msg_enc;

    return str_msg_enc;
}


void LdpcCode::SubtractLLR(int layer, float* a, float* b, float* c) {
    // vectorization

    for (auto block : PCM[layer]) {
        int idx = block.index;
        for (int j = 0; j < dec_reg_nums; j++, idx += 8) {
            _mm256_storeu_ps(a + idx, _mm256_sub_ps(_mm256_loadu_ps(b + idx), _mm256_loadu_ps(c + idx)));
        }
    }
    // scalar
//	for (int i = 0; i < layer; i++) a[i] = b[i] - c[i];

    return;
}

void LdpcCode::UpdateLLR(int layer, float* a, float* b, float* c) {
    // vectorization
    for (auto block : PCM[layer]) {
        int idx = block.index;
        for (int j = 0; j < dec_reg_nums; j++, idx += 8) {
            _mm256_storeu_ps(a + idx, _mm256_add_ps(_mm256_loadu_ps(b + idx), _mm256_loadu_ps(c + idx)));
        }
    }
    // scalar
//	for (int i = 0; i < layer; i++) a[i] = b[i] + c[i];

    return;
}

/*
	   Normalized layered brief propagation decoder using min-sum algorithm
	*/
std::string LdpcCode::LdpcDecoder(std::string& msg_enc, int& iters) {
    int len = msg_enc.length(), iter, l, layer;
    int Blen = sizeof(float) * len;
    int i, j, t1, negcount;
    int len_infobits = k * zc;
    float* block_buf_f;
    uint32_t* index_buf;
    auto llr_type = llr[0];

    // normalize factor, range : (0,1]
    float a = 0.75;
    __m256 min1[dec_reg_nums];
    __m256 min2[dec_reg_nums];
    __m256 min1_idx[dec_reg_nums];
    __m256 sign[dec_reg_nums];

    // initialization
    memset(b, 0, Blen);
    for (i = 0; i < m; i++) {
        memset(r[i], 0, Blen);
    }
    block_buf_f = new float[zc];
    memset(block_buf_f, 0, zc * sizeof(float));
    index_buf = new uint32_t[zc];
    memset(index_buf, 0, zc * sizeof(uint32_t));

    // bpsk modulation, llr = sigma^(-2)*2*y
    auto p = &msg_enc[0];
    for (i = 0; i < len; i++)
        llr[i] = (p[i] & 1) ? -2.0 : 2.0;

    // r[i] record llrs for  layer i in last iteration
    for (iter = 0; iter < maxiter; iter++) {

        // layer process
        for (layer = 0; layer < m; layer++) {

            l = PCM[layer].size();
            //	vec_shift[0] = 0;
            //	for (i = 1; i < l; i++)
            //		vec_shift[i] = vec_shift[i - 1] + zc;

            // subtract this layer's llr in the last iteration
            SubtractLLR(layer, b, llr, r[layer]);

            // row process
/*			for (i = 0; i < zc; i++) {
				double min1 = 100000.0, min2 = 100000.0;
				double* p = &min1;
				negcount = 0;
				uint16_t min1_index = 1 << 10;
				for (j = 0; j < l; j++) {
					double t = b[Table[layer][vec_shift[j]]];
					if (t < 0) {
						negcount++;
						t = -t;
					}
					if (t < min1) {
						min2 = min1;
						min1 = t;
						min1_index = j;
					}
					else
						if (t < min2)
							min2 = t;
				}

				double signed_a = (negcount & 1) ? -a : a;
				min1 *= signed_a;
				for (j = 0; j < l; j++) {
					t1 = Table[layer][vec_shift[j]];
					double* pt = r[layer];
					if (j != min1_index) {
						pt[t1] = b[t1] > 0 ? min1 : -min1;
					}
					else {
						pt[t1] = b[t1] > 0 ? signed_a * min2 : -signed_a * min2;
					}
					llr[t1] = b[t1] + pt[t1];
					vec_shift[j]++;
				}
			}// end for row process
*/

            for (i = 0; i < dec_reg_nums; i++) {
                min1[i] = _mm256_set1_ps(1000.0);
                min2[i] = _mm256_set1_ps(1000.0);
                sign[i] = _mm256_setzero_ps();
            }
            auto ptr = PCM_layer_header[layer];
            // process every block in this layer
            for (i = 0; i < l; i++) {
                int idx = ptr[i].index;
                int shift_t = ptr[i].shift;

                memcpy(index_buf, index_nums + shift_t, (zc - shift_t) * sizeof(uint32_t));
                memcpy(index_buf + zc - shift_t, index_nums, shift_t * sizeof(uint32_t));
                for (j = 0; j < zc; j += 8) {
                    __m256i r = _mm256_loadu_si256((__m256i*)(index_buf + j));
                    _mm256_storeu_si256((__m256i*)(index_buf + j), _mm256_add_epi32(r, _mm256_set1_epi32(idx)));
                }

                // store block i's llr in buffer
                memcpy(block_buf_f, b + idx + shift_t, (zc - shift_t) * sizeof(decltype(llr_type)));
                memcpy(block_buf_f + zc - shift_t, b + idx, shift_t * sizeof(decltype(llr_type)));
                int reg_i = 0;
                for (j = 0; j < zc; j += 8, reg_i++) {
                    __m256 nums = _mm256_loadu_ps(block_buf_f + j), r, mask;
                    __m256i index_t = _mm256_loadu_si256((__m256i*)(index_buf + j));
                    sign[reg_i] = _mm256_xor_ps(nums, sign[reg_i]);
                    // vectorized abs(nums)
                    nums = _mm256_andnot_ps(_mm256_set1_ps(-0.), nums);
                    r = _mm256_min_ps(min1[reg_i], nums);
                    mask = _mm256_cmp_ps(r, min1[reg_i], _CMP_NEQ_OQ);
                    min1_idx[reg_i] = _mm256_or_ps(_mm256_andnot_ps(mask, min1_idx[reg_i]), _mm256_and_ps(mask, _mm256_castsi256_ps(index_t)));
                    min2[reg_i] = _mm256_or_ps(_mm256_min_ps(nums, _mm256_andnot_ps(mask, min2[reg_i])), _mm256_and_ps(mask, min1[reg_i]));
                    min1[reg_i] = r;
                }
            }
            int tttt = 0;
            // normalize min1 and min2
            for (i = 0; i < dec_reg_nums; i++) {
                sign[i] = _mm256_and_ps(sign[i], _mm256_set1_ps(-0.));
                min1[i] = _mm256_xor_ps(_mm256_mul_ps(_mm256_set1_ps(a), min1[i]), sign[i]);
                min2[i] = _mm256_xor_ps(_mm256_mul_ps(_mm256_set1_ps(a), min2[i]), sign[i]);
            }

            for (i = 0; i < l; i++) {
                int idx = ptr[i].index;
                int shift_t = ptr[i].shift;
                int reg_i = 0;
                __m256 mask = _mm256_set1_ps(-0.);
                memcpy(block_buf_f, b + idx + shift_t, (zc - shift_t) * sizeof(decltype(llr_type)));
                memcpy(block_buf_f + zc - shift_t, b + idx, shift_t * sizeof(decltype(llr_type)));
                for (j = 0; j < zc; j += 8, reg_i++) {
                    __m256 res = _mm256_xor_ps(_mm256_and_ps(_mm256_loadu_ps(block_buf_f + j), mask), min1[reg_i]);
                    _mm256_storeu_ps(block_buf_f + j, res);
                }
                auto pt = r[layer];
                memcpy(pt + idx + shift_t, block_buf_f, (zc - shift_t) * sizeof(decltype(llr_type)));
                memcpy(pt + idx, block_buf_f + zc - shift_t, shift_t * sizeof(decltype(llr_type)));
            }
            auto pt = r[layer];
            for (i = 0; i < dec_reg_nums; i++) {
                __m256i t11 = _mm256_castps_si256(min1_idx[i]);
                for (j = 0; j < 8; j+=2) {
                    int idx2 = (int)((t11[j>>1] >> 32) & 0xFFFFFFFF), idx1 = (int)(t11[j>>1] & 0xFFFFFFFF);
                    pt[idx1] = b[idx1] > 0 ? min2[i][j] : -min2[i][j];
                    pt[idx2] = b[idx2] > 0 ? min2[i][j+1] : -min2[i][j+1];
                }
            }
            /*	for (i = 0; i < l; i++) {
                    int idx = ptr[i].index;
                    int shift_t = ptr[i].shift;
                    int reg_i = 0, tt = shift_t;
                    for (j = 0; j < dec_reg_nums; j++) {
                        for (int kk = 0; kk < 4; kk++,tt++) {
                            int ttt = idx + tt % zc;
                            auto pt = r[layer];
                            if (b[ttt] > 0) pt[ttt] = min1[j].m256d_f64[kk];
                            else pt[ttt] = -min1[j].m256d_f64[kk];
                            if ((long long)ttt == _mm256_castpd_si256(min1_idx[j]).m256i_i64[kk]) {
                                if (b[ttt] > 0) pt[ttt] = min2[j].m256d_f64[kk];
                                else pt[ttt] = -min2[j].m256d_f64[kk];
                            }
                        }
                    }
                }*/

            UpdateLLR(layer, llr, b, r[layer]);

        }// end for layer process

        // hard decision
        /*	for (i = 0; i < len; i++)
                hard_decision[i] = llr[i] < 0;*/
        for (i = 0; i < len; i += 32) {
            __m256i r1 = _mm256_castps_si256(_mm256_cmp_ps(_mm256_loadu_ps(llr + i), _mm256_setzero_ps(), _CMP_LT_OQ));
            __m256i r2 = _mm256_castps_si256(_mm256_cmp_ps(_mm256_loadu_ps(llr + i + 8), _mm256_setzero_ps(), _CMP_LT_OQ));
            __m256i r3 = _mm256_castps_si256(_mm256_cmp_ps(_mm256_loadu_ps(llr + i + 16), _mm256_setzero_ps(), _CMP_LT_OQ));
            __m256i r4 = _mm256_castps_si256(_mm256_cmp_ps(_mm256_loadu_ps(llr + i + 24), _mm256_setzero_ps(), _CMP_LT_OQ));
            __m256i tmp1 = _mm256_packs_epi32(r1, r2);
            tmp1 = _mm256_permute4x64_epi64(tmp1, 0xD8);
            __m256i tmp2 = _mm256_packs_epi32(r3, r4);
            tmp2 = _mm256_permute4x64_epi64(tmp2, 0xD8);
            __m256i tmp = _mm256_packs_epi16(tmp1, tmp2);
            tmp = _mm256_permute4x64_epi64(tmp, 0xD8);
            tmp = _mm256_and_si256(tmp, _mm256_set1_epi8(1));
            _mm256_storeu_si256((__m256i*)(hard_decision + i), tmp);
            __m256i t = _mm256_setzero_si256();
        }

        	if (isvalid(hard_decision)) {
        //std::cout << "codeword valid, iter = " << iter << std::endl;
        		break;
        	}

    }// end for iter

    delete[] index_buf;
    delete[] block_buf_f;

   // itercounter += (iter == maxiter) ? iter : iter + 1;
    //	std::cout << "iter : " << iter << std::endl;
    iters = iter;

    return bin2str(hard_decision, len_infobits);
}
