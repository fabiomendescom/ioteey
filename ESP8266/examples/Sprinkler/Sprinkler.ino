extern "C" {
#include "user_interface.h"
}
#include <Ioteey.h>
execCommandCallback execCommandCallout;

Ioteey iot;   //sourceofrequest(http,mqtt,serial),parms,sensororactuator,actuatororsensorid,messageprocessedsuccessfully) 

#define PIN D1

long starthightime;

void setup() {
    Serial.begin(9600);
    
    pinMode(PIN,OUTPUT);
    digitalWrite(PIN,LOW);
 
    iot.loadSettings();  //load from EEPROM and put values in Hashmap in iot object

    //Default setting
    iot.setThingPassword("1234567890");
    iot.saveSettingsToEEPROM();

    iot.addDevice("Irrigationxxxx00000x","Ioteey Switch");

    iot.addCapability(0, "Switch");

    iot.addCommand(0, "Switch");
    iot.addCommandValue(0,0, "ON");
    iot.addCommandValue(0,0, "OFF"); 

    iot.addAttribute(0, "Switch");

    iot.addSetting(0, "AutoShutOff");
    
    //Default settings. They only get added if they don't exist
    iot.addSettingValue(0,0, "10");

    iot.setExecCommandCallback(execCommandCallout);

    iot.connectAll();
   
}

void loop() {
    iot.process();  
    
    if(iot.connected()) {  //only do things if you are connected
      //Automatic shutoff
      if(iot.digitalConsistentRead(PIN)==HIGH) {
        if((millis()-starthightime) > iot.getSettingValue(0,0).toInt()*60*1000) {
          digitalWrite(PIN,LOW); 
          Serial.println("AUTOMATIC SHUTOFF");
        }
      }
    } 
}

bool execCommandCallout(const char* sourceaddress, int deviceid, int commandid, const char* command, int commandparametercount, const char commandparameternames[][PARAMETERSIZE], const char commandparametervalues[][PARAMETERSIZE]) { 
    if(strcmp(command,"ON")==0) {
       digitalWrite(PIN,HIGH); 
       starthightime = millis();
       Serial.println("Turned ON");
    } else {
       digitalWrite(PIN,LOW); 
       Serial.println("Turned OFF");
    }
    
    return true;
};


