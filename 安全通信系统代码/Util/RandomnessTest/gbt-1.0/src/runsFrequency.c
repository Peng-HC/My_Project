/* got rid of unused 'k' */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../include/externs.h"
#include "../include/cephes.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                      R U N S  F R E Q U E N C Y  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

double psiR(int k, int n);

void
RunsFrequency(int n)
{
	double			pval, V;
    int				i, k = 0;

	for ( i=1; i<=n; i++ ) {
        if( (n-i+3) >= (5*(int)pow(2, i + 2))) 
            k++;
        else break;
	}

    V = psiR(k, n);

	pval = cephes_igamc((double)(k-1), V / 2.0);



	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t\t  RUNS FREQUENCY TEST\n");
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\tCOMPUTATIONAL INFORMATION:\n");
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t(a) V (Runs Frequency) = %f\n", V);
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t(b) pval (P_value) = %f\n", pval);
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t(c) k (max) = %d\n", k);
	fprintf(stats[TEST_RUNS_FREQUENCY], "\t\t---------------------------------------------\n");

	fprintf(stats[TEST_RUNS_FREQUENCY], "%s\t\tp_value = %f\n\n", pval < ALPHA ? "FAILURE" : "SUCCESS", pval); fflush(stats[TEST_RUNS_FREQUENCY]);
	fprintf(results[TEST_RUNS_FREQUENCY], "%f\n", pval); fflush(results[TEST_RUNS_FREQUENCY]);
}

double psiR(int k, int n) {

    int i, run0 = 0, run1 = 0;
    double sum;
    double *e, * b, * g;

    if ((k == 0) || (k == -1))
        return 0.0;

    if ((e = (double*)calloc(k, sizeof(double))) == NULL) {
        fprintf(stats[TEST_SERIAL], "RunsFrequency Test:  Insufficient memory available.\n");
        fflush(stats[TEST_SERIAL]);
        return 0.0;
    }

    if ((b = (double*)calloc(k, sizeof(double))) == NULL){
        fprintf(stats[TEST_SERIAL], "RunsFrequency Test:  Insufficient memory available.\n");
        fflush(stats[TEST_SERIAL]);
        return 0.0;
    }

    if ((g = (double*)calloc(k, sizeof(double))) == NULL){
        fprintf(stats[TEST_SERIAL], "RunsFrequency Test:  Insufficient memory available.\n");
        fflush(stats[TEST_SERIAL]);
        return 0.0;
    }

    for (i = 0; i < k; i++) {
        e[i] = (double)((n - i + 2) / ((int)pow(2, i + 3)));
        b[i] = 0.0;
        g[i] = 0.0;
    }

    for (i = 0; i < n; i++) {
        if (epsilon[i] == 0) {
            run0++;
        }
        else if (run0 > 0 && run0 <= k) {
            g[run0 - 1]++;
            run0 = 0;
        }
        else run0 = 0;
    }

    for (i = 0; i < n; i++) {
        if (epsilon[i] == 1) {
            run1++;
        }
        else if (run1 > 0 && run1 <= k) {
            b[run1 - 1]++;
            run1 = 0;
        }
        else run1 = 0;
    }

    sum = 0.0;

    for (i = 0; i < k; i++) {
        sum += (pow((b[i] - e[i]), 2) + pow((g[i] - e[i]), 2)) / e[i];
    }

    free(e);
    free(b);
    free(g);
    return(sum);
}