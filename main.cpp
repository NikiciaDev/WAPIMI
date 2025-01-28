#pragma once
#include "Memory.h"

int main() {
	Memory mem("Notepad.exe");
	std::cout << mem.pid << "\n";
	return 1;
}