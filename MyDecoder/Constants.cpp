
#ifndef CONSTANTS_H
#define CONSTANTS_H

// TODO: reference additional headers your program requires here
 int LUM_QUANT_TABLE[][8] = {{4,4,4,8,8,16,16,32},
							 {4,4,4,8,8,16,16,32},
							 {4,4,8,8,16,16,32,32},
							 {8,8,8,16,16,32,32,32},
							 {8,8,16,16,32,32,32,32},
							 {16,16,16,32,32,32,32,32},
							 {16,16,32,32,32,32,32,32},
							 {32,32,32,32,32,32,32,32}
							};

 int CHROM_QUANT_TABLE[][8] = {{8,8,8,16,32,32,32,32},
							 {8,8,8,16,32,32,32,32},
							 {8,8,16,32,32,32,32,32},
							 {16,16,32,32,32,32,32,32},
							 {32,32,32,32,32,32,32,32},
							 {32,32,32,32,32,32,32,32},
							 {32,32,32,32,32,32,32,32},
							 {32,32,32,32,32,32,32,32}
							};
 int THRESHOLD = 10;

 char *FINAL_OUT_FILE = "Finalout.rgb";
 char *FINAL_ENHANCED_OUT_FILE = "Finalenhanced.rgb";
 char * ENCODE_OUTPUT = "outputfile.txt";
 char *ENCODE_ENHANCED_OUTPUT = "enhancedfile.txt";
 char * INPUT_FILEPATH = "sample1_352x288.576v";
 int FRAME_WIDTH = 352;
 int FRAME_HEIGHT = 288;
 int FRAMES_PER_SECOND = 24;
 int SECONDS_FOR_CONTENT = 5;
 int QPARAMETER = 6;
 //int K;



#endif