using System;
using System.Net.Sockets;
using System.IO;

class Authentication
{
    private string strMessage;
    private string strVersion;
    private string strCVR;
    private string strUsr;
    private string strNSAddress;
    private string strNSPort;
    private NetworkStream networkStream;
    private StreamReader streamReader;
    private StreamWriter streamWriter;

    public Authentication(string strUsername)
    {

        TcpClient msnTcp = new TcpClient();
        try
        {
            msnTcp.Connect("messenger.hotmail.com", 1863);

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
        streamWriter = new StreamWriter(networkStream);
        streamReader = new StreamReader(networkStream);

        streamWriter.WriteLine("VER 1 MSNP8 CVR0");
        streamWriter.Flush();
        strVersion = streamReader.ReadLine();

        streamWriter.WriteLine("CVR 2 0x0409 win 4.10 i386 MSNMSGR 6.2.0208 MSMSGS " + strUsername);
        streamWriter.Flush();
        strCVR = streamReader.ReadLine();

        streamWriter.WriteLine("USR 3 TWN I " + strUsername);
        streamWriter.Flush();
        strUsr = streamReader.ReadLine();
        string[] split_server_response;
        split_server_response = strUsr.Split(' ');
        string[] split_ip_from_port;
        split_ip_from_port = split_server_response[3].Split(':');
        strNSAddress = split_ip_from_port[0];
        strNSPort = split_ip_from_port[1];

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

    public string ReturnNSAddress()
    {
        return strNSAddress;
    }

    public int ReturnNSPort()
    {
        return int.Parse(strNSPort);
    }

}
