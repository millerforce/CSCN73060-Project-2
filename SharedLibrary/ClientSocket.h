#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <optional>
#include <string>

// Alias for an optional byte buffer to simplify functions
using SocketBuffer = std::optional<std::vector<char>>;

// Abstraction for a TCP socket with methods for sending and receiving data
class ClientSocket {
private:
	SOCKET connectionSocket;

public:
	ClientSocket(SOCKET socket) : connectionSocket(socket) {} // Create object with existing socket
	ClientSocket(const std::string& IPAddr, int port); // Create object with new socket

	SocketBuffer receive(size_t size) const; // Receive provided number of bytes and return data in a SocketBuffer
	int send(const SocketBuffer& data) const; // Send data from provided buffer

	void close(); // Close the contained socket
	bool isConnected() const; // Get whether the socket is currently connected or not
};