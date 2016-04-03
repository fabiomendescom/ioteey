#include <stdint.h>

#define PACKET_SIZE 32
#define PACKET_HEADER_SIZE 13
#define PACKET_DATA_SIZE (PACKET_SIZE - PACKET_HEADER_SIZE)

#define NOACKPROTOCOL 0
#define MINIACKPROTOCOL 1
#define ACKPROTOCOL 2

struct miniacktransportmessage
{
	    uint32_t sourceaddress;
        uint32_t destinationaddress;
        uint16_t checksumother;
        uint8_t sequencenumber;
        uint8_t acknowledgementnumber;
        uint8_t window;
        uint8_t data[PACKET_DATA_SIZE];
};

class AutuinoTransport
{
	public:
		AutuinoTransport();
		void start();	
		uint32_t resetMACAddress();
		uint32_t setMACAddress(uint32_t newaddress);
		uint32_t getMACAddress();	
		bool miniAckSend(uint32_t to, void* data, uint8_t len);
		void broadcast(const void* data, uint8_t len);
		bool miniAckPacketAvailable(void* data);
		uint8_t receive(void* data);
		void powerDown();		
};
