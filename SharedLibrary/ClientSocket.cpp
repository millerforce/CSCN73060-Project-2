
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
}

SocketBuffer ClientSocket::receive(size_t size) const {
	std::vector<char> buffer(size);

	// Attempt to read provided max number of bytes
	int bytesRead = recv(connectionSocket, buffer.data(), size, 0);

	// If nothing was read then return a null optional
	if (bytesRead <= 0) return std::nullopt;

	buffer.resize(bytesRead); // Resize to the number of bytes read
	return buffer;
}

int ClientSocket::send(const SocketBuffer& data) const {
	if (!data) return 0;

	const auto& buffer = *data;

	return ::send(connectionSocket, buffer.data(), static_cast<int>(buffer.size()), 0);
}

void ClientSocket::close() {
	if (connectionSocket != INVALID_SOCKET) {
		closesocket(connectionSocket);
		connectionSocket = INVALID_SOCKET;
	}
}

bool ClientSocket::isConnected() const {
	return connectionSocket != INVALID_SOCKET;
}