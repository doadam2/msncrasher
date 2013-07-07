using System;
using System.Net;
using System.Collections;



class Ticket
{
    private string strMessage;
    private string Msn_Ticket;
    private string AuthenticationInfo;
    private int ChallengeStartPos;
    private int ChallengeStopPos;
    private int subLength;
    private ArrayList PassportUrls;
    private string Request;

    public Ticket(string ChallengeString, string strUsername, string strPassword)
    {
        HttpWebRequest ServerRequest = (HttpWebRequest)WebRequest.Create("https://nexus.passport.com/rdr/pprdr.asp");
        Request = "Headers: ";
        foreach (HttpRequestHeader h in ServerRequest.Headers)
        {
            Request += h.ToString();
            Request += " ";
        }
        HttpWebResponse ServerResponse = (HttpWebResponse)ServerRequest.GetResponse();

        if (ServerResponse.StatusCode == HttpStatusCode.OK)
        {
            PassportUrls = new ArrayList();
            string[] result = ServerResponse.Headers.Get("PassportURLs").Split(',');
            foreach (string s in result)
            {
                PassportUrls.Add(s.Substring(s.IndexOf('=') + 1));
            }

            string uri = "https://" + PassportUrls[1];


            try
            {

                ServerRequest = (HttpWebRequest)HttpWebRequest.Create(uri);
                ServerRequest.AllowAutoRedirect = false;
                ServerRequest.Pipelined = false;
                ServerRequest.KeepAlive = false;
                ServerRequest.ProtocolVersion = new Version(1, 0);

                strUsername = strUsername.Replace("@", "%40");
                ServerRequest.Headers.Add("Authorization", "Passport1.4 OrgVerb=GET,OrgURL=http%3A%2F%2Fmessenger%2Emsn%2Ecom,sign-in=" + strUsername + ",pwd=" + strPassword + "," + ChallengeString + "\n");
                ServerResponse = (HttpWebResponse)ServerRequest.GetResponse();

                AuthenticationInfo = ServerResponse.Headers.Get("Authentication-Info");

                ChallengeStartPos = AuthenticationInfo.IndexOf('\'');
                ChallengeStopPos = AuthenticationInfo.LastIndexOf('\'');
                subLength = ChallengeStopPos - ChallengeStartPos;
                Msn_Ticket = AuthenticationInfo.Substring(ChallengeStartPos + 1, subLength - 1);

            }
            catch (WebException WE)
            {
                strMessage = WE.Message;
            }

        }
    }

    public string ReturnRequest()
    {
        return Request;
    }

    public string ErrorMessage()
    {
        return strMessage;
    }

    public string ReturnTicket()
    {
        return Msn_Ticket;
    }



}

