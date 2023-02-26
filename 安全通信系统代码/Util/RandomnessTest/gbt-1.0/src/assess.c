/* -----------------------------------------------------------------------------
   Title       :  The Randomness Statistical Test Suite based on GB/T 32915-2016

   Date        :  March 2020

   Programmer  :  Wang Xuyang

   Summary     :  For use in the evaluation of the randomness of bitstreams
                  produced by cryptographic random number generators.

   Package     :  Version 1.0

   Copyright   :  (c) 2020 by the Purple Mountain Laboratory

   History     :  Version 1.0 by Wang Xuyang, March 2020

   Keywords    :  Pseudorandom Number Generator (PRNG), Randomness, Statistical
                  Tests, Complementary Error functions, Incomplete Gamma
                  Function, Random Walks, Rank, Fast Fourier Transform,
                  Template, Cryptographically Secure PRNG (CSPRNG),
                  Approximate Entropy (ApEn), Secure Hash Algorithm (SHA-1),
                  Blum-Blum-Shub (BBS) CSPRNG, Micali-Schnorr (MS) CSPRNG,

   Warning     :  Portability Issues.

   Limitation  :  Amount of memory allocated for workspace.

   Restrictions:  Permission to use, copy, and modify this software without
                  fee is hereby granted, provided that this entire notice is
                  included in all copies of any software which is or includes
                  a copy or modification of this software and in all copies
                  of the supporting documentation for such software.

   Note        :  Part of the test algorithms comes from the following NIST Test Suite:
   -----------------------------------------------------------------------------
   Title       :  The NIST Statistical Test Suite

   Date        :  December 1999

   Programmer  :  Juan Soto

   Summary     :  For use in the evaluation of the randomness of bitstreams
                  produced by cryptographic random number generators.

   Package     :  Version 1.0

   Copyright   :  (c) 1999 by the National Institute Of Standards & Technology

   History     :  Version 1.0 by J. Soto, October 1999
                  Revised by J. Soto, November 1999
                  Revised by Larry Bassham, March 2008

   Keywords    :  Pseudorandom Number Generator (PRNG), Randomness, Statistical
                  Tests, Complementary Error functions, Incomplete Gamma
                  Function, Random Walks, Rank, Fast Fourier Transform,
                  Template, Cryptographically Secure PRNG (CSPRNG),
                  Approximate Entropy (ApEn), Secure Hash Algorithm (SHA-1),
                  Blum-Blum-Shub (BBS) CSPRNG, Micali-Schnorr (MS) CSPRNG,

   Source      :  David Banks, Elaine Barker, James Dray, Allen Heckert,
                  Stefan Leigh, Mark Levenson, James Nechvatal, Andrew Rukhin,
                  Miles Smid, Juan Soto, Mark Vangel, and San Vo.

   Technical
   Assistance  :  Larry Bassham, Ron Boisvert, James Filliben, Daniel Lozier,
                  and Bert Rust.

   Warning     :  Portability Issues.

   Limitation  :  Amount of memory allocated for workspace.

   Restrictions:  Permission to use, copy, and modify this software without
                  fee is hereby granted, provided that this entire notice is
                  included in all copies of any software which is or includes
                  a copy or modification of this software and in all copies
                  of the supporting documentation for such software.
   -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/decls.h"
#include "../include/cephes.h"
#include "../include/utilities.h"

#define MAX_PATH 1000
//const char* ROOT_PATH;

void	partitionResultFile(int numOfFiles, int numOfSequences, int option, int testNameID);
void	postProcessResults(int option);
int		cmp(const double *a, const double *b);
int		computeMetrics(char *s, int test);

//通过的随机性检测算法的总数量(<15)
int passedRandomnessTestCount = 0;

int
main(int argc, char *argv[])
{
	int		i;
	int		option;			/* TEMPLATE LENGTH/STREAM LENGTH/GENERATOR*/
	char	*streamFile;	/* STREAM FILENAME     */

//    ROOT_PATH = getPath();

	if ( argc < 2 ) {
        printf("至少传入两个参数!\n");
		printf("Usage: %s <stream length>\n", argv[0]);
		printf("   <stream length> is the length of the individual bit stream(s) to be processed\n");
		return 0;
	}
//    passedRandomnessTestCount = 0;

    // 取出参数
    int generator_selection_option=atoi(argv[2]);   //随机数来源,默认为0,来自用户自己生成的随机数文件
    char *user_prescribed_input_file=argv[3];   //需要进行随机性检测的文件地址
    streamFile = user_prescribed_input_file;
    int randomness_statistical_tests_option=atoi(argv[4]); //选择15种随机性检测方法,默认为1
    char *choose_randomness_tests=argv[5];  //"" or "111111111111101"
    printf("\nchoose_randomness_tests:%s\n",choose_randomness_tests);
    int parameter_adjustments_option=atoi(argv[6]); //随机性检测方法的参数设定,默认为0
    int bitstreams=atoi(argv[7]); //样本数
    int select_input_mode_option=atoi(argv[8]); //输入模式:0为ascill，


	tp.n = atoi(argv[1]);
	tp.blockFrequencyBlockLength = 100;
	tp.pokerBlockLength = 8;                       /*Ĭ�Ϸ��ص��鳤Ϊ8����*/
	tp.serialBlockLength = 5;
	tp.binaryXORTimes = 3;                         /*Ĭ��ִ��3��������*/
	tp.autoMoveLength = 8;                         /*Ĭ������8����*/
	tp.linearComplexitySequenceLength = 500;
	tp.approximateEntropyBlockLength = 5;

	tp.numOfBitStreams = 1;


//	option = generatorOptions(int generator_selection_option,&streamFile);
	option = generatorOptions(generator_selection_option,&user_prescribed_input_file);
	chooseTests(randomness_statistical_tests_option,choose_randomness_tests);
	fixParameters(parameter_adjustments_option);
//    printf("     即将执行openOutputStreams方法.........\n\n");
	openOutputStreams(option,bitstreams);
//    printf("     执行openOutputStreams方法完毕.........\n\n");
//    printf("     即将执行invokeTestSuite方法.........\n\n");
	invokeTestSuite(option, streamFile,select_input_mode_option);
//    printf("     执行invokeTestSuite方法完毕.........\n\n");
	fclose(freqfp);
	for( i=1; i<=NUMOFTESTS; i++ ) {
		if ( stats[i] != NULL )
			fclose(stats[i]);
		if ( results[i] != NULL )
			fclose(results[i]);
	}
	if ( (testVector[0] == 1) || (testVector[TEST_SERIAL] == 1) )
		partitionResultFile(2, tp.numOfBitStreams, option, TEST_SERIAL);

	fprintf(summary, "------------------------------------------------------------------------------\n");
	fprintf(summary, "RESULTS FOR THE UNIFORMITY OF P-VALUES AND THE PROPORTION OF PASSING SEQUENCES\n");
	fprintf(summary, "------------------------------------------------------------------------------\n");
	fprintf(summary, "   generator is <%s>\n", streamFile);
	fprintf(summary, "------------------------------------------------------------------------------\n");
	fprintf(summary, " C1  C2  C3  C4  C5  C6  C7  C8  C9 C10  P-VALUE  PROPORTION  STATISTICAL TEST\n");
	fprintf(summary, "------------------------------------------------------------------------------\n");
	postProcessResults(option);
	fclose(summary);

//    sprintf(argv[9], "%d", passedRandomnessTestCount);
    FILE* fpWrite = NULL;
    fpWrite = fopen(argv[9], "w");
    if (  fpWrite == NULL ) {
        printf("文件%s不存在\n", argv[9]);
        exit(-1);
    }
    fprintf(fpWrite,"%d",passedRandomnessTestCount);
    fclose(fpWrite);

//    argv[9] = _itoa(passedRandomnessTestCount);

//    printf("argv[1]=%s\n", argv[1]);
//    printf("样本数:%s\n", argv[7]);
//    printf("argv[9]=%s\n", argv[9]);
//    printf("Config::init_zucKey_seq=%d\n", Config::init_zucKey_seq);
//    printf("passedRandomnessTestCount_main=%d\n", passedRandomnessTestCount);

	return 1;
}

void
partitionResultFile(int numOfFiles, int numOfSequences, int option, int testNameID)
{
//    printf("\n开始执行partitionResultFile方法------\n");
	int		i, k, m, j, start, end, num, numread;
	float	c;
	FILE	**fp = (FILE **)calloc(numOfFiles+1, sizeof(FILE *));
	int		*results = (int *)calloc(numOfFiles, sizeof(int *));
	char	*s[MAXFILESPERMITTEDFORPARTITION];
	char	resultsDir[200];

	for ( i=0; i<MAXFILESPERMITTEDFORPARTITION; i++ )
		s[i] = (char*)calloc(200, sizeof(char));

	sprintf(resultsDir, strcat(getPath(),"experiments/%s/%s/results.txt"), generatorDir[option], testNames[testNameID]);
//    printf("resultsDir=%s\n",resultsDir);
//	sprintf(resultsDir, "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/%s/results.txt", generatorDir[option], testNames[testNameID]);

	if ( (fp[numOfFiles] = fopen(resultsDir, "r")) == NULL ) {
		printf("%s", resultsDir);
		printf(" -- file not found. Exiting program.\n");
		exit(-1);
	}


	for ( i=0; i<numOfFiles; i++ ) {
		if ( i < 10 )
			sprintf(s[i], strcat(getPath(),"experiments/%s/%s/data%1d.txt"), generatorDir[option], testNames[testNameID], i+1);
//			sprintf(s[i], "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/%s/data%1d.txt", generatorDir[option], testNames[testNameID], i+1);
		else if (i < 100)
			sprintf(s[i], strcat(getPath(),"experiments/%s/%s/data%2d.txt"), generatorDir[option], testNames[testNameID], i+1);
//			sprintf(s[i], "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/%s/data%2d.txt", generatorDir[option], testNames[testNameID], i+1);
		else
			sprintf(s[i], strcat(getPath(),"experiments/%s/%s/data%3d.txt"), generatorDir[option], testNames[testNameID], i+1);
//			sprintf(s[i], "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/%s/data%3d.txt", generatorDir[option], testNames[testNameID], i+1);
	}
	numread = 0;
	m = numOfFiles/20;
	if ( (numOfFiles%20) != 0 )
		m++;
	for ( i=0; i<numOfFiles; i++ ) {
		if ( (fp[i] = fopen(s[i], "w")) == NULL ) {
			printf("%s", s[i]);
			printf(" -- file not found. Exiting program.\n");
			exit(-1);
		}
		fclose(fp[i]);
	}
	for ( num=0; num<numOfSequences; num++ ) {
		for ( k=0; k<m; k++ ) { 			/* FOR EACH BATCH */

			start = k*20;		/* BOUNDARY SEGMENTS */
			end   = k*20+19;
			if ( k == (m-1) )
				end = numOfFiles-1;

			for ( i=start; i<=end; i++ ) {		/* OPEN FILE */
				if ( (fp[i] = fopen(s[i], "a")) == NULL ) {
					printf("%s", s[i]);
					printf(" -- file not found. Exiting program.\n");
					exit(-1);
				}
			}

			for ( j=start; j<=end; j++ ) {		/* POPULATE FILE */
				fscanf(fp[numOfFiles], "%f", &c);
				fprintf(fp[j], "%f\n", c);
				numread++;
			}

			for ( i=start; i<=end; i++ )		/* CLOSE FILE */
				fclose(fp[i]);
		}
	}
	fclose(fp[numOfFiles]);
	for ( i=0; i<MAXFILESPERMITTEDFORPARTITION; i++ )
		free(s[i]);

//    printf("\n执行完毕partitionResultFile方法------\n");
	return;
}

int
cmp(const double *a, const double *b)
{
	if ( *a < *b )
		return -1;
	if ( *a == *b )
		return 0;
	return 1;
}

void
postProcessResults(int option)
{
//    printf("\n开始执行postProcessResults方法------\n");
	int		i, k, generalSampleSize;
	int		passRate, case1;
	double	p_hat;
	char	s[200];
	for ( i=1; i<=NUMOFTESTS; i++ ) {		// FOR EACH TEST
		if ( testVector[i] ) {
			// SPECIAL CASES -- HANDLING MULTIPLE FILES FOR A SINGLE TEST
			if ( ((i == TEST_SERIAL) && testVector[TEST_SERIAL]) ) {

				for ( k=0; k<2; k++ ) {

                    sprintf(s, strcat(getPath(),"experiments/%s/%s/data%1d.txt"), generatorDir[option], testNames[i], k+1);
//                    sprintf(s, "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/%s/data%1d.txt", generatorDir[option], testNames[i], k+1);

					generalSampleSize = computeMetrics(s,i);
				}
			}
			else {
				sprintf(s, strcat(getPath(),"experiments/%s/%s/results.txt"), generatorDir[option], testNames[i]);
//                printf("results.txt=%s\n",s);
//				sprintf(s, "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/%s/results.txt", generatorDir[option], testNames[i]);
				generalSampleSize = computeMetrics(s,i);
			}
		}
	}

	fprintf(summary, "\n\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	case1 = 0;

	for ( i=1; i<=NUMOFTESTS; i++ ) {
		if ( testVector[i] ) {
			case1 = 1;
			break;
		}
	}
	p_hat = 1.0 - ALPHA;
	if ( case1 ) {
		if ( generalSampleSize == 0 ) {
			fprintf(summary, "The minimum pass rate for each statistical test with the exception of the\n");
			fprintf(summary, "random excursion (variant) test is undefined.\n\n");
		}
		else {
			passRate = (p_hat - 3.0 * sqrt((p_hat*ALPHA)/generalSampleSize)) * generalSampleSize;
			fprintf(summary, "The minimum pass rate for each statistical test with the exception of the\n");
			fprintf(summary, "random excursion (variant) test is approximately = %d for a\n", generalSampleSize ? passRate : 0);
			fprintf(summary, "sample size = %d binary sequences.\n\n", generalSampleSize);
		}
	}
	fprintf(summary, "For further guidelines construct a probability table using the MAPLE program\n");
	fprintf(summary, "provided in the addendum section of the documentation.\n");
	fprintf(summary, "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
//    printf("\n执行完毕postProcessResults方法------\n");
}

int
computeMetrics(char *s, int test)
{
//    printf("\n开始执行computeMetrics方法------\n");
	int		j, pos, count, passCount, sampleSize, expCount, proportion_threshold_min, proportion_threshold_max;
	int		freqPerBin[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	double	*A, chi2, uniformity, p_hat;
	float	c;
	FILE	*fp;

	if ( (fp = fopen(s, "r")) == NULL ) {
		printf("%s",s);
		printf(" -- file not found. Exiting program.\n");
		exit(-1);
	}

	if ( (A = (double *)calloc(tp.numOfBitStreams, sizeof(double))) == NULL ) {
		printf("Final Analysis Report aborted due to insufficient workspace\n");
		return 0;
	}

	/* Compute Metric 1: Proportion of Passing Sequences */

	count = 0;
	sampleSize = tp.numOfBitStreams;


		if ( (A = (double *)calloc(sampleSize, sizeof(double))) == NULL ) {
			printf("Final Analysis Report aborted due to insufficient workspace\n");
			return 0;
		}
		for ( j=0; j<sampleSize; j++ ) {
			fscanf(fp, "%f", &c);
			if ( c < ALPHA )
				count++;
			A[j] = c;
		}

	if ( sampleSize == 0 )
		passCount = 0;
	else
		passCount = sampleSize - count;

	p_hat = 1.0 - ALPHA;
	proportion_threshold_max = (p_hat + 3.0 * sqrt((p_hat*ALPHA)/sampleSize)) * sampleSize;
	proportion_threshold_min = (p_hat - 3.0 * sqrt((p_hat*ALPHA)/sampleSize)) * sampleSize;

	/* Compute Metric 2: Histogram */

	qsort((void *)A, sampleSize, sizeof(double), (void *)cmp);
	for ( j=0; j<sampleSize; j++ ) {
		pos = (int)floor(A[j]*10);
		if ( pos == 10 )
			pos--;
		freqPerBin[pos]++;
	}
	chi2 = 0.0;
	expCount = sampleSize/10;
	if ( expCount == 0 )
		uniformity = 0.0;
	else {
		for ( j=0; j<10; j++ )
			chi2 += pow(freqPerBin[j]-expCount, 2)/expCount;
		uniformity = cephes_igamc(9.0/2.0, chi2/2.0);
	}

	for ( j=0; j<10; j++ )			/* DISPLAY RESULTS */
		fprintf(summary, "%3d ", freqPerBin[j]);

	if ( expCount == 0 )
		fprintf(summary, "    ----    ");
	else if ( uniformity < 0.0001 )
		fprintf(summary, " %8.6f * ", uniformity);
	else
		fprintf(summary, " %8.6f   ", uniformity);

	if ( sampleSize == 0 )
		fprintf(summary, " ------     %s\n", testNames[test]);
	//	else if ( proportion < 0.96 )
	else if ( (passCount < proportion_threshold_min) || (passCount > proportion_threshold_max))
		fprintf(summary, "%4d/%-4d *  %s\n", passCount, sampleSize, testNames[test]);
	else
		fprintf(summary, "%4d/%-4d    %s\n", passCount, sampleSize, testNames[test]);

    if(passCount>0) {
        (passedRandomnessTestCount)++;
    }

//    printf("passedRandomnessTestCount_computeMetrics=%d\n", passedRandomnessTestCount);

	fclose(fp);
	free(A);
//    printf("\n执行完毕computeMetrics方法------\n");
	return sampleSize;
}
