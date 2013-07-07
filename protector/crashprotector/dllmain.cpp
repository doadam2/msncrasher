// dllmain.cpp : Defines the entry point for the DLL application.
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#pragma comment (lib, "detours")
#include <detours.h>
#include <map>
#include <sstream>
#include <vector>
#include <time.h>

using namespace std;
//#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma comment (lib, "comctl32")
#pragma warning(disable:4996)

#define TEXT_STATUS_CONST 0
#define TEXT_STATUS		  1
#define TEXT_CRASHER	  2
#define TEXT_BUTTON		  3

//#define EXPIRE
#define EXPIRE_ON		1265471115 + (60 * 60 * 24 * 2)
#define CURRENT_TIME	time(NULL)

//#define WRITTEN_FOR_SPECIFIC_MSN
#ifdef WRITTEN_FOR_SPECIFIC_MSN
#define ACTIVE_ON_MSN						"nitzan@012.net.il"
#define LACTIVE_ON_MSN						L"nitzan@012.net.il"
#endif
DWORD LoggedAs = (DWORD)GetModuleHandle(L"uccapi.dll") + 0x3F4366;
//typedef pair<int, int> SocketRecord;
const wchar_t ClassName[] = L"MSNSpammer", *Classes[] = { L"Static", L"Static", L"Static", L"Button" }, *Texts[] = { 
L"Status:", L"Not Protecting...", L"Info", L"Start Protection" };
const int Styles[] = { 0, 0, 0, 0 };
const int Ycoords[] = { 10, 30, 50, 90 };

int (WINAPI *pRecv)(SOCKET s, char* buf, int len, int flags) = recv;
int WINAPI BypassRecv(SOCKET s, char* buf, int len, int flags);
int (WINAPI *pWSARecv)(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = NULL;
int WINAPI MyWSARecv(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

bool Protecting = false;
int LastSentSocket = 0;
std::string TextMsg;
HWND hwnd;
DWORD Address = (DWORD)GetProcAddress(GetModuleHandle(L"ws2_32.dll"), "recv");
DWORD dwOldProtection;
unsigned char cOldAddress;
DWORD dwOldAddress;
HMODULE Module;

void Exit()
{
	UnregisterClass(ClassName, Module);
	FreeLibraryAndExitThread(Module, 0);
}

void ExitWithError()
{
	MessageBoxA(0, "Error!", "Error!", MB_ICONEXCLAMATION);
	Exit();
}

std::wstring s2ws(const std::string &s);

#ifdef WRITTEN_FOR_SPECIFIC_MSN
bool CheckMSNAuthentication()
{
		if(int Length = strlen(ACTIVE_ON_MSN))
		{
			SIZE_T BytesRead;
			char NickBuffer[50];		//50 should be enough
			char Buffer;
			DWORD dwOld;
			VirtualProtect((LPVOID)LoggedAs, sizeof(ACTIVE_ON_MSN)-1, PAGE_EXECUTE_READWRITE, &dwOld);
//			VirtualProtect((LPVOID)LoggedAs, 50, PAGE_EXECUTE_READWRITE, &dwOld);
/*			int offset = 0;
			do
			{
				ReadProcessMemory(GetCurrentProcess(), (LPCVOID)( LoggedAs + (offset++)), (void*)Buffer, 1, &BytesRead);
				Nick += Buffer;
			}
			while(Buffer != '\0');*/
			ReadProcessMemory(GetCurrentProcess(), (LPCVOID) LoggedAs, (void*)NickBuffer, sizeof(ACTIVE_ON_MSN)-1, &BytesRead);
			std::string Nick = NickBuffer;
			if(!strstr(Nick.c_str(), ACTIVE_ON_MSN))
			{
				wchar_t Msg[200];
				wsprintf(Msg, L"You are logged as %s, written for %s [%d]", s2ws(Nick).c_str(), LACTIVE_ON_MSN, sizeof(ACTIVE_ON_MSN)-1);
				MessageBox(NULL, Msg, L"Error Activation!", NULL);
				return false;
			}
			else
			{
				wchar_t Msg[200];
				wsprintf(Msg, L"You are logged as %s, confirmed!", s2ws(Nick).c_str());
				MessageBox(NULL, Msg, L"Activation Notification", NULL);
				return true;
			}
		}
		return true;
}
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_DESTROY:
		//Remove hook to prevent msn from crashing.			
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		//attach? or detach?
		//DetourAttach(&(PVOID&)BypassRecv,pRecv);
		//DetourAttach(&(PVOID&)MyWSARecv, pWSARecv);
		DetourDetach(&(PVOID&)pRecv,	BypassRecv);
		DetourDetach(&(PVOID&)pWSARecv, MyWSARecv);
		DetourTransactionCommit();
		PostQuitMessage(0);
		Exit();
		break;
	case WM_COMMAND:
		if(wParam == TEXT_BUTTON)
		{
			if(Protecting)
			{
				Protecting = false;
				SetDlgItemText(hwnd, TEXT_BUTTON, L"Start Protection");
				SetDlgItemText(hwnd, TEXT_STATUS, L"Not Protecting...");
			}
			else
			{
				SetDlgItemText(hwnd, TEXT_BUTTON, L"Stop Protection");
				SetDlgItemText(hwnd, TEXT_STATUS, L"Protecting");
				Protecting = true;
			}
		}
		break;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int WINAPI BypassRecv( IN SOCKET s, char FAR * buf, IN int len, IN int flags )
{	
	return pRecv(s, buf, len, flags);
	//MessageBox(hwnd, Msg, L"Recv", NULL);
	//::closesocket(Socket);
}

vector<string> explode( const string &delimiter, const string &str)
{
    vector<string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng==0)
        return arr;//no change

    int i=0;
    int k=0;
    while( i<strleng )
    {
        int j=0;
        while (i+j<strleng && j<delleng && str[i+j]==delimiter[j])
            j++;
        if (j==delleng)//found delimiter
        {
            arr.push_back(  str.substr(k, i-k) );
            i+=delleng;
            k=i;
        }
        else
        {
            i++;
        }
    }
    arr.push_back(  str.substr(k, i-k) );
    return arr;
}

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

int WINAPI MyWSARecv(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	if(Protecting)
	{
		if(lpBuffers->len > 5)
		{
			if(strstr(lpBuffers->buf, "MSG ") != 0 && (strstr(lpBuffers->buf, "MIME-Version") != 0
				&& strstr(lpBuffers->buf, "X-MMS-IM-Format") != 0))
			{
				
				FILE* pRecvLogFile;
				fopen_s(&pRecvLogFile, "E:\\WRecvLog.txt", "a+");
				fprintf(pRecvLogFile, "Socket: %u, Length: %d, Buffer: %s\n\n\nE-N-D=========", socket, lpBuffers->len, lpBuffers->buf);
				fclose(pRecvLogFile);


				std::string CrashMsg = ":'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(";

				if(strstr(lpBuffers->buf, CrashMsg.c_str()))
				{/*
					std::string in = "";
					in += lpBuffers->buf;
					wchar_t Msg[400];
					wsprintfW(Msg, L"Someone tried to crash you!", explode(in).c_str());*/
					vector<string> v = explode(" ", lpBuffers->buf);
					std::string Crasher = v[2];
					wchar_t Msg[200];
					wsprintf(Msg, L"%s tried to crash you!", s2ws(Crasher).c_str());
					SetDlgItemText(hwnd, 2, Msg);
					SetLastError(WSAENOTSOCK);
					return SOCKET_ERROR;
				}
				else
					SetDlgItemText(hwnd, 2, L"");
					
			}
		}
	}
	return pWSARecv(socket, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd,
		lpFlags, lpOverlapped, lpCompletionRoutine);
}

/*
__declspec(naked) void HookRecv()
{
	__asm
	{
		push ebp
			mov ebp,esp
			mov eax,dword ptr ss:[ebp+14]		//Receive Flags
		push eax
			mov ecx,dword ptr ss:[ebp+10]		//Packet Length
		push ecx
			mov edi,dword ptr ss:[ebp+0xC]		//Buffer
		push edi
			mov edx,dword ptr ss:[ebp+8]		//Socket ID
		push edx
			call dword ptr ds:[HandleIncomingMessage]
			//retn
			jmp End
End:
			jmp dword ptr ds:[Address]
	}
}
*/
void Window()
{
	MSG Msg;
	WNDCLASSEX wcx;
	INITCOMMONCONTROLSEX iccex; //XP Manifest style.

	iccex.dwICC = ICC_STANDARD_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	
	ZeroMemory(&Msg, sizeof(MSG));
	ZeroMemory(&wcx, sizeof(WNDCLASSEX));

	wcx.cbSize           = sizeof(WNDCLASSEX);
	wcx.lpfnWndProc      = (WNDPROC)WndProc;
	wcx.hInstance        = Module;
	wcx.hCursor          = (HCURSOR)LoadImage(0,IDC_ARROW,IMAGE_CURSOR,0,0,LR_SHARED);
	wcx.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
	wcx.lpszClassName    = ClassName;

	if (!RegisterClassEx(&wcx))
		ExitWithError();

	hwnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_APPWINDOW ,
        ClassName,
        L"Endlesskiss's MSN Protector",
		WS_SYSMENU,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		270,		//200
		170,
        NULL,
        NULL,
        Module,
        NULL);

	ShowWindow(hwnd, SW_SHOWNORMAL);

	if (!InitCommonControlsEx(&iccex) || !hwnd)
		ExitWithError();

	HFONT Font = CreateFont(0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Comic Sans MS");
	for (int i = 0; i < 4; ++i)
	{
		SendMessage(CreateWindowEx(0, Classes[i], Texts[i], WS_VISIBLE | WS_CHILD | Styles[i], 60, Ycoords[i],(i == 2 ? 250 : 150), (i ? 25 : 
			120), hwnd, (HMENU)i, 0, 0), WM_SETFONT, (WPARAM)Font, TRUE);
	}

	UpdateWindow(hwnd);

		while (GetMessage(&Msg, 0, 0, 0) > 0)
    {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
    }

	Exit();
}

string narrow( const wstring& str )
{
ostringstream stm ;
const ctype<char>& ctfacet =
use_facet< ctype<char> >( stm.getloc() ) ;
for( size_t i=0 ; i<str.size() ; ++i )
stm << ctfacet.narrow( str[i], 0 ) ;
return stm.str() ;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{

	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
#ifdef EXPIRE
		if(EXPIRE_ON < CURRENT_TIME)
		{
			MessageBox(NULL, L"Protection Expired!", L"Activation System", NULL);
			FreeLibraryAndExitThread(hModule, 0);
			return TRUE;
		}
#endif
#ifdef WRITTEN_FOR_SPECIFIC_MSN
		if(!CheckMSNAuthentication())
		{
			FreeLibraryAndExitThread(hModule, 0);
			return TRUE;
		}
#endif
		Module = hModule;
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)pRecv, BypassRecv);
		pWSARecv = (int (WINAPI *)(SOCKET, LPWSABUF, DWORD,
			LPDWORD, LPDWORD, LPWSAOVERLAPPED,  LPWSAOVERLAPPED_COMPLETION_ROUTINE))
			DetourFindFunction("Ws2_32.dll", "WSARecv");
		DetourAttach(&(PVOID&)pWSARecv, MyWSARecv);
		if(DetourTransactionCommit() == NO_ERROR)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Window, (LPVOID)NULL, 0, 0);
		}
		else
		{
			Exit();
		}
	}
	return TRUE;
}
