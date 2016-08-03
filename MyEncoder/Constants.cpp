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

 
 int FRAME_WIDTH = 352;
 int FRAME_HEIGHT = 288;
 int FRAMES_PER_SECOND = 24;
 int SECONDS_FOR_CONTENT = 5;
char *FINAL_OUT_FILE = "Finalout.rgb";
int QPARAMETER = 0;
int K = 0;
char * INPUT_FILEPATH = "";
char * ENCODE_OUTPUT = "";
char *ENCODE_ENHANCED_OUTPUT = "";

