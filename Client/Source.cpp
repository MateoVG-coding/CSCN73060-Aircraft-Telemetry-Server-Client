#define _CRT_SECURE_NO_WARNINGS

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <fstream>
#include <string>
#include "Packet.h"
#include <string>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iomanip>

// Function to generate a unique ID for the client
std::string generateUniqueID() {
	// Generate a random uppercase letter
	char randomLetter = 'A' + rand() % 26;

	// Generate a timestamp-based component for the ID
	std::time_t currentTime = std::time(nullptr);
	std::tm* localTime = std::localtime(&currentTime);
	std::stringstream timestampStream;
	timestampStream << std::put_time(localTime, "%Y%m%d%H%M%S");

	std::string uniqueID = "AP_" + timestampStream.str() + "_" + randomLetter;
	return uniqueID;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <IP address>" << std::endl;
		return 1;
	}

	char* ipAddress = argv[1];

	std::cout << "The client is looking for a server...";

	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return -1;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		return -1;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr(ipAddress);	//IP address

	std::cout << "The client has connected to a server.";

	std::string InputStr = "";
	std::string uniqueID = generateUniqueID();
	Packet newPkt;

	std::ifstream f("Telem_2023_3_12 16_26_4.txt");
	if (f.is_open())
	{
		while (std::getline(f, InputStr))
		{
			size_t pos = InputStr.find(',');

			std::string timeData = InputStr.substr(0, pos);
			std::string fuelData = InputStr.substr(pos + 1, InputStr.length() - pos - 3);

			// Set id,time, fuel data for the packet
			newPkt.SetData((char*)timeData.c_str(), (char*)fuelData.c_str(), (char*)uniqueID.c_str(), timeData.length(), fuelData.length(), uniqueID.length());

			int Size = 0;
			char* Tx = newPkt.SerializeData(Size);

			sendto(ClientSocket, Tx, Size, 0, (sockaddr*)&SvrAddr, sizeof(sockaddr_in));
		}

		f.close();
	}

	closesocket(ClientSocket);
	WSACleanup();

	std::cout << "The client has closed the connection with the server.";

	system("pause");

	return 1;
}