#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <map>
#include "../Client/Packet.h"

#pragma comment(lib, "ws2_32.lib")

void handleClient(SOCKET);

float calculateFuelConsumption(char* previousFuel, char* currentFuel, float elapsedTime);