#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

void handleClient(SOCKET);