#include "AutuinoTransportNRF24L01.h"

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

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
};

uint8_t hmacKey[]={
  0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b
};  


AutuinoTransportNRF24L01 radio;
miniacktransportmessage msg;

  uint8_t source[MAC_ADDRESS_SIZE];
  uint8_t destination[9] = {1,2,3,4,5,6,7,8,9};
  uint64_t netid;
  uint8_t secret[NETWORK_SECRET_SIZE];
  uint8_t signature[25] = {'0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4'};
  uint16_t sourcenodeaddress; // = radio.getNodeAddress(); //0x2D;
  uint16_t destnodeaddress = 0x2E;
  uint8_t signaturesize = 32;
  uint16_t effectivedatasize = 49;  
  uint8_t effectivedata[MAX_DATA_SIZE] = {'F','a','b','i','o',' ','L','u','i','s',' ','M','a','r','q','u','e','s',' ','M','e','n','d','e','s',' ','&',' ','E','l','i','a','n','a',' ','M','a','u','r','m','a','n',' ','M','e','n','d','e','s'};
  uint8_t parameters[4] = {'P','a','r','m'};
  
void setup() {
  pinMode(9,OUTPUT);
  Serial.begin(9600);    
  Serial.println("Starting radio");

  radio = AutuinoTransportNRF24L01(8, 7, RF24_PA_LOW);
  radio.setTransmissionStatusPin(2);
  radio.setDeviceErrorStatusPin(3);
  radio.getMACAddress(source);  
  netid = radio.getNetworkId(); //0xA8A8E100000;
  radio.getNetworkSecretKey(secret);
  sourcenodeaddress = radio.getNodeAddress(); //0x2D; 

  //radio.setMACAddress(source);
  //radio.setNetworkId(netid);
  //radio.setNetworkSecretKey(secret);
  //radio.setNodeAddress(sourcenodeaddress);

  radio.start();  
  radio.setExecuteFunction(&functionExecuted);
    
  Serial.println("Radio Started");

  Serial.println("Preparing to send message");
  
  delay(10000); // we seem to need to allow some time for things to boot up ohterwise we get transmission failutres

/*
  Sha256Class Sha256;
  Serial.println(1);
  uint8_t *hash;
  Serial.println(2);
  Sha256.initHmac(hmacKey,20); // key, and length of key in bytes
  Serial.println(3);  
  uint8_t msg[] = {"this is a message"};
  for(int i = 0;i<17;i++) {
    Sha256.write(msg[i]);
  }
  Serial.println(4);  
  hash = Sha256.resultHmac(); 
  Serial.println(5);   
  Serial.print("HASH: ");
  for(int i = 0;i<32;i++) {
    Serial.print("0x");
    Serial.print(hash[i],HEX);
    Serial.print(" ");  
  }
*/  

}

void loop() {

  radio.executeFunction(sourcenodeaddress,destnodeaddress,10,1,1,433);
  
  delay(5000);
  
  radio.networkProcess();

  //Serial.print("RAM: ");
  //Serial.println(freeRam()); 
}
