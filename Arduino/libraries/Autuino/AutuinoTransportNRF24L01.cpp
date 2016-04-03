#include "AutuinoTransportNRF24L01.h"
//#include "sha256.h"

AutuinoTransportNRF24L01::AutuinoTransportNRF24L01(uint8_t ce, uint8_t cs, uint8_t paLevel, uint8_t channel)
	:
	AutuinoTransport(),
	rf24(ce, cs),
	_paLevel(paLevel)
{
	_ce = ce;
	_cs = cs;
	_paLevel = paLevel;
	_channel = channel;

   receipt_state.packetnumber = 0;
   receipt_state.datapacketreceived = false;
   receipt_state.firstpacketreceived = false;
   receipt_state.lastpacketreceived = false;
   receipt_state.segmentreceived = false;
   receipt_state.sourcenodeaddressdefined = false;
   receipt_state.sourcenodeaddress = 0;	
   receipt_state.segmentreceivedinerror = false;
   receipt_state.processdestinations = false;
   receipt_state.numberdestinationsprocessed=0;
}

void AutuinoTransportNRF24L01::start() {
	if(transmissionstatuspin>=0) {
		pinMode(transmissionstatuspin, OUTPUT);
	}
	if(deviceerrorstatuspin>=0) {
		pinMode(deviceerrorstatuspin, OUTPUT);
	}	
	if(deviceerrorstatuspin>=0) {
		digitalWrite(deviceerrorstatuspin,HIGH);
	}		
	while(!AutuinoTransportNRF24L01::radiostart()) {
		delay(10000);
	}
#ifdef DEBUG
	dump();
#endif	

}

void AutuinoTransportNRF24L01::setTransmissionStatusPin(int pin) {
   transmissionstatuspin = pin;
}

void AutuinoTransportNRF24L01::setDeviceErrorStatusPin(int pin) {
   deviceerrorstatuspin = pin;
}

//void AutuinoTransportNRF24L01::setReceiveFunction(void(*receivepacketfunc)(uint16_t,uint16_t,uint8_t*)) {
//    receivefunc = receivepacketfunc;
//}

void AutuinoTransportNRF24L01::setExecuteFunction(void(*executefunc)(uint16_t,uint8_t,notificationdata*)) {
    executefunction = executefunc;
}

bool AutuinoTransportNRF24L01::radiostart() {
	if(deviceerrorstatuspin>=0) {
		digitalWrite(deviceerrorstatuspin,HIGH);
	}		
	// Start up the radio library
	rf24.begin();

	if (!rf24.isPVariant()) {
		if(deviceerrorstatuspin>=0) {
			digitalWrite(deviceerrorstatuspin,HIGH);
		}		
		return false;
	}
	rf24.setAutoAck(1);
	rf24.setAutoAck(BROADCAST_PIPE,false); // Turn off auto ack for broadcast
	rf24.enableAckPayload();
	rf24.setChannel(AutuinoTransportNRF24L01::getChannel());
	rf24.setPALevel(_paLevel);
	rf24.setDataRate(RF24_DATARATE);
	rf24.setRetries(5,15);
	rf24.setCRCLength(RF24_CRC_16);
	rf24.enableDynamicPayloads();

	rf24.openReadingPipe(WRITE_PIPE, (uint64_t)AutuinoTransportNRF24L01::getRadioAddress());
	rf24.openReadingPipe(CURRENT_NODE_PIPE, (uint64_t)AutuinoTransportNRF24L01::getRadioAddress());
	rf24.startListening();

	if(deviceerrorstatuspin>=0) {
		digitalWrite(deviceerrorstatuspin,LOW);
	}
			
	return true;
}

void AutuinoTransportNRF24L01::powerUp() {
	if(deviceerrorstatuspin>=0) {
		digitalWrite(deviceerrorstatuspin,HIGH);
	}		
	
	rf24.powerUp();
	
	if(deviceerrorstatuspin>=0) {
		digitalWrite(deviceerrorstatuspin,LOW);
	}			
	delay(3000);
}

void AutuinoTransportNRF24L01::powerDown() {
	rf24.powerDown();
}

int AutuinoTransportNRF24L01::totalPackets(segment_data_send* segment) {
  int bytestosendafterheader = segment->header.signaturesize + segment->header.datasize - DATACAPACITYINHEADER; 	
  return ceil((double)bytestosendafterheader / (double)DATACAPACITYINEXTENSION) + 1;
}

uint16_t AutuinoTransportNRF24L01::buildEffectiveDataPart(uint8_t* effectivedata, uint16_t sourcenodeaddress, uint16_t notificationtype, uint8_t functionid, uint8_t notificationunit, uint32_t value) {
	int byteposition = 0;
	int memsize = 2;
	memcpy((void*)(effectivedata+byteposition),(void*)&sourcenodeaddress,memsize); //move 2 bytes (uint16_t) for sourcenodeaddress
	byteposition = byteposition + memsize;
	memsize = 2;
	memcpy((void*)(effectivedata+byteposition),(void*)&notificationtype,memsize);  //move 2 bytes (uint16_t) for the notificationtype
	byteposition = byteposition + memsize;
	memsize = 1;
	memcpy((void*)(effectivedata+byteposition),(void*)&functionid,memsize); //function id
	byteposition = byteposition + memsize;
	memsize = 1;
	memcpy((void*)(effectivedata+byteposition),(void*)&notificationunit,memsize); //notification unit
	byteposition = byteposition + memsize;
	memsize = 4;
	memcpy((void*)(effectivedata+byteposition),(void*)&value,memsize); //value
	
	return byteposition + memsize + 1;
}

/*
void AutuinoTransportNRF24L01::saveNotificationConfiguration(notificationconfig* config, int numberofitems) {
	uint8_t* resultingobject;
	uint8_t totalbytes=0;
	
	//allocate 1 byte for the number of items
	totalbytes=totalbytes+1;
	
	//figure out how much memory to allocate
	for(int i=0;i<numberofitems;i++) {
		//2 bytes for the source address
		totalbytes=totalbytes+2;
		//2 bytes for the notification type
		totalbytes=totalbytes+2;
		//1 byte or the number of destinations
		totalbytes=totalbytes+1;
		//2 bytes per destination address
		totalbytes=totalbytes+(config[i].numberofdestinations*2);
	}
	resultingobject = (uint8_t*)malloc(totalbytes);
	Serial.print("# of bytes: ");
	Serial.println(totalbytes);

	uint16_t position=0;
	resultingobject[position]=numberofitems;
	for(int i=0;i<numberofitems;i++) {
		position=position+1;
		memcpy(resultingobject+position,&config[i].sourcenodeaddress,2);
		position=position+2;
		memcpy(resultingobject+position,&config[i].notificationtype,2);
		position=position+2;
		memcpy(resultingobject+position,&config[i].numberofdestinations,1);
		position=position+1;
		for(int j=0;j<config[i].numberofdestinations;j++) {
			memcpy(resultingobject+position,&config[i].destinationaddresses[j],2);
			position=position+2;
		}
	}	
	Serial.print("DATAAA: ");
	for(int i=0;i<totalbytes;i++) {
		Serial.print(resultingobject[i],HEX);
		Serial.print(":");
	}
	Serial.println("");
}
*/ 

void AutuinoTransportNRF24L01::buildDataSendSegment(segment_data_send* request, uint8_t protpacket, uint16_t sourcenodeaddress, uint16_t destnodeaddress, uint8_t signaturesize, uint8_t* effectivedata, uint16_t effectivedatasize) {
  int bytestosendafterheader = signaturesize + effectivedatasize - DATACAPACITYINHEADER;   
  int numberofpacketstosendafterheader = ceil((double)bytestosendafterheader / (double)DATACAPACITYINEXTENSION);

  //going for a good seed....
  unsigned long seed = 0;
  for (int i=0; i<32; i++)
  {
    seed = seed | ((analogRead(A0) & 0x01) << i);
  }
  randomSeed(seed);
  uint16_t start = random(0, 65534-numberofpacketstosendafterheader);
  
  //Calculate hash...TODO: FINISH THIS
  uint8_t hash[HASH_MAX_SIZE] = {'T','H','I','S',' ','I','S',' ','T','H','E',' ','H','A','S','H','T','H','I','S',' ','I','S',' ','T','H','E',' ','H','A','S','H'};
  
  //Combine the hash to the effective data to get the "data'

  request->header.packettype = PACKET_TYPE_FIRST;
  request->header.frompacket = start;
  request->header.topacket = start+numberofpacketstosendafterheader;
  request->header.protpacket = protpacket;
  request->header.sourcenodeaddress = sourcenodeaddress;
  request->header.destnodeaddress = destnodeaddress;
  request->header.signaturesize = signaturesize; 
  request->header.datasize = effectivedatasize;
    
  int bytesprocessed = 0;  
  int bytesonheader;
  if(bytestosendafterheader>=0) {
	  bytesonheader = DATACAPACITYINHEADER;
  } else {
      bytesonheader = signaturesize + effectivedatasize;  
  }
  
  for(int j=0;j<bytesonheader;j++) {
	 if(bytesprocessed>=signaturesize) {
		request->header.data[j] = effectivedata[j-signaturesize];
	} else {
		request->header.data[j] = hash[j];
	}
	bytesprocessed++; 
  }
  
  int bytesleft = bytestosendafterheader;
  int index = DATACAPACITYINHEADER;
  for(int i=1;i<=numberofpacketstosendafterheader;i++) {
	if(i==numberofpacketstosendafterheader) {
	    request->data[i-1].packettype = PACKET_TYPE_LAST;
	} else {
	    request->data[i-1].packettype = PACKET_TYPE_DATA;
	}
    request->data[i-1].frompacket = start+i;
    request->data[i-1].topacket = request->header.topacket;	  
    for(int j=0;j<DATACAPACITYINEXTENSION;j++) {
		if(bytesleft>0) {
			if(bytesprocessed>=signaturesize) {
				request->data[i-1].data[j] = effectivedata[index-signaturesize];
			} else {
				request->data[i-1].data[j] = hash[index];
			}
			index = index + 1;
			bytesprocessed++;
		}	
		bytesleft = bytesleft - 1;
	}
  }
}

//bool AutuinoTransportNRF24L01::notifyDestinations(notificationdata& notification, uint16_t sourcenodeaddress, uint8_t signaturesize, uint8_t numberofdestinations, uint16_t* destinationaddresses) {
	
//   for(int i=0;i<numberofdestinations;i++) {
		//sendData(1, sourcenodeaddress, destinationaddresses[i], signaturesize, effectivedata, effectivedatasize);
//   }
//}

bool AutuinoTransportNRF24L01::sendData(uint8_t protpacket, uint16_t sourcenodeaddress, uint16_t destnodeaddress, uint8_t signaturesize, uint8_t* effectivedata, uint16_t effectivedatasize) {
  if(transmissionstatuspin>=0) {
	digitalWrite(transmissionstatuspin,HIGH);
  }
  
  segment_data_send datarequest;
  buildDataSendSegment(&datarequest, protpacket, sourcenodeaddress, destnodeaddress, signaturesize, effectivedata, effectivedatasize);
#ifdef DEBUG
  dump(&datarequest);
#endif  
  //Send packet
  uint64_t destination = getNetworkId()+destnodeaddress;
  rf24.stopListening();
  rf24.openWritingPipe(destination);
  bool ok;
  for(int i=0;i<AutuinoTransportNRF24L01::totalPackets(&datarequest);i++) {
	 if(i==0) {  //first packet. Send header
		ok = rf24.write(&datarequest.header, sizeof(datarequest.header), false);
		if(!ok) {
		   if(deviceerrorstatuspin>=0) {
				digitalWrite(deviceerrorstatuspin,HIGH);
		   }
		   break;
		}	    
	 } else {
		ok = rf24.write(&datarequest.data[i-1], sizeof(datarequest.data[i-1]), false);	    	    
		if(!ok) {
		   if(deviceerrorstatuspin>=0) {
				digitalWrite(deviceerrorstatuspin,HIGH);
		   }
		   break;
		}	
	 }	
  }	
  if(ok) {
	if(deviceerrorstatuspin>=0) {
		digitalWrite(deviceerrorstatuspin,LOW);
	}  
  }
  rf24.startListening();	
  
  if(transmissionstatuspin>=0) {
	digitalWrite(transmissionstatuspin,LOW);
  }
  
  return ok;
}

void AutuinoTransportNRF24L01::setNetworkId(uint64_t networkid) {
	EEPROM.put(EEPROM_START_NETID, networkid);
}

uint64_t AutuinoTransportNRF24L01::getNetworkId() {	
	uint64_t networkid;
	EEPROM.get(EEPROM_START_NETID,networkid);
		
	return networkid;  	
}

void AutuinoTransportNRF24L01::setNetworkSecretKey(uint8_t* networksecretkey) {
	for(int i=0;i<NETWORK_SECRET_SIZE;i++) {
	   EEPROM.write(EEPROM_START_NETSECRET+i, networksecretkey[i]);
	}
}

void AutuinoTransportNRF24L01::getNetworkSecretKey(uint8_t* secretkey) {
	for(int i=0;i<NETWORK_SECRET_SIZE;i++) {
	   secretkey[i] = EEPROM.read(EEPROM_START_NETSECRET+i);
	} 
}

void AutuinoTransportNRF24L01::setNodeAddress(uint16_t nodeaddress) {
	EEPROM.put(EEPROM_START_NODE,nodeaddress);
}

uint16_t AutuinoTransportNRF24L01::getNodeAddress() {
	uint16_t nodeaddress;
	EEPROM.get(EEPROM_START_NODE,nodeaddress);
	return nodeaddress;
}

void AutuinoTransportNRF24L01::setMACAddress(uint8_t* newaddress) {
	for(int i=0;i<MAC_ADDRESS_SIZE;i++) {
	   EEPROM.write(EEPROM_START_MAC+i, newaddress[i]);
	}
}

void AutuinoTransportNRF24L01::getMACAddress(uint8_t* newaddress) {
	for(int i=0;i<MAC_ADDRESS_SIZE;i++) {
	   newaddress[i] = EEPROM.read(EEPROM_START_MAC+i);
	}
}

uint64_t AutuinoTransportNRF24L01::getRadioAddress() {
	return getNetworkId() + getNodeAddress();
}

uint8_t AutuinoTransportNRF24L01::getChannel() {
	return _channel;
}

bool AutuinoTransportNRF24L01::available(uint16_t *to) {
	uint8_t pipe = 255;
	boolean avail = rf24.available(&pipe);
	(void)avail; //until somebody makes use of 'avail'
	if (pipe == CURRENT_NODE_PIPE)
		*to = AutuinoTransportNRF24L01::getNodeAddress();
	else if (pipe == BROADCAST_PIPE)
		*to = BROADCAST_ADDRESS;
	return (rf24.available() && pipe < 6);
}

uint8_t AutuinoTransportNRF24L01::receive(void* data) {
	uint8_t len = rf24.getDynamicPayloadSize();
	rf24.read(data, len);
	return len;
}

//notificationdata* AutuinoTransportNRF24L01::castDataToNotification(uint16_t datasize, uint8_t* data) {
//	return (notificationdata*) data;
//}

void AutuinoTransportNRF24L01::executeFunction(uint16_t sourcenodeaddress, uint16_t destinationnodeaddress, uint16_t notificationtype, uint8_t functionid, uint8_t notificationunit, uint32_t value) {
	uint8_t effectivedata[MAX_DATA_SIZE];
	uint16_t effectivedatasize;
	
	//if source and destination are this is a local function call. Call the method. If not, send to radio
	if(sourcenodeaddress==destinationnodeaddress) {	
		//call the notificationreceivefunction if exists
		if(executefunction) {	
			notificationdata* tmpdata = (notificationdata*)malloc(sizeof(notificationdata));
			tmpdata->sourcenodeaddress = sourcenodeaddress;
			tmpdata->notificationtype = notificationtype;
			tmpdata->functionid = functionid;
			tmpdata->notificationunit = notificationunit;
			tmpdata->notificationvalue = value;	
			executefunction(sourcenodeaddress,functionid,tmpdata);
		}	
	} else {	    
		effectivedatasize = buildEffectiveDataPart(effectivedata, sourcenodeaddress, notificationtype,functionid,notificationunit,value);
		if(sendData(1, sourcenodeaddress, destinationnodeaddress,32, effectivedata, effectivedatasize)) {
#ifdef DEBUG			
			Serial.print(millis());
			Serial.print(" - ");
			Serial.println("Message sent");
#endif
		} else {
#ifdef DEBUG			
			Serial.print(millis());
			Serial.print(" - ");    
			Serial.println("Problems sending message");
#endif
		}	
	}	
}	 

//this is a local call since not source and destination are defined.
void AutuinoTransportNRF24L01::executeFunction(uint16_t notificationtype, uint8_t functionid, uint8_t notificationunit, uint32_t value) {
	executeFunction(getNodeAddress(),getNodeAddress(),notificationtype,functionid,notificationunit,value);	
}	

void AutuinoTransportNRF24L01::processIncomingMessages() {
	uint16_t receiver;
	uint8_t data[PACKET_SIZE];
  			
  		
	//STATE: Packet available for receipt. NEXT STATE: Data received
	if(available(&receiver)&&(receipt_state.processdestinations==false)) {
		if(transmissionstatuspin>=0) {
			digitalWrite(transmissionstatuspin,HIGH);
		}		
#ifdef DEBUG		
		Serial.print(F("Data ready to be received on address: "));
		Serial.print(receiver);  
		Serial.println("");
		Serial.print(F("Bytes received: "));
#endif
		uint8_t bytestoreceive = receive(data);
#ifdef DEBUG
        Serial.print((int)bytestoreceive);
        Serial.println("");
#endif	
	    if(data[PACKET_TYPE_POSITION]==PACKET_TYPE_FIRST) {
#ifdef DEBUG
			Serial.println(F("Received First packet"));
#endif	
		   receipt_state.packetnumber = 1;
		   receipt_state.firstpacketreceived = true;
		   receipt_state.sourcenodeaddressdefined = true;
		   receipt_state.sourcenodeaddress = receipt_state.segment.header.sourcenodeaddress;
		   //copy header
		   memcpy(&receipt_state.segment.header,data,bytestoreceive);
		   //find out how many packets after header
		   int packetsafterheader = receipt_state.segment.header.topacket - receipt_state.segment.header.frompacket;	   
		} else if(data[PACKET_TYPE_POSITION]==PACKET_TYPE_DATA) {
#ifdef DEBUG
			Serial.println(F("Received Data packet"));
#endif	
		   receipt_state.datapacketreceived = true;
		   receipt_state.packetnumber++;	
		   memcpy(&receipt_state.segment.data[receipt_state.packetnumber-2],data,bytestoreceive);
		} else if(data[PACKET_TYPE_POSITION]==PACKET_TYPE_LAST) {
#ifdef DEBUG
			Serial.println(F("Received last packet"));
#endif	
		   receipt_state.lastpacketreceived = true;
		   receipt_state.segmentreceived = true;
		   receipt_state.packetnumber++;	
		   memcpy(&receipt_state.segment.data[receipt_state.packetnumber-2],data,bytestoreceive);
		}
		if(transmissionstatuspin>=0) {
			digitalWrite(transmissionstatuspin,LOW);
		}	
	}
	
}

void AutuinoTransportNRF24L01::processSegmentReceipt() {

	// Data was fully and successfully received
	if(receipt_state.segmentreceived) {
		if(transmissionstatuspin>=0) {
			digitalWrite(transmissionstatuspin,HIGH);
		}		
		uint8_t data[MAX_DATA_SIZE];
		getDataFromSegment(data, &receipt_state.segment);
#ifdef DEBUG
		Serial.println(F("Segment Received Successfully!"));
		Serial.println("");
		dump(&receipt_state.segment);
		Serial.println("");
		Serial.print(F("Data: >>>"));
		for(int i=0;i<receipt_state.segment.header.datasize;i++) {
			Serial.print(data[i],HEX);
			Serial.print(F(":"));
		}
		Serial.println(F("<<<"));
#endif	

		//Check the notification that just arrived and see if it maps to any function of our device
		//if it does, raise the event to be dealt with
		bool foundmapper = false;
		uint8_t mapperindex = -1;
		notificationdata* tmpdata = (notificationdata*)data;
		for(int i=0;i<numberoffunctionmappers;i++) {
			//check for a match of the source data coming and mapper to know which function id to trigger
#ifdef DEBUG			
			Serial.println(F("COMPARING"));
			Serial.print(functionmappers[i].sourcenodeaddress);
			Serial.print(F("=="));
			Serial.println(receipt_state.segment.header.sourcenodeaddress);
			Serial.print(functionmappers[i].sourcenotificationtype);
			Serial.print(F("=="));
			Serial.println(tmpdata->notificationtype);
			Serial.print(functionmappers[i].sourcefunctionid);
			Serial.print(F("=="));
			Serial.println(tmpdata->functionid);
#endif			
			if((functionmappers[i].sourcenodeaddress==receipt_state.segment.header.sourcenodeaddress)&&(functionmappers[i].sourcenotificationtype==tmpdata->notificationtype)&&(functionmappers[i].sourcefunctionid==tmpdata->functionid)) {
				foundmapper=true;
				mapperindex = i;
				break;
			}
		}
		if(foundmapper) {
#ifdef DEBUG			
			Serial.println(F("FOUND FUNCTION MAPPER:"));
			Serial.print(F("FUNCTION ID TO EXECUTE: "));
			Serial.println(functionmappers[mapperindex].maptofunctionid);			
#endif
			//if there is a function to be called for this mapping call it
			if(executefunction) {
				executefunction(receipt_state.segment.header.sourcenodeaddress,functionmappers[mapperindex].maptofunctionid,(notificationdata*)data);
			}
		}

       // if(receivefunc) {
			//receivefunc(receipt_state.segment.header.sourcenodeaddress,receipt_state.segment.header.datasize,data);			
			//notificationreceivefunc(receipt_state.segment.header.sourcenodeaddress,(notificationdata*)data);
		//}
		receipt_state.packetnumber = 0;
		receipt_state.datapacketreceived = false;
		receipt_state.firstpacketreceived = false;
		receipt_state.lastpacketreceived = false;
		receipt_state.segmentreceived = false;
		receipt_state.sourcenodeaddressdefined = false;
		receipt_state.segmentreceivedinerror = false;
		receipt_state.sourcenodeaddress = 0;		
		
		//collect the destinations that will receive the notification because of the subscriptions
		receipt_state.processdestinations=true;
		receipt_state.numberofdestinations=0;
		if(numberofsubscriptions) {
			receipt_state.numberofdestinations=0;		
			receipt_state.destinationaddresses = 0;		
			for(int i=0;i<numberofsubscriptions;i++) {
			   notificationdata* tmp = (notificationdata*)data;
			   if((subscriptions[i].notificationtype==tmp->notificationtype)&&(subscriptions[i].functionid==tmp->functionid)) {
				  receipt_state.numberofdestinations = subscriptions[i].numberofdestinations;
				  receipt_state.destinationaddresses = subscriptions[i].destinationaddresses;
			   }
			}
		}
        receipt_state.numberdestinationsprocessed=0;		
		
		if(transmissionstatuspin>=0) {
			digitalWrite(transmissionstatuspin,LOW);
		}			
	}

}

void AutuinoTransportNRF24L01::processSubscriptions() {
	//segment received. Now it is time to process the destinations and send them the message
	if(receipt_state.processdestinations) {
		//Send 1 destination at a time to avoid holding the CPU from the loop to get a better multitasking
#ifdef DEBUG
		Serial.println(F("DESTINATIONS TO BE NOTIFIED: "));
#endif
		uint8_t data[MAX_DATA_SIZE];
		getDataFromSegment(data, &receipt_state.segment);
		notificationdata* tmpdata = (notificationdata*)data;
		
		for(int i=0;i<receipt_state.numberofdestinations;i++) {
#ifdef DEBUG
			Serial.print(receipt_state.destinationaddresses[i]);
			Serial.print(",");
#endif			
			//executeFunction(getNodeAddress(), receipt_state.destinationaddresses[i], tmpdata->notificationtype, tmpdata->functionid, tmpdata->notificationunit, tmpdata->notificationvalue);
		}
#ifdef DEBUG		
		Serial.println("");
#endif		
		receipt_state.processdestinations=false;
		receipt_state.numberdestinationsprocessed=0;
	}
}

//Mappers map the source/notification/functionid from the source to the functionid of the device. This means the functionid of the device must react to the value of the message (turn ON or off for example
//Once this is done, the triggers are loaded. See triggers
void AutuinoTransportNRF24L01::setRemoteToLocalFunctionMapping(uint16_t numfunctionmapper, functionmapper* functionmapper) {
	functionmappers = functionmapper;
	numberoffunctionmappers = numfunctionmapper;
}

void AutuinoTransportNRF24L01::addFunctionMapperItem(uint16_t sourcenodeaddress,uint16_t notificationtype,uint8_t functionid, uint8_t maptofunctionid) {
	numberoffunctionmappers++;
	realloc(functionmappers,sizeof(functionmapper)*numberoffunctionmappers);
	functionmappers[numberoffunctionmappers-1].sourcenodeaddress = sourcenodeaddress;
	functionmappers[numberoffunctionmappers-1].sourcenotificationtype = notificationtype;
	functionmappers[numberoffunctionmappers-1].sourcefunctionid = functionid;
	functionmappers[numberoffunctionmappers-1].maptofunctionid = maptofunctionid;
}

//Triggers are called from a functionid of the device. Any matches between the notificationtype from the message and functionid of the device will trigger a message to the
//destination
void AutuinoTransportNRF24L01::setFunctionSubscriptions(uint16_t numsubscription, functionsubscription* subscription) {
	subscriptions = subscription;
	numberofsubscriptions = numsubscription;
}

void AutuinoTransportNRF24L01::processErrors() {
	//Data was received in error
	if(receipt_state.segmentreceivedinerror) {
		if(deviceerrorstatuspin>=0) {
			digitalWrite(deviceerrorstatuspin,HIGH);
		}		
		receipt_state.packetnumber = 0;
		receipt_state.datapacketreceived = false;
		receipt_state.firstpacketreceived = false;
		receipt_state.lastpacketreceived = false;
		receipt_state.segmentreceived = false;
		receipt_state.sourcenodeaddressdefined = false;
		receipt_state.segmentreceivedinerror = false;
		receipt_state.sourcenodeaddress = 0;				
		if(deviceerrorstatuspin>=0) {
			digitalWrite(deviceerrorstatuspin,LOW);
		}		
	}
}

void AutuinoTransportNRF24L01::networkProcess() {
	static int turn = 0;
	
	//This allows the rotation of different processes to
	//give the illusion of multitasking
	if(turn==0) {
		processIncomingMessages();
		turn++;	
	} else if (turn==1) {
		processSegmentReceipt();
		turn++;
	} else if (turn==2) {
		processSubscriptions();
		turn++;
	} else if (turn==3) {
		processErrors();
		turn++;	
	} else {
		turn=0;
	}
}

void AutuinoTransportNRF24L01::getDataFromSegment(uint8_t* data, segment_data_send* segment) {
      bool datastarted=false;
	  int headerdatasize;
	  if(segment->header.datasize+segment->header.signaturesize>=DATACAPACITYINHEADER) {
	     headerdatasize = DATACAPACITYINHEADER;
	  } else {
		 headerdatasize = segment->header.datasize+segment->header.signaturesize;
	  }
	  int j=0;
	  for(int i=0;i<headerdatasize;i++) {
		if(i==segment->header.signaturesize) {
			datastarted=true;
		}  
		if(datastarted) {
			data[j] = segment->header.data[i];
		}
	  };
	  int lastindex = j;

	  datastarted=false;
	  int from = segment->header.frompacket;
	  int to = segment->header.topacket;
      int index = DATACAPACITYINHEADER;
      for(int i=0;i<(to-from);i++) {
	    for(int j=0;j<DATACAPACITYINEXTENSION;j++) {
		   if(index==segment->header.signaturesize) {
		      datastarted=true;
		   }
		   if(datastarted) {
				if(((i*DATACAPACITYINEXTENSION)+j)<(segment->header.datasize+segment->header.signaturesize-headerdatasize)) {			   
					data[lastindex] = segment->data[i].data[j]; 
					lastindex++;
				}	
		   }
		   index = index + 1;
		}		
	  }			  	
}

#ifdef DEBUG
void AutuinoTransportNRF24L01::dump(segment_data_send* segment) {
	  Serial.println("");  	
	  Serial.println(F("--DATA REQUEST DUMP START--"));		
	  Serial.print(F("       Packet Type............: "));
	  Serial.println(segment->header.packettype,DEC);
	  Serial.print(F("       From...................: "));
	  Serial.println(segment->header.frompacket,DEC);
	  Serial.print(F("       To.....................: "));
	  Serial.println(segment->header.topacket,DEC);
	  Serial.print(F("       Protocol...............: "));
	  Serial.println(segment->header.protpacket,HEX); 	
	  Serial.print(F("       Source Node Address....: "));
	  Serial.println(segment->header.sourcenodeaddress);
	  Serial.print(F("       Dest. Node Address.....: "));
	  Serial.println(segment->header.destnodeaddress);	
	  Serial.print(F("       Signature Size.........: "));
      Serial.println(segment->header.signaturesize);		
	  Serial.print(F("       Data size..............: "));
      Serial.println(segment->header.datasize);
	  Serial.print(F("       Data...................: "));
	  
	  int headerdatasize;
	  if(segment->header.datasize+segment->header.signaturesize>=DATACAPACITYINHEADER) {
	     headerdatasize = DATACAPACITYINHEADER;
	  } else {
		 headerdatasize = segment->header.datasize+segment->header.signaturesize;
	  }
	  for(int i=0;i<headerdatasize;i++) {
		if(i==segment->header.signaturesize) {
			Serial.print(F("***:"));
		}  
	 	Serial.print(segment->header.data[i],HEX);
		if(i!=headerdatasize-1) {
			Serial.print(":");
		}		
	  };
	  Serial.println("");
	  Serial.print(F("       Additional Packets.....: "));
	  int from = segment->header.frompacket;
	  int to = segment->header.topacket;
      Serial.println(to-from);
      
      Serial.println("");

      int index = DATACAPACITYINHEADER;
      for(int i=0;i<(to-from);i++) {
		Serial.println("");
		Serial.print(F("    PACKET:  "));
		Serial.println(i+1);
	    Serial.print(F("       Packet Type............: "));
	    Serial.println(segment->data[i].packettype,DEC);				
	    Serial.print(F("       From...................: "));
	    Serial.println(segment->data[i].frompacket,DEC);				
	    Serial.print(F("       To.....................: "));
	    Serial.println(segment->data[i].topacket,DEC);
	    Serial.print(F("       Data...................: "));	    	
	    for(int j=0;j<DATACAPACITYINEXTENSION;j++) {
		   if(index==segment->header.signaturesize) {
		      Serial.print(F("***:"));
		   }
		   if(((i*DATACAPACITYINEXTENSION)+j)<(segment->header.datasize+segment->header.signaturesize-headerdatasize)) {
				Serial.print(segment->data[i].data[j],HEX); 
				Serial.print(":");
		   }	
		   index = index + 1;
		}		
		Serial.println("");	
	  }
	  
	  Serial.println(F("--DATA REQUEST DUMP END--"));	
}

void AutuinoTransportNRF24L01::dump() {
	Serial.println("");
	Serial.println(F("--DUMP--"));	
	Serial.print(F("CE Pin................: "));
	Serial.println((long)_ce);		
	Serial.print(F("CS Pin................: "));
	Serial.println((long)_cs);		
	Serial.print(F("PA Level..............: "));
	Serial.println((long)_paLevel);		
	Serial.print(F("Channel...............: "));
	Serial.println((long)_channel);		
	Serial.print(F("Current MAC Address...: "));
	uint8_t mac[MAC_ADDRESS_SIZE];
	AutuinoTransportNRF24L01::getMACAddress(mac);
	for(int i=0;i<MAC_ADDRESS_SIZE;i++) {
		Serial.print(mac[i],HEX);
		if(i!=MAC_ADDRESS_SIZE-1) {
			Serial.print(F(":"));
		}		
	};	
	Serial.println("");
	Serial.print(F("Network ID............: "));
	uint64_t networkid = AutuinoTransportNRF24L01::getNetworkId();
    //Work around to print uint64_t number
    uint64_t xx = networkid/1000000000ULL;
    if (xx >0) Serial.print((long)xx);
    Serial.print((long)(networkid-xx*1000000000));    
    
    //Serial.print((long)networkid,HEX);
    Serial.println("");
	Serial.print(F("Network Secret Key....: "));
	uint8_t secret[NETWORK_SECRET_SIZE];
	AutuinoTransportNRF24L01::getNetworkSecretKey(secret);
	for(int i=0;i<NETWORK_SECRET_SIZE;i++) {
		Serial.print(secret[i],HEX);
		if(i!=NETWORK_SECRET_SIZE-1) {
			Serial.print(":");
		}		
	};		
	Serial.println("");
	Serial.print(F("Network Node Address..: "));
	uint16_t node = AutuinoTransportNRF24L01::getNodeAddress();
	Serial.print(node);
	Serial.println("");	
	Serial.print(F("Radio Address.........: "));
	uint64_t radioaddress = AutuinoTransportNRF24L01::getRadioAddress();
    //Work around to print uint64_t number
    uint64_t yy = radioaddress/1000000000ULL;
    if (yy >0) Serial.print((long)yy);
    Serial.print((long)(radioaddress-yy*1000000000));    
	Serial.println("");
	Serial.println(F("--END DUMP--"));
	Serial.println("");	
}
#endif


/*
void AutuinoTransportNRF24L01::gethmacsha256(uint8_t* hash, uint8_t protpacket, uint8_t* sourcemacaddress, uint8_t* destmacaddress, uint16_t sourcenodeaddress, uint16_t destnodeaddress, uint8_t handshakeinfo) {
	struct hashstruct {
		uint8_t frompacket;
		uint8_t topacket;
		uint8_t protpacket;
		uint8_t sourcemacaddress[9];
		uint8_t destmacaddress[9];
		uint16_t sourcenodeaddress;
		uint16_t destnodeaddress;
		uint8_t linkid[3];
		uint8_t handshakeinfo;
		uint8_t signature[32];
	};
	hashstruct thash;
	thash.protpacket = protpacket;
	memcpy(thash.sourcemacaddress,sourcemacaddress,9);
	memcpy(thash.destmacaddress,destmacaddress,9);
	thash.sourcenodeaddress = sourcenodeaddress;
	thash.destnodeaddress = destnodeaddress;
	thash.handshakeinfo = handshakeinfo;
	Sha256Class sha256;
	uint8_t hmacKey[]={
		0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b
	};    
    sha256.initHmac(hmacKey,20);
	for(unsigned int i = 0;i<sizeof(thash);i++) {
		sha256.write(*((uint8_t*)(&thash) + i));
	};
	hash = sha256.resultHmac();
}
*/



