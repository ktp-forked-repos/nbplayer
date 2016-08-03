#pragma once
#include "DecodeFrame.h"
#include ".\decodeframe.h"

Decode::Decode(void)
{
}

void Decode::Initialize(int Width, int Height, int NoOfFrames, char* File,int qParam)
{
	FrameWidth = Width;
	FrameHeight = Height;
	Frames = NoOfFrames;
	FrameNumber = 0;
	QParameter = qParam;

	RBuf = new unsigned char[FrameWidth*FrameHeight];
	BBuf = new unsigned char[FrameWidth*FrameHeight];
	GBuf = new unsigned char[FrameWidth*FrameHeight];

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


void Decode::Initialize()
{
	FrameNumber = 0;

	RBuf = new unsigned char[FrameWidth*FrameHeight];
	BBuf = new unsigned char[FrameWidth*FrameHeight];
	GBuf = new unsigned char[FrameWidth*FrameHeight];

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

Decode::~Decode(void)
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
	delete CbEncode;
	delete X_MotionVector;
	delete Y_MotionVector;*/
}


void Decode::ReadEncodedFileHeader(FILE *IN_FILE)
{
	
	char * line;
	line = (char *) malloc(100);
	memset(line,'\0',100);
	
	// Read the header
	fgets(line,100,IN_FILE);	
	FrameWidth = atoi(line);
	memset(line,'\0',100);
	
	// Read the header
	fgets(line,100,IN_FILE);	
	FrameHeight = atoi(line);
	memset(line,'\0',100);
	
	// Read the header
	fgets(line,100,IN_FILE);	
	Frames = atoi(line);
	memset(line,'\0',100);

		// Read the header
	fgets(line,100,IN_FILE);	
	QParameter = atoi(line);
	QPARAMETER = QParameter;
	memset(line,'\0',100);
}

void Decode::ReadEncodedFile(FILE *IN_FILE)
{
	char ch = ' ';
	short *buffer = NULL;
	long numBytes = 0;
	long retVal  = (long) fread(&ch,sizeof(char),1,IN_FILE);
	long YBytes = 0, CrBytes = FrameWidth* FrameHeight - (FrameWidth*FrameHeight/64 * 28), CbBytes = 2*(FrameWidth* FrameHeight- (FrameWidth*FrameHeight/64 * 28));
	int step = 0;
	int i = 0, j = 0;
	if(ch == 'I')
	{
		isIFrame = true;
		buffer = (short*) malloc((FrameWidth*FrameHeight*3 - (FrameWidth*FrameHeight/64 * 28)) * sizeof(short));	
		retVal = (long)fread(buffer,sizeof(short),(size_t)((FrameWidth*FrameHeight - (FrameWidth*FrameHeight/64 * 28))*3), IN_FILE);
	}
	else	
	{
		if(ch == 'P')
		{
			isIFrame = false;
			//buffer = (short*) malloc((FrameWidth*FrameHeight*3 + 2 * FrameWidth*FrameHeight/256) * sizeof(short));
			buffer = (short*) malloc((FrameWidth*FrameHeight*3 + 2 * FrameWidth*FrameHeight/256 - (FrameWidth*FrameHeight/64 * 28)) * sizeof(short));
			retVal = (long)fread(buffer,sizeof(short),(size_t)(FrameWidth*FrameHeight*3 + 2* FrameWidth*FrameHeight/256-(FrameWidth*FrameHeight/64 * 28)*3), IN_FILE);
		}
		else return;
	}
	
	if(buffer)
	{
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
					X_MotionVector[x*(FrameWidth/16) + y] = buffer[YBytes++];
					Y_MotionVector[x*(FrameWidth/16) + y] = buffer[YBytes++];

				}

				// Block 1
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
							YEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth] = buffer[YBytes++];
							CrEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth] = buffer[CrBytes++];
							CbEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth] = buffer[CbBytes++];
							i--;
							j++;
						}
						step++;
					}

					while(step > IBlockStart)
					{
						step--;
						i = IBlockEnd - 1; j = JBlockStart + IBlockEnd-step;
						while(i >= (IBlockEnd - step + IBlockStart))
						{
							YEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth] = 0;//buffer[YBytes++];
							CrEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth] = 0;//buffer[CrBytes++];
							CbEncode[x*FrameWidth*16 + y*16 + i + j*FrameWidth] = 0;//buffer[CbBytes++];
							i--;
							j++;
						}
					}				
				}
			}
		}
	}	
}

void Decode::ConvertToRGB()
{
	float r=0,g=0,b=0;
	for (int i = 0; i < FrameWidth*FrameHeight; i ++)
	{
		r = (float)(1.0000 * (YRef[i]+128) + 1.4020 * (CrRef[i]+0.5) + 0 * (CbRef[i]+0.5));
		if(r<0)
		{
			r = 0;
		}
		if(r>255)
		{
			r = 255;	
		}
		RBuf[i] = (unsigned char)r;
		g = (float)(1.0000 * (YRef[i]+128) - 0.7141 * (CrRef[i]+0.5) - 0.3441 * (CbRef[i]+0.5));
		if(g<0)
		{
			g = 0;
		}
		if(g>255)
		{
			g = 255;	
		}
		GBuf[i] = (unsigned char)g;
		b = (float)(1.0000 * (YRef[i]+128) - 0 * (CrRef[i]+0.5) + 1.7720 * (CbRef[i]+0.5));
		if(b<0)
		{
			b = 0;
		}
		if(b>255)
		{
			b = 255;	
		}
		BBuf[i] = (unsigned char)b;
	}	
}

void Decode::DeQuantize()
{
	
}

void Decode::IDCT_Frame()
{
	// Decode
	float Cu=1,Cv=1;
	if(!isIFrame)
	{
		QParameter= QPARAMETER * 4;
	}
	else
	{
		QParameter= QPARAMETER; 
	}

	for(int horiz=0;horiz<FrameHeight/8;horiz++)
	{
		for(int vert=0;vert<FrameWidth/8;vert++)
		{	
			for(int x=0;x<8;x++)
			{
				for(int y=0;y<8;y++)
				{	
					YBuf[horiz * FrameWidth * 8 + (vert * 8) + x * FrameWidth + y] = 0;
					CrBuf[horiz * FrameWidth * 8 + (vert * 8) + x * FrameWidth + y] = 0;
					CbBuf[horiz * FrameWidth * 8 + (vert * 8) + x * FrameWidth + y] = 0;
					for(int u=0;u<8;u++)
					{
						for(int v=0;v<8;v++)
						{
							(u==0)?(Cu=(float)(1/1.414)):(Cu=1);
							(v==0)?(Cv=(float)(1/1.414)):(Cv=1);
							YBuf[horiz * FrameWidth * 8 + (vert * 8) + x * FrameWidth + y] += (float)(0.25 * Cu * Cv * YEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] * (LUM_QUANT_TABLE[u][v]*QParameter) * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0));
							CrBuf[horiz * FrameWidth * 8 + (vert * 8) + x * FrameWidth + y] += (float)(0.25 * Cu * Cv * CrEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] * (CHROM_QUANT_TABLE[u][v]*QParameter)* cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0));
							CbBuf[horiz * FrameWidth * 8 + (vert * 8) + x * FrameWidth + y] += (float)(0.25 * Cu * Cv * CbEncode[horiz * FrameWidth * 8 + (vert * 8) + u * FrameWidth + v] * (CHROM_QUANT_TABLE[u][v]*QParameter) * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0));
						}
					}
				}
			}
		}
	}
}



void Decode::ComputeResultantImage()
{	
	if(!isIFrame)
	{
		float *tempY, *tempCr, *tempCb;
		tempY = new float[FrameHeight*FrameWidth];
		tempCr = new float[FrameHeight*FrameWidth];
		tempCb = new float[FrameHeight*FrameWidth];

		//for each macro block do this
		for(int x = 0; x < FrameHeight/16; x++)
		{
			for(int y = 0; y < FrameWidth/16; y++)
			{

				//get the motion vectors for that block that has been read from the file 
				int u = X_MotionVector[x*(FrameWidth/16) + y];
				int v = Y_MotionVector[x*(FrameWidth/16) + y];
				
				//set start locations for this macroblock and traverse the entire 16X16 block after that
				int Start = x*FrameWidth*16 + u*FrameWidth + y*16 + v;
				
				//compute sum of differences and old for this macroblock
				for(int row = 0; row < 16; row++)
				{
					for(int col = 0; col < 16; col++)
					{
						tempY[x*FrameWidth*16 + y*16 + row*FrameWidth + col] = YBuf[x*FrameWidth*16 + y*16 + row*FrameWidth + col] + YRef[Start + row*FrameWidth + col];
						tempCr[x*FrameWidth*16 + y*16 + row*FrameWidth + col] = CrBuf[x*FrameWidth*16 + y*16 + row*FrameWidth + col] + CrRef[Start + row*FrameWidth + col];
						tempCb[x*FrameWidth*16 + y*16 + row*FrameWidth + col] = CbBuf[x*FrameWidth*16 + y*16 + row*FrameWidth + col] + CbRef[Start + row*FrameWidth + col];
					}
				}
			}
		}
		for(int i = 0; i < FrameHeight*FrameWidth; i++)
		{
			YRef[i] = tempY[i];
			CrRef[i] = tempCr[i];
			CbRef[i] = tempCb[i];
		}
	}
	else
	{
		for(int i = 0; i < FrameHeight*FrameWidth; i++)
		{
			YRef[i] = YBuf[i];
			CrRef[i] = CrBuf[i];
			CbRef[i] = CbBuf[i];
		}
	}
}


void Decode::WriteOutputRGBFile(int enhanced)
{
	FILE * OUT_FILE;
	if(enhanced == 0)
	{
		OUT_FILE = fopen(FINAL_OUT_FILE, "ab");
		if (OUT_FILE == NULL) 
		{
			//Handle error - Could not read file
			return;
		}
	}
	else
	{
		OUT_FILE = fopen(FINAL_ENHANCED_OUT_FILE, "ab");
		if (OUT_FILE == NULL) 
		{
			//Handle error - Could not read file
			return;
		}
	}

	for(int x = 0; x < FrameHeight*FrameWidth; x++)
	{
		fputc((RBuf[x]), OUT_FILE);
	}
	for(int x = 0; x < FrameHeight*FrameWidth; x++)
	{
		fputc((GBuf[x]), OUT_FILE);
		//fprintf(OUT_FILE, "%c", (char)(GBuf[x]));
	}
	for(int x = 0; x < FrameHeight*FrameWidth; x++)
	{
		fputc((BBuf[x]), OUT_FILE);
		//fprintf(OUT_FILE, "%c", (char)(BBuf[x]));
	}
	
	fclose(OUT_FILE);
}

