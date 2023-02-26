#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../include/externs.h"
#include "../include/cephes.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                          P O K E R       T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

double psiP(int a, int b, int c);

void
Poker(int m, int n)
{
	int             M, N;
	double			S, psi_v, p_value;

	N = n/m;
	M = (int)pow(2, m);
	
	S = psiP(M, N, m);

	psi_v = (S * M/(double)N) - (double)N;

	p_value = cephes_igamc((double)(M-1.0)/2.0, psi_v/2.0);

	fprintf(stats[TEST_POKER], "\t\t\t       POKER TEST\n");
	fprintf(stats[TEST_POKER], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_POKER], "\t\t COMPUTATIONAL INFORMATION:		  \n");
	fprintf(stats[TEST_POKER], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_POKER], "\t\t(a) Block length    (m) = %d\n", m);
	fprintf(stats[TEST_POKER], "\t\t(b) Sequence length (n) = %d\n", n);
	fprintf(stats[TEST_POKER], "\t\t(c) Frequency           = %f\n", S);
	fprintf(stats[TEST_POKER], "\t\t(c) Psi_v               = %f\n", psi_v);

	fprintf(stats[TEST_POKER], "\t\t---------------------------------------------\n");

	fprintf(stats[TEST_POKER], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); fflush(stats[TEST_POKER]);
	fprintf(results[TEST_POKER], "%f\n", p_value); fflush(results[TEST_POKER]);
}

double psiP(int M, int N, int m) {
	int			  i, j, k;
	double		  sum;
	unsigned int* P;

	if ((m == 0) || (m == -1))
		return 0.0;

	if ((P = (unsigned int*)calloc(M, sizeof(unsigned int))) == NULL) {
		fprintf(stats[TEST_SERIAL], "Poker Test:  Insufficient memory available.\n");
		fflush(stats[TEST_SERIAL]);
		return 0.0;
	}

	for (i = 0; i < M; i++) {
		P[i] = 0;
	}



	for (i = 0; i < N; i++) {		 /* COMPUTE FREQUENCY */
		k = 0;
		for (j = 0; j < m; j++) {
			k += epsilon[i * m + j] * (int)pow(2, j);
		}
		P[k]++;
	}
	sum = 0.0;

	for (i = 0; i < M; i++) {
		sum += pow(P[i], 2);
	}
	free(P);
	return sum;
}