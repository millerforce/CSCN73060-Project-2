
#include <iostream>

#include "UUID.h"
#include "PacketHandler.h"
#include "Packet.h"

#include "cli.h"
#include "utils.h"
#include "file_io.h"

int main(int argc, char* argv[]) {
	Args args = parseArgs(argc, argv);
	int port = getPort(args, 9000);
	std::string ipAddress = getIp(args, "10.144.114.53");
	std::string telemDir = getDir(args, "../../Client/data/");
	//std::string telemDir = getDir(args, "./data/");

	std::string telmFile = getRandomTelemFilename(telemDir);
	std::vector<std::string> telemetryData = readFile(telemDir + telmFile);
	//std::vector<std::string> telemetryData = readFile("../../test.txt");

	UUID_T clientId;
	std::cout << "Initalizing Client: " << clientId.toString() << std::endl;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
		return 1;
	}

	std::cout << "Connecting to: " << ipAddress << ":" << port << std::endl;
	ClientSocket socket(ipAddress, port);
	if (socket.isConnected()) {
		std::cout << "Connected to Server" << std::endl;
	}
	else {
		std::cout << "Failed to connect to server" << std::endl;
		WSACleanup();
		return -1;
	}

	while (!telemetryData.empty()) {
		Packet packet = parseTelemLineToPacket(clientId, telemetryData[0]);
		telemetryData.erase(telemetryData.begin());
		int sent = socket.send(PacketHandler::serialize(packet));
		if (sent > 0) {
			std::cout << "Packet Sent: " << packet.clientId.toString() << " " << packet.fuel << " " << packet.dateTime << std::endl;
		}
		else {
			std::cout << "Failed to send packet: " << sent << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
		}
		Sleep(1000);
	}
	std::cout << "All telemtry data sent." << std::endl;

	socket.close();
	WSACleanup();

	std::cout << "Closing..." << std::endl;

	return 0;
}
