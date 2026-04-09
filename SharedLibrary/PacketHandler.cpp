
#include <bit>

#include "PacketHandler.h"

uint16_t PacketHandler::calculateCRC(SocketBuffer& buffer) {
	if (!buffer.has_value()) return 0; // If optional is null then return 0

	uint16_t total = 0;

	// Loop through each byte and add the number of 1 bits to the total
	for (unsigned char byte : buffer.value()) {
		total += std::popcount(byte);
	}

	return total;
}

template<typename T>
void PacketHandler::appendBytes(std::vector<char>& buffer, const T& value) {
	// Create a pointer to the values memory
	const char* pointer = reinterpret_cast<const char*>(&value);

	// Append to buffer
	buffer.insert(buffer.end(), pointer, pointer + sizeof(T));
}

SocketBuffer PacketHandler::serialize(Packet& packet) {
	// Confirm a fully formed packet has been passed
	if (!packet.validate()) return std::nullopt;

	std::vector<char> buffer; // Buffer to populate

	// Copy UUID
	auto& clientIdBuf = packet.clientId.bytes();
	buffer.insert(buffer.end(), clientIdBuf.begin(), clientIdBuf.end());

	// Copy timestamp
	appendBytes(buffer, packet.dateTime);

	// Copy fuel level
	appendBytes(buffer, packet.fuel);


}