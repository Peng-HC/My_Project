#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../include/externs.h"
#include "../include/cephes.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                          P O K E R       T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

double psiP(int m, int n);

void
Poker(int m, int n)
{
	double	p_value=0.0, psiv=0.0;

	psiv = psiP(m, n);

	p_value = cephes_igamc((pow(2, m)-1)/2, psiv/2.0);

	fprintf(stats[TEST_POKER], "\t\t\t       POKER TEST\n");
	fprintf(stats[TEST_POKER], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_POKER], "\t\t COMPUTATIONAL INFORMATION:		  \n");
	fprintf(stats[TEST_POKER], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_POKER], "\t\t(a) Block length    (m) = %d\n", m);
	fprintf(stats[TEST_POKER], "\t\t(b) Sequence length (n) = %d\n", n);
	fprintf(stats[TEST_POKER], "\t\t(c) Psi_v               = %f\n", psiv);

	fprintf(stats[TEST_POKER], "\t\t---------------------------------------------\n");

	fprintf(stats[TEST_POKER], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); fflush(stats[TEST_POKER]);
	fprintf(results[TEST_POKER], "%f\n", p_value); fflush(results[TEST_POKER]);
}

double
psiP(int m, int n)
{
	int				i, j, k, M;
	double			sum, numOfBlocks;

	if ( (m == 0) || (m == -1) )
		return 0.0;
	numOfBlocks = n/m;
	M = pow(2, m);
	int freq[M];

	for( i=0; i<M; i++){
        freq[i] = 0;
	}

	for ( i=0; i<numOfBlocks; i++) {		 /* COMPUTE FREQUENCY */

            k = 0;

		for ( j=0; j<m; j++ ) {
                k += pow(2, epsilon[i*m+j]*j);
                freq[k]++;
		}
	}
	sum = 0.0;
	for ( i=0; i<M; i++ )
		sum += pow(freq[i], 2);
	sum = (sum * M/(double)numOfBlocks) - (double)numOfBlocks;

	return sum;
}
