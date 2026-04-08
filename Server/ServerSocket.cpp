
#include <WS2tcpip.h>

#include "ServerSocket.h"
#include "InvalidConnectionState.h"

// Standard constructor to initialize server socket and begin listening for connections
ServerSocket::ServerSocket(int port) {

	// Create socket
	this->listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		throw InvalidConnectionStateException("Error occured when attempting to start listenSocket");
	}

	// Initialize server to listen on provided port for connections from and IP address
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	// Bind listener socket to port
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress))) {
		closesocket(listenSocket);
		throw InvalidConnectionStateException("Error occured when binding listenSocket");
	}

	// Listen on the socket
	if (listen(listenSocket, 1)) {
		closesocket(listenSocket);
		throw InvalidConnectionStateException("Error occured when listening on listenSocket");
	}

	// Set flag so acceptLoop begins accepting client connections
	isRunning = true;

	// Pass private accept loop the be run in new thread
	acceptThread = std::thread(&ServerSocket::acceptLoop, this);
}

ServerSocket::~ServerSocket() {
	isRunning = false; // Stop client connection loop

	// Close listening socket
	if (listenSocket != INVALID_SOCKET) {
		closesocket(listenSocket);
		listenSocket = INVALID_SOCKET;
	}

	if (acceptThread.joinable()) acceptThread.join();

	for (auto& thread : clientThreads)
		if (thread.joinable()) thread.join();
}

void ServerSocket::acceptLoop() {
	while (isRunning) {
		sockaddr_in clientAddress{};
		int addressLength = sizeof(clientAddress);

		SOCKET clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddress), &addressLength);
		if (clientSocket == INVALID_SOCKET) {
			throw InvalidConnectionStateException("Error occured when accepting client connection");
		}

		clientThreads.emplace_back([this, clientSocket]() {
			ClientSocket connection(clientSocket);
			handler(connection);
		});

	}
}