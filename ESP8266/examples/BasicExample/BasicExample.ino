/*
 * Basic Example
 * 
 * This shows a simple Ioteey working with ESP8266. The code below will make the ESP8266 into an Ioteey device. The device will have
 * the following features:
 * 1. It will have a capability to serve as an Access point so you can configure your network settings by joining its SSID
 * 2. It will broadcast its address through mDNS
 * 3. It will broadcat its address through SSDP/UPnP (it does not broacast its services though)
 * 4. It will get the time from NST
 * 5. Once joined to a network, it will have the capability to serve as a web server, allowing the user to execute commands
 *    find out attribute values and modify settings defined by you.
 * 6. It will have the capability of joining a Smartthings network and be controled by it (see other examples)
 */
 
#include <Ioteey.h>

execCommandCallback execCommandCallout;

//Instantiate the ioteey object
Ioteey iot;    

void setup() {
    Serial.begin(9600);

    //This loads the settings that are stored in EEPROM
    //This allows users to modify the settings through the web
    iot.loadSettings();  

    //****************
    //Default settings
    //****************
    //Password that will be used when the THING behaves as an Access Point 
    //Access point is used to go to a site and setup the SSID and PASSWORD to join the network
    iot.setThingPassword("1234567890");
    //Save any settings to EEPROM
    iot.saveSettingsToEEPROM();

    //A "thing" can have one or multiple devices. Set first device of name "Devicetoadd" and type "Ioteey Switch"
    //The type groups Ioteey devices. It is also used for Samsung Smartthings for example to drive behavior
    int deviceid = iot.addDevice("Devicetoadd","Ioteey Switch");

    //Add capability to the device. Follow the convention defined here:
    //http://docs.smartthings.com/en/latest/capabilities-reference.html
    iot.addCapability(deviceid, "Switch");

    //Make sure the commands defined here follow the command definitions that match the capability above
    //see http://docs.smartthings.com/en/latest/capabilities-reference.html for reference
    int commandid = iot.addCommand(deviceid, "switch");
    iot.addCommandValue(deviceid,commandid, "on");
    iot.addCommandValue(deviceid,commandid, "off"); 

    //Make sure the attributes defined here follow the command definitions that match the capability above
    //see http://docs.smartthings.com/en/latest/capabilities-reference.html for reference
    iot.addAttribute(deviceid, "switch");

    //There are settings that you require users to configure for each device. They will show up on the
    //settings page for the device
    int setting = iot.addSetting(deviceid, "somesetting");
    
    //Default settings. They only get added if they don't exist
    //Use this to store defaults unless the user changes them
    iot.addSettingValue(deviceid,setting, "10");

    //define the name of the function that will receive the data for a command coming from http or serial port
    iot.setExecCommandCallback(execCommandCallout);

    //connect everything that needs to be connected. At a minimum, it connects to WIFI
    iot.connectAll();   
}

void loop() {
    //This does a lot of things to keep ioteey devices doing what they need to do.
    //This is mandatory.
    iot.process();  

    //Always put your processing logic after confirming that you have successfully connected
    if(iot.connected()) { 
        //Your logic here
    } 
}

bool execCommandCallout(const char* sourceaddress, int deviceid, int commandid, const char* command, int commandparametercount, const char commandparameternames[][PARAMETERSIZE], const char commandparametervalues[][PARAMETERSIZE]) { 
    //Add your logic here to react to the command. The command can come from the serial port or http call. Sourceaddress
    //parameter will help distinguish it
    //After device is joined your network, test this by going to: 
    // http://<yourdeviceip>/api/Devicetoadd/commands/execute/switch/on"
    //or
    // http://<yourdeviceip>/api/Devicetoadd/commands/execute/switch/off"
    //
    //There are various ways to know your device's IP
    //1. If your device is connected through USB go to Serial monitor and type >>>MYIP
    //2. If you have a smartphone, download the app "Zentry Discovery". This app looks for mDNS broacasts. Ioteey broadcast their
    //   name. Get the IP address from there.

    if(strcmp(command,"on")==0) {
      Serial.println("Turned ON");
    } else { //off
      Serial.println("Turned OFF");
    }
        
    return true;
};


