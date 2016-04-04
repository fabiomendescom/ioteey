#include "AutuinoTransport.h"
#include "AutuinoSigning.h"
#include "AutuinoEncryption.h"
#include "RF24.h"
#include "RF24_config.h"
#include <EEPROM.h>
#include "AutuinoPowerSupply.h"

/*
 
WHY IOTEEY?
* modular. pieces are bought separately
* every component accessible through internet
* No single point of failure. 
* Redundancy is built into the system. No server. You can  
* Self-healing. Anythiung goes down and it will try to fix itself
* Full access to devices from smartphone (through network or bluetooth)
* Secure. 256 bit enryption
* Open standard. New ioteey devices can be created

TRANSCEIVER (ioteey base)
-------------
A transreceiver is the base of ioteey. It is in charge of transmitting and receiving messages to one another through
the radio. It is responsible for network activities, mesh, forward, encryption and signing transmission. 

IOTHING (ioteey)
-------
A doer or sensor
 
DOERS
-----
DOERS DO things. They turn a tv, they turn a light, etc. A doer is mounted on a transceiver. It communicates its commands
to it
They do something
- They can receive commands (turn on, turn off)
- They can send their state (off, on)
- They can send their status (working, battery low, low connection)
- They can send their metadata (type, is network connected, contains password)

SENSORS
-------
A sensor senses things and must be mounted on a transceiver to send its messages
They sense something
- They can receive command (reset)
- They can send their state(temperature level, humidity value, etc)
- They can send their status (woarking, battery low, low connection)
- They can send their metadata (type, is network connected, contains password)

GATEWAYS
--------
Gateways are also mounted on transceivers. They receive commands, status, states, etc and provide a gateway to HTTP. An
HTTP request made to a gateway can make its way to the device

IOTEEY UNIT
-----------
An ioteey device is either a sensor, doer or gateway mounted on a transceiver

SERVERS
-------
Openhab...

NODE
----
An ioteey unit when it is joined to a network

SMARTPHONE
----------
Technically speaking the smartphone is a device through the app. But it only talks to ioteeys through the dungle

IOTEEY DUNGLE
-------------
A bluetooth enabled device that attaches to the top of the transceiver. It talks to the smart phone allowing the phone to
controls its settings, join network, etc

SIZES:

Network id 		= 3 bytes
Node Address 	= 2 bytes
MAC address  	= 9 bytes
Link ID 		= 3 bytes

PROTOCOL: 001 (PROTOCOL_LINK)
==================================================================================
Every node links to one or more nodes. Nodes can only communicate if they are linked

General guidelines
------------------
* A device must first be added to the network to even communicate. A device outside of the network can only
send a broadcast message in search of networks
* A device cannot talk to another device in network without being linked first. Adding a device follows this process:
	- Join network
	- Link to another
* A device can talk to another device through a forward. This means a device that is closer to the other will forward the
packet and response back
* Commands, states and statuses are divided into high level and low level. Low level commands are things directly to the 
hardware, for example, turn pin 9 high, etc. High level commands are things like trun the light on, etc
* Transceivers alone cannot join networks. THey must be attached to a thing

Joining network
---------------
ioteey networks don't have a centralized server. A device joins a network when it happens to know the secret key to that
network and is able to sign messages with the secret key.
The joining happens in 2 different ways:
1. A ioteey dungle is inserted into the device. Dungle is paired to a smart phone where an app can be configured to contain
the secret key to the network. Through the app, the secret key can be transferred to the device.
2. An ioteey can be attached to another physically. THen user presses button on the ioteey that has the key and after
presses the button on the ioteey that receives it. Key is exchanged and green light comes on. This option can be disabled
for security reasons. By default an ioteey has no password, is not joined to a network and allows physical network join as
described. It can be turned off through the app
When an ioteey device gets the key it also assigns to itself a random 2 byte node address. There is no server DHCP. Remember
ioteey networks are decentralized. If node addresses duplicates are handled through the linking process.

Bluetooth data (figure out format details)
- network id (3 bytes)
- network secret key (32 bytes)

The node should store the key in its secured EEPROM. Network id should also be stored

Leaving network
---------------
Leaving the network is simple. Simply delete the secret key and network id and you are done.

Distance Request
-----------------
* Packet type: PCKT_REQ_DIST (Distance Request)
* 
A signed broadcast packet is sent in order to find the distance from a source node to a destination node. Each node that can talk
to the destination responds with two things: 1. a measure of the distance to the destination node, 2. how many nodes it is 
already forwarding (to make an optimal choise). NOTE: only non-battery devices should respond. Battery operated ones sleep,
not reliable for forwarding.
The signature will be some defined one with 16 bytes to fill one packet
* DEFINE WHAT THE SIGNATURE ALGORITHM WILL BE (SHA???)

LINK PACKET SIZE: 32 bytes
Radio address: Broadcast
--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |         9 bytes               |          9 bytes              |       2 bytes                 |
--------------------------------------------------------------------------------------------------------------------------------
|  Protocol |     Packet type   |     Source mac address        | Dest mac address              | Source node address           |
--------------------------------------------------------------------------------------------------------------------------------
|           2 bytes             |           16 bytes            |
--------------------------------------------------------------------------------------------------------------------------------
| Destination node address      | Signature                     |
--------------------------------------------------------------------------------------------------------------------------------

* Packet type: PCKT_RES_DIST (Distance Response)
* The response is not broadcasted. It is a direct response to the node.
* All nodes, if they have communication with the destination node, respond with distance and number of forward
* The signature is only 15 bytes on the response to leavfe one byte for the distance reponse.
* The distance is a formula (DEFINE FORMULA PLEASE!!!!) that will combine a sense of distance to the node and 
* how many nodes it is already forwarding. The math should work in a way to allow an optimal choice of the forwarding node.

LINK PACKET SIZE: 32 bytes
Radio address: same as destination node address
--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |         9 bytes               |          9 bytes              |       2 bytes                 |
--------------------------------------------------------------------------------------------------------------------------------
|  Protocol |     Packet type   |     Source mac address        | Dest mac address              | Source node address           |
--------------------------------------------------------------------------------------------------------------------------------
|           2 bytes             | 15 bytes                      | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
--------------------------------------------------------------------------------------------------------------------------------
| Destination node address      | Signature                     | Distance                      |
--------------------------------------------------------------------------------------------------------------------------------

 
Linking
-------
SENDER
* Packet type: PCKT_REQ_LINK1 and  PCKT_REQ_LINK2 (Link Request)
* Collect all distance responses and pick the smallest one.
* Generate a link id randomly from 0 to 2^3 - 1
* Send packet with your info and your transmission id
* wait for a response from destination
* if response takes more than 800 millis considered failed
* if response check
* 	if positive, save the transmission number received like this (size: 10 bytes)
         SOURCE NODE ADDRESS | LINK ID | FWD= yes or no
* 	if negative consider failed 

When a node receives a link request it should compare the destination address with its own address. If it matches, the packet
is for the node. If it does not match, it should forward the packet to the destination node. Even if it is a forward, then
still log the SOURCE NODE ADDRESS and LINK ID but put a flag for FWD=yes. This will ensure that we don't forward invalid packets
and overload the network. 

LINK REQUEST 

Size: 32
--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |           9 bytes             |           9 bytes             |             2 bytes           |
--------------------------------------------------------------------------------------------------------------------------------
|  Protocol | PCKT_REQ_LINK1    | Source mac address            | Dest mac address              | Source node address           |
--------------------------------------------------------------------------------------------------------------------------------
|           2 bytes             | 3 bytes                       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |  5 bytes
--------------------------------------------------------------------------------------------------------------------------------
| Destination node address      | Link Id                       | E |      SIGNING LENGTH       | partial signing
--------------------------------------------------------------------------------------------------------------------------------

LINK PACKET SIZE: 31 bytes
--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |          3 bytes             |           27 bytes             | 
--------------------------------------------------------------------------------------------------------------------------------
|  Protocol | PCKT_REQ_LINK2    | Link Id                      | rest of signature              |
--------------------------------------------------------------------------------------------------------------------------------



RECEIVER
* Packet type: PCKT_RES_LINK_POS (Link Response Positive) PCKT_RES_LINK_NEG (Link Response Negative)
* Get receipt packet
* Generate a transmission id randomly from 0 to 2^3 -1
* save the transmission address from the destination like this: (size: 10 bytes)
		SOURCE NODE ADDRESS | LINK ID 

LINK PACKET SIZE: 32 bytes
--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |         3 bytes               |          3 bytes              |       2 bytes                 |
--------------------------------------------------------------------------------------------------------------------------------
|  Protocol | PCKT_RES_LINK_POS | Source mac address (first 3)  | Dest mac address (first 3)    | Source node address           |
--------------------------------------------------------------------------------------------------------------------------------
|           2 bytes             | 16 bytes                      | 3 bytes                       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
--------------------------------------------------------------------------------------------------------------------------------
| Destination node address      | Signature                     | Link Id                       |          SIGNING LENGTH       |
--------------------------------------------------------------------------------------------------------------------------------

LINK PACKET SIZE: 32 bytes
--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |         3 bytes               |          3 bytes              |       2 bytes                 |
--------------------------------------------------------------------------------------------------------------------------------
|  Protocol | PCKT_RES_LINK_NEG | Source mac address (first 3)  | Dest mac address (first 3)    | Source node address           |
--------------------------------------------------------------------------------------------------------------------------------
|           2 bytes             | 16 bytes                      | 3 bytes                       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
--------------------------------------------------------------------------------------------------------------------------------
| Destination node address      | Signature                     | Error Code                    |          SIGNING LENGTH       |
--------------------------------------------------------------------------------------------------------------------------------


ENC TYPE = Encryption type
ENC LENGTH = Encryption length

** NOTE: if the destination node address is different than the radio address, this means that the packet is to be forwarded.
The transmission id is still used but the destination should understand that this transmission is a forward


Data exchange
-------------
A data exchange can be high or low level. High level means things such as turn on and off, etc. A low level data exchange
involves sensing the actual pins and knowing their values based on a sampling rate.

SENDER
Packet type: PCKT_REQ_SENDDATA_HIGH (Data exchange starter)
Packet header size: 10 bytes
Rules about transmission: Send the signing key first filling all available packet space. Only when this is done, start
sending the data
HOw do I differentiate between a key and the data?
Rememeber that on the linking process, there is a handshake where it is agreed on the size of the signing key. There is no
need to send this again. The process should count down on the key bytes on all packets. When the key size is exhausted,
the rest of the bytes are data

--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |            2 bytes            |            2 bytes            |            3 bytes            |
--------------------------------------------------------------------------------------------------------------------------------
|  Protocol |     Packet type   |     Source node address       | Destination node address      |            Link Id            |
--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
---------------------------------------------------------------------------------------------------------------------------------
| (1)CUR PACKET | (1)TOT PACKTS | DATA SIZE                     |                                                           |
---------------------------------------------------------------------------------------------------------------------------------
| DATA (see DATA FORMAT below)--->
---------------------------------------------------------------------------------------------------------------------------------

DATA SIZE is only sent on the last packet. It also only represents the remaining data on the packet. It is not needed on the
other packets because we fill the entire packet with key and/or data.


RECEIVER
Packet type: PCKT_RES_SENDDATA_HIGH_POS  (Data exchange receipt positive) PCKT_RES_SENDDATA_HIGH_NEG (Data exchange receipt negative)
Packet header size: 32 bytes
--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |            2 bytes            |          3 bytes              | 24 bytes                      |
--------------------------------------------------------------------------------------------------------------------------------
|  Protocol |     Packet type   |     Source node address       | Transmission id               | Signature                     |
--------------------------------------------------------------------------------------------------------------------------------
| 2 byte                        |
---------------------------------
| Checksum                      |
---------------------------------
* 
You must attach a piece of the signature on each packet. In order to do this, divide the 32 byte signature by the number of 
packets will be sent for the segment. Each piece must be sent on each packet. This is important in order to fail fast. If the
beginning of the signature does not match, receiver will refuse anything subsequent unless a CON bit is set again. Receiver will
not acknowledge either.
You do this for the first 4 packets, since this will exhaust the 
 
DATA FORMAT:

The format of the data once transmitted and signed
- Start by decrypting it first

--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
--------------------------------------------------------------------------------------------------------------------------------
|                               |
|         MESSAGE TYPE          |  MESSAGE DATA --->
|                               |
--------------------------------------------------------------------------------------------------------------------------------


MESSAGE TYPES:
All byte data show below is within the MESSAGE DATA SPACE

Network related
---------------
* Rekey Network
The private key should really be kept secret. However, if it is compromised, rekying your network is an arduous process. The
only way to do it is to individually apply the dungle to each ioteey device with the new network secret key
 
Router related
--------------
* Router table delete request(node address, alias)
Request from router to an origin node to remove a particular alias
* Router table delete request response(status,alias)
Response from origin node that the alias has been removed

Link related
------------
* Link Request
Request a link to another node. A link allows devices to talk to each other. For example, a sensor light switch can
be linked to the light. 
* Link Request Response
Response from destination device if link was successful

Node communication related
--------------------------
* Command Send Request(node address,command, parameters)
Send command a node address (node address must be only actuator type)
Commands are sent low level. 
---------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
---------------------------------------------------------------------------------------------------------------------------------
|                               |                               |                               |                               |
|         MESSAGE TYPE          |          # COMMANDS           | COMMAND TYPE                  |  PIN #                        |
|                               |                               |                               |                               |
---------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
---------------------------------------------------------------------------------------------------------------------------------
|                                                               |                                                                                               |
|       PIN VALUE                                               |    SO ON... 
|                                                               |                                                                                               |
---------------------------------------------------------------------------------------------------------------------------------

PIN # and PIN VALUE can continue repeating 
COMMAND TYPE determines how to parse the rest of the bytes before the next command byte comes. They can be:
* HIGHLEVEL = For example turn on or turn off
* LOWLEVEL = For example turn pin 9 high and pin 7 low

* Command Send Request Response(status)
Reponse if command was successful. This should include detailed status like 1. received command, 2. sent to actuator and 3. 
Successfully executed in actuator (validating that the light actually turn on for example).
---------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
---------------------------------------------------------------------------------------------------------------------------------
|                               |                               |                               |                               
|         MESSAGE TYPE          |          # RESPONSES          | PIN #                         |  PIN VALUE --->                   
|                               |                               |                               |                               
---------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
---------------------------------------------------------------------------------------------------------------------------------
|                               |                                                                                               |
|   PIN VALUE CONTINUED         | SO ON... 
|                               |                                                                                               |
---------------------------------------------------------------------------------------------------------------------------------
The response can be HIGH LEVEL (on, confirmed that it is on, etc) or LOWLEVEL
* If LOWLEVEL then the pin # and state is sent back. 

* State Send Request(node address)
* State Send Response(state details)

* Status Send Request(node address)
* Status Send Request Response(status details)
* Metadata Send Request(node address)
* Metadata Send Response(mac address, node address, battery level, type, subtype, description, network id, password protected,
powersource(BATTERY, OUTLET, SOLAR, BATTERYOUTLET)

DEVICE SIGNAL TYPES
===================
- BI-state (ON OFF)
- Multistate (multiple discrete values)
- Linear Integer (linear values)
- Linear double

DEVICE
======
Switcher - each button is a function. 4 buttons, 4 functions
Relay - 1 function - on and off
CD Relay (DC current instead of relay above on AC110)
Motion Sensor - 1 function - on and off
Door sensor - 1 function - on and off
IR remote control - multiple function. function 1: on and off. Function 2: channel switch, etc..
IR break sensor - function on and off
temperature sensor - function sensor value (maybe rule? if temp > 80 do something else do something else0
humidity sensor - same as temperature
atmospheric pressure - value
gps sensor - function sensor value (coordinates)
tilt sensor - function sensor value
gyroscope sensor - function sensor value
gas sensor - function sensor value
co2 sensor - sensor value
Earthquake sensor - value
camera
power consumption meter - value
NFC - nfc value
Dust sensor - value
Luminance sensor - value
Bluetooth
Water level sensor

--------------------------------------------------------------------------------------------------------------------------------
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |         3 bytes               |          3 bytes              |       2 bytes                 |
--------------------------------------------------------------------------------------------------------------------------------
|   

Source Node Address 2
Notification type 2
Notification parameter count 1
Notification parameter name 3
Notification parameter type 1
Notification parameter size 1
Notification parameter value 5

 

*/

#define WRITE_PIPE ((uint8_t)0)
#define CURRENT_NODE_PIPE ((uint8_t)1)
#define BROADCAST_PIPE ((uint8_t)2)
#define GATEWAY_ADDRESS ((uint8_t)0)
#define BROADCAST_ADDRESS ((uint8_t)0xFF)
#define MAIN_ADDRESS ((uint8_t)0x2D)

#define NETWORK_SECRET_SIZE 32
#define NETWORK_ID_SIZE 3
#define NETWORK_ID_SIZE64 8
#define NODE_ADDRESS_SIZE 2
#define MAC_ADDRESS_SIZE 9

#define EEPROM_START_MAC 0
#define EEPROM_START_NODE (EEPROM_START_MAC + MAC_ADDRESS_SIZE) 
#define EEPROM_START_NETID (EEPROM_START_NODE + NODE_ADDRESS_SIZE)
#define EEPROM_START_NETSECRET (EEPROM_START_NETID + NETWORK_ID_SIZE64)

#define PACKET_SIZE 32
#define PACKET_TYPE_POSITION 4
#define DATACAPACITYINHEADER 19
#define DATACAPACITYINEXTENSION 27

#define HASH_MAX_SIZE 32   				// <-- This MAX can be changed here. All other values dependent on this will be appropriately calculated. The max in only to allocate maximum memory. Sizes can be smaller but not greater
#define EFFECTIVE_DATA_MAX_SIZE 100  	// <-- This MAX can be changed here. All other values dependent on this will be appropriately calculated. Sizes can be smaller but not greater
#define MAX_DATA_SIZE (HASH_MAX_SIZE + EFFECTIVE_DATA_MAX_SIZE)

#define MAX_PACKETS_AFTER_HEADER (uint8_t)ceil((double)(MAX_DATA_SIZE-DATACAPACITYINHEADER) / (double)DATACAPACITYINEXTENSION)

//Handshake infos
#define HANDSHAKE_SHA256_NOENC = 1
#define HANDSHAKE_SHA256_AESENC = 2

#define PACKET_TYPE_FIRST  1
#define PACKET_TYPE_DATA  2
#define PACKET_TYPE_LAST  3

#define MAX_DESTINATION_ADDRESSES 30

/*
struct segment_dist_request {
  uint8_t protpacket;
  uint8_t sourcemacaddress[9];
  uint8_t destmacaddress[9];
  uint16_t sourcenodeaddress;
  uint16_t destnodeaddress;
  uint8_t signature[19];    	
};

struct segment_dist_response {
  uint8_t protpacket;
  uint8_t sourcemacaddress[9];
  uint8_t destmacaddress[9];
  uint16_t sourcenodeaddress;
  uint16_t destnodeaddress;
  uint8_t signature[18];    	
  uint8_t distance;
};
*/

struct packet_data_send {
  uint16_t frompacket;
  uint16_t topacket;  
  uint8_t packettype;
  uint8_t protpacket;
  uint16_t sourcenodeaddress;
  uint16_t destnodeaddress;
  uint8_t signaturesize; 
  uint16_t datasize; 	
  uint8_t data[DATACAPACITYINHEADER];
}; 

struct packet_data_sendext {
  uint16_t frompacket;
  uint16_t topacket;  
  uint8_t packettype;
  uint8_t data[DATACAPACITYINEXTENSION];
};

struct segment_data_send {
  packet_data_send header;
  packet_data_sendext data[MAX_PACKETS_AFTER_HEADER];
};

struct state_receive {
  int packetnumber;
  bool datapacketreceived;
  bool firstpacketreceived;
  bool lastpacketreceived;
  bool segmentreceived;
  bool segmentreceivedinerror;
  bool processdestinations;
  uint8_t numberofdestinations;
  uint16_t* destinationaddresses;
  uint8_t numberdestinationsprocessed;
  uint16_t sourcenodeaddress;
  bool sourcenodeaddressdefined;
  segment_data_send segment;
};

struct notificationdata {
	uint16_t sourcenodeaddress;
	uint16_t notificationtype;	
	uint8_t functionid;
	uint8_t notificationunit;
	uint32_t notificationvalue;
};

struct functionmapper {
	uint16_t sourcenodeaddress;
	uint16_t sourcenotificationtype;
	uint8_t sourcefunctionid;
	uint8_t maptofunctionid;
};

struct functionsubscription {
	uint16_t notificationtype;
	uint8_t functionid; 
	uint8_t numberofdestinations;
	uint16_t* destinationaddresses;
};



class AutuinoTransportNRF24L01 : public AutuinoTransport
{
	public:
		AutuinoTransportNRF24L01(uint8_t ce=RF24_CE_PIN, uint8_t cs=RF24_CS_PIN, uint8_t paLevel=RF24_PA_LEVEL, uint8_t channel=RF24_CHANNEL);
		void powerUp();
		void powerDown();	
		void dump();		
		void start();
		void setSigningAlgorithm(AutuinoSigning* signingobject);
		void setEncryptionAlgorithm(AutuinoEncryption* encryptionobject);
		void setAutuinoPowerSupply(AutuinoPowerSupply* powersupply);
		uint8_t getChannel();
		//The network ID is the base address to the nrf chip. 24 bits long.
		void setNetworkId(uint64_t networkid);	
		uint64_t getNetworkId();			
		void setNetworkSecretKey(uint8_t* networksecretkey);		
		void getNetworkSecretKey(uint8_t* secretkey);				
		void setNodeAddress(uint16_t nodeaddress); 		
		uint16_t getNodeAddress();		
		void setMACAddress(uint8_t* newaddress);
		void getMACAddress(uint8_t* newaddress);	
        //Radio address is obtained by adding the network id to the node id
        uint64_t getRadioAddress();		 
        void setTransmissionStatusPin(int pin);
        void setDeviceErrorStatusPin(int pin);
        uint16_t buildEffectiveDataPart(uint8_t* effectivedata, uint16_t sourcenodeaddress, uint16_t notificationtype, uint8_t functionid, uint8_t notificationunit, uint32_t value);
		void buildDataSendSegment(segment_data_send* request, uint8_t protpacket, uint16_t sourcenodeaddress, uint16_t destnodeaddress, uint8_t signaturesize, uint8_t* effectivedata, uint16_t effectivedatasize);
		void getDataFromSegment(uint8_t* data, segment_data_send* segment);
		bool available(uint16_t *to);
		uint8_t receive(void* data);
		int totalPackets(segment_data_send* segment);  
        bool sendData(uint8_t protpacket, uint16_t sourcenodeaddress, uint16_t destnodeaddress, uint8_t signaturesize, uint8_t* effectivedata, uint16_t effectivedatasize);
        //void setReceiveFunction(void(*receivepacketfunc)(uint16_t,uint16_t,uint8_t*));
        void processIncomingMessages();
		void processSegmentReceipt();
		void processSubscriptions();
		void processErrors();			        
        void networkProcess();  
        void setExecuteFunction(void(*executefunc)(uint16_t,uint8_t,notificationdata*));        
        void executeFunction(uint16_t sourcenodeaddress, uint16_t destinationnodeaddress, uint16_t notificationtype, uint8_t functionid, uint8_t notificationunit, uint32_t value);
        void executeFunction(uint16_t notificationtype, uint8_t functionid, uint8_t notificationunit, uint32_t value);
        void setFunctionSubscriptions(uint16_t numsubscription, functionsubscription* subscription);   
        void setRemoteToLocalFunctionMapping(uint16_t numfunctionmapper, functionmapper* functionmapper);
        void addRemoteToLocalFunctionMapping(uint16_t sourcenodeaddress,uint16_t notificationtype,uint8_t functionid, uint8_t maptofunctionid);
		void addFunctionSubscription(uint16_t notificationtype, uint8_t functionid, uint16_t destination);

		//void gethmacsha256(uint8_t* hash, uint8_t protpacket, uint8_t* sourcemacaddress, uint8_t* destmacaddress, uint16_t sourcenodeaddress, uint16_t destnodeaddress, uint8_t handshakeinfo);				
		//void buildDistanceRequestSegment(segment_dist_request* request, uint8_t* sourcemacaddress, uint8_t* destmacaddress, uint16_t sourcenodeaddress, uint16_t destnodeaddress, uint8_t* signature);
		//void buildDistanceResponseSegment(segment_dist_response* response, uint8_t* sourcemacaddress, uint8_t* destmacaddress, uint16_t sourcenodeaddress, uint16_t destnodeaddress, uint8_t* linkid, uint8_t* signature, uint8_t distance);        
        //returns the same pointer to data, but casted to notificationdata...
        //notificationdata* castDataToNotification(uint16_t datasize, uint8_t* data);
        //saves all configurations to EEPROM. Saving must be optimized. Don't store blanks. 
        //void saveNotificationConfiguration(notificationconfig* config, int numberofitems);
        //Reads EEPROM configuration, but only pull the configuration that matches the source address and notification type provided
        //void getNotificationDestinations(size_t& numberofdestinations, uint16_t*destinationaddresses, uint16_t sourcenodeaddress,uint16_t notificationtype);
        //Sends a notification to all destinations that match the sourcenodeaddress and notification type 
        //bool notifyDestinations(notificationdata& notification, uint16_t sourcenodeaddress, uint8_t signaturesize, uint8_t numberofdestinations, uint16_t* destinationaddresses);

#ifdef DEBUG  
        //void dump(segment_dist_request* segment);
        //void dump(segment_dist_response* segment);						
		void dump(segment_data_send* segment);																
#endif
	private:
		bool radiostart();	
		RF24 rf24;
		uint8_t _ce;
		uint8_t _cs;
		uint8_t _paLevel;
		uint8_t _channel;
		uint16_t nodeaddress=-1;
		uint64_t networkid=-1;
		uint8_t* networksecretkey=nullptr;
		uint8_t* macaddress=nullptr;
		int transmissionstatuspin = -1;
		int deviceerrorstatuspin = -1;
		state_receive receipt_state;	
		//void(*receivefunc)(uint16_t,uint16_t,uint8_t*);	
		void(*executefunction)(uint16_t,uint8_t,notificationdata*)=nullptr;	
		uint16_t numberofsubscriptions=0;
		functionsubscription* subscriptions=nullptr;
		uint16_t numberoffunctionmappers=0;
		functionmapper* functionmappers=nullptr;		
		AutuinoPowerSupply* powersupply=nullptr;
		AutuinoSigning* _signingobject=nullptr;
		AutuinoEncryption* _encryptionobject=nullptr;
};
