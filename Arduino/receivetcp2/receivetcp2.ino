
#include "AutuinoTransportNRF24L01.h"

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
};

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
    Serial.print("Notification value size.......: ");
    Serial.println(notification->notificationvaluesize);        
    Serial.print("Notification Value............: ");
    for(int i=0;i<notification->notificationvaluesize;i++) {
        Serial.print(notification->notificationvalue[i]);
        Serial.print(":");      
    }
    Serial.println("");
}

  //Global for the network
  uint64_t netid = 0xA8A8E100000; 
  uint8_t networksecret[NETWORK_SECRET_SIZE] = {0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x0,0x0,0x0,0x0,0x0};
  uint16_t radio1address = 0x2D; 
  uint16_t radio2address = 0x2E;
  uint16_t radio3address = 0x64;

  //specific to this device
  uint8_t macaddress[MAC_ADDRESS_SIZE] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x8};
  uint16_t nodeaddress = radio3address; 
  
  uint8_t signature[25] = {'0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4'};
  uint8_t signaturesize = 32;
  uint16_t effectivedatasize = 49;  
  uint8_t effectivedata[MAX_DATA_SIZE] = {'F','a','b','i','o',' ','L','u','i','s',' ','M','a','r','q','u','e','s',' ','M','e','n','d','e','s',' ','&',' ','E','l','i','a','n','a',' ','M','a','u','r','m','a','n',' ','M','e','n','d','e','s'};

  long timer;

  AutuinoTransportNRF24L01 radio;

void setup() {
  //pinMode(9,OUTPUT);  
  Serial.begin(9600);    
  Serial.println("Starting radio");
                    
  radio = AutuinoTransportNRF24L01(8, 7, RF24_PA_LOW);
  //radio.setTransmissionStatusPin(2);
  //radio.setDeviceErrorStatusPin(3);  
  radio.setMACAddress(macaddress);
  radio.setNetworkId(netid);
  radio.setNetworkSecretKey(networksecret);
  radio.setNodeAddress(nodeaddress);
  radio.setExecuteFunction(&functionExecuted);

  radio.addRemoteToLocalFunctionMapping(radio2address,10,1,1);
  radio.addFunctionSubscription(10, 1, radio1address);

  radio.start();  
  Serial.println("Radio Started");
  Serial.println("Preparing to receive message");
  timer=millis();
}

void loop() {
  radio.networkProcess();
  //Serial.print("RAM: ");
  //Serial.println(freeRam());  
}

