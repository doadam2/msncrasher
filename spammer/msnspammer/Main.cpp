#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <shlwapi.h>
#include <iostream>
#include <commctrl.h>


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
DWORD Address = (DWORD)GetProcAddress(GetModuleHandle(L"ws2_32.dll"), "send");
/* MSN has a check for hooks inside, this is a bypass */
DWORD PresenceIMEP = (DWORD)GetModuleHandle(L"PresenceIM.dll") + 0x63A5C;
HMODULE Module;
HWND hwnd;
bool Lockon = false;
DWORD dwOldProtection;
unsigned char cOldAddress;
DWORD dwOldAddress;

bool bInfinite = false;

const unsigned char OldBytes[3][6] = 
{
	{0x89 ,0x86 ,0x5c ,0x03 ,0x00 ,0x00},
	{0x89, 0x86, 0x60, 0x03, 0x00, 0x00},
	{0xc7, 0x00, 0x01, 0x00, 0x00, 0x00}
};
using namespace std;

void SetSocketID(int Value)
{
	if(!Lockon)
		SetDlgItemInt(hwnd, IDC_SOCKET, Value, 0);
}

__declspec(naked) void Hook()
{

	__asm
	{
		push ebp
			mov ebp,esp
			cmp dword ptr ds:[Lockon],1 //if (Lockon), continue
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

BOOL WINAPI MainDlgProc (HWND hWnd,	// handle to dialog box
						   UINT uMsg,      // message
						   WPARAM wParam,  // first message parameter
						   LPARAM lParam ) // second message parameter
						   ;

void StartWindow()
{
		VirtualProtect((LPVOID)Address, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
	cOldAddress = *(unsigned char*)Address;
	*(unsigned char*)Address = 0xE9;
	dwOldAddress = *(DWORD*)(Address+1);
	*(DWORD*)(Address+1) = ((int)&Hook - Address) - 5;
	Address += 5;

	DialogBoxParam (Module, MAKEINTRESOURCE (IDD_DIALOG1), NULL, MainDlgProc, NULL);
}

void SpamNudge(unsigned int Length)
{
		char Packet[200];
        ZeroMemory(&Packet[0], 200);
        int Socket = GetDlgItemInt(hwnd, IDC_SOCKET, 0, 0); //Get the socket ID
		SendDlgItemMessage(hwnd, NUDGE_PROGRESS, (WPARAM)PBM_SETSTEP , 0, 0);
        for (unsigned int i = 0; i < Length; ++i)
        {
                wsprintfA(Packet, "MSG %d N 69\x0D\x0AMIME-Version: 1.0\x0D\x0A\x43ontent-Type: text/x-msnmsgr-"
"datacast\x0D\x0A\x0D\x0AID: 1\x0D\x0A\x0D\x0A\0", (*(DWORD*)PresenceIMEP)++);
                send(Socket, Packet, strlen(Packet), 0);
                Sleep(5);
				SendDlgItemMessage(hwnd, NUDGE_PROGRESS, PBM_SETSTEP , ((WPARAM)(i/Length)*100), 0);
        }
		SendDlgItemMessage(hwnd, NUDGE_PROGRESS, PBM_SETSTEP , (WPARAM)100, 0);
        MessageBox(hwnd, L"Done!", L"Done!", MB_ICONINFORMATION);
        ExitThread(0);
}

void SendTextMessage(SOCKET session, const char* message);

void SpamMessage(unsigned int Length)
{
	SOCKET s = GetDlgItemInt(hwnd, IDC_SOCKET, 0, 0);
	char Text[300];
	GetDlgItemTextA(hwnd, IDC_TEXT, Text, 300);
	SendDlgItemMessage(hwnd, SPAM_PROGRESS, (WPARAM)PBM_SETSTEP , 0, 0);
	if(s)
	{
		for(unsigned int i = 0; i < Length; ++i)
		{
			SendTextMessage(s, Text);
			Sleep(5);
			SendDlgItemMessage(hwnd, SPAM_PROGRESS, PBM_SETSTEP , ((WPARAM)(i/Length)*100), 0);
		}
	}
	SendDlgItemMessage(hwnd, SPAM_PROGRESS, (WPARAM)PBM_SETSTEP , 100, 0);
	MessageBox(hwnd, L"Done!", L"Done!", MB_ICONINFORMATION);
	ExitThread(0);
}

void SendTextMessage(SOCKET session, const char* message)
{
    char packetSize[8];
    ZeroMemory(packetSize, 8);
	//sprintf(pHeader, "MSG %d N ", (*(DWORD*)PresenceIMEP)++);
    string packetHeader = "MSG 10 N ";
    string packetSettings = "MIME-Version: 1.0\r\nContent-Type: " 
                            "text/plain; charset=UTF-8\r\n";
    packetSettings += "X-MMS-IM-Format: FN=MS%20Shell%20Dlg; " 
                      "EF=; CO=0; CS=0; PF=0\r\n\r\n";
	(*(DWORD*)PresenceIMEP)++;
	std::string packetMessage = message;
    int sizeOfPacket = packetSettings.length() + packetMessage.length();
    _itoa_s(sizeOfPacket, packetSize, 8, 10);
    packetHeader += packetSize;
    packetHeader += "\r\n";
    string fullPacket = packetHeader;
            fullPacket += packetSettings;
            fullPacket += packetMessage;
    send(session, fullPacket.c_str(), fullPacket.length(), 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
					 if(ul_reason_for_call == DLL_PROCESS_ATTACH)
					 {
						 Module = hModule;
						 CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&StartWindow, NULL, NULL, NULL);
					 }
					 return TRUE;
}

void SwitchNudgeStatus(bool Infinite)
{
	//a for could do this but i'm too lazy to arrange them into an array
	if(Infinite)
	{
		const unsigned char NewBypass[6] = {0x90,0x90,0x90,0x90,0x90,0x90};
		SIZE_T BytesRead;
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)0x00368CE8, &NewBypass, 6, &BytesRead);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)0x00368F02, &NewBypass, 6, &BytesRead);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)0x00368E7B, &NewBypass, 6, &BytesRead);
	}
	else
	{
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)0x00368CE8, &OldBytes[0], sizeof(OldBytes[0]), NULL);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)0x00368F02, &OldBytes[1], sizeof(OldBytes[1]), NULL);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)0x00368E7B, &OldBytes[2], sizeof(OldBytes[2]), NULL);
	}
	bInfinite = Infinite;
}

BOOL WINAPI MainDlgProc (HWND hWnd,	// handle to dialog box
						   UINT uMsg,      // message
						   WPARAM wParam,  // first message parameter
						   LPARAM lParam ) // second message parameter
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			hwnd = hWnd;
		}break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_LOCKON:
				Lockon = true;
				break;
			case IDC_REMOVELOCKON:
				Lockon = false;
				SetDlgItemInt(hWnd, IDC_SOCKET, 0, 0);
				break;
			case IDC_BUTTON2:
				{
					if(!bInfinite)
					{
						SetWindowText(::GetDlgItem(hWnd, IDC_BUTTON2), L"Finite Nudge");
						SwitchNudgeStatus(!bInfinite);
					}
					else
					{
						SetWindowText(::GetDlgItem(hWnd, IDC_BUTTON2), L"Infinite Nudge");
						SwitchNudgeStatus(!bInfinite);
					}
				}
				break;
			case IDC_BUTTON3:
				int Loop;
				Loop = GetDlgItemInt(hWnd, IDC_EDIT1, 0, 0);
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&SpamNudge, (LPVOID)Loop, NULL, NULL);
				break;
			case IDC_BUTTON4:
				int MsgLoop = GetDlgItemInt(hWnd, IDC_EDIT3, 0, 0);
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&SpamMessage, (LPVOID)MsgLoop, NULL, NULL);
				break;
			}
		}break;
	case WM_CLOSE:
	case WM_DESTROY:
		{
			Address -= 5;
			*(DWORD*)(Address+1) = dwOldAddress;
			*(unsigned char*)Address = cOldAddress;
			VirtualProtect((LPVOID)Address, 5, dwOldProtection, NULL);
			PostQuitMessage(0);
			wchar_t ClassName[30];
			GetClassName(hWnd, ClassName, 30);
			UnregisterClass(ClassName, Module);
			FreeLibraryAndExitThread(Module, 0);
		}
	}
	return 0;
}