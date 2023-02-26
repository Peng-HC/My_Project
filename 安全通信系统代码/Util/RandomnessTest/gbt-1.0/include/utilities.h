/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
              U T I L I T Y  F U N C T I O N  P R O T O T Y P E S
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char    *getPath();
int		displayGeneratorOptions(int generator_selection_option);
int		generatorOptions(int generator_selection_option,char** streamFile);
void	chooseTests(int randomness_statistical_tests_option,char *choose_randomness_tests);
void	fixParameters(int parameter_adjustments_option);
void	fileBasedBitStreams(char *streamFile,int select_input_mode_option);
void	readBinaryDigitsInASCIIFormat(FILE *fp, char *streamFile);
void	readHexDigitsInBinaryFormat(FILE *fp);
int		convertToBits(BYTE *x, int xBitLength, int bitsNeeded, int *num_0s, int *num_1s, int *bitsRead);
void	openOutputStreams(int option,int bitstreams);
void	invokeTestSuite(int option, char *streamFile,int select_input_mode_option);
void	gbt_test_suite();
