#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../include/externs.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                   A U T O  C O R R E L A T I O N  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
AutoCorrelation(int d, int n)
{
	int		i;
	double	V, f, p_value, sum, sqrt2 = 1.41421356237309504880;

	sum = 0.0;
	for ( i=0; i<n-d; i++ )
		sum += (int)epsilon[i]^(int)epsilon[i+d];

	V = ( 2*sum - ( n - d ) )/sqrt( n - d );

	f = fabs(V)/sqrt2;

	p_value = erfc( f );

	fprintf(stats[TEST_AUTO_CORRELATION], "\t\t\t   AUTO CORRELATION TEST\n");
	fprintf(stats[TEST_AUTO_CORRELATION], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_AUTO_CORRELATION], "\t\tCOMPUTATIONAL INFORMATION:\n");
	fprintf(stats[TEST_AUTO_CORRELATION], "\t\t---------------------------------------------\n");
    fprintf(stats[TEST_AUTO_CORRELATION], "\t\t(a) The AutoCorrelation Move Length = %d\n", (int)d);
	fprintf(stats[TEST_AUTO_CORRELATION], "\t\t(a) The AutoCorrelation sum = %d\n", (int)sum);
	fprintf(stats[TEST_AUTO_CORRELATION], "\t\t---------------------------------------------\n");

	fprintf(stats[TEST_AUTO_CORRELATION], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); fflush(stats[TEST_FREQUENCY]);
	fprintf(results[TEST_AUTO_CORRELATION], "%f\n", p_value); fflush(results[TEST_FREQUENCY]);
}
