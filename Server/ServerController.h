#pragma once

#include "ServerSocket.h"

#include <atomic>
#include <memory>

// Alias for a function that will be passed to all incoming client threads
using ClientHandler = std::function<void(ClientSocket&)>;

struct ClientThread {
	std::thread thread;
	std::shared_ptr<std::atomic<bool>> done;
};

class ServerController {
private:
	ServerSocket& serverSocket;
	ClientHandler& clientHandler;
	std::atomic<bool> isRunning = false; // Used to moderate the accept loop within the acceptThread
	std::thread acceptThread; // Seperate thread used to listen and accept new clients
	std::vector<ClientThread> clientThreads; // A collection of all the active client connections/threads

	void acceptLoop(); // Method to be run within the acceptThread
	void cleanupThreads();
public:
	ServerController(ServerSocket& socket, ClientHandler& handler) : serverSocket(socket), clientHandler(handler) {}
	~ServerController() { stop(); };

	void start();
	void stop();

	std::thread& getAcceptThread();
};