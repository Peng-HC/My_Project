/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                       D E B U G G I N G  A I D E S
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "config.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                              M A C R O S
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define MAX(x,y)             ((x) <  (y)  ? (y)  : (x))
#define MIN(x,y)             ((x) >  (y)  ? (y)  : (x))
#define isNonPositive(x)     ((x) <= 0.e0 ?   1  : 0)
#define isPositive(x)        ((x) >  0.e0 ?   1 : 0)
#define isNegative(x)        ((x) <  0.e0 ?   1 : 0)
#define isGreaterThanOne(x)  ((x) >  1.e0 ?   1 : 0)
#define isZero(x)            ((x) == 0.e0 ?   1 : 0)
#define isOne(x)             ((x) == 1.e0 ?   1 : 0)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                         G L O B A L  C O N S T A N T S
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define ALPHA							0.01	/* SIGNIFICANCE LEVEL */
#define MAXNUMOFTEMPLATES				148		/* APERIODIC TEMPLATES: 148=>temp_length=9 */
#define NUMOFTESTS						15		/* MAX TESTS DEFINED  */
#define NUMOFGENERATORS					10		/* MAX PRNGs */
#define MAXFILESPERMITTEDFORPARTITION	148
#define	TEST_FREQUENCY					1       /*������Ƶ�����*/
#define	TEST_BLOCK_FREQUENCY			2       /*����Ƶ�����*/
#define	TEST_POKER 						3       /*�˿˼��*/
#define	TEST_SERIAL 					4       /*�ص������м��*/
#define	TEST_RUNS 			        	5       /*�γ��������*/
#define	TEST_RUNS_FREQUENCY 			6       /*�γ�Ƶ�ʼ��*/
#define	TEST_LONGEST_RUN 				7       /*���1�γ̼��*/
#define	TEST_BINARY_DERIVATION			8       /*��Ԫ�Ƶ����*/
#define	TEST_AUTO_CORRELATION		    9       /*����ؼ��*/
#define	TEST_RANK 					    10      /*�����ȼ��*/
#define	TEST_CUSUM 						11      /*�ۼӺͼ��*/
#define	TEST_APEN 						12      /*�����ؼ��*/
#define TEST_LINEARCOMPLEXITY           13      /*���Ը��Ӷȼ��*/
#define TEST_UNIVERSAL                  14      /*Maurerͨ��ͳ�Ƽ��*/
#define	TEST_FFT            			15      /*��ɢ����Ҷ���*/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                   G L O B A L   D A T A  S T R U C T U R E S
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef unsigned char	BitSequence;

typedef struct _testParameters {
	int		n;
	int		blockFrequencyBlockLength;
	int		serialBlockLength;
	int		linearComplexitySequenceLength;
	int		approximateEntropyBlockLength;
	int		numOfBitStreams;
	int     pokerBlockLength;
	int     binaryXORTimes;
	int     autoMoveLength;
} TP;
