// Image.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Image.h"
#define MAX_LOADSTRING 100 

char ENHANCED_OUT_FILE [500];//= "C:\\Test Data\\Sample2_Q6K20\\sample2_352x288Enhanced.rgb";
char BASE_OUT_FILE [500];//= "C:\\Test Data\\Sample2_Q6K20\\sample2_352x288Base.rgb";
char INPUT_FILEPATH [500];//= "C:\\Test Data\\Sample2_Q6K20\\sample2_352x288.576v";
int FRAME_WIDTH = 352;
int FRAME_HEIGHT = 288;
int FRAMES_PER_SECOND = 24;
int SECONDS_FOR_CONTENT = 5;
int QPARAMETER = 1;
int K = 6;

// Global Variables:
HINSTANCE hInst; 
HWND btnDisplay;
HWND btnNext;
HWND btnPrevious;
HWND btnStop;
HWND btnEnhanced;
UINT_PTR TIMER_ID;
bool isEnhancedMode = false;

// current instance 
TCHAR szTitle[MAX_LOADSTRING]; 

// The title bar text 
TCHAR szWindowClass[MAX_LOADSTRING]; 

// The title bar text 
// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int); 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EncodeFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DecodeFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK UserPrefs(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Define instance of encoode class.
EncodeFrame encoder,final,enhanced;

void ClearDisplay(HWND hWnd)
{	
	RECT rt1;
	rt1.left = 0;
	rt1.top = 100;
	rt1.right = 2 *encoder.FrameWidth + 50 + 50;
	rt1.bottom = 100 + encoder.FrameHeight;
	RedrawWindow(hWnd,&rt1,NULL,RDW_INVALIDATE);
}

void Display(HWND window, HDC hdc) 
{
	RECT rt;
	GetClientRect(window,&rt); 
	char text[1000]; 
	strcpy(text, "MPEG Video Display\n"); 
	DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
	BITMAPINFO bmi;
	CBitmap bitmap;
	memset(&bmi,0,sizeof(bmi)); 
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader); 
	bmi.bmiHeader.biWidth = encoder.FrameWidth;
	bmi.bmiHeader.biHeight = -encoder.FrameHeight; 
	
	// Use negative height. DIB is top-down. 
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = encoder.FrameWidth *encoder.FrameHeight;
	SetDIBitsToDevice(hdc,50,100,encoder.FrameWidth,encoder.FrameHeight,0,0,0,encoder.FrameHeight,encoder.Data,&bmi,DIB_RGB_COLORS);
	
	memset(&bmi,0,sizeof(bmi)); 
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader); 
	bmi.bmiHeader.biWidth = final.FrameWidth;
	bmi.bmiHeader.biHeight = -final.FrameHeight; 
	
	// Use negative height. DIB is top-down. 
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = final.FrameWidth * final.FrameHeight;
	if(isEnhancedMode)
		SetDIBitsToDevice(hdc,50+encoder.FrameWidth+50,100,encoder.FrameWidth,encoder.FrameHeight,0,0,0,encoder.FrameHeight,enhanced.Data,&bmi,DIB_RGB_COLORS);
	else
		SetDIBitsToDevice(hdc,50+encoder.FrameWidth+50,100,final.FrameWidth,final.FrameHeight,0,0,0,final.FrameHeight,final.Data,&bmi,DIB_RGB_COLORS);	
}

void Init()
{
	// Initialize the encoder video.
	encoder.Initialize(FRAME_WIDTH, FRAME_HEIGHT, FRAMES_PER_SECOND, SECONDS_FOR_CONTENT, INPUT_FILEPATH,QPARAMETER, K);

	// Initialize the final video.
	final.Initialize(FRAME_WIDTH, FRAME_HEIGHT, FRAMES_PER_SECOND, SECONDS_FOR_CONTENT, BASE_OUT_FILE,QPARAMETER, K);	

	// Initialize the enhanced ouput video
	enhanced.Initialize(FRAME_WIDTH, FRAME_HEIGHT, FRAMES_PER_SECOND, SECONDS_FOR_CONTENT, ENHANCED_OUT_FILE,QPARAMETER, K);	
}

void stopVideo(HWND hWnd)
{
	KillTimer(hWnd, TIMER_ID); 	
	return;
}

void displayNextFrame(HWND hWnd,HDC hdc,bool showCurrent)
{		
	//UpdateWindow(hWnd);
	PAINTSTRUCT ps;
	bool displayFrame = false;
	
	if(!hdc)
	{
		displayFrame = true;
	}
	if(displayFrame)
	{
		ClearDisplay(hWnd);
		hdc = BeginPaint(hWnd, &ps); 
	}
	
	if(showCurrent)
	{
		Display(hWnd,hdc); 
	}
	else
	{
		if((encoder.FrameNumber + 1) < (FRAMES_PER_SECOND * SECONDS_FOR_CONTENT))
		{	
			// Read the frame data from the file.
			encoder.ReadFile();
			encoder.FrameNumber++;
			final.ReadFile();
			final.FrameNumber++;
			enhanced.ReadFile();
			enhanced.FrameNumber++;		
			Display(hWnd,hdc); 
		}
		else
		{
			stopVideo(hWnd);
			MessageBox(hWnd,"Frames End","Frame Read Error",1);
			encoder.FrameNumber = 0;
			final.FrameNumber = 0;
			enhanced.FrameNumber = 0;		
		}
	}
	if(displayFrame)
		EndPaint(hWnd, &ps);	
}


void displayVideo(HWND hWnd)
{			
	UINT_PTR timerResult = SetTimer(hWnd, TIMER_ID, 1000/(FRAMES_PER_SECOND) , (TIMERPROC)NULL ); 	
	if(timerResult == 0)
		MessageBox(hWnd,"Error","Error",1);	
}


void displayPreviousFrame(HWND hWnd,HDC hdc)
{
	PAINTSTRUCT ps;
	bool displayFrame = false;
	
	if(!hdc)
	{
		displayFrame = true;
	}
	if(displayFrame)
	{
		ClearDisplay(hWnd);
		hdc = BeginPaint(hWnd, &ps); 
	}
	
	if(encoder.FrameNumber > 0)
	{	
		encoder.FrameNumber--;
		final.FrameNumber--;
		enhanced.FrameNumber--;

		// Read the frame data from the file.
		encoder.ReadFile();
		final.ReadFile();
		enhanced.ReadFile();
		Display(hWnd,hdc); 
	
	}
	else
	{
		MessageBox(hWnd,"Frames End","Frame Read Error",1);
		encoder.FrameNumber = 0;
		final.FrameNumber = 0;
		enhanced.FrameNumber = 0;
	}

	if(displayFrame)
		EndPaint(hWnd, &ps);
}


void ProcessImage() 
{
	Init();
}

void SetGlobalVariables(HINSTANCE hInstance) 
{
	// Initialize global strings 
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow) 
{
	// TODO: Place code here.	
	SetGlobalVariables(hInstance);
	MSG msg;
	HACCEL hAccelTable; 
	MyRegisterClass(hInstance);
	
	memset(ENHANCED_OUT_FILE ,'\0',100);
	memset(INPUT_FILEPATH ,'\0',100);
	memset(BASE_OUT_FILE,'\0',100);

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
// FUNCTION: MyRegisterClass()
//
// PURPOSE: Registers the window class.
//
// COMMENTS:
//
// This function and its usage is only necessary if you want this code
// to be compatible with Win32 systems prior to the 'RegisterClassEx'
// function that was added to Windows 95. It is important to call this function
// so that the application will get 'well formed' small icons associated
// with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);//(COLOR_WINDOW);
	wcex.lpszMenuName = (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	return RegisterClassEx(&wcex); 
}

// FUNCTION: InitInstance(HANDLE, int)
// PURPOSE: Saves instance handle and creates main window
// COMMENTS:
// In this function, we save the instance handle in a global variable and
// create and display the main program window.
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) 
{
	hInst = hInstance; 

	// Store instance handle in our global variable 
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	btnDisplay = CreateWindow("Button","&Play",WS_CHILD,200,470,100,20,hWnd,NULL,hInstance,NULL) ; 
	btnStop = CreateWindow("Button","&Pause",WS_CHILD,310,470,100,20,hWnd,NULL,hInstance,NULL) ; 
	btnNext = CreateWindow("Button","&Next",WS_CHILD,420,470,100,20,hWnd,NULL,hInstance,NULL) ; 
	btnPrevious = CreateWindow("Button","Pre&vious",WS_CHILD,530,470,100,20,hWnd,NULL,hInstance,NULL) ; 	
	btnEnhanced = CreateWindowEx(0,"Button","&Enhance",BS_AUTOCHECKBOX|WS_CHILD,640,470,100,20,hWnd,NULL,hInstance,NULL) ; 	
	
	ShowWindow(btnDisplay,SW_SHOW);
	ShowWindow(btnStop,SW_SHOW);	
	ShowWindow(btnNext,SW_SHOW);
	ShowWindow(btnPrevious,SW_SHOW);
	ShowWindow(btnEnhanced,SW_SHOW);

	if (!hWnd) 
	{
		return FALSE; 
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE; 
}


//
// FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
// PURPOSE: Processes messages for the main window.
//
// WM_COMMAND - process the application menu
// WM_PAINT - Paint the main window
// WM_DESTROY - post a quit message and return
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent; 
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
			
	switch (message) 
	{
	case WM_COMMAND: 
		if((HWND)lParam == btnDisplay)
		{
			displayVideo(hWnd);
			break; 
		}
		if((HWND)lParam == btnNext)
		{
			displayNextFrame(hWnd,NULL,false);
			break;
		}
		if((HWND)lParam == btnPrevious)
		{
			displayPreviousFrame(hWnd,NULL);
			break;
		}
		if((HWND)lParam == btnEnhanced)
		{
			//showEnhanced();
			isEnhancedMode = !isEnhancedMode;
			displayNextFrame(hWnd,NULL,true);			
			break;
		}
		if((HWND)lParam == btnStop)
		{
			stopVideo(hWnd);
			break;
		}

		wmId = LOWORD(wParam); 
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
			case ID_FILE_ENCODE: 			
			DialogBox(hInst, (LPCTSTR)IDD_ENCODEFORM, hWnd, (DLGPROC)EncodeFile);
			break; 
			case ID_FILE_DECODE: 			
			DialogBox(hInst, (LPCTSTR)IDD_DECODE_FORM, hWnd, (DLGPROC)DecodeFile);
			break;
			case ID_FILE_DISPLAYPREFERENCES:
			DialogBox(hInst, (LPCTSTR)IDD_USERPREF, hWnd, (DLGPROC)UserPrefs);
			break; 
			default: 
			return DefWindowProc(hWnd, message, wParam, lParam); 
			}
	break; 
	case WM_TIMER:
		displayNextFrame(hWnd,NULL,false);
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
LRESULT CALLBACK EncodeFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_INITDIALOG: 
			return TRUE; 
		case WM_COMMAND: 
		if (LOWORD(wParam) == IDC_ENCODE)
		{	
			int filePathLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_FilePath));
			int kLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_KValue));
			int qParamLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_Quantization));
			if(filePathLen > 0 && kLen >0 && qParamLen > 0)
			{
				int i;
				char* filepath;
				char* kValue;
				char* qParam;

				filepath = (char*)GlobalAlloc(GPTR, filePathLen + 1);
				kValue = (char*)GlobalAlloc(GPTR, kLen + 1);
				qParam = (char*)GlobalAlloc(GPTR, qParamLen + 1);
				
				GetDlgItemText(hDlg, IDC_FilePath, filepath, filePathLen + 1);
				GetDlgItemText(hDlg, IDC_KValue, kValue, filePathLen + 1);
				GetDlgItemText(hDlg, IDC_Quantization, qParam, filePathLen + 1);
				
				char * messageStr = NULL;
				messageStr = (char *) malloc(15 + filePathLen+1+kLen+1+qParamLen+2);
				memset(messageStr,'\0',15 + filePathLen+1+kLen+1+qParamLen+2);
				strcpy(messageStr,"MyEncoder.exe");
				messageStr[13] = ' ';
				for(i=0;i<filePathLen;i++)
				{
					messageStr[14 + i] = filepath[i];
				}
				messageStr[14 + filePathLen] = ' '; 
									
				for(i=0;i<qParamLen;i++)
				{
					messageStr[14 + filePathLen+1+i] = qParam[i];
				}

				messageStr[14 + filePathLen+1+qParamLen] = ' ';
				
				for(i=0;i<kLen;i++)
				{
					messageStr[14 + filePathLen+1+qParamLen+1+i] = kValue[i];
				}
				MessageBox(hDlg,messageStr,"ENCODE",1);

				// Call to encode exe.
				// Create process
				PROCESS_INFORMATION pInfo;
				STARTUPINFO sInfo;
				DWORD exitCode;
				sInfo.cb = sizeof(STARTUPINFO);
				sInfo.lpReserved = NULL;
				sInfo.lpReserved2 = NULL;
				sInfo.cbReserved2 = 0;
				sInfo.lpDesktop = NULL;
				sInfo.lpTitle = NULL;
				sInfo.dwFlags = 0;
				sInfo.dwX = 0;
				sInfo.dwY = 0;
				sInfo.dwFillAttribute = 0;
				sInfo.wShowWindow = SW_SHOW;
				char * command = "MyEncoder.exe";
				if (!CreateProcess(command,messageStr, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo))
				{
					printf("Error\n");
					exit(1);
				}




				//Delete(messageStr);
				GlobalFree((HANDLE)filepath);
				GlobalFree((HANDLE)kValue);
				GlobalFree((HANDLE)qParam);

			}

			
		}
		else
		{
			if(LOWORD(wParam) == IDC_EXIT) 
			{
				EndDialog(hDlg, LOWORD(wParam));		
			}		 
		}
		return TRUE; 
		break;
	}
	return FALSE; 
}


// Mesage handler for about box.
LRESULT CALLBACK UserPrefs(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_INITDIALOG: 
			return TRUE; 
		case WM_COMMAND: 
		if (LOWORD(wParam) == IDC_DECODE)
		{			
		int sourceFilePathLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_SourceFilePath));
			int baseFilePathLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_BaseFilePath));
			int enhFilePathLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_EnhFilePath));
			if(sourceFilePathLen > 0 && enhFilePathLen >0 && baseFilePathLen> 0)
			{
				int i;
				char* sourceFilePath;
				char* baseFilePath;
				char* enhFilePath;

				sourceFilePath = (char*)GlobalAlloc(GPTR, sourceFilePathLen + 1);
				baseFilePath = (char*)GlobalAlloc(GPTR, baseFilePathLen + 1);
				enhFilePath = (char*)GlobalAlloc(GPTR, enhFilePathLen + 1);
				
				GetDlgItemText(hDlg, IDC_SourceFilePath, sourceFilePath, sourceFilePathLen + 1);
				GetDlgItemText(hDlg, IDC_BaseFilePath, baseFilePath, baseFilePathLen + 1);
				GetDlgItemText(hDlg, IDC_EnhFilePath, enhFilePath, enhFilePathLen + 1);
				
				strcpy(INPUT_FILEPATH,sourceFilePath);
				strcpy(BASE_OUT_FILE,baseFilePath);
				strcpy(ENHANCED_OUT_FILE,enhFilePath);
				ProcessImage();
				//MessageBox(hDlg,INPUT_FILEPATH,"DECODE",1);
				//MessageBox(hDlg,BASE_OUT_FILE,"DECODE",1);
				//MessageBox(hDlg,ENHANCED_OUT_FILE,"DECODE",1);

				GlobalFree((HANDLE)sourceFilePath);
				GlobalFree((HANDLE)baseFilePath);
				GlobalFree((HANDLE)enhFilePath);
			}
		}
		else
		{
			if(LOWORD(wParam) == IDC_DEXIT) 
			{
				EndDialog(hDlg, LOWORD(wParam));				
			}		 
		}
		return TRUE; 		
	}
	return FALSE; 
}
// Mesage handler for about box.
LRESULT CALLBACK DecodeFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_INITDIALOG: 
			return TRUE; 
		case WM_COMMAND: 
		if (LOWORD(wParam) == IDC_DECODE)
		{			

			int baseFilePathLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_BaseFilePath));
			int enhFilePathLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_EnhancedFilePath));
			int kValueLen = GetWindowTextLength(GetDlgItem(hDlg, IDC_KDcodeValue));
			if(baseFilePathLen > 0 && enhFilePathLen >0 && kValueLen > 0)
			{
				int i;
				char* baseFilepath;
				char* kDecodeValue;
				char* enhFilePath;

				baseFilepath = (char*)GlobalAlloc(GPTR, baseFilePathLen + 1);
				kDecodeValue = (char*)GlobalAlloc(GPTR, kValueLen + 1);
				enhFilePath = (char*)GlobalAlloc(GPTR, enhFilePathLen + 1);
				
				GetDlgItemText(hDlg, IDC_BaseFilePath, baseFilepath, baseFilePathLen + 1);
				GetDlgItemText(hDlg, IDC_KDcodeValue, kDecodeValue, kValueLen + 1);
				GetDlgItemText(hDlg, IDC_EnhancedFilePath, enhFilePath, enhFilePathLen + 1);
				
				char * messageStr = NULL;
				messageStr = (char *) malloc(15 + baseFilePathLen+1+kValueLen+1+enhFilePathLen+2);
				memset(messageStr,'\0',15 + baseFilePathLen+1+kValueLen+1+enhFilePathLen+2);
				strcpy(messageStr,"MyDecoder.exe");
				messageStr[13] = ' ';
				for(i=0;i<baseFilePathLen;i++)
				{
					messageStr[14 + i] = baseFilepath[i];
				}
				messageStr[14 + baseFilePathLen] = ' '; 
									
				for(i=0;i<enhFilePathLen;i++)
				{
					messageStr[14 + baseFilePathLen+1+i] = enhFilePath[i];
				}

				messageStr[14 + baseFilePathLen+1+enhFilePathLen] = ' ';
				
				for(i=0;i<kValueLen;i++)
				{
					messageStr[14 + baseFilePathLen+1+enhFilePathLen+1+i] = kDecodeValue[i];
				}
				MessageBox(hDlg,messageStr,"DECODE",1);

				// Call to encode exe.
				// Create process
				PROCESS_INFORMATION pInfo;
				STARTUPINFO sInfo;
				DWORD exitCode;
				sInfo.cb = sizeof(STARTUPINFO);
				sInfo.lpReserved = NULL;
				sInfo.lpReserved2 = NULL;
				sInfo.cbReserved2 = 0;
				sInfo.lpDesktop = NULL;
				sInfo.lpTitle = NULL;
				sInfo.dwFlags = 0;
				sInfo.dwX = 0;
				sInfo.dwY = 0;
				sInfo.dwFillAttribute = 0;
				sInfo.wShowWindow = SW_SHOW;
				char * command = "MyDecoder.exe";
				if (!CreateProcess(command,messageStr, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo))
				{
					printf("Error\n");
					exit(1);
				}
				//Delete(messageStr);
				GlobalFree((HANDLE)baseFilepath);
				GlobalFree((HANDLE)kDecodeValue);
				GlobalFree((HANDLE)enhFilePath);
			}	
		}
		else
		{
			if(LOWORD(wParam) == IDC_DEXIT) 
			{
				EndDialog(hDlg, LOWORD(wParam));				
			}		 
		}
		return TRUE; 		
	}
	return FALSE; 
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


EncodeFrame::EncodeFrame(void)
{
}

EncodeFrame::~EncodeFrame(void)
{
}

void EncodeFrame::Initialize(int Width, int Height, int FramesPerSec, int Seconds, char* File,int qParam, int KParam)
{
	FrameWidth = Width;
	FrameHeight = Height;
	Frames = FramesPerSec*Seconds;
	FrameNumber = 0;
	isIFrame = true;
	QParameter = qParam;
	K = KParam;

	RBuf = new unsigned char[FrameWidth*FrameHeight];
	BBuf = new unsigned char[FrameWidth*FrameHeight];
	GBuf = new unsigned char[FrameWidth*FrameHeight];
	Data = new unsigned char[FrameWidth*FrameHeight*3];

	FileName = new char[_MAX_PATH];
	strcpy(FileName, File);
}


void EncodeFrame::ReadFile()
{
	long i;
	FILE * IN_FILE;
	char * buffer = NULL;
	buffer = (char *) malloc(3*FrameWidth*FrameHeight);
	if(buffer)
	{
		IN_FILE = fopen(FileName, "rb");
		int count =0;
		if (IN_FILE == NULL) 
		{
			//Handle error - Could not read file
			return;
		}
		long retVal = fseek(IN_FILE, FrameWidth*FrameHeight*3*FrameNumber, SEEK_SET);
		if(retVal)
		{
			return;
		}
		else
		{
			retVal = (long) fread(buffer,sizeof(char),3*FrameWidth*FrameHeight,IN_FILE);	
			for (i = 0; i < FrameWidth*FrameHeight; i ++)
			{
				RBuf[i] = buffer[i];
				GBuf[i] = buffer[FrameWidth*FrameHeight +i];
				BBuf[i] = buffer[2*FrameWidth*FrameHeight +i];
				Data[count] = BBuf[i];
				Data[count+1] = GBuf[i];
				Data[count+2] = RBuf[i];
				count +=3;
			}
		}
		fclose(IN_FILE);

	}
}


