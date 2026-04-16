
#include "ServerController.h"
#include "InvalidConnectionState.h"

#include <iostream>
#include <syncstream>

void ServerController::start() {
	isRunning = true; // Set loop flag

	// Create accept thread and pass it loop to begin accepting clients
	acceptThread = std::thread(&ServerController::acceptLoop, this);
}

void ServerController::stop() {
	isRunning = false;

	serverSocket.close();

	if (acceptThread.joinable()) acceptThread.join();

	for (auto& client : clientThreads)
		if (client.thread.joinable()) client.thread.join();
}

void ServerController::acceptLoop() {
	while (isRunning) {
		try {
			// Block until a client connects
			ClientSocket client = serverSocket.acceptConnection();

			std::osyncstream(std::cout) << "New client connected" << std::endl;

			auto done = std::make_shared<std::atomic<bool>>(false);

			// Pass client into its own thread
			clientThreads.emplace_back(ClientThread{
				std::thread([this, client = std::move(client), done]() mutable {
				clientHandler(client);
				*done = true;
				}),
				done
			});

			cleanupThreads();
		}
		catch (InvalidConnectionStateException& e) {
			std::cerr << "Exception caught during accept loop:" << e.what() << std::endl;
		}
		
	}
}

void ServerController::cleanupThreads() {
	clientThreads.erase(
		std::remove_if(clientThreads.begin(), clientThreads.end(),
			[](ClientThread& client) {
				if (*client.done) {
					if (client.thread.joinable())
						client.thread.join();
					return true;
				}
				return false;
			}),
		clientThreads.end()
		);
}

std::thread& ServerController::getAcceptThread() {
	return acceptThread;
}