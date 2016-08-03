#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "constants.h"



class EncodeFrame
{
public:
	    int FrameWidth;
		int FrameHeight;
		int Frames;
		int FrameNumber;
		unsigned char *RBuf, *BBuf, *GBuf,*Data;
		int QParameter;
		float *YBuf, *CrBuf, *CbBuf;
		float *YRef, *CrRef, *CbRef;
		char *FileName;
		bool isIFrame;
		double * YEncode, *CrEncode, *CbEncode;
		int *X_MotionVector, *Y_MotionVector;
		int K;
		int PadZeros;
		float Threshold;
	
public:
	EncodeFrame(void);
	void Initialize(int , int , int , int, char*,int, int,int);
	void ReadFile(FILE*);
	void ConvertToYUV();
	void DCT_Frame();
	void WriteFile(int);
	~EncodeFrame(void);
	void Quantize();
	void ComputeMotionVectors();
	//void ComputeDiffImage();
	
};
