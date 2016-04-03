
#include "AutuinoTransportNRF24L01.h"

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
};

AutuinoTransportNRF24L01 radio;

void functionExecuted(uint16_t sourceaddress, uint8_t functionid, notificationdata* notification) {    
    Serial.println("");
    Serial.print("Mapped Function ID............: ");
    Serial.println(functionid);     
    Serial.print("Source node address...........: ");
    Serial.println(notification->sourcenodeaddress);     
    Serial.print("Notification type.............: ");
    Serial.println(notification->notificationtype);     
    Serial.print("Function id...................: ");
    Serial.println(notification->functionid);        
    Serial.print("Notification unit.............: ");
    Serial.println(notification->notificationunit);        
    Serial.print("Notification Value............: ");
    Serial.println(notification->notificationvalue);     
    Serial.println("");
}

uint8_t hmacKey[]={
  0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b
};  
  
void setup() {
  Serial.begin(9600);    
  Serial.println("Starting radio");
                  
  uint8_t signaturesize = 30;
  uint8_t datasize = 46;
  
  radio = AutuinoTransportNRF24L01(8, 7, RF24_PA_LOW);
  radio.setTransmissionStatusPin(2);
  radio.setDeviceErrorStatusPin(3);  
  uint8_t source[MAC_ADDRESS_SIZE];
  radio.getMACAddress(source);
  uint8_t destination[9] = {1,2,3,4,5,6,7,8,9};
  uint64_t netid;
  netid = radio.getNetworkId();
  uint8_t secret[NETWORK_SECRET_SIZE];
  radio.getNetworkSecretKey(secret);
  uint8_t signature[25] = {'0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4'};
  uint16_t sourcenodeaddress = radio.getNodeAddress(); //0x2D;
  uint16_t destnodeaddress = 0x2E;
  uint8_t data[76] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','W','Y','Z','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9'};

  //radio.setMACAddress(source);
  //radio.setNetworkId(netid);
  //radio.setNetworkSecretKey(secret);
  //radio.setNodeAddress(sourcenodeaddress);

  radio.start();  
  Serial.println("Radio Started");

  //radio.sendDataResponse(123, 1, sourcenodeaddress, 567, signature); 
   
  Serial.println("Preparing to receive message");
  delay(10000); // we seem to need to allow some time for things to boot up ohterwise we get transmission failutres
  //is there a way to check if it is up or somethi8ng?

   radio.setExecuteFunction(&functionExecuted);

   radio.addFunctionMapperItem(45,10,1,3);
   
   //functionmapper* mappervar;
   //mappervar = (functionmapper*)malloc(sizeof(functionmapper));
   //mappervar[0].sourcenodeaddress = 45;
   //mappervar[0].sourcenotificationtype = 10;
   //mappervar[0].sourcefunctionid = 1;
   //mappervar[0].maptofunctionid = 3;
   //radio.setRemoteToLocalFunctionMapping(1,mappervar);
   
   //functionsubscription* triggervar;
   //triggervar = (functionsubscription*)malloc(sizeof(functionsubscription));
   //triggervar[0].destinationaddresses = (uint16_t*)malloc(sizeof(uint16_t));
   //triggervar[0].notificationtype = 10;
   //triggervar[0].functionid = 1;
   //triggervar[0].numberofdestinations = 1;
   //triggervar[0].destinationaddresses[0] = 48;  

   //radio.setFunctionSubscriptions(1,triggervar);
}

void loop() {
  radio.networkProcess();
  //Serial.print("RAM: ");
  //Serial.println(freeRam());  
}

