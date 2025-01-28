#pragma once
#include "Memory.h"

const int LOCAL_PLAYER_PTR = 0x0017E0A8;
const int HEALTH = 0xEC;
const int ARMOR = 0xF0;

DWORD localPlayer;

int main() { // example for assault cube 1.3.0.2
	Memory mem("ac_client.exe");

	mem.read<DWORD>(mem.getBase() + LOCAL_PLAYER_PTR, &localPlayer);

	while(true) {
		mem.write<DWORD>(localPlayer + HEALTH, 9999);
		std::cout << "Armor: " << mem.read<int>(localPlayer + ARMOR) << "\n";
	}

	return 0;
}