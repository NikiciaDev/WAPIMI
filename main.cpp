#pragma once
#include "WAPIMI.h"

const int LOCAL_PLAYER_PTR = 0x0017E0A8;
const int HEALTH = 0xEC;
const int ARMOR = 0xF0;

DWORD localPlayer;

int main() { // example for assault cube 1.3.0.2
	WAPIMI wapimi("ac_client.exe");

	wapimi.read<DWORD>(wapimi.getBase() + LOCAL_PLAYER_PTR, &localPlayer);

	while(true) {
		wapimi.write<DWORD>(localPlayer + HEALTH, 9999);
		std::cout << "Armor: " << wapimi.read<int>(localPlayer + ARMOR) << "\n";
	}

	return 0;
}