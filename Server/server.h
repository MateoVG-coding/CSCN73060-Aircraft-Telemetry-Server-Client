#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <map>
#include <fstream>
#include <chrono>
#include <string>
#include "../Client/Packet.h"

#pragma comment(lib, "ws2_32.lib")

void handleClient(SOCKET);

// Data structure that stores fuel consumption data
struct PlaneData {
    float totalFuelConsumption;
    int flights;

    PlaneData() : totalFuelConsumption(0.0f), flights(0) {}
};

// Define a map to store PlaneData objects using the plane ID
extern std::map<std::string, PlaneData> planeDataMap;

//Prototypes
void writePlaneData(const std::map<std::string, PlaneData>& planeDataMap);
void loadPlaneData(std::map<std::string, PlaneData>& planeDataMap);
float calculateFuelConsumption(char* previousFuel, char* currentFuel, float elapsedTime);