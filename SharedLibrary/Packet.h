#pragma once

#include "UUID.h"

#define PACKET_SIZE 26

// Represents a deserialized data packet that is sent between the client and server
struct Packet {
	UUID_T clientId; // The clients unique identifier
	int dateTime; // The unix timestamp
	float fuel;
	uint16_t crc = 0;

	Packet(UUID_T clientId, int dateTime, float fuel, uint16_t crc) : clientId(clientId), dateTime(dateTime), fuel(fuel), crc(crc) {}
	Packet(UUID_T clientId, int dateTime, float fuel) : clientId(clientId), dateTime(dateTime), fuel(fuel) {}
};