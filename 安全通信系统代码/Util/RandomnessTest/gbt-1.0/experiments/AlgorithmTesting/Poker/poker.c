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
	double	p_value, psiv;

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
	int				i, j, k, powLen;
	double			sum, numOfBlocks, N;
	unsigned int	*P;

	if ( (m == 0) || (m == -1) )
		return 0.0;
	numOfBlocks = n/m;
	N = numOfBlocks*m;
	powLen = (int)pow(2, m+1)-1;
	if ( (P = (unsigned int*)calloc(powLen,sizeof(unsigned int)))== NULL ) {
		fprintf(stats[TEST_SERIAL], "Poker Test:  Insufficient memory available.\n");
		fflush(stats[TEST_SERIAL]);
		return 0.0;
	}
	for ( i=1; i<powLen-1; i++ )
		P[i] = 0;	  /* INITIALIZE NODES */
	for ( i=0; i<N; ) {		 /* COMPUTE FREQUENCY */
		k = 1;
		for ( j=0; j<m; j++ ) {
			if ( epsilon[(i+j)%n] == 0 )
				k *= 2;
			else if ( epsilon[(i+j)%n] == 1 )
				k = 2*k+1;
		}
		P[k-1]++;
		i++;
		//i = i*m;
	}
	sum = 0.0;
	for ( i=(int)pow(2, m)-1; i<(int)pow(2, m+1)-1; i++ )
		sum += pow(P[i], 2);
	sum = (sum * pow(2, m)/(double)N) - (double)N;
	free(P);

	return sum;
}
