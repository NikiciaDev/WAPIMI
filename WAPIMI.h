#pragma once
#include <string>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);
typedef NTSTATUS(WINAPI* pNtWriteVirtualMemory)(HANDLE Processhandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);

class WAPIMI {
private:
	pNtReadVirtualMemory MRead;
	pNtWriteVirtualMemory MWrite;
	std::string processName;
	DWORD pid;
	HANDLE handle;
	uintptr_t baseAdress;

public:
	WAPIMI(std::string processName) : processName(processName) {
		MRead = (pNtReadVirtualMemory) GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
		MWrite = (pNtWriteVirtualMemory) GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");
		setPH();
		setBase();
	}

	template<typename T> T read(uintptr_t address) {
		T buffer{};
		MRead(handle, (void*) address, &buffer, sizeof(T), NULL);
		return buffer;
	}

	template<typename T> void read(uintptr_t address, T* value) {
		MRead(handle, (void*) address, (void*) value, sizeof(T), NULL);
	}

	void read(uintptr_t address, void* buffer, size_t size) {
		MRead(handle, (void*) address, buffer, size, NULL);
	}

	template <typename T> void write(uintptr_t address, T value) {
		MWrite(handle, (void*) address, &value, sizeof(T), NULL);
	}

	void write(uintptr_t address, void* value, size_t size) {
		MWrite(handle, (void*) address, value, size, NULL);
	}

	std::string getProcessName() const {
		return processName;
	}

	DWORD getPID() const {
		return pid;
	}

	HANDLE getHandle() const {
		return handle;
	}

	DWORD getBase() const {
		return baseAdress;
	}

	~WAPIMI() {
		if(handle) CloseHandle(handle);
	}

private:
	void setPH() {
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if(INVALID_HANDLE_VALUE == hSnapshot) {
			std::cerr << "Failed to create snapshot! Error code: " << GetLastError() << std::endl;
		} else {
			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if(Process32First(hSnapshot, &pe32)) {
				do {
					if(std::wstring(pe32.szExeFile) == std::wstring(processName.begin(), processName.end())) {
						pid = pe32.th32ProcessID;
						handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
						break;
					}
				} while(Process32Next(hSnapshot, &pe32));
			} else {
				std::cerr << "Failed to enumerate processes! Error code: " << GetLastError() << std::endl;
			}
		}

		if(hSnapshot) CloseHandle(hSnapshot);
	}

	void setBase() {
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
		if(INVALID_HANDLE_VALUE == hSnapshot) {
			std::cerr << "Failed to create snapshot! Error code: " << GetLastError() << std::endl;
		} else {
			MODULEENTRY32 me;
			me.dwSize = sizeof(me);

			if(Module32First(hSnapshot, &me)) {
				do {
					if(std::wstring(me.szModule) == std::wstring(processName.begin(), processName.end())) {
						baseAdress = (uintptr_t) me.modBaseAddr;
						break;
					}
				} while(Module32Next(hSnapshot, &me));
			} else {
				std::cerr << "Failed to enumerate modules! Error code: " << GetLastError() << std::endl;
			}
		}
		if(hSnapshot) CloseHandle(hSnapshot);
	}
};
