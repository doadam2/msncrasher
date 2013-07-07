using System;
using System.Net.Sockets;
using System.IO;


class Notification
{
    private string strMessage;
    private string strVersion;
    private string strCVR;
    private string strUsr;
    private string strUsr2;
    private string ChallengeString;
    public string ticket;
    private NetworkStream networkStream;
    private StreamReader streamReader;
    private StreamWriter streamWriter;

    public Notification(string ns_ip, int ns_port, string strUsername, string strPassword)
    {
        TcpClient msnTcp = new TcpClient();
        try
        {
            msnTcp.Connect(ns_ip, ns_port);

            if (msnTcp.Connected)
            {
                strMessage = "Connected";
            }
            else
            {
                strMessage = "Not Connected";
            }
        }
        catch (UriFormatException UFE)
        {
            strMessage = UFE.Message;
        }

        networkStream = msnTcp.GetStream();
        streamReader = new StreamReader(networkStream);
        streamWriter = new StreamWriter(networkStream);

        streamWriter.WriteLine("VER 4 MSNP8 CVR0");
        streamWriter.Flush();
        strVersion = streamReader.ReadLine();

        streamWriter.WriteLine("CVR 5 0x0409 win 4.10 i386 MSNMSGR 6.2.0208 MSMSGS " + strUsername);
        streamWriter.Flush();
        strCVR = streamReader.ReadLine();

        streamWriter.WriteLine("USR 6 TWN I " + strUsername);
        streamWriter.Flush();
        strUsr = streamReader.ReadLine();
        string[] split_response = strUsr.Split(' ');
        ChallengeString = split_response[4];

        Ticket TkMsn = new Ticket(ChallengeString, strUsername, strPassword);
        ticket = TkMsn.ReturnRequest();
        streamWriter.WriteLine("USR 7 TWN S " + TkMsn.ReturnTicket());
        streamWriter.Flush();
        strUsr2 = streamReader.ReadLine();
    }

    public string CheckConnection()
    {
        return strMessage;
    }

    public string CheckVersion()
    {
        return strVersion;
    }

    public string CheckCVR()
    {
        return strCVR;
    }

    public string CheckUsr()
    {
        return strUsr;
    }

    public string CheckUsr2()
    {
        return strUsr2;
    }

    public string ReturnChallengeString()
    {
        return ChallengeString;
    }

}

