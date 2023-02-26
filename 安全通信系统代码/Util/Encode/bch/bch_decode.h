#ifndef _H_BCH_DECODE_
#define _H_BCH_DECODE_

#include<iostream>
#include<iomanip>
#include <stdio.h> 
#include <stdlib.h> 
#include<vector.h>
#include<matrix.h>   

splab::Vector<int> bch_decoder(splab::Vector<int>y, int codelength);

#endif