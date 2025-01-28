#pragma once
#include <string>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <cstdint>

typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);
typedef NTSTATUS(WINAPI* pNtWriteVirtualMemory)(HANDLE Processhandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);

class Memory {
private:
	pNtReadVirtualMemory MRead;
	pNtWriteVirtualMemory MWrite;

public:
	std::string processName;
	DWORD pid;
	HANDLE handle;

	Memory(std::string processName) : processName(processName) {
		MRead = (pNtReadVirtualMemory) GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
		MWrite = (pNtWriteVirtualMemory) GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if(INVALID_HANDLE_VALUE == hSnapshot) {
			std::cerr << "Failed to create snapshot! Error code: " << GetLastError() << std::endl;
		}
		
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if(Process32First(hSnapshot, &pe32)) {
			do {
				if(std::wstring(pe32.szExeFile) == std::wstring(processName.begin(), processName.end())) {
					pid = pe32.th32ProcessID;
					handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				}
			} while(Process32Next(hSnapshot, &pe32));
		} else {
			std::cerr << "Failed to enumerate processes! Error code: " << GetLastError() << std::endl;
		}

		if(hSnapshot) CloseHandle(hSnapshot);
	}

	template<typename T> T Read(uintptr_t address) {
		T buffer{};
		MRead(handle, (void*)address, &T, sizeof(T), NULL);
		return T;
	}

	void Read(uintptr_t address, void* buffer, size_t size) {
		MRead(handle, (void*)address, buffer, size, NULL);
	}

	template <typename T> T Write(uintptr_t address, T value) {
		MWrite(handle, (void*)address, &value, sizeof(T), NULL);
	}

	~Memory() {
		if(handle) CloseHandle(handle);
	}
};