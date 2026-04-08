
#include <iostream>
#include "ServerSocket.h"

int main(void) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Error occured when starting Winsock" << std::endl;
	}



	WSACleanup();
}