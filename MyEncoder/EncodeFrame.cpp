#pragma once
#include "EncodeFrame.h"

EncodeFrame::EncodeFrame(void)
{
}

void EncodeFrame::Initialize(int Width, int Height, int FramesPerSec, int Seconds, char* File,int qParam, int KParam,int zeroPadding)
{
	FrameWidth = Width;
	FrameHeight = Height;
	Frames = FramesPerSec*Seconds;
	FrameNumber = 0;
	isIFrame = true;
	QParameter = qParam;
	K = KParam;
	PadZeros = zeroPadding;
	Threshold =0;

	RBuf = new unsigned char[FrameWidth*FrameHeight];
	BBuf = new unsigned char[FrameWidth*FrameHeight];
	GBuf = new unsigned char[FrameWidth*FrameHeight];
	Data = new unsigned char[FrameWidth*FrameHeight*3];

	FileName = new char[_MAX_PATH];
	strcpy(FileName, File);

	YBuf = new float[FrameWidth*FrameHeight];
	CrBuf = new float[FrameWidth*FrameHeight];
	CbBuf = new float[FrameWidth*FrameHeight];

	YRef = new float[FrameWidth*FrameHeight];
	CrRef = new float[FrameWidth*FrameHeight];
	CbRef = new float[FrameWidth*FrameHeight];

	YEncode = new double[FrameWidth*FrameHeight];
	CrEncode = new double[FrameWidth*FrameHeight];
	CbEncode = new double[FrameWidth*FrameHeight];

	X_MotionVector = new int[FrameWidth*FrameHeight/(16*16)];
	Y_MotionVector = new int[FrameWidth*FrameHeight/(16*16)];
	
}

EncodeFrame::~EncodeFrame(void)
{
	/*delete RBuf;
	delete BBuf;
	delete GBuf;
	delete YBuf;
	delete CrBuf;
	delete CbBuf;
	delete YRef;
	delete CrRef;
	delete CbRef;
	delete YEncode;
	delete CrEncode;
	delete CbEncode;*/
}


void EncodeFrame::ReadFile(FILE *IN_FILE)
{
	int i,count = 0;
	//FILE * IN_FILE;
	//IN_FILE = fopen(FileName, "rb");
	//if (IN_FILE == NULL) 
	//{
	//	//Handle error - Could not read file
	//	return;
	//}
	
	/*int retVal = fseek(IN_FILE, (FrameWidth+1)*(FrameHeight+1)*3*FrameNumber+1, SEEK_SET);
	if(retVal)
	{
		return;
	}
	else*/
	{
		FrameNumber++;
		for (i = 0; i < FrameWidth*FrameHeight; i ++)
		{			
			if(i!= 0 && !(i%(FrameWidth-PadZeros)))
			{
				while(i% FrameWidth)
				{
					RBuf[i] = 0;
					i++;
				}
			}
			RBuf[i] = fgetc(IN_FILE);		
		}

		for (i = 0; i < FrameWidth*FrameHeight; i ++)
		{
			if(i!= 0 && !(i%(FrameWidth-PadZeros)))
			{
				while(i% FrameWidth)
				{
					GBuf[i] = 0;
					i++;
				}
			}
				
			GBuf[i] = fgetc(IN_FILE);
		}
		for (i = 0; i < FrameWidth*FrameHeight; i ++)
		{
			if(i!= 0 &&  !(i%(FrameWidth-PadZeros)))
			{
				while(i% FrameWidth)
				{
					BBuf[i] = 0;
					i++;
				}
			}
		
			BBuf[i] = fgetc(IN_FILE);
		}
		for (i = 0; i < FrameWidth*FrameHeight; i ++)
		{
			Data[count] = BBuf[i];
			Data[count+1] = GBuf[i];
			Data[count+2] = RBuf[i];
			count +=3;
		}
	}
	//fclose(IN_FILE);
}

void EncodeFrame::ConvertToYUV()
{
	for (int i = 0; i < FrameWidth*FrameHeight; i ++)
	{
		YBuf[i] = (float)((0.2990 * RBuf[i] + 0.5870 * GBuf[i] + 0.1140 * BBuf[i])- 128);
		CrBuf[i] = (float)((0.5000 * RBuf[i] - 0.4187 * GBuf[i] - 0.0813 * BBuf[i]) - 0.5);
		CbBuf[i] = (float)((-0.1687 * RBuf[i] -0.3313 * GBuf[i] + 0.5000 * BBuf[i]) - 0.5);
	}	
}

void EncodeFrame::DCT_Frame()
{
	int left=0;
	float Cu=1,Cv=1;

	if(!isIFrame)
	{
		QParameter = QPARAMETER * 4;
	}
	else
	{
		QParameter = QPARAMETER;
	}

	for(int horiz=0;horiz<FrameHeight/8;horiz++)
	{
		for(int vert=0;vert<FrameWidth/8;vert++)
		{			
			// DCT Encode
			for(int u=0;u<8;u++)
			{
				for(int v=0;v<8;v++)
				{	
					YEncode[horiz  * FrameWidth * 8 +  u * FrameWidth +(vert * 8) + v]=0;
					CrEncode[horiz * FrameWidth * 8 +  u * FrameWidth + (vert * 8) + v]=0;
					CbEncode[horiz * FrameWidth * 8 +  u * FrameWidth + (vert * 8) + v]=0;
					(u==0)?(Cu=(float)(1/1.414)):(Cu=1);
					(v==0)?(Cv=(float)(1/1.414)):(Cv=1);
					for(int x=0;x<8;x++)
					{
						for(int y=0;y<8;y++)
						{
							YEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] += 0.25 * Cu * Cv * YBuf[horiz * FrameWidth * 8 + vert * 8 + x * FrameWidth + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
							CrEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] += 0.25 * Cu * Cv * CrBuf[horiz * FrameWidth * 8 + vert * 8 + x * FrameWidth + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
							CbEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] += 0.25 * Cu * Cv * CbBuf[horiz * FrameWidth * 8 + vert * 8 + x * FrameWidth + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
						}
					}
					
					YEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] = (int)(YEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] / (LUM_QUANT_TABLE[u][v]*QParameter));
					CrEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] = (int)(CrEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] / (CHROM_QUANT_TABLE[u][v]*QParameter));
					CbEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] = (int)(CbEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] / (CHROM_QUANT_TABLE[u][v]*QParameter));
				}
			}		
		}
	}
}

void EncodeFrame::WriteFile(int enhanced)
{
	FILE * OUT_FILE;
	short *buffer = NULL;
	buffer = (short*) malloc((3 * FrameWidth * FrameHeight + 1000) * sizeof(short));
	
	if(buffer == NULL)
		return;

	long YBytes = 0, CrBytes = FrameWidth* FrameHeight - (FrameWidth*FrameHeight/64 * 28), CbBytes = 2*(FrameWidth* FrameHeight- (FrameWidth*FrameHeight/64 * 28));
	int step = 0;
	int i = 0, j = 0;
	if(enhanced == 0)
	{
		OUT_FILE = fopen(ENCODE_OUTPUT, "ab");
		if (OUT_FILE == NULL) 
		{
			//Handle error - Could not read file
			return;
		}
	}
	else
	{
		OUT_FILE = fopen(ENCODE_ENHANCED_OUTPUT, "ab");
		if (OUT_FILE == NULL) 
		{
			//Handle error - Could not read file
			return;
		}
	}
	
	// Data Corresponding to each frame.
	char ch = 'P';
	if(isIFrame)
	{
		ch = 'I';
	}
	fwrite(&ch,sizeof(char),1,OUT_FILE);
	
	if(!isIFrame)
	{
		CrBytes += FrameWidth*FrameHeight/256*2;
		CbBytes += FrameWidth*FrameHeight/256*2;
	}

	for(int x = 0; x < FrameHeight/16; x++)
	{
		for(int y = 0; y < FrameWidth/16; y++)
		{
			if(!isIFrame)
			{
				buffer[YBytes++] = (short) X_MotionVector[x*(FrameWidth/16) + y];
				buffer[YBytes++] = (short) Y_MotionVector[x*(FrameWidth/16) + y];
				
			}

			int block = 1;
			while(block <= 4)
			{
				if(block == 1)
				{
					i = 0; j = 0; 
				}
				else if(block == 2)
				{
					i = 0; j = 8; 
				}
				else if(block == 3)
				{
					i = 8; j = 0; 
				}
				else if(block == 4)
				{
					i = 8; j = 8; 
				}
				
				block ++;
				step = i;
				int IBlockStart = i, IBlockEnd = IBlockStart + 8;
				int JBlockStart = j,JBlockEnd = JBlockStart + 8;
				while(step < IBlockEnd)
				{
					i = step; 
					j = JBlockStart;
					while (i >= IBlockStart)
					{
						buffer[YBytes++] = (short) (YEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth]);
						buffer[CrBytes++] = (short) (CrEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth]);
						buffer[CbBytes++] = (short) (CbEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth]);
						i--;
						j++;
					}
					step++;
				}

				/*while(step > IBlockStart)
				{
					step--;
					i = IBlockEnd - 1; j = JBlockStart + IBlockEnd-step;
					while(i >= (IBlockEnd - step + IBlockStart))
					{
						buffer[YBytes++] = (short) (YEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth]);
						buffer[CrBytes++] = (short) (CrEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth]);
						buffer[CbBytes++] = (short) (CbEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth]);
						i--;
						j++;
					}
				}*/
			}
		}
	}
	fwrite(buffer,sizeof(short),CbBytes,OUT_FILE);
	delete(buffer);
	fclose(OUT_FILE);
}

void EncodeFrame::Quantize()
{

}

void EncodeFrame::ComputeMotionVectors()
{
	bool discard = false;
	long uFound,vFound,minSumOfDifferences,SumOfDifferences,Start;
	float totalFrameDifferences=0;

	FILE * temp;
	temp = fopen("test.txt","a");
	if(!isIFrame)
	{
		float *YBufDiff = new float[FrameWidth*FrameHeight];
		float *CrBufDiff = new float[FrameWidth*FrameHeight];
		float *CbBufDiff = new float[FrameWidth*FrameHeight];
	
		//for each macro block do this
		for(int x = 0; x < FrameHeight/16; x++)
		{
			for(int y = 0; y < FrameWidth/16; y++)
			{
				uFound=0;
				vFound=0;				
				minSumOfDifferences=0;
				SumOfDifferences = 0;
				Start =0;
				discard=false;

				//Checking if the macroblock in the current frame is at the same location in the old frame
				for(int u = 0; u < 16; u++)
				{
					for(int v = 0; v < 16; v++)
					{
						SumOfDifferences += (long)abs(YBuf[x*FrameWidth*16 + y*16 + u*FrameWidth + v] - YRef[x*FrameWidth*16 + y*16 + u*FrameWidth + v]);						
					}					
				}
				uFound = 0;
				vFound = 0;
				minSumOfDifferences = SumOfDifferences;
				

				int count = 0;
				SumOfDifferences = 0;
				//Code to find the SumOfDifferences over the area surrounding the macroblock k pixels from each side
				for(int u = -K; u <= K; u++)
				{
					for(int v = -K; v <= K; v++)
					{
						if(u == 0 && v == 0)
						{
							//this has already been checked as the base condition
							continue;
						}
						else
						{
							//set start locations for this macroblock and traverse the entire 16X16 block after that
							Start = x*FrameWidth*16 + u*FrameWidth + y*16 + v;
							if((Start < 0) || ((x*16+u)+16 >= FrameHeight) || ((y*16+v)+16 >= FrameWidth))
								continue;
							SumOfDifferences = 0;
							
							//compute sum of differences for this macroblock within the search area - BRUTE FORCE
							for(int row = 0; row < 16; row++)
							{
								for(int col = 0; col < 16; col++)
								{
									SumOfDifferences += (long)abs(YBuf[x*FrameWidth*16 + y*16 + row*FrameWidth + col] - YRef[Start + row*FrameWidth + col]);
									if(SumOfDifferences >= minSumOfDifferences)
									{
										discard = true;
										break;
									}
								}
								if(discard)
								{
									discard = false;
									break;
								}
							}
							if(SumOfDifferences < minSumOfDifferences)
							{
								uFound = u;
								vFound = v;
								minSumOfDifferences = SumOfDifferences;									
							}							
						}
					}
				}
				//fprintf(temp,"UFound=%d VFound=%d SumDiff=%d\n",uFound,vFound,minSumOfDifferences);
				totalFrameDifferences += minSumOfDifferences;				
				X_MotionVector[x*(FrameWidth/16) + y] = uFound;
				Y_MotionVector[x*(FrameWidth/16) + y] = vFound;
				
				//Computing the differences between the current and the previous buffer
				Start = x*FrameWidth*16 + uFound*FrameWidth + y*16 + vFound;
				for(int row = 0; row < 16; row ++)
				{
					for(int col = 0; col < 16; col++)
					{
						YBufDiff[x*FrameWidth*16 + y*16 + row*FrameWidth + col] = YBuf[x*FrameWidth*16 + y*16 + row*FrameWidth + col] - YRef[Start + row*FrameWidth + col];
						CrBufDiff[x*FrameWidth*16 + y*16 + row*FrameWidth + col] = CrBuf[x*FrameWidth*16 + y*16 + row*FrameWidth + col] - CrRef[Start + row*FrameWidth + col];
						CbBufDiff[x*FrameWidth*16 + y*16 + row*FrameWidth + col] = CbBuf[x*FrameWidth*16 + y*16 + row*FrameWidth + col] - CbRef[Start + row*FrameWidth + col];
					}
				}						
			}
		}
		
		totalFrameDifferences = (float)(totalFrameDifferences * 256.0 / (FrameHeight * FrameWidth));
		if(Threshold==0)
		{
			Threshold = (float)(1.4 * totalFrameDifferences) ;
			for(int i = 0; i < FrameWidth*FrameHeight; i++)	
			{
				YBuf[i] = YBufDiff[i];
				CrBuf[i] = CrBufDiff[i];
				CbBuf[i] = CbBufDiff[i];
			}
		}
		else
		{
				if(totalFrameDifferences > Threshold)
				{
					isIFrame = true;
				}
				else
				{
					for(int i = 0; i < FrameWidth*FrameHeight; i++)	
					{
						YBuf[i] = YBufDiff[i];
						CrBuf[i] = CrBufDiff[i];
						CbBuf[i] = CbBufDiff[i];
					}						
				}
		}
		delete YBufDiff;
		delete CrBufDiff;
		delete CbBufDiff;	
		
	}
	fprintf(temp,"Total Sum of Differences for the frame %d = %f isIFrame = %d\n",FrameNumber,totalFrameDifferences ,isIFrame);				
	fclose(temp);
}
																																																																																																																				



