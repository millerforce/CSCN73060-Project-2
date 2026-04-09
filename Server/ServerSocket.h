#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <functional>
#include <vector>
#include <atomic>
#include <thread>

#include "ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")

// Alias for a function that will be passed to all incoming client threads
using ClientHandler = std::function<void(ClientSocket&)>;

class ServerSocket {
private:
	int port = -1; // Port to server will listen on
	SOCKET listenSocket; // Socket server opens for clients to connect to
	sockaddr_in serverAddress;
	ClientHandler handler;
	std::atomic<bool> isRunning; // Used to moderate the accept loop within the acceptThread
	std::thread acceptThread; // Seperate thread used to listen and accept new clients
	std::vector<std::thread> clientThreads; // A collection of all the active client connections/threads

	void acceptLoop(); // Method to be run within the acceptThread

public:
	ServerSocket(int port);
	~ServerSocket();
};