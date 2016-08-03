
#if !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
#define AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "resource.h"
#include <stdio.h>
#endif // !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)

class EncodeFrame
{
public:
	    int FrameWidth;
		int FrameHeight;
		int Frames;
		int FrameNumber;
		unsigned char *RBuf, *BBuf, *GBuf,*Data;
		int QParameter;
		char *FileName;
		bool isIFrame;
		int K;
	
public:
	EncodeFrame(void);
	void Initialize(int , int , int , int, char*,int, int);
	void ReadFile();
	~EncodeFrame(void);
	
};
