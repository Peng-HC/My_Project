#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "../include/externs.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                    B I N A R Y  D E R I V A T I O N  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
BinaryDerivation(int k, int n)
{
    int		i, j;
	double	sum, f, s_obs, p_value, sqrt2 = 1.41421356237309504880;

	for ( i=0; i<k; i++ ){
        for ( j=0; j<n-i-1; j++){
            epsilon[j] = epsilon[j]^epsilon[j+1];
        }
	}

	sum = 0.0;

	for ( i=0; i<n-k; i++)
		sum += 2*epsilon[i]-1;

	s_obs = fabs(sum)/sqrt(n-k);
	f = s_obs/sqrt2;
	p_value = erfc(f);

	fprintf(stats[TEST_BINARY_DERIVATION], "\t\t\t      BINARY DERIVATION TEST\n");
	fprintf(stats[TEST_BINARY_DERIVATION], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_BINARY_DERIVATION], "\t\tCOMPUTATIONAL INFORMATION:\n");
	fprintf(stats[TEST_BINARY_DERIVATION], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_BINARY_DERIVATION], "\t\t(a) The (n-k)th partial sum = %f\n", (double)sum);
	//fprintf(stats[TEST_BINARY_DERIVATION], "\t\t(b) S_n/n               = %f\n", sum/(n-k);
	fprintf(stats[TEST_BINARY_DERIVATION], "\t\t---------------------------------------------\n");

	fprintf(stats[TEST_BINARY_DERIVATION], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); fflush(stats[TEST_BINARY_DERIVATION]);
	fprintf(results[TEST_BINARY_DERIVATION], "%f\n", p_value); fflush(results[TEST_BINARY_DERIVATION]);
}
