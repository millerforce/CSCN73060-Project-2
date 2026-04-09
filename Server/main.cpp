
#include <iostream>
#include "ServerController.h"
#include "PacketHandler.h"

void clientFunc(ClientSocket& socket) {
	while (true) {
		std::optional<SocketBuffer> buffer = socket.receive(26);

		if (buffer.has_value()) {
			std::optional<Packet> pkt = PacketHandler::deserialize(buffer.value());

			if (pkt.has_value()) {
				std::cout << "UUID: " << pkt.value().clientId.toString() << " DateTime: " << pkt.value().dateTime << " Fuel: " << pkt.value().fuel << std::endl;
			}
			else {
				std::cerr << "Failed to parse packet" << std::endl;
			}
		}
		else {
			std::cerr << "Failed to receive from client" << std::endl;
		}
	}
}

int main(void) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Error occured when starting Winsock" << std::endl;
	}

	int port = 9000;

	ServerSocket socket(port);

	ClientHandler handler = clientFunc;

	ServerController controller(socket, handler);

	std::cout << "Starting server on port: " << port << std::endl;
	controller.start();
	std::cout << "Server started" << std::endl;

	controller.getAcceptThread().join();

	WSACleanup();
}