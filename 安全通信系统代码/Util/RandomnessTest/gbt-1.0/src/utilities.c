/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
U T I L I T I E S
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "../include/externs.h"
#include "../include/utilities.h"
#include "../include/generators.h"
#include "../include/stat_fncs.h"

#define MAX_PATH 1000
//const char* ROOT_PATH;

char *getPath() {
    char s_path[MAX_PATH];
    char *source_path=getcwd(s_path, MAX_PATH); //获得当前项目的绝对地址 bin/
//    char additional_path[MAX_PATH];
    strcat(source_path,"/../Util/RandomnessTest/gbt-1.0/");
    return source_path;
}

int
displayGeneratorOptions(int generator_selection_option)
{
//	int		option = 0;
	int		option = generator_selection_option;

	printf("           G E N E R A T O R    S E L E C T I O N \n");
	printf("           ______________________________________\n\n");
	printf("    [0] Input File                 [1] Linear Congruential\n");
	printf("    [2] Quadratic Congruential I   [3] Quadratic Congruential II\n");
	printf("    [4] Cubic Congruential         [5] XOR\n");
	printf("    [6] Modular Exponentiation     [7] Blum-Blum-Shub\n");
	printf("    [8] Micali-Schnorr             [9] G Using SHA-1\n\n");
	printf("   Enter Choice: %d",option);

//	scanf("%d", &option);
	printf("\n\n");

	return option;
}


int
generatorOptions(int generator_selection_option,char** streamFile)
{
	char	file[200];
	int		option = NUMOFGENERATORS+1;
	FILE	*fp;

	while ( (option < 0) || (option > NUMOFGENERATORS) ) {
		option = displayGeneratorOptions(generator_selection_option);
		switch( option ) {
			case 0:
//				scanf("%s", file);
//				*streamFile = (char*)calloc(200, sizeof(char));
//				sprintf(*streamFile, "%s", file);
                printf("\t\tUser Prescribed Input File: %s",*streamFile);
				printf("\n");
                printf("streamFile:%s\n",*streamFile);
				if ( (fp = fopen(*streamFile, "r")) == NULL ) {
					printf("File Error:  file %s could not be opened.\n",  *streamFile);
					exit(-1);
				}
				else
					fclose(fp);
				break;
			case 1:
				*streamFile = "Linear-Congruential";
				break;
			case 2:
				*streamFile = "Quadratic-Congruential-1";
				break;
			case 3:
				*streamFile = "Quadratic-Congruential-2";
				break;
			case 4:
				*streamFile = "Cubic-Congruential";
				break;
			case 5:
				*streamFile = "XOR";
				break;
			case 6:
				*streamFile = "Modular-Exponentiation";
				break;
			case 7:
				*streamFile = "Blum-Blum-Shub";
				break;
			case 8:
				*streamFile = "Micali-Schnorr";
				break;
			case 9:
				*streamFile = "G using SHA-1";
				break;

			/* INTRODUCE NEW PRNG NAMES HERE */
			/*
			case 10:  *streamFile = "myNewPRNG";
				break;
			*/
			default:
				printf("Error:  Out of range - Try again!\n");
				break;
		}
	}
	return option;
}


void
chooseTests(int randomness_statistical_tests_option,char *choose_randomness_tests)
{
	int		i;

	printf("        R A N D O M N E S S   S T A T I S T I C A L   T E S T S\n");
	printf("                _________________________________\n\n");
	printf("    [01] Frequency                       [02] Block Frequency\n");
	printf("    [03] Poker                           [04] Serial\n");
	printf("    [05] Runs                            [06] Runs Frequency\n");
	printf("    [07] Longest Run of Ones             [08] Binary Derivation\n");
	printf("    [09] AutoCorrelation                 [10] Rank\n");
	printf("    [11] Cumulative Sums                 [12] Approximate Entropy\n");
	printf("    [13] Linear Complexity               [14] Universal Statistical\n");
	printf("    [15] Discrete Fourier Transform\n\n");
	printf("         INSTRUCTIONS\n");
	printf("            Enter 0 if you DO NOT want to apply all of the\n");
	printf("            statistical tests to each sequence and 1 if you DO.\n\n");
    testVector[0] = randomness_statistical_tests_option;
    printf("   Enter Choice: %d",testVector[0]);
//	scanf("%d", &testVector[0]);
	printf("\n");
	if ( testVector[0] == 1 )
		for( i=1; i<=NUMOFTESTS; i++ )
			testVector[i] = 1;
	else {
		printf("         INSTRUCTIONS\n");
		printf("            Enter a 0 or 1 to indicate whether or not the numbered statistical\n");
		printf("            test should be applied to each sequence.\n\n");
		printf("      123456789111111\n");
		printf("               012345\n");
		printf("      ");
		for ( i=1; i<=NUMOFTESTS; i++ ) {
			char *temp = malloc(sizeof(char));
			*temp = choose_randomness_tests[i];
			testVector[i]=atoi(temp);
//			printf("\n**********************************************\n");
//			printf(atoi(&choose_randomness_tests[i])+"");
//			printf(testVector[i]+"");
//			printf("\n**********************************************\n");
		}
		printf(choose_randomness_tests);
		printf("\n\n");
	}

}


void
fixParameters(int parameter_adjustments_option)
{
	int		counter, testid;

	//  Check to see if any parameterized tests are selected
	if ( (testVector[TEST_BLOCK_FREQUENCY] != 1) && (testVector[TEST_APEN] != 1) &&
         (testVector[TEST_SERIAL] != 1) && (testVector[TEST_LINEARCOMPLEXITY] != 1) &&
         (testVector[TEST_POKER] != 1) && (testVector[TEST_BINARY_DERIVATION] != 1) &&
         (testVector[TEST_AUTO_CORRELATION] != 1))
			return;

	do {
		counter = 1;
		printf("        P a r a m e t e r   A d j u s t m e n t s\n");
		printf("        -----------------------------------------\n");
		if ( testVector[TEST_BLOCK_FREQUENCY] == 1 )
			printf("    [%d] Block Frequency Test - block length(M):         %d\n", counter++, tp.blockFrequencyBlockLength);
		if ( testVector[TEST_POKER] == 1 )
			printf("    [%d] Poker Test - block length(m):                   %d\n", counter++, tp.pokerBlockLength);
        if ( testVector[TEST_SERIAL] == 1 )
			printf("    [%d] Serial Test - block length(m):                  %d\n", counter++, tp.serialBlockLength);
        if ( testVector[TEST_BINARY_DERIVATION] == 1 )
			printf("    [%d] Binary Derivation Test - times(k):              %d\n", counter++, tp.binaryXORTimes);
        if ( testVector[TEST_AUTO_CORRELATION] == 1 )
			printf("    [%d] Auto Correlation Test - move length(d):         %d\n", counter++, tp.autoMoveLength);
		if ( testVector[TEST_LINEARCOMPLEXITY] == 1 )
			printf("    [%d] Linear Complexity Test - block length(M):       %d\n", counter++, tp.linearComplexitySequenceLength);
        if ( testVector[TEST_APEN] == 1 )
			printf("    [%d] Approximate Entropy Test - block length(m):     %d\n", counter++, tp.approximateEntropyBlockLength);

		printf("\n");
        testid = parameter_adjustments_option;
		printf("   Select Test (0 to continue): %1d",testid);
//		scanf("%1d", &testid);
		printf("\n");
        printf("\n");

//        printf("testVector[TEST_BLOCK_FREQUENCY]=%1d\n",testVector[TEST_BLOCK_FREQUENCY]);
//        printf("testVector[TEST_POKER]=%1d\n",testVector[TEST_POKER]);
//        printf("testVector[TEST_SERIAL]=%1d\n",testVector[TEST_SERIAL]);
//        printf("testVector[TEST_BINARY_DERIVATION]=%1d\n",testVector[TEST_BINARY_DERIVATION]);
//        printf("testVector[TEST_AUTO_CORRELATION]=%1d\n",testVector[TEST_AUTO_CORRELATION]);
//        printf("testVector[TEST_LINEARCOMPLEXITY]=%1d\n",testVector[TEST_LINEARCOMPLEXITY]);
//        printf("testVector[TEST_APEN] =%1d\n",testVector[TEST_APEN]);

		counter = 0;
		if ( testVector[TEST_BLOCK_FREQUENCY] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Block Frequency Test block length: ");
				scanf("%d", &tp.blockFrequencyBlockLength);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_POKER] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Poker Test block Length: ");
				scanf("%d", &tp.pokerBlockLength);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_SERIAL] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Serial Test block Length: ");
				scanf("%d", &tp.serialBlockLength);
				printf("\n");
				continue;
			}
		}

		if ( testVector[TEST_BINARY_DERIVATION] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Binary Derivation Test XOR Times: ");
				scanf("%d", &tp.binaryXORTimes);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_AUTO_CORRELATION] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Auto correlation Test Move Length: ");
				scanf("%d", &tp.autoMoveLength);
				printf("\n");
				continue;
			}
		}
		if ( testVector[TEST_LINEARCOMPLEXITY] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Linear Complexity Test block Length: ");
				scanf("%d", &tp.linearComplexitySequenceLength);
				printf("\n");
				continue;
			}
		}

		if ( testVector[TEST_APEN] == 1 ) {
			counter++;
			if ( counter == testid ) {
				printf("   Enter Approximate Entropy Test block Length: ");
				scanf("%d", &tp.approximateEntropyBlockLength);
				printf("\n");
				continue;
			}
		}
	} while ( testid != 0 );
}


void
fileBasedBitStreams(char *streamFile,int select_input_mode_option)
{
	FILE	*fp;
	int		mode;

	printf("   Input File Format:\n");
	printf("    [0] ASCII - A sequence of ASCII 0's and 1's\n");
	printf("    [1] Binary - Each byte in data file contains 8 bits of data\n\n");
    mode = select_input_mode_option;
	printf("   Select input mode:  %1d",mode);
//	scanf("%1d", &mode);
	printf("\n");
	if ( mode == 0 ) {
		if ( (fp = fopen(streamFile, "r")) == NULL ) {
			printf("ERROR IN FUNCTION fileBasedBitStreams:  file %s could not be opened.\n",  streamFile);
			exit(-1);
		}
		readBinaryDigitsInASCIIFormat(fp, streamFile);
		fclose(fp);
	}
	else if ( mode == 1 ) {
		if ( (fp = fopen(streamFile, "rb")) == NULL ) {
			printf("ERROR IN FUNCTION fileBasedBitStreams:  file %s could not be opened.\n", streamFile);
			exit(-1);
		}
		readHexDigitsInBinaryFormat(fp);
		fclose(fp);
	}
}


void
readBinaryDigitsInASCIIFormat(FILE *fp, char *streamFile)
{
	int		i, j, num_0s, num_1s, bitsRead, bit;

	if ( (epsilon = (BitSequence *) calloc(tp.n, sizeof(BitSequence))) == NULL ) {
		printf("BITSTREAM DEFINITION:  Insufficient memory available.\n");
		printf("Statistical Testing Aborted!\n");
		return;
	}
	printf("     Statistical Testing In Progress.........\n\n");
	for ( i=0; i<tp.numOfBitStreams; i++ ) {
		num_0s = 0;
		num_1s = 0;
		bitsRead = 0;
		for ( j=0; j<tp.n; j++ ) {
			if ( fscanf(fp, "%1d", &bit) == EOF ) {
				printf("ERROR:  Insufficient data in file %s.  %d bits were read.\n", streamFile, bitsRead);
				fclose(fp);
				free(epsilon);
				return;
			}
			else {
				bitsRead++;
				if ( bit == 0 )
					num_0s++;
				else
					num_1s++;
				epsilon[j] = bit;
			}
		}
		fprintf(freqfp, "\t\tBITSREAD = %d 0s = %d 1s = %d\n", bitsRead, num_0s, num_1s);
		gbt_test_suite();
	}
	free(epsilon);
}


void
readHexDigitsInBinaryFormat(FILE *fp)
{
	int		i, done, num_0s, num_1s, bitsRead;
	BYTE	buffer[4];

	if ( (epsilon = (BitSequence *) calloc(tp.n,sizeof(BitSequence))) == NULL ) {
		printf("BITSTREAM DEFINITION:  Insufficient memory available.\n");
		return;
	}

	printf("     Statistical Testing In Progress.........\n\n");
	for ( i=0; i<tp.numOfBitStreams; i++ ) {
		num_0s = 0;
		num_1s = 0;
		bitsRead = 0;
		done = 0;
		do {
			if ( fread(buffer, sizeof(unsigned char), 4, fp) != 4 ) {
				printf("READ ERROR:  Insufficient data in file.\n");
				free(epsilon);
				return;
			}
			done = convertToBits(buffer, 32, tp.n, &num_0s, &num_1s, &bitsRead);
		} while ( !done );
		fprintf(freqfp, "\t\tBITSREAD = %d 0s = %d 1s = %d\n", bitsRead, num_0s, num_1s);

		gbt_test_suite();

	}
	free(epsilon);
}


int
convertToBits(BYTE *x, int xBitLength, int bitsNeeded, int *num_0s, int *num_1s, int *bitsRead)
{
	int		i, j, count, bit;
	BYTE	mask;
	int		zeros, ones;

	count = 0;
	zeros = ones = 0;
	for ( i=0; i<(xBitLength+7)/8; i++ ) {
		mask = 0x80;
		for ( j=0; j<8; j++ ) {
			if ( *(x+i) & mask ) {
				bit = 1;
				(*num_1s)++;
				ones++;
			}
			else {
				bit = 0;
				(*num_0s)++;
				zeros++;
			}
			mask >>= 1;
			epsilon[*bitsRead] = bit;
			(*bitsRead)++;
			if ( *bitsRead == bitsNeeded )
				return 1;
			if ( ++count == xBitLength )
				return 0;
		}
	}

	return 0;
}


void
openOutputStreams(int option,int bitstreams)
{
//    ROOT_PATH = getPath();
	int		i, numOfBitStreams, numOfOpenFiles = 0;
	char	freqfn[200], summaryfn[200], statsDir[200], resultsDir[200];

//    printf("\n\n***************************getPath()***********************\n\n");

//    printf("\nROOT_PATH=%s\n",ROOT_PATH);

    /*******************************************/
    char s_path[MAX_PATH];
    char *source_path=getcwd(s_path, MAX_PATH); //获得当前项目的绝对地址 bin/
//    char additional_path[MAX_PATH];
    strcat(source_path,"/../Util/RandomnessTest/gbt-1.0/");
//    printf("\nsource_path1=%s\n",source_path);
    strcat(source_path,"experiments/%s/freq.txt");
//    printf("\nsource_path2=%s\n",source_path);
    sprintf(freqfn, source_path, generatorDir[option]);
//    printf("\nfreqfn=%s\n",freqfn);
    /*******************************************/
	if ( (freqfp = fopen(freqfn, "w")) == NULL ) {
		printf("\t\tMAIN:  Could not open freq file: <%s>", freqfn);
		exit(-1);
	}
	sprintf(summaryfn, strcat(getPath(),"experiments/%s/finalAnalysisReport.txt"), generatorDir[option]);
//	sprintf(summaryfn, "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/finalAnalysisReport.txt", generatorDir[option]);
//    printf("\nsummaryfn=%s\n",summaryfn);
    if ( (summary = fopen(summaryfn, "w")) == NULL ) {
		printf("\t\tMAIN:  Could not open stats file: <%s>", summaryfn);
		exit(-1);
	}

	for( i=1; i<=NUMOFTESTS; i++ ) {
		if ( testVector[i] == 1 ) {
			sprintf(statsDir, strcat(getPath(),"experiments/%s/%s/stats.txt"), generatorDir[option], testNames[i]);
//            sprintf(statsDir, "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/%s/stats.txt", generatorDir[option], testNames[i]);
			sprintf(resultsDir, strcat(getPath(),"experiments/%s/%s/results.txt"), generatorDir[option], testNames[i]);
//			sprintf(resultsDir, "/home/pc/PHC/ROI/Util/RandomnessTest/gbt-1.0/experiments/%s/%s/results.txt", generatorDir[option], testNames[i]);
//            printf("\nstatsDir=%s\n",statsDir);
//            printf("\nresultsDir=%s\n",resultsDir);
            if ( (stats[i] = fopen(statsDir, "w")) == NULL ) {	/* STATISTICS LOG */
				printf("ERROR: LOG FILES COULD NOT BE OPENED.\n");
				printf("       MAX # OF OPENED FILES HAS BEEN REACHED = %d\n", numOfOpenFiles);
				printf("-OR-   THE OUTPUT DIRECTORY DOES NOT EXIST.\n");
				exit(-1);
			}
			else
				numOfOpenFiles++;
			if ( (results[i] = fopen(resultsDir, "w")) == NULL ) {	/* P_VALUES LOG   */
				 printf("ERROR: LOG FILES COULD NOT BE OPENED.\n");
				 printf("       MAX # OF OPENED FILES HAS BEEN REACHED = %d\n", numOfOpenFiles);
				 printf("-OR-   THE OUTPUT DIRECTORY DOES NOT EXIST.\n");
				 exit(-1);
			}
			else
				numOfOpenFiles++;
		}
	}
    numOfBitStreams = bitstreams;
	printf("   How many bitstreams? %d",numOfBitStreams);
//	scanf("%d", &numOfBitStreams);

	tp.numOfBitStreams = numOfBitStreams;
	printf("\n");
}


void
invokeTestSuite(int option, char *streamFile,int select_input_mode_option)
{
//    printf("     正在执行invokeTestSuite方法.........\n\n");
	fprintf(freqfp, "________________________________________________________________________________\n\n");
	fprintf(freqfp, "\t\tFILE = %s\t\tALPHA = %6.4f\n", streamFile, ALPHA);
	fprintf(freqfp, "________________________________________________________________________________\n\n");
	if ( option != 0 )
		printf("     Statistical Testing In Progress.........\n\n");
	switch( option ) {
		case 0:
			fileBasedBitStreams(streamFile,select_input_mode_option);
			break;
		case 1:
			lcg();
			break;
		case 2:
			quadRes1();
			break;
		case 3:
			quadRes2();
			break;
		case 4:
			cubicRes();
			break;
		case 5:
			exclusiveOR();
			break;
		case 6:
			modExp();
			break;
		case 7:
			bbs();
			break;
		case 8:
			micali_schnorr();
			break;
		case 9:
			SHA1();
			break;

		/* INTRODUCE NEW PSEUDO RANDOM NUMBER GENERATORS HERE */

		default:
			printf("Error in invokeTestSuite!\n");
			break;
	}
	printf("     Statistical Testing Complete!!!!!!!!!!!!\n\n");
}


void
gbt_test_suite()
{
	if ( (testVector[0] == 1) || (testVector[TEST_FREQUENCY] == 1) )
		Frequency(tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_BLOCK_FREQUENCY] == 1) )
		BlockFrequency(tp.blockFrequencyBlockLength, tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_CUSUM] == 1) )
		CumulativeSums(tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_RUNS] == 1) )
		Runs(tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_LONGEST_RUN] == 1) )
		LongestRunOfOnes(tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_RANK] == 1) )
		Rank(tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_FFT] == 1) )
		DiscreteFourierTransform(tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_UNIVERSAL] == 1) )
		Universal(tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_APEN] == 1) )
		ApproximateEntropy(tp.approximateEntropyBlockLength, tp.n);


	if ( (testVector[0] == 1) || (testVector[TEST_SERIAL] == 1) )
		Serial(tp.serialBlockLength,tp.n);

	if ( (testVector[0] == 1) || (testVector[TEST_LINEARCOMPLEXITY] == 1) )
		LinearComplexity(tp.linearComplexitySequenceLength, tp.n);

    if ( (testVector[0] == 1) || (testVector[TEST_POKER] == 1) )
		Poker(tp.pokerBlockLength, tp.n);

    if ( (testVector[0] == 1) || (testVector[TEST_RUNS_FREQUENCY] == 1) )
		RunsFrequency(tp.n);

    if ( (testVector[0] == 1) || (testVector[TEST_BINARY_DERIVATION] == 1) )
		BinaryDerivation(tp.binaryXORTimes, tp.n);

    if ( (testVector[0] == 1) || (testVector[TEST_AUTO_CORRELATION] == 1) )
		AutoCorrelation(tp.autoMoveLength, tp.n);
}
