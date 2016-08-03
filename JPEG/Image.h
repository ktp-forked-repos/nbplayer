
/*******************************************************************************************
* Project:JPEG Encoder-Dcoder															   *
* Developed By:Nimish Bhonsale															   *
* Description: Implements the Image.h header file for JPEG Encoder-Decoder.				   *
* Date: 03/06/2008																		   *
*																						   *
* Modification Log:																		   *
*																						   *
* Date		   Details 																	   *
* 02/20/2008   Created																	   *
*																						   *
********************************************************************************************/

#if !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
#define AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "stdafx.h"
#include "resource.h"
#include <stdio.h>

// Class structure of Image 
class MyImage 
{

private:
	int		Width;
	int		Height;
	char	ImagePath[_MAX_PATH];
	char*	Data;

public:
	MyImage() {};
	unsigned char *Rbuf; 
	unsigned char *Gbuf; 
	unsigned char *Bbuf; 
	~MyImage() { if(Data) delete Data; };
	void	setWidth(int w)  { Width = w; }; 
	void	setHeight(int h) { Height = h; }; 
	void	setImageData(char *img ) { Data = img; };
	void	setImagePath(char *path) { strcpy(ImagePath, path); }
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	char*	getImageData() { return Data; };
	char*	getImagePath() { return ImagePath; }
	void	ReadImage();

};

#endif // !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
