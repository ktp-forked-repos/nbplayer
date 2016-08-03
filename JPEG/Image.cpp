// Image.cpp : Defines the entry point for the application.
//

/*******************************************************************************************
* Project:JPEG Encoder-Dcoder															   *
* Developed By:Nimish Bhonsale															   *
* Description: Implements the main class for JPEG Encoder-Decoder.						   *
* Date: 03/06/2008																		   *
*																						   *
* Modification Log:																		   *
*																						   *
* Date		   Details 																	   *
* 02/20/2008   Created																	   *
* 03/01/2008   Added Encoder functionality												   *
* 03/04/2008   Added JPEG Decoder functionality											   *
* 03/05/2008   Made changes to the decoder logic for IDCT quantization case.			   *
*																						   *
********************************************************************************************/

// Header files.
#include "Image.h"
#include <math.h>

// Global variables.
MyImage myImage;
const int WIDTH = 352;
const int HEIGHT = 288;
const int WIDTH1 = 10;
const int HEIGHT1 = 10;
int quantization =0,deliveryMode=0,latency=0,errorcode=0;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	
	char ImagePath[_MAX_PATH];

	sscanf(lpCmdLine, "%s %d %d %d", &ImagePath,&quantization,&deliveryMode,&latency);
	myImage.setWidth(WIDTH);
	myImage.setHeight(HEIGHT);
	myImage.setImagePath(ImagePath);
	myImage.ReadImage();

	if(quantization >=0 && quantization <=7)
	quantization = pow(2,quantization);
	else
		errorcode = 1;

	if(!errorcode && latency < 0)
	{
		errorcode = 2;
	}

	if(!errorcode && (deliveryMode >3 || deliveryMode < 0))
	{
		errorcode = 3;
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	int MilliSeconds = 0, Seconds = 0, Minutes = 0;
	char text[1000];
	RECT rt;
	SYSTEMTIME lpStartTime;
	SYSTEMTIME lpEndTime;
	GetSystemTime(&lpStartTime);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				RECT rt;
				GetClientRect(hWnd, &rt);
				char text[1000];
				if(errorcode == 4)
				{
					strcpy(text, "Incorrect File path\n");
					DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				}
				
				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = myImage.getWidth();
				bmi.bmiHeader.biHeight = -myImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = myImage.getWidth()*myImage.getHeight();

				SetDIBitsToDevice(hdc,
								   10,100,myImage.getWidth(),myImage.getHeight(),
								  0,0,0,myImage.getHeight(),
								  myImage.getImageData(),&bmi,DIB_RGB_COLORS);
				if( errorcode == 0)
				{
				switch(deliveryMode)
				{
				case 1:
					{
						double REncode[64],GEncode[64],BEncode[64], tempR = 0, tempG = 0, tempB = 0;
						unsigned char O[64*3];
						int left=0;
						float Cu=1,Cv=1;

						for(int horiz=0;horiz<HEIGHT/8;horiz++)
						{
							for(int vert=0;vert<WIDTH/8;vert++)
							{			
								for(int x=0;x<64;x++)
								{
									REncode[x]=0;
									BEncode[x]=0;
									GEncode[x]=0;
								}
								// Encode
								for(int u=0;u<8;u++)
								{
									for(int v=0;v<8;v++)
									{	
										(u==0)?(Cu=(1/1.414)):(Cu=1);
										(v==0)?(Cv=(1/1.414)):(Cv=1);
										for(int x=0;x<8;x++)
										{
											for(int y=0;y<8;y++)
											{
												REncode[u * 8 + v] += 0.25 * Cu * Cv * myImage.Rbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
												GEncode[u * 8 + v] += 0.25 * Cu * Cv * myImage.Gbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
												BEncode[u * 8 + v] += 0.25 * Cu * Cv * myImage.Bbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
											}
										}
									}
								}
									
								// Quantize
								for(int u=0;u<64;u++)
								{
									REncode[u]=REncode[u]/quantization;
									BEncode[u]=BEncode[u]/quantization;
									GEncode[u]=GEncode[u]/quantization;
								}		

								// DeQuantize
		
								for(int u=0;u<64;u++)
								{
									REncode[u]=REncode[u]*quantization;
									BEncode[u]=BEncode[u]*quantization;
									GEncode[u]=GEncode[u]*quantization;
								}
									
								// Decode
								for(int u=0;u<64*3;u++)
								{
									O[u]=0;											
								}
								for(int x=0;x<8;x++)
								{
									for(int y=0;y<8;y++)
									{	
										tempR = 0;
										tempG = 0;
										tempB = 0;
										for(int u=0;u<8;u++)
										{
											for(int v=0;v<8;v++)
											{
												(u==0)?(Cu=(1/1.414)):(Cu=1);
												(v==0)?(Cv=(1/1.414)):(Cv=1);
												tempR += 0.25 * Cu * Cv * REncode[u * 8 + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
												tempG += 0.25 * Cu * Cv * GEncode[u * 8 + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
												tempB += 0.25 * Cu * Cv * BEncode[u * 8 + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
											}
										}
										if(tempR <0) tempR = 0;
										if(tempR > 255) tempR = 255;
										if(tempG <0) tempG = 0;
										if(tempG > 255) tempG = 255;
										if(tempB <0) tempB = 0;
										if(tempB > 255) tempB = 255;
										O[x * 8 * 3 + y * 3+2] = (unsigned char)tempR;
										O[x * 8 * 3 + y * 3 + 1] = (unsigned char)tempG;
										O[x * 8 * 3 + y * 3] = (unsigned char)tempB;
									}
								}

								(vert * 8 > WIDTH)?(left = 0):(left = vert * 8);

								memset(&bmi,0,sizeof(bmi));
								bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
								bmi.bmiHeader.biWidth = 8;
								bmi.bmiHeader.biHeight = -8;  // Use negative height.  DIB is top-down.
								bmi.bmiHeader.biPlanes = 1;
								bmi.bmiHeader.biBitCount = 24;
								bmi.bmiHeader.biCompression = BI_RGB;
								bmi.bmiHeader.biSizeImage = 64;
								SetDIBitsToDevice(hdc,WIDTH + 150 + left,100 + horiz * 8,8,8,0,0,0,8,O,&bmi,DIB_RGB_COLORS); 
									Sleep(latency);
							}
						}
	

					}
					break;

				case 2:
					{
						//float REncode[HEIGHT*WIDTH],GEncode[HEIGHT*WIDTH],BEncode[HEIGHT*WIDTH],REncode1[HEIGHT*WIDTH],GEncode1[HEIGHT*WIDTH],BEncode1[HEIGHT*WIDTH], tempR = 0, tempG = 0, tempB = 0;
						float *REncode,*GEncode,*BEncode,*REncode1,*GEncode1,*BEncode1,tempR = 0, tempG = 0, tempB = 0;
						unsigned char *O;//[HEIGHT*WIDTH*3];
						int left=0;
						float Cu=1,Cv=1;
						O = (unsigned char *)malloc(HEIGHT*WIDTH*3*sizeof(unsigned char));
						REncode = (float *)malloc(HEIGHT*WIDTH*sizeof(float));
						GEncode = (float *)malloc(HEIGHT*WIDTH*sizeof(float));
						BEncode = (float *)malloc(HEIGHT*WIDTH*sizeof(float));
						REncode1 = (float *)malloc(HEIGHT*WIDTH*sizeof(float));
						GEncode1 = (float *)malloc(HEIGHT*WIDTH*sizeof(float));
						BEncode1 = (float *)malloc(HEIGHT*WIDTH*sizeof(float));
						memset(REncode,0,HEIGHT*WIDTH*sizeof(float));
						memset(GEncode,0,HEIGHT*WIDTH*sizeof(float));
						memset(BEncode,0,HEIGHT*WIDTH*sizeof(float));
						memset(REncode1,0,HEIGHT*WIDTH*sizeof(float));
						memset(GEncode1,0,HEIGHT*WIDTH*sizeof(float));
						memset(BEncode1,0,HEIGHT*WIDTH*sizeof(float));
						
						for(int horiz=0;horiz<HEIGHT/8;horiz++)
						{
							for(int vert=0;vert<WIDTH/8;vert++)
							{							
								// Encode
								for(int u=0;u<8;u++)
								{
									for(int v=0;v<8;v++)
									{	
										(u==0)?(Cu=(1/1.414)):(Cu=1);
										(v==0)?(Cv=(1/1.414)):(Cv=1);
										for(int x=0;x<8;x++)
										{
											for(int y=0;y<8;y++)
											{
												REncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v] += 0.25 * Cu * Cv * myImage.Rbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
												GEncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v] += 0.25 * Cu * Cv * myImage.Gbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
												BEncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v] += 0.25 * Cu * Cv * myImage.Bbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
											}
										}
										REncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v]/=quantization;
										GEncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v]/=quantization;
										BEncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v]/=quantization;
									}
								}
							}
						}
						
						for(int row=0;row<8;row++)
						for(int col=0;col<8;col++)
						{	
								// DeQuantize
								for(int u=0;u<HEIGHT/8;u++)
									for(int v=0;v<WIDTH/8;v++)
								{
									REncode[u * WIDTH * 8 + v * 8 + row * WIDTH + col]=REncode1[u * WIDTH * 8 + v * 8 + row * WIDTH + col]*quantization;
									BEncode[u * WIDTH * 8 + v * 8 + row * WIDTH + col]=BEncode1[u * WIDTH * 8 + v * 8 + row * WIDTH + col]*quantization;
									GEncode[u * WIDTH * 8 + v * 8 + row * WIDTH + col]=GEncode1[u * WIDTH * 8 + v * 8 + row * WIDTH + col]*quantization;
								}
									
								// Decode
								for(int u=0;u<WIDTH*HEIGHT*3;u++)
								{
									O[u]=0;											
								}
								for(int a=0;a<HEIGHT/8;a++)
								for(int b=0;b<WIDTH/8;b++)
								{
									for(int x=0;x<8;x++)
									{
										for(int y=0;y<8;y++)
										{	
											tempR = 0;
											tempG = 0;
											tempB = 0;
											for(int u=0;u<8;u++)
											{
												for(int v=0;v<8;v++)
												{
													(u==0)?(Cu=(1/1.414)):(Cu=1);
													(v==0)?(Cv=(1/1.414)):(Cv=1);
													tempR += 0.25 * Cu * Cv * REncode[a * WIDTH * 8 + b * 8 + u * WIDTH + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
													tempG += 0.25 * Cu * Cv * GEncode[a * WIDTH * 8 + b * 8 + u * WIDTH + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
													tempB += 0.25 * Cu * Cv * BEncode[a * WIDTH * 8 + b * 8 + u * WIDTH + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
												}
											}
											if(tempR <0) tempR = 0;
											if(tempR > 255) tempR = 255;
											if(tempG <0) tempG = 0;
											if(tempG > 255) tempG = 255;
											if(tempB <0) tempB = 0;
											if(tempB > 255) tempB = 255;
											O[a * WIDTH * 8 * 3 + b * 8 * 3 + x * WIDTH * 3 + y * 3 + 2] = (unsigned char)tempR;
											O[a * WIDTH * 8 * 3 + b * 8 * 3 + x * WIDTH * 3 + y * 3 + 1] = (unsigned char)tempG;
											O[a * WIDTH * 8 * 3 + b * 8 * 3 + x * WIDTH * 3 + y * 3] = (unsigned char)tempB;
										}
									}
								}
								memset(&bmi,0,sizeof(bmi));
								bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
								bmi.bmiHeader.biWidth = WIDTH;
								bmi.bmiHeader.biHeight = -HEIGHT;  // Use negative height.  DIB is top-down.
								bmi.bmiHeader.biPlanes = 1;
								bmi.bmiHeader.biBitCount = 24;
								bmi.bmiHeader.biCompression = BI_RGB;
								bmi.bmiHeader.biSizeImage = WIDTH*HEIGHT;
								SetDIBitsToDevice(hdc,WIDTH + 150,100,WIDTH,HEIGHT,0,0,0,HEIGHT,O,&bmi,DIB_RGB_COLORS); 
								Sleep(latency);
							}
							
					}					
					break;
		
				case 3:
				{
						double *REncode,*GEncode,*BEncode;
						double *REncode1,*GEncode1,*BEncode1,*REncode2,*GEncode2,*BEncode2;
						double tempR = 0, tempG = 0, tempB = 0;
						int bitArrayR[16], bitArrayG[16], bitArrayB[16];
						unsigned char *O;
						int left=0, count=0, temp1 = 0, temp2 = 0, temp3 = 0;
						float Cu=1,Cv=1;
						O = (unsigned char *)malloc(HEIGHT*WIDTH*3*sizeof(unsigned char));
						REncode = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						GEncode = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						BEncode = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						REncode1 = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						GEncode1 = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						BEncode1 = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						REncode2 = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						GEncode2 = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						BEncode2 = (double *)malloc(HEIGHT*WIDTH*sizeof(double));
						memset(REncode,0,HEIGHT*WIDTH*sizeof(double));
						memset(GEncode,0,HEIGHT*WIDTH*sizeof(double));
						memset(BEncode,0,HEIGHT*WIDTH*sizeof(double));
						memset(REncode1,0,HEIGHT*WIDTH*sizeof(double));
						memset(GEncode1,0,HEIGHT*WIDTH*sizeof(double));
						memset(BEncode1,0,HEIGHT*WIDTH*sizeof(double));
						memset(REncode2,0,HEIGHT*WIDTH*sizeof(double));
						memset(GEncode2,0,HEIGHT*WIDTH*sizeof(double));
						memset(BEncode2,0,HEIGHT*WIDTH*sizeof(double));

						// Encode the entire data on the encoder side.
						for(int horiz=0;horiz<HEIGHT/8;horiz++)
						{
							for(int vert=0;vert<WIDTH/8;vert++)
							{							
								// Encode
								for(int u=0;u<8;u++)
								{
									for(int v=0;v<8;v++)
									{	
										(u==0)?(Cu=(1/1.414)):(Cu=1);
										(v==0)?(Cv=(1/1.414)):(Cv=1);
										for(int x=0;x<8;x++)
										{
											for(int y=0;y<8;y++)
											{
												REncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v] += 0.25 * Cu * Cv * myImage.Rbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
												GEncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v] += 0.25 * Cu * Cv * myImage.Gbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
												BEncode1[horiz * WIDTH * 8 + vert * 8 + u * WIDTH + v] += 0.25 * Cu * Cv * myImage.Bbuf[horiz * WIDTH * 8 + vert * 8 + x * WIDTH + y] * cos((2*x+1)*u*3.14159/16)* cos((2*y+1)*v*3.14159/16);
											}
										}									
									}
								}
							}
						}
						
						// For each bit in the data.
						for(int msb=16;msb >=0;msb--)
						{
							memset(REncode2,0,HEIGHT*WIDTH*sizeof(double));
							memset(GEncode2,0,HEIGHT*WIDTH*sizeof(double));
							memset(BEncode2,0,HEIGHT*WIDTH*sizeof(double));

							// DC Component B00 case
							if(msb==16)
							{								
								for(int horiz=0;horiz<HEIGHT/8;horiz++)
								{
									for(int vert=0;vert<WIDTH/8;vert++)
									{		
										REncode2[horiz * WIDTH * 8 + vert * 8 ] = REncode1[horiz * WIDTH * 8 + vert * 8 ];
										GEncode2[horiz * WIDTH * 8 + vert * 8] = GEncode1[horiz * WIDTH * 8 + vert * 8 ];
										BEncode2[horiz * WIDTH * 8 + vert * 8] = BEncode1[horiz * WIDTH * 8 + vert * 8 ];	
										REncode[horiz * WIDTH * 8 + vert * 8]=REncode2[horiz * WIDTH * 8 + vert * 8]/quantization*quantization;
										BEncode[horiz * WIDTH * 8 + vert * 8]=BEncode2[horiz * WIDTH * 8 + vert * 8]/quantization*quantization;
										GEncode[horiz * WIDTH * 8 + vert * 8 ]=GEncode2[horiz * WIDTH * 8 + vert * 8]/quantization*quantization;
									}
								}								
 							}
							else
							{
								for(int horiz=0;horiz<HEIGHT/8;horiz++)
								{
									for(int vert=0;vert<WIDTH/8;vert++)
									{		
							 			for(int row=0;row<8;row++)
										for(int col=0;col<8;col++)
										{	
											if( row == 0 && col == 0)
											{
												REncode2[horiz * WIDTH * 8 + vert * 8 ] = REncode1[horiz * WIDTH * 8 + vert * 8 ];
												GEncode2[horiz * WIDTH * 8 + vert * 8] = GEncode1[horiz * WIDTH * 8 + vert * 8 ];
												BEncode2[horiz * WIDTH * 8 + vert * 8] = BEncode1[horiz * WIDTH * 8 + vert * 8 ];											
											}
											else
											{
												 /*((abs(REncode1[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]) & (unsigned short int)pow(2,msb)) ==  (unsigned short int)pow(2,msb))? (REncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] += (unsigned short int)pow(2,msb)) : (REncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] +=0);
												 ((abs(GEncode1[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]) & (unsigned short int)pow(2,msb)) == (unsigned short int)pow(2,msb))?(GEncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] +=  (unsigned short int)pow(2,msb)):(GEncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] += 0);
												 ((abs(BEncode1[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]) & (unsigned short int)pow(2,msb)) == (unsigned short int)pow(2,msb))?(BEncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] += (unsigned short int)pow(2,msb)):(BEncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] += 0);*/

												memset(bitArrayR, 0, 16*sizeof(int)); 
												memset(bitArrayG, 0, 16*sizeof(int)); 
												memset(bitArrayB, 0, 16*sizeof(int)); 
												count = 0;
												temp1 = REncode1[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col];
												temp2 = GEncode1[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col];
												temp3 = BEncode1[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col];
												while(temp1)
												{
													bitArrayR[count] = temp1%2;
													temp1/=2;
													count++;
												}
												count = 0;
												while(temp2)
												{
													bitArrayG[count] = temp2%2;
													temp2/=2;
													count++;
												}
												count = 0;
												while(temp3)
												{
													bitArrayB[count] = temp3%2;
													temp3/=2;
													count++;
												}

												for(int bit = 0; bit < msb; bit++)
												{
													bitArrayR[bit] = 0;
													bitArrayG[bit] = 0;
													bitArrayB[bit] = 0;
												}
												for(int bit = 0; bit < 16; bit++)
												{
													REncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] += bitArrayR[bit]*pow(2,bit);
													GEncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] += bitArrayG[bit]*pow(2,bit);
													BEncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col] += bitArrayB[bit]*pow(2,bit);
												}

											}
											REncode[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]=(REncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]/quantization)*quantization;
											BEncode[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]=(BEncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]/quantization)*quantization;
											GEncode[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]=(GEncode2[horiz * WIDTH * 8 + vert * 8 + row * WIDTH + col]/quantization)*quantization;
										}
									}
								}
							}

								
							// Decode
							memset(O,0,HEIGHT*WIDTH*3*sizeof(unsigned char));
							
							for(int a=0;a<HEIGHT/8;a++)
							for(int b=0;b<WIDTH/8;b++)
							{
								for(int x=0;x<8;x++)
								{
									for(int y=0;y<8;y++)
									{	
										tempR = 0;
										tempG = 0;
										tempB = 0;
										for(int u=0;u<8;u++)
										{
											for(int v=0;v<8;v++)
											{
												(u==0)?(Cu=(1/1.414)):(Cu=1);
												(v==0)?(Cv=(1/1.414)):(Cv=1);
												tempR += 0.25 * Cu * Cv * REncode[a * WIDTH * 8 + b * 8 + u * WIDTH + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
												tempG += 0.25 * Cu * Cv * GEncode[a * WIDTH * 8 + b * 8 + u * WIDTH + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
												tempB += 0.25 * Cu * Cv * BEncode[a * WIDTH * 8 + b * 8 + u * WIDTH + v] * cos((2*x+1)*u*(3.14159)/16.0)* cos((2*y+1)*v*(3.14159)/16.0);
											}
										}
										if(tempR <0) tempR = 0;
										if(tempR > 255) tempR = 255;
										if(tempG <0) tempG = 0;
										if(tempG > 255) tempG = 255;
										if(tempB <0) tempB = 0;
										if(tempB > 255) tempB = 255;
										O[a * WIDTH * 8 * 3 + b * 8 * 3 + x * WIDTH * 3 + y * 3 + 2] = (unsigned char)tempR;
										O[a * WIDTH * 8 * 3 + b * 8 * 3 + x * WIDTH * 3 + y * 3 + 1] = (unsigned char)tempG;
										O[a * WIDTH * 8 * 3 + b * 8 * 3 + x * WIDTH * 3 + y * 3] = (unsigned char)tempB;
									}
								}
							}							
							memset(&bmi,0,sizeof(bmi));
							bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
							bmi.bmiHeader.biWidth = WIDTH;
							bmi.bmiHeader.biHeight = -HEIGHT;  // Use negative height.  DIB is top-down.
							bmi.bmiHeader.biPlanes = 1;
							bmi.bmiHeader.biBitCount = 24;
							bmi.bmiHeader.biCompression = BI_RGB;
							bmi.bmiHeader.biSizeImage = WIDTH*HEIGHT;
							SetDIBitsToDevice(hdc,WIDTH + 150,100,WIDTH,HEIGHT,0,0,0,HEIGHT,O,&bmi,DIB_RGB_COLORS); 
							Sleep(latency);
						}	
					}					
					break;
				default: break;

			}				
						
				
			
			}
				if(errorcode == 1)
				{
					strcpy(text, "Quantization Factor does not lie within 0-7 range\n");
					DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				}

				if(errorcode == 2)
				{
					strcpy(text, "Time to sleep cannot be neative.\n");
					DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				}

				if(errorcode == 3)
				{
					strcpy(text, "Incorrect Delivery Mode\n");
					DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				}

				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }

   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}



// MyImage functions defined here

void MyImage::ReadImage()
{

	FILE *IN_FILE;
	int i;
	Rbuf = new unsigned char[HEIGHT*WIDTH]; 
	Gbuf = new unsigned char[HEIGHT*WIDTH]; 
	Bbuf = new unsigned char[HEIGHT*WIDTH]; 


		IN_FILE = fopen(ImagePath, "rb");


	if (IN_FILE == NULL) 
	{
		errorcode = 4;
		return;
		//fprintf(stderr, "Error");
		//exit(0);
	}

	for (i = 0; i < WIDTH*HEIGHT; i ++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < WIDTH*HEIGHT; i ++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < WIDTH*HEIGHT; i ++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}
	
	Data = new char[WIDTH*HEIGHT*3];

	for (i = 0; i < HEIGHT*WIDTH; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1]	= Gbuf[i];
		Data[3*i+2]	= Rbuf[i];
	}

	//delete [] Rbuf;
	//delete [] Gbuf;
	//delete [] Bbuf;
	fclose(IN_FILE);

}


