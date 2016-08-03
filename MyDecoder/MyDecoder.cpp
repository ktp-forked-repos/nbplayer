// MyDecoder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DecodeFrame.h"
#include "Constants.h"

int _tmain(int argc, _TCHAR* argv[])
{
	// Define instance of the decoder class
	Decode decoder;
	Decode enhanced;

	// Repeate the process for each of the frame.
	int i=0, len=0;

	for(int k=0;k<argc;k++)
	{
		printf("Parameter %d: %s\n",k,argv[k]);
	}
		
	ENCODE_OUTPUT = (char *)malloc(_MAX_PATH*sizeof(char));
	memset(ENCODE_OUTPUT, 0,sizeof(char));
	strcpy(ENCODE_OUTPUT, argv[1]);

	ENCODE_ENHANCED_OUTPUT = (char *)malloc(_MAX_PATH*sizeof(char));
	memset(ENCODE_ENHANCED_OUTPUT, 0,sizeof(char));
	strcpy(ENCODE_ENHANCED_OUTPUT, argv[2]);

	int K = atoi(argv[3]);
	

	FINAL_OUT_FILE = (char *)malloc(_MAX_PATH*sizeof(char));
	memset(FINAL_OUT_FILE, 0,sizeof(char));

	FINAL_ENHANCED_OUT_FILE = (char *)malloc(_MAX_PATH*sizeof(char));
	memset(FINAL_ENHANCED_OUT_FILE, 0,sizeof(char));

	len = (int)strlen(ENCODE_OUTPUT);
	for(i=len; i>0; i--)
	{
		if(ENCODE_OUTPUT[i] == '.')
			{
				memcpy(FINAL_OUT_FILE,ENCODE_OUTPUT, i);
				FINAL_OUT_FILE[i] = '\0';
				FINAL_OUT_FILE = strcat(FINAL_OUT_FILE,"Base.rgb"); 

				memcpy(FINAL_ENHANCED_OUT_FILE,ENCODE_OUTPUT, i);
				FINAL_ENHANCED_OUT_FILE[i] = '\0';
				FINAL_ENHANCED_OUT_FILE = strcat(FINAL_ENHANCED_OUT_FILE,"Enhanced.rgb"); 

				break;
			}
	}
	//Initialize Decoder class
	//decoder.Initialize(FRAME_WIDTH, FRAME_HEIGHT, FRAMES_PER_SECOND*SECONDS_FOR_CONTENT, (char*)ENCODE_OUTPUT, QPARAMETER);
	//enhanced.Initialize(FRAME_WIDTH, FRAME_HEIGHT, FRAMES_PER_SECOND*SECONDS_FOR_CONTENT, (char*)ENCODE_OUTPUT, QPARAMETER);

	FILE * IN_FILE;
	IN_FILE = fopen((char*)ENCODE_OUTPUT, "rb");
	if (IN_FILE == NULL) 
	{
		//Handle error - Could not read file
		return -1;
	}

	FILE * IN_ENHANCED_FILE;
	IN_ENHANCED_FILE = fopen((char*)ENCODE_ENHANCED_OUTPUT, "rb");
	if (IN_FILE == NULL) 
	{
		//Handle error - Could not read file
		return -1;
	}

	decoder.ReadEncodedFileHeader(IN_FILE);
	decoder.Initialize();

	enhanced.ReadEncodedFileHeader(IN_ENHANCED_FILE);
	enhanced.Initialize();

	for(int i = 0; i < (FRAMES_PER_SECOND * SECONDS_FOR_CONTENT); i++)
	{
		decoder.FrameNumber = i;
		enhanced.FrameNumber = i;
		decoder.ReadEncodedFile(IN_FILE);
		enhanced.ReadEncodedFile(IN_ENHANCED_FILE);

		decoder.IDCT_Frame();
		enhanced.IDCT_Frame();

		decoder.ComputeResultantImage();

		for(int x = 0; x < enhanced.FrameHeight*enhanced.FrameWidth; x++)
		{
			enhanced.YRef[x] = decoder.YRef[x] + enhanced.YBuf[x];
			enhanced.CrRef[x] = decoder.CrRef[x] + enhanced.CrBuf[x];
			enhanced.CbRef[x] = decoder.CbRef[x] + enhanced.CbBuf[x];
		}

		decoder.ConvertToRGB();
		enhanced.ConvertToRGB();

		decoder.WriteOutputRGBFile(0);
		enhanced.WriteOutputRGBFile(1);

	}

	if(IN_FILE != NULL)
        fclose(IN_FILE);

	if(IN_ENHANCED_FILE != NULL)
		fclose(IN_ENHANCED_FILE);

	return 0;
}

