#pragma once

#include <vector>

#include "Packet.h"
#include "ClientSocket.h"

/* A handler class used for serializing and deserializing data packets.
Currently provides static methods as there is no necessary reason for 
it to store data. This can change if needed. */ 
class PacketHandler {
private:
	static uint16_t calculateCRC(SocketBuffer& buffer); // Take a buffer and return the number of 1s

	template<typename T>
	static void appendBytes(std::vector<char>& buffer, const T& value); // Used to append a provide values binary representation into a vector of bytes

public:
	static SocketBuffer serialize(Packet& packet); // Convert a packet into a buffer
	static Packet deserialize(SocketBuffer& buffer); // Convert an incoming buffer into a packet

	static bool checkCRC(SocketBuffer&, uint16_t crc);
};