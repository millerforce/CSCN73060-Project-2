
#include <iostream>

#include "ServerController.h"
#include "PacketHandler.h"
#include "utils.h"
#include "TelemetryWriter.h"

#define CLIENT_TIMEOUT 5

static void clientFunc(ClientSocket& socket) {
	TelemetryWriter writer;

	socket.setSocketTimeout(CLIENT_TIMEOUT);

	while (socket.isConnected()) {
		std::optional<SocketBuffer> buffer = socket.receive(26);

		if (buffer.has_value()) {
			std::optional<Packet> pkt = PacketHandler::deserialize(buffer.value());

			if (pkt.has_value()) {
				#ifdef _DEBUG
					std::cout << "Packet received" << std::endl;
				#endif // DEBUG
				
				writer.processPacket(pkt.value());
			}
			else {
				std::cerr << "Failed to parse packet" << std::endl;
			}
		}
		else {
			std::cerr << "Failed to receive from client" << std::endl;
			std::cerr << "Error: " << WSAGetLastError() << std::endl;
		}
	}

	bool result = writer.close();

	if (result) std::cout << "Client flight ended successfully" << std::endl;
	else std::cerr << "Client flight ended unsuccessfully" << std::endl;

	return;
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