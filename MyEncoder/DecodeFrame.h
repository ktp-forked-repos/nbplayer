#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "constants.h"

class Decode
{

public:
	    int FrameWidth;
		int FrameHeight;
		int Frames;
		int FrameNumber;
		unsigned char *RBuf, *BBuf, *GBuf;
		int QParameter;
		float *YBuf, *CrBuf, *CbBuf;
		float *YRef, *CrRef, *CbRef;
		char *FileName;
		bool isIFrame;
		double * YEncode, *CrEncode, *CbEncode;
		int *X_MotionVector, *Y_MotionVector;

public:
	Decode(void);
	~Decode(void);
	void ReadEncodedFile(FILE *);
	void ReadEncodedFileHeader(FILE *);
	void DeQuantize();
	void IDCT_Frame();
	void ComputeResultantImage();
	void ConvertToRGB();
	void Initialize(int , int , int , char*,int);
	void Initialize();
	void WriteOutputRGBFile();
	
};
