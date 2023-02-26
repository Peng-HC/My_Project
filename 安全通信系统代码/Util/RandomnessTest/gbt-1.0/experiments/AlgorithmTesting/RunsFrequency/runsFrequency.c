/* got rid of unused 'k' */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../include/externs.h"
#include "../include/cephes.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                      R U N S  F R E Q U E N C Y  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
RunsFrequency(int n)
{
	double			pval, V;
	int				run0, run1, i, k=0;
	/*int             k1=0, k2=0;*/
	for ( i=1; i<=n; i++ ) {
        /*k1=n-i+3;
        k2=5*(int)pow(2,i+2);
        if(k1>=k2) k++;*/
        if( n-i+3>=5*(int)pow(2,i+2) ) {
            k++;
        }
	}

	int e[k], b[k], g[k];

	for ( i=0; i<k; i++ ) {
        e[i] = (n-i+2)/((int)pow(2,i+3));
        b[i] = 0;
        g[i] = 0;
	}

    run0 = 0;
    run1 = 0;

	for ( i=0; i<n; i++) {
        if ( epsilon[i] == 0 ) {
            run0++;
        }
        else if ( run0>0 && run0<k ) {
            g[run0-1]++;
            run0 = 0;
        }
	}

	for ( i=0; i<n; i++) {
        if ( epsilon[i] == 1 ) {
            run1++;
        }
        else if ( run1>0 && run1<k ) {
            b[run1-1]++;
            run1 = 0;
        }
	}

	V = 0.0;
	for ( i=0; i<k; i++ ) {
		V += ((int)pow( (b[i] - e[i]), 2) + (int)pow ( (g[i] - e[i]), 2)) / e[i];
	}

	pval = cephes_igamc((double)(k-1), V / 2.0);

	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t\t  RUNS FREQUENCY TEST\n");
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\tCOMPUTATIONAL INFORMATION:\n");
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t(a) V (Runs Frequency) = %f\n", V);
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t(b) pval (P_value) = %f\n", pval);
	/*fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t(c) pval (value) = %f\n", k);*/
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t---------------------------------------------\n");

	fprintf(stats[TEST_RUNS_FREQUENCY], "%s\t\tp_value = %f\n\n", pval < ALPHA ? "FAILURE" : "SUCCESS", pval); fflush(stats[TEST_RUNS_FREQUENCY]);
	fprintf(results[TEST_RUNS_FREQUENCY], "%f\n", pval); fflush(results[TEST_RUNS_FREQUENCY]);
}
