#include <windows.h>
#include "resource.h"
#include <string>
#include <shlwapi.h>

BOOL WINAPI MainDlgProc (HWND hWnd,	// handle to dialog box
						   UINT uMsg,      // message
						   WPARAM wParam,  // first message parameter
						   LPARAM lParam ) // second message parameter
						   ;


int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
					 int       nCmdShow){
	 
	::DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_DIALOG1), NULL, MainDlgProc, NULL);
return 0;
}

extern bool InjectDLL(DWORD procID, std::string dll);
extern HANDLE GetProcessHandle(LPSTR szExeName);
extern std::string getPath();

std::wstring s2ws(const std::string& s)
{
 int len;
 int slength = (int)s.length() + 1;
 len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
 wchar_t* buf = new wchar_t[len];
 MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
 std::wstring r(buf);
 delete[] buf;
 return r;
}


BOOL WINAPI MainDlgProc (HWND hWnd,	// handle to dialog box
						   UINT uMsg,      // message
						   WPARAM wParam,  // first message parameter
						   LPARAM lParam ) // second message parameter
{
	switch (uMsg) {
		
	case WM_INITDIALOG:
		{
			HICON hIcon = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, LR_COPYFROMRESOURCE);
			if(hIcon)
			{
				    //wc.hIcon  = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
				SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
				SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
				UpdateWindow(hWnd);
			}
			SendDlgItemMessage(hWnd, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)L"Crasher");
			SendDlgItemMessage(hWnd, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)L"Protector");
			SendDlgItemMessage(hWnd, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)L"Spammer");
			SendDlgItemMessage(hWnd, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)L"ZTools");
			SendDlgItemMessage(hWnd, IDC_COMBO1, CB_SETCURSEL, 0, 0);
		}
	     	break;
            case WM_CLOSE:
				::ExitProcess(0);
	            break;
			case WM_DESTROY:
				::SendMessage(hWnd,WM_CLOSE,0,0);
				break;
   			case WM_COMMAND:
				if(wParam == IDC_BUTTON1)
				{
					SetDlgItemText(hWnd, STATUSMSG, L"Injecting...");
					HANDLE hMSN = GetProcessHandle("msnmsgr.exe");
					if(hMSN)
					{
						DWORD dwMsnProcessId = GetProcessId(hMSN);
						CloseHandle(hMSN);
						HWND hComboBox = ::GetDlgItem(hWnd, IDC_COMBO1);
						int index = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
						//if(!InjectDLL(dwMsnProcessId, getPath() + "\\senderinjection.dll"))
						std::string DLLName = "\\senderinjection.dll";
						switch(index)
						{
						case 1:
							DLLName = "\\crashprotector.dll";
							break;
						case 2:
							DLLName = "\\msnspammer.dll";
							break;
						case 3:
							DLLName = "\\msnhack.dll";
							break;
						default:
							break;
						}
						if(!InjectDLL(dwMsnProcessId, getPath() + DLLName.c_str()))
						{
							SetDlgItemText(hWnd, STATUSMSG, L"Failed to inject!");
							return 0;
						}
						else
						{
							ExitProcess(0);
						}
					}
					else
					{
						SetDlgItemText(hWnd, STATUSMSG, L"Make sure MSN is opened...");
					}
				}
				else if(wParam == IDC_BUTTON2)
				{
					MessageBox(hWnd, L"Credits:\nAdam (doadam@gmail.com) for writing this application."
						,L"Credits", MB_OK);
				}
				else if(LOWORD(wParam) == IDC_COMBO1)
				{
					if(HIWORD(wParam) == CBN_SELCHANGE)
					{
						int index = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
						//index == 0 => crasher
						//index == 1 => protector
						wchar_t PathToDLL[MAX_PATH];
						GetCurrentDirectory(MAX_PATH, PathToDLL);
						wchar_t FullPath[MAX_PATH];
						if(index == 0)
							wsprintf(FullPath, L"%s\\senderinjection.dll", PathToDLL);
						else if(index == 1)
							wsprintf(FullPath, L"%s\\crashprotector.dll", PathToDLL);
						else if(index == 2)
							wsprintf(FullPath, L"%s\\msnspammer.dll", PathToDLL);
						else
							wsprintf(FullPath, L"%s\\msnhack.dll", PathToDLL);
							
						HWND hInjectButton = ::GetDlgItem(hWnd, IDC_BUTTON1);
						if(hInjectButton)
						{
							if(PathFileExists(FullPath))
								EnableWindow(hInjectButton, true);
							else
								EnableWindow(hInjectButton, false);
						}
					}
				}
            break;

		}
	return 0;
}