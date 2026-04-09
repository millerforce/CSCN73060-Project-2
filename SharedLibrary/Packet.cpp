
#include "Packet.h"

bool Packet::validate() const {
	return clientId.toString().length() == 16 && dateTime > 0 && fuel >= 0 && crc > 0;
}