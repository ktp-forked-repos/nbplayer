// MSD_Final_Project.cpp : Defines the entry point for the application.
//
#pragma once
#include "stdafx.h"

#include "encodeframe.h"
#include "decodeframe.h"
#include "constants.h"
#define MAX_LOADSTRING 100

// Define instance of encoode class.
EncodeFrame encoder;
EncodeFrame enhanced;
// Define instance of the decoder class
Decode decoder;


///<comment>
/// Writes the output header to an intermediate encoded file.
///</comment>
void writeOutputHeader(int width,int height,int frames, int qParameter)
{
	// Open output file to write.
	FILE *OUT_FILE, *OUT_ENHANCED_FILE;
	OUT_FILE = fopen(ENCODE_OUTPUT, "wb");
	OUT_ENHANCED_FILE =  fopen(ENCODE_ENHANCED_OUTPUT, "wb");
	if (OUT_FILE == NULL) 
	{
		//Handle error - Could not read file
		return;
	}
	if (OUT_ENHANCED_FILE == NULL) 
	{
		//Handle error - Could not read file
		return;
	}

	// Number of columns in each frame.
	fprintf(OUT_FILE,"%d\n",width);
	fprintf(OUT_ENHANCED_FILE,"%d\n",width);
	
	// Number of rows in each frame.
	fprintf(OUT_FILE,"%d\n",height);
	fprintf(OUT_ENHANCED_FILE,"%d\n",height);

	// Number of frames in the video.
	fprintf(OUT_FILE,"%d\n",frames);
	fprintf(OUT_ENHANCED_FILE,"%d\n",frames);

	// qParameter Value.
	fprintf(OUT_FILE,"%d\n",qParameter);
	fprintf(OUT_ENHANCED_FILE,"%d\n",qParameter);

	// Close the file pointer.
	fclose(OUT_FILE);
	fclose(OUT_ENHANCED_FILE);
}

//Computes the enhanced layer and writes into a file
void ComputeEnhancedLayer()
{
	for(int i = 0; i < enhanced.FrameWidth*enhanced.FrameHeight; i++)
	{
		enhanced.YBuf[i] = enhanced.YBuf[i]-encoder.YRef[i];
		enhanced.CrBuf[i] = enhanced.CrBuf[i]-encoder.CrRef[i];
		enhanced.CbBuf[i] = enhanced.CbBuf[i]-encoder.CbRef[i];
	}

	enhanced.DCT_Frame();

	enhanced.WriteFile(1);
}


///<comment>
/// The function handles the encoding of all the frames. It initializes the encode variable, reads the corresponding frame from the file, converts
/// the RGB frame to YUV, computes the DCT corresponding to the frame blocks and finally writes the encoded data to the intermediate encoded file.
///</comment>
void encodeFile()
{

	// Repeate the process for each of the frame.
	int i=0;

	// Initialize the encoder class.
	encoder.Initialize((FRAME_WIDTH + (FRAME_HEIGHT) * (FRAME_WIDTH % 8)),(FRAME_HEIGHT + (FRAME_WIDTH) * (FRAME_HEIGHT % 8)), FRAMES_PER_SECOND, SECONDS_FOR_CONTENT, (char*)INPUT_FILEPATH,QPARAMETER, K, (FRAME_WIDTH % 8));

	//Initialize Decoder class
	decoder.Initialize((FRAME_WIDTH + (FRAME_HEIGHT) * (FRAME_WIDTH % 8)),(FRAME_HEIGHT + (FRAME_WIDTH) * (FRAME_HEIGHT % 8)), FRAMES_PER_SECOND*SECONDS_FOR_CONTENT, (char*)ENCODE_OUTPUT, QPARAMETER);

	//Initialize enahnced layer
	enhanced.Initialize((FRAME_WIDTH + (FRAME_HEIGHT) * (FRAME_WIDTH % 8)),(FRAME_HEIGHT + (FRAME_WIDTH) * (FRAME_HEIGHT % 8)), FRAMES_PER_SECOND, SECONDS_FOR_CONTENT, (char*)INPUT_FILEPATH,QPARAMETER, K,(FRAME_WIDTH % 8));
	

	FILE * IN_FILE;
	IN_FILE = fopen((char*)INPUT_FILEPATH, "rb");
	if (IN_FILE == NULL) 
	{
		//Handle error - Could not read file
		return;
	}

	for(i = 0; i <(FRAMES_PER_SECOND * SECONDS_FOR_CONTENT); i ++)
	{		
		if(i==0 || (i%30)==0)
		{
			encoder.isIFrame = true;
			//decoder.isIFrame = true;
		}
		else
		{
			encoder.isIFrame = false;
			//decoder.isIFrame = false;
		}

		// Read the frame data from the file.
		encoder.ReadFile(IN_FILE);

		// Convert the RGB data correspoding to the frame to YUV.
		encoder.ConvertToYUV();

		for(int i = 0; i < enhanced.FrameWidth*enhanced.FrameHeight; i++)
		{
			enhanced.YBuf[i] = encoder.YBuf[i];
			enhanced.CrBuf[i] = encoder.CrBuf[i];
			enhanced.CbBuf[i] = encoder.CbBuf[i];
		}

		//
		if(!encoder.isIFrame) 
		{
			//ComputeMotionCompensation
			encoder.ComputeMotionVectors();
		}
		
		// Compute the DCT.
		encoder.DCT_Frame();
		
		// Write the encoded data to an intermediate file.
		encoder.WriteFile(0);
		
		//Assign the buffers that need to be decoded
		for(int i=0;i<encoder.FrameWidth*encoder.FrameHeight;i++)
		{
			if(i<encoder.FrameWidth*encoder.FrameHeight/256)
			{
				decoder.X_MotionVector[i] = encoder.X_MotionVector[i];
				decoder.Y_MotionVector[i] = encoder.Y_MotionVector[i];			
			}
			decoder.YEncode[i] = encoder.YEncode[i];
			decoder.CrEncode[i] = encoder.CrEncode[i];
			decoder.CbEncode[i] = encoder.CbEncode[i];
		}
		
		// Asign if the frame is I-frame of P-Frame
		decoder.isIFrame = encoder.isIFrame;

		//decode the encoded output
		decoder.IDCT_Frame();
		
		////Compute the resultant image which will be the reference for the next frame
		decoder.ComputeResultantImage();

		//Assign the reference buffers to the encoder for the next frame to use
		for(int i=0;i<encoder.FrameWidth*encoder.FrameHeight;i++)
		{
			encoder.YRef[i] = decoder.YRef[i];
			encoder.CrRef[i] = decoder.CrRef[i];
			encoder.CbRef[i] =  decoder.CbRef[i];
		}

		ComputeEnhancedLayer();
	}
	fclose(IN_FILE);
}

///<comment>
/// Main entry point for the application.
///</comment>
int main(int argc, char *argv[])
{
	int i = 0, len = 0;

	for(int k=0;k<argc;k++)
	{
		printf("Param %d %s\n",k,argv[k]);
	}

	INPUT_FILEPATH = (char *)malloc(_MAX_PATH*sizeof(char));
	memset(INPUT_FILEPATH, 0,sizeof(char));
	strcpy(INPUT_FILEPATH, argv[1]);

	ENCODE_OUTPUT = (char *)malloc(_MAX_PATH*sizeof(char));
	memset(ENCODE_OUTPUT, 0,sizeof(char));

	ENCODE_ENHANCED_OUTPUT = (char *)malloc(_MAX_PATH*sizeof(char));
	memset(ENCODE_ENHANCED_OUTPUT, 0,sizeof(char));
	//strcpy(INPUT_FILEPATH, argv[1]);

	QPARAMETER = atoi(argv[2]);
	K = atoi(argv[3]);

	len = (int)strlen(INPUT_FILEPATH);
	for(i=len; i>0; i--)
	{
		if(INPUT_FILEPATH[i] == '.')
			{
				memcpy(ENCODE_OUTPUT,INPUT_FILEPATH, i+1);
				ENCODE_OUTPUT[i+1] = '\0';
				ENCODE_OUTPUT = strcat(ENCODE_OUTPUT,"base"); 

				memcpy(ENCODE_ENHANCED_OUTPUT,INPUT_FILEPATH, i+1);
				ENCODE_ENHANCED_OUTPUT[i+1] = '\0';
				ENCODE_ENHANCED_OUTPUT = strcat(ENCODE_ENHANCED_OUTPUT,"enhanced"); 

				break;
			}
	}

	// Write out the header information.
	writeOutputHeader((FRAME_WIDTH + (FRAME_HEIGHT) * (FRAME_WIDTH % 8)),(FRAME_HEIGHT + (FRAME_WIDTH) * (FRAME_HEIGHT % 8)),FRAMES_PER_SECOND * SECONDS_FOR_CONTENT,QPARAMETER);
	
	// Encode the file.
	encodeFile();
	return 1;
}



