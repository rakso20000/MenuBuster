using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace MenuBuster {
	
	[Flags]
	enum ProcessAccessFlags : uint {
		All = 0x001F0FFF,
		Terminate = 0x00000001,
		CreateThread = 0x00000002,
		VirtualMemoryOperation = 0x00000008,
		VirtualMemoryRead = 0x00000010,
		VirtualMemoryWrite = 0x00000020,
		DuplicateHandle = 0x00000040,
		CreateProcess = 0x000000080,
		SetQuota = 0x00000100,
		SetInformation = 0x00000200,
		QueryInformation = 0x00000400,
		QueryLimitedInformation = 0x00001000,
		Synchronize = 0x00100000
	}
	
	[Flags]
	enum AllocationType
	{
		 Commit = 0x1000,
		 Reserve = 0x2000,
		 Decommit = 0x4000,
		 Release = 0x8000,
		Reset = 0x80000,
		Physical = 0x400000,
		TopDown = 0x100000,
		WriteWatch = 0x200000,
		LargePages = 0x20000000
	}
	
	[Flags]
	enum MemoryProtection {
		Execute = 0x10,
		ExecuteRead = 0x20,
		ExecuteReadWrite = 0x40,
		ExecuteWriteCopy = 0x80,
		NoAccess = 0x01,
		ReadOnly = 0x02,
		ReadWrite = 0x04,
		WriteCopy = 0x08,
		GuardModifierflag = 0x100,
		NoCacheModifierflag = 0x200,
		WriteCombineModifierflag = 0x400
	}
	
	class DLLInjector {
		
		[DllImport("kernel32.dll", SetLastError=true)]
		[return: MarshalAs(UnmanagedType.Bool)]
		static extern bool CloseHandle(IntPtr hObject);
		
		[DllImport("kernel32.dll", SetLastError = true)]
		public static extern IntPtr OpenProcess(
			ProcessAccessFlags processAccess,
			bool bInheritHandle,
			int processId
		);
		
		[DllImport("kernel32.dll", SetLastError = true)]
		public static extern bool WriteProcessMemory(
			IntPtr hProcess,
			IntPtr lpBaseAddress,
			byte[] lpBuffer,
			Int32 nSize,
			out IntPtr lpNumberOfBytesWritten);
		
		[DllImport("kernel32.dll")]
		static extern IntPtr CreateRemoteThread(IntPtr hProcess,
			IntPtr lpThreadAttributes, uint dwStackSize, IntPtr
			lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);
		
		[DllImport("kernel32.dll", SetLastError=true, ExactSpelling=true)]
		static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress,
			uint dwSize, AllocationType flAllocationType, MemoryProtection flProtect);
		
		[DllImport("kernel32.dll", CharSet=CharSet.Ansi, ExactSpelling=true, SetLastError=true)]
		static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
		
		[DllImport("kernel32.dll", CharSet=CharSet.Auto)]
		public static extern IntPtr GetModuleHandle(string lpModuleName);
		
		private static Process GetProcess(string processName) {
			
			Process[] processes = Process.GetProcesses();
			
			foreach (Process process in processes) {
				
				if (process.ProcessName == processName)
					return process;
				
			}
			
			return null;
		}
		
		public static bool InjectDLL(string processName, string path) {
			
			Process process = GetProcess(processName);
			
			if (process == null)
				return false;
			
			IntPtr processHandle = OpenProcess(
				ProcessAccessFlags.CreateThread | ProcessAccessFlags.VirtualMemoryWrite | ProcessAccessFlags.VirtualMemoryOperation,
				false, process.Id);
			
			if (processHandle.ToInt32() == -1)
				return false;
			
			byte[] pathASCII = Encoding.ASCII.GetBytes(path);
			IntPtr allocatedMemory = VirtualAllocEx(processHandle, new IntPtr(0), (uint) pathASCII.Length, AllocationType.Commit, MemoryProtection.ReadWrite);
			
			if (allocatedMemory.ToInt32() == 0)
				return false;
			
			IntPtr bytesWritten = new IntPtr(0);
			if (!WriteProcessMemory(processHandle, allocatedMemory, pathASCII, pathASCII.Length, out bytesWritten))
				return false;
			
			IntPtr kernel32 = GetModuleHandle("kernel32.dll");
			IntPtr loadLibraryA = GetProcAddress(kernel32, "LoadLibraryA");
			
			if (CreateRemoteThread(processHandle, new IntPtr(0), 0, loadLibraryA, allocatedMemory, 0, new IntPtr(0)).ToInt32() == 0)
				return false;
			
			return true;
			
		}
		
	}
	
}