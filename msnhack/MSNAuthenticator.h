#include "Define.h"

#ifndef MSN_AUTHENTICATOR_H
#define MSN_AUTHENTICATOR_H

using namespace std;
#define HOTMAIL_HOST				"messenger.hotmail.com"

extern void TextMe(const char* a);
extern vector<string> explode( const char* delimiter, const string &str);

class MSN_Authentication
{
public:
	MSN_Authentication(string user) : strUser(user)
	{
		host = gethostbyname(HOTMAIL_HOST);
		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		memset(&dest_addr,0,sizeof(dest_addr));
		memcpy(&(dest_addr.sin_addr),host->h_addr,host->h_length);
		dest_addr.sin_family= host->h_addrtype;  
		dest_addr.sin_port= htons(1863); 
		EstablishConnection();
	};
	~MSN_Authentication()
	{
		TerminateThread(LockThread, 1);
		closesocket(Socket);
	}

	SOCKET GetSocket() const { return Socket; }
	void LockMSN();
private:
		bool EstablishConnection()
	{
		if (connect(Socket, (struct sockaddr *)&dest_addr,sizeof(dest_addr)) == -1){
			TextMe("Connection Failed!");
        return false;
        }
		TextMe("Connected!");
		    #define bufsize 300
		Rec_Buf = (char*) malloc(bufsize+1);
	};
	std::string GetChallengeString(string ns_ip, int ns_port, string strUsername, string strPassword);
	std::string GetTicket(string ChallengeString, string strUsername, string strPassword);
	    char *Rec_Buf;
        //string strMessage;
        string strVersion;
        string strCVR;
		string strUser;
        string strUsr;
        string strNSAddress;
        string strNSPort;
		struct hostent* host;
		struct sockaddr_in dest_addr;  
        SOCKET Socket;
		char line[1000];
		HANDLE LockThread;
};

#endif