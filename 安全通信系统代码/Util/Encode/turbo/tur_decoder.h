#ifndef _TURDECODER_H_
#define _TURDECODER_H_

#include<iostream>
#include<iomanip>
#include<algorithm>
#include <utilities.h>
#include <vector.h>
#include <matrix.h>
#include <random.h>
#include <vectormath.h>

using namespace std;
using namespace splab;

void Trellis(Matrix<double>&next_out, Matrix<double>&next_state, Matrix<double>&last_out, Matrix<double>&last_state, Matrix<double>G);
Vector<double> logmap(Vector<double>rsc_out, Matrix<double>G, Vector<double>La, int index_dec);
Vector<double>sova(Vector<double>rsc_out, Matrix<double>G, Vector<double>L_a, int index_dec);
Vector<int>tur_decoder(Vector<int>bob_recieved, Vector<int>alpha);

#endif