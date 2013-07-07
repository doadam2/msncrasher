#include "MSNAuthenticator.h"
#include "Wininet.h"



void MSN_Authentication::LockMSN()
{
	while(true)
	{
			int err;
		strcpy(line, "VER 1 MSNP8 CVR0");
		TextMe(line);
		send(Socket,line,strlen(line),0);
		err=recv(Socket,Rec_Buf,bufsize,0);
		Rec_Buf[err] = '\0';
		strVersion = Rec_Buf;
		//string msg1 = "CVR 2 0x0409 win 4.10 i386 MSNMSGR 6.2.0208 MSMSGS ";
		//msg1 += strUser.c_str(); 
		wsprintfA(line, "CVR 2 0x0409 win 4.10 i386 MSNMSGR 6.2.0208 MSMSGS %s", strUser.c_str());
		//strcpy(line, msg1.c_str());
		TextMe(line);
		//TextMe("nice");
		send(Socket,line,strlen(line),0);
		//TextMe("0");
		err=recv(Socket,Rec_Buf,bufsize,0);
		//TextMe("1");
		Rec_Buf[err] = '\0';
		strCVR = Rec_Buf;
		//msg1 = "USR 3 TWN I ";
		//msg1 += strUser.c_str();
		wsprintfA(line, "USR 3 TWN I %s", strUser.c_str());
		TextMe("we're going to send it...");
		send(Socket,line,strlen(line),0);
		TextMe("1");
		err=recv(Socket,Rec_Buf,bufsize,0);
		TextMe("2");
		Rec_Buf[err] = '\0';
		strUsr = Rec_Buf;
		TextMe("3");
		vector<string> server_response = explode(" ", strUsr);
		TextMe("4");
		/*
		vector<string> split_ip_from_port;
        split_ip_from_port = explode(":", server_response[3]);//.Split(':');
		TextMe("5");
        strNSAddress = split_ip_from_port[0];
        strNSPort = split_ip_from_port[1];
		int p = atoi(strNSPort.c_str());
		GetChallengeString(strNSAddress, p, strUser.c_str(), string("12345"));
		*/
	}
}

std::string MSN_Authentication::GetChallengeString(string ns_ip, int ns_port, string strUsername, string strPassword)
{
	int err;
	strcpy(line, "VER 4 MSNP8 CVR0");
	TextMe(line);
	send(Socket,line,strlen(line),0);
	err=recv(Socket,Rec_Buf,bufsize,0);
	Rec_Buf[err] = '\0';
	strVersion = Rec_Buf;
	string msg1 = "CVR 5 0x0409 win 4.10 i386 MSNMSGR 6.2.0208 MSMSGS ";
	TextMe(line);
	msg1 += strUser.c_str(); 
	strcpy(line, msg1.c_str());
	send(Socket,line,strlen(line),0);
	err=recv(Socket,Rec_Buf,bufsize,0);
	Rec_Buf[err] = '\0';
	strCVR = Rec_Buf;
	msg1 = "USR 6 TWN I ";
	TextMe(line);
	msg1 += strUser.c_str();
	err=recv(Socket,Rec_Buf,bufsize,0);
	Rec_Buf[err] = '\0';
	strUsr = Rec_Buf;
	vector<string> server_response = explode(" ", strUsr);
	return server_response[4];
}

/*
std::string MSN_Authentication::GetTicket(string ChallengeString, string strUsername, string strPassword)
{
	HINTERNET hConnection, hOpen, hData;
	hOpen = InternetOpen(L"MSN-Locker",INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);
	hConnection = InternetConnect(hOpen,L"https://nexus.passport.com/rdr",80,NULL,NULL,INTERNET_SERVICE_HTTP,0,0);
	hData = HttpOpenRequest( hConnection, L"GET", L"/pprdr.asp",NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION,0);
	LPDWORD BytesReceived;
	InternetReadFile(hData, Rec_Buf, bufsize, BytesReceived);

}*/