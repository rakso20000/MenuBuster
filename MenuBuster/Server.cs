using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace MenuBuster {
	class Server {
		
		private static bool isRunning = false;
		private static bool newData = false;
		private static byte inSong = 0;
		private static byte shouldExit = 0;
		
		public static void ServerLoop() {
			
			byte[] addr = {127, 0, 0, 1};
			
			TcpListener server = new TcpListener(IPAddress.Any, 12616);
			
			server.Start();
			
			isRunning = true;
			
			TcpClient client = server.AcceptTcpClient();
			
			NetworkStream ns = client.GetStream();
			
			while (true) {
				
				while (!newData)
					Thread.Sleep(5);
				
				byte[] msg = {inSong, shouldExit};
				
				ns.Write(msg, 0, msg.Length);
				
				newData = false;
				
				if (shouldExit == 1)
					break;
				
			}
			
			isRunning = false;
			
		}

		public static void SetInSong(bool b) {
			
			while (newData);
			
			if (b)
				inSong = 1;
			else
				inSong = 0;
			
			newData = true;
			
		}
		
		public static void Exit() {
			
			while (newData);
			
			shouldExit = 1;
			newData = true;
			
			while (isRunning);
			
		}
		
		public static void StartServer() {
			
			ThreadStart threadRef = new ThreadStart(ServerLoop);
			Thread thread = new Thread(threadRef);
			thread.Start();
			
			while (!isRunning);
			
		}
		
	}
}