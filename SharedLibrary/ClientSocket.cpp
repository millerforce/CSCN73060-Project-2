
#include "ClientSocket.h"
#include "InvalidConnectionState.h"

ClientSocket::ClientSocket(const std::string& IPAddr, int port) {
	connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectionSocket == INVALID_SOCKET) {
		throw InvalidConnectionStateException("Error occured when creating client socket");
	}

	sockaddr_in srvAddr{};
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(port);

	inet_pton(AF_INET, IPAddr.c_str(), &srvAddr.sin_addr);

	if (connect(connectionSocket, reinterpret_cast<sockaddr*>(&srvAddr), sizeof(srvAddr))) {
		closesocket(connectionSocket);
		throw InvalidConnectionStateException("Error occured, unable to connect to server");
	}

	connected = true;
}

bool ClientSocket::receive(char* buffer, int size) {
	// Attempt to read provided max number of bytes
	int bytesRead = recv(connectionSocket, buffer, size, 0);

	// If nothing was read then return a null optional and set connected to false
	if (bytesRead <= 0) {
		if (bytesRead < 0)
			std::osyncstream(std::cout) << "ClientSocket read " << bytesRead << " bytes indicating unexpected client disconnect" << std::endl;
		else
			std::osyncstream(std::cout) << "ClientSocket read " << bytesRead << " indicating graceful client disconnection" << std::endl;

		connected = false;
		return false;
	}

	return true;
}

int ClientSocket::send(const SocketBuffer& buffer) const {
	return ::send(connectionSocket, buffer.data(), static_cast<int>(buffer.size()), 0);
}

void ClientSocket::close() {
	if (connectionSocket != INVALID_SOCKET) {
		closesocket(connectionSocket);
		connectionSocket = INVALID_SOCKET;
	}
}

bool ClientSocket::isConnected() const {
	return connected;
}

bool ClientSocket::setSocketTimeout(int timeoutSeconds) const {
	if (connectionSocket == INVALID_SOCKET || timeoutSeconds < 0) return false;

	int timeoutMs = timeoutSeconds * 1000;

	return setsockopt(connectionSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutMs), sizeof(timeoutMs)) == 0;
}