#include <Windows.h>
#include "resource.h"
#include <GdiPlus.h>
#include <GdiPlusBase.h>
#include <GdiPlusBrush.h>
#include <GdiPlusFont.h>
#include <shlwapi.h>
#include <string>
#include "PNG.h"
#include <tchar.h>

using namespace Gdiplus;
using namespace std;

//#define SHIMON 0xFFF
enum Messages
{
	NOTHING,
	MSN_CLOSED,
	LOADING,
	INJECTED
};
bool ImageLoaded = false;
unsigned int LastMessage = NOTHING;
Gdiplus::Status status;
HINSTANCE hInst;
HWND hwnd;
bool AppStarted = false;
extern bool InjectDLL(DWORD procID, std::string dll);
extern HANDLE GetProcessHandle(LPSTR szExeName);
extern std::string getPath();

BOOL WINAPI MainDlgProc (HWND hWnd,	// handle to dialog box
	UINT uMsg,      // message
	WPARAM wParam,  // first message parameter
	LPARAM lParam ) // second message parameter
	;

void StartProgram(HWND hWnd)
{
	AppStarted = true;
	CGdiPlusBitmapResource* m_image = new CGdiPlusBitmapResource();
	if(m_image->Load(IDB_PNG1, _TEXT("PNG"), hInst))
	{
		// And draw it using Graphics object:

		// Create graphics object from HDC
		HDC hDC = GetWindowDC(hWnd);
		//SetBkMode(hDC, TRANSPARENT);
		Gdiplus::Graphics g(hDC);

		//SetLayeredWindowAttributes(hWnd, 0, 50, 0x00000002);

		// Draw the PNG image using graohics object
		g.DrawImage(*m_image, 50, 27);
		FontFamily fontFamily(L"Arial");
		StringFormat strformat;
		wchar_t pszbuf[90] = L"Loading...";

		GraphicsPath path;
		path.AddString(pszbuf, wcslen(pszbuf), &fontFamily, 
			FontStyleRegular, 24, Gdiplus::Point(250,27*7), &strformat );
		Pen pen(Color(0,153,204), 6);
		g.DrawPath(&pen, &path);
		SolidBrush brush(Color(0,204,255));
		g.FillPath(&brush, &path);
		HANDLE hMSN = GetProcessHandle("msnmsgr.exe");
		if(hMSN)
		{
			DWORD dwMsnProcessId = GetProcessId(hMSN);
			CloseHandle(hMSN);
			std::string DLLName = "msnhack.dll";
			if(!InjectDLL(dwMsnProcessId, getPath() + DLLName.c_str()))
			{
				wchar_t pszbuf[90] = L"Injection Failed!";

				GraphicsPath path;
				path.AddString(pszbuf, wcslen(pszbuf), &fontFamily, 
					FontStyleRegular, 24, Gdiplus::Point(250,27*7), &strformat );
				Pen pen(Color(0,0,0), 6);
				g.DrawPath(&pen, &path);
				SolidBrush brush(Color(255,0,0));
				g.FillPath(&brush, &path);
				return;
			}
			else
			{
				ExitProcess(0);
			}
		}
		else
		{
			wchar_t pszbuf[90] = L"Please open MSN!";

			GraphicsPath path;
			path.AddString(pszbuf, wcslen(pszbuf), &fontFamily, 
				FontStyleRegular, 24, Gdiplus::Point(250,27*7), &strformat );
			Pen pen(Color(0,0,0), 6);
			g.DrawPath(&pen, &path);
			SolidBrush brush(Color(255,0,0));
			g.FillPath(&brush, &path);
			return;
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow){

		ULONG_PTR m_gdiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		status = Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
		hInst = hInstance;
		DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_DIALOG1), NULL, MainDlgProc, NULL);
		return 0;
}

BOOL WINAPI MainDlgProc (HWND hWnd,	// handle to dialog box
	UINT uMsg,      // message
	WPARAM wParam,  // first message parameter
	LPARAM lParam ) // second message parameter
{
	if(NULL != status)
		MessageBox(hWnd, L"Couldn't load GDI!", L"GDI Error!", NULL);
	/*
	HDC hDC = GetWindowDC(hWnd);
	//SetBkMode(hDC, TRANSPARENT);
	//ShowWindow(hWnd, SW_SHOW);
	//UpdateWindow(hWnd);
	CGdiPlusBitmapResource* m_image = new CGdiPlusBitmapResource();
	if(m_image->Load(IDB_PNG1, _TEXT("PNG"), hInst))
	{
	// And draw it using Graphics object:

	// Create graphics object from HDC
	HDC hDC = GetWindowDC(hWnd);
	//SetBkMode(hDC, TRANSPARENT);
	Gdiplus::Graphics g(hDC);

	//	SetLayeredWindowAttributes(hWnd, 0, 50, 0x00000002);
	// Draw the PNG image using graohics object
	g.DrawImage(*m_image, 0, 0);
	*/
	/*
	if(uMsg = WM_INITDIALOG)
	{
		HICON hIcon = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, LR_COPYFROMRESOURCE);
		if(hIcon)
		{
			//wc.hIcon  = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			UpdateWindow(hWnd);
			ShowWindow(hWnd, SW_SHOW);
		}
		}*/
	if(hwnd == NULL)
		hwnd = hWnd;
	UpdateWindow(hWnd);
			ShowWindow(hWnd, SW_SHOW);
			CGdiPlusBitmapResource* m_image = new CGdiPlusBitmapResource();
			if(ImageLoaded || m_image->Load(IDB_PNG1, _TEXT("PNG"), hInst))
			{
				// And draw it using Graphics object:

				// Create graphics object from HDC
				HDC hDC = GetWindowDC(hWnd);
				//SetBkMode(hDC, TRANSPARENT);
				Gdiplus::Graphics g(hDC);

				//SetLayeredWindowAttributes(hWnd, 0, 50, 0x00000002);

				// Draw the PNG image using graohics object
				Gdiplus::Status drawstatus = (Gdiplus::Status)0;
				if(!ImageLoaded)
					drawstatus = g.DrawImage(*m_image, 50, 27);
				if(drawstatus)
				{
					MessageBox(hWnd, L"ERR", L"ERR", NULL);
					return DefWindowProc(hWnd, uMsg, wParam, lParam);
				}
				ImageLoaded = true;

				FontFamily fontFamily(L"Arial");
				StringFormat strformat;
				wchar_t pszbuf[90] = L"Loading...";
				if(LastMessage != LOADING && LastMessage != MSN_CLOSED && LastMessage != INJECTED)
				{
					GraphicsPath path;
					path.AddString(pszbuf, wcslen(pszbuf), &fontFamily, 
						FontStyleRegular, 24, Gdiplus::Point(250,27*7), &strformat );
					Pen pen(Color(0,153,204), 6);
					g.DrawPath(&pen, &path);
					SolidBrush brush(Color(0,204,255));
					g.FillPath(&brush, &path);
					LastMessage = LOADING;
				}

				HANDLE hMSN = GetProcessHandle("msnmsgr.exe");
				if(hMSN)
				{
					DWORD dwMsnProcessId = GetProcessId(hMSN);
					CloseHandle(hMSN);
					std::string DLLName = "\\msnhack.dll";
					if(!InjectDLL(dwMsnProcessId, getPath() + DLLName.c_str()))
					{
						wchar_t pszbuf[90] = L"Injection Failed!";

						GraphicsPath path;
						path.AddString(pszbuf, wcslen(pszbuf), &fontFamily, 
							FontStyleRegular, 24, Gdiplus::Point(250,27*7), &strformat );
						Pen pen(Color(0,0,0), 6);
						g.DrawPath(&pen, &path);
						SolidBrush brush(Color(255,0,0));
						g.FillPath(&brush, &path);
						return DefWindowProc(hWnd, uMsg, wParam, lParam);
					}
					else
					{
						//ExitProcess(0);
						if(LastMessage != INJECTED)
						{
								wchar_t pszbuf[] = L"Injected!";
	FontFamily fontFamily(L"Arial");
				StringFormat strformat;
								GraphicsPath path;
							path.AddString(pszbuf, wcslen(pszbuf), &fontFamily, 
								FontStyleRegular, 24, Gdiplus::Point(250,27*7), &strformat );
							Pen pen(Color(0,0,0), 6);
							g.DrawPath(&pen, &path);
							SolidBrush brush(Color(50,205,50)); //34-139-34
							g.FillPath(&brush, &path);
							LastMessage = INJECTED;
							Sleep(5000);
							ExitProcess(0);
						}
					}
				}
				else
				{
					if(LastMessage != MSN_CLOSED && LastMessage != INJECTED)
					{
					wchar_t pszbuf[90] = L"Please open MSN!";

					GraphicsPath path;
					path.AddString(pszbuf, wcslen(pszbuf), &fontFamily, 
						FontStyleRegular, 24, Gdiplus::Point(250,27*8), &strformat );
					Pen pen(Color(0,0,0), 6);
					g.DrawPath(&pen, &path);
					SolidBrush brush(Color(255,0,0));
					g.FillPath(&brush, &path);
					LastMessage = MSN_CLOSED;
					}
					return DefWindowProc(hWnd, uMsg, wParam, lParam);
				}
			}
			else
			{
				MessageBox(hWnd, L"ERROR!", L"ERROR", NULL);
			}
		
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}




