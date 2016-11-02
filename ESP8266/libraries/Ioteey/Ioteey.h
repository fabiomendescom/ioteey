#include <stdint.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

//TODO: DO MQTT LATER
//#include <PubSubClient.h> //obtained from https://github.com/knolleary/pubsubclient  <<<- Not used for now. I need to finish a complete MQTT implementation

//Hashmap below was obtained by creating a HashMap dir in libraries and pulling the following files in there
//https://github.com/WiringProject/Wiring/blob/e0656394dcbc35586bcdac4e9a2b04082187f212/framework/cores/Common/Countable.h
//I severely modified it into a specific library for ioteey
#include <HashMap.h>
#include <ESP8266mDNS.h>
#include <SSDP.h>
#include <UPnP.h>
#include <NTPtimeESP.h>
#include <EEPROM.h>
#include <ESP8266TrueRandom.h>
#include "ioteeyhtmlhelper.h"
#include <esp8266defines.h>
#include <DNSServer.h>

//Maximum number of bytes to write to EEPROM
#define MAXEEPROMWRITESIZE			2024
#define MAXKEYSIZE					80
#define MAXVALUESIZE				80

#define SSIDMAXSIZE					120
#define SSIDPASSWORDMAXSIZE			50

//TODO: DO MQTT LATER
//#define MQTTSERVERSIZE				50
//#define MQTTUSERSIZE				50
//#define	MQTTPASSWORDSIZE			50

#define NUMBEROFTIMERS				10

#define THINGID 					"00"
#define THINGNAME					"02"
#define THINGPASSWORD 				"03"
#define THINGNETWORKID				"04"
#define THINGCODE					"30"

//TODO: DO MQTT LATER
//#define THINGMQTTPORT 				"85"

#define DEVICEID					"00"
#define DEVICENAME					"02"
#define DEVICETYPE					"09"
#define DEVICECONNECTION			"32"
#define DEVICECAPABILITY			"03"
#define DEVICECOMMAND				"05"
#define DEVICECOMMANDVALUE			"07"
#define DEVICECOMMANDVALUEPARAMETER "08"
#define DEVICEATTRIBUTE				"04"
#define DEVICESETTING				"06"

#define ATTRIBUTENAME				"0"
#define ATTRIBUTEVALUE				"1"
#define ATTRIBUTETYPE				"2"

#define SETTINGNAME					"0"
#define SETTINGVALUE				"1"
#define SETTINGTYPE					"2"

//codes for the connection types
#define CONNTHINGIDKEY				"0"
#define CONNTHINGCODEKEY			"1"
#define CONNTYPEKEY					"2" 
#define CONNSENSORORACTUATORKEY		"3"
#define CONNSENSORORACTUATORIDKEY	"4"
#define CONNTYPEPARAMETERKEY		"5"

//Connection types
#define CONNTYPESERIAL				'S'
#define CONNTYPETCP					'T'
//TODO: DO MQTT LATER
//#define CONNTYPEMQTT				'M'
#define CONNTYPEBLUETOOTH			'B'
#define CONNTYPEBLANK				' '

#define PARAMETERARRAYSIZE			20
#define PARAMETERSIZE				30

//For the command callback
typedef bool execCommandCallback(const char* sourceaddress, int deviceid, int commandid, const char* command, int commandparametercount, const char commandparameternames[][PARAMETERSIZE], const char commandparametervalues[][PARAMETERSIZE]); 


class Ioteey {
	private:
		char* htmlbuffer;  // used to send http responses
		
		char _commandparameternames[PARAMETERARRAYSIZE][PARAMETERSIZE];
		char _commandparametervalues[PARAMETERARRAYSIZE][PARAMETERSIZE];
		
		const char* htmltopbeforetitle = "<html><head><title>";
		const char* htmltopaftertitle = "</title></head><style type='text/css'>.link{width:100%;background: #222;margin:6px;padding:10px} .wrapper{width:100%;font-family:Verdana, sans-serif;font-size: 50px;}fieldset {padding:20px;border-color:rgba(4, 129, 177, 0.4);} input[type='text']{border:solid 2px black;outline:solid:10px silver} input {font-size:50px;padding: 8px 0px;height: 70px;width:100%;}a {text-decoration: none;}a,input[type='submit'],input[type='button']{background: #222;border: none;color: #eee;margin: 5px 0;}.value {font-size: 60px;font-weight:bold;}.titlebig {background: #99ccff;padding: 5px 22px;}</style>";
		
		const char* htmlbodybegin =    "<body>";
		const char* containerbegin =     "<div class='wrapper'>";
		const char* containerend =       "</div><br><br>";
		const char* htmlbodyend =      "</body></html>";
		
		//templates for website responses
		char* datetimestring = "%u/%u/%u %u:%u:%u";
		char* save = "Save";
		char* postsavesetting = "/savesettings";
		char* formbegin = "<form action='%s' method='post'>";
		char* formend = "</form>";
		char* fieldsetbegin = "<fieldset>";
		char* fieldsetend = "</fieldset><br>";
		char* legendbegin  = "<legend>";
		char* legendend  = "</legend><br>";
		char* divbegin = "<div>";
		char* divend = "</div><br>";
		char* labelbegin = "<div class='label'>";
		char* labelend = "</div><br>";
		char* valuebegin = "<div class='value'>";
		char* valueend = "</div><br>";  	
		char* attributes  = "Attributes";
		char* commands  = "Commands";
		char* button = "<div><input %s style='width:100%' type='%s' name='submit' value='%s'/></div><br>"; 	
		char* settingslink = "<div class='link'><a style='width:100%' href='/settings'>Click here to go to Settings</a></div><br>";
		char* infolink = "<div class='link'><a style='width:100%' href='/'>Click here to go to back to Information</a></div><br>";
		char* input = "<input type='text' name='%s' id='%s' value='%s'/><br><br>";	
		char* linebreak = "<br>";
		char* link = "<div class='link'><a style='width:100%' href='%s'>%s</a></div><br>";
		
		bool wifidefined;
		bool wifidefinedincorrectly;
		HashMap settings;	
		bool wificonnected;
		bool defaultsmodified;
		strDateTime dateTime;
		
		int getDeviceIdByName(const char* devicename);
		
		long timerstart[NUMBEROFTIMERS];		
		bool dnsactive;
		DNSServer dnsServer;
		
		uint32_t ip;
		char devicelist[100];
		String name[20];
		String prevvalue[20];	
		long lastupdatemillis[20];
		int keepalive;		
		int accesspointport;
		bool isaccesspoint;
		WiFiClient espClient;
		bool reconnect();
		int getKeyIndex(char* key);	
		void saveValue(char* key, char* value);
		char* getValue(char* key);			
		long getValueMillis(char* key);
		ESP8266WebServer server;
		void handleRootAPConfig();
		void handleWifiSave();
		void handleSettings();
		void handleInfo();
		execCommandCallback *execCommand;
		String buildThingKey(const char* element);
		String buildDeviceKey(int deviceid, const char* element);
		String buildDeviceCapabilityKey(int deviceid, int capabilityid, const char* element);
		String buildDeviceCommandKey(int deviceid, int commandid, const char* element);
		String buildDeviceCommandValueKey(int deviceid, int commandid, int commandvalueid, const char* element);
		String buildDeviceCommandValueParameterKey(int deviceid, int commandid, int commandvalueid, int commandvalueparameterid, const char* element);
		String buildDeviceAttributeKey(int deviceid, int attributeid, const char* element);
		String buildDeviceSettingKey(int deviceid, int settingid, const char* element);		
		//TODO: DO MQTT LATER
		//bool mqttconnected;
		//int numfailedmqtttries;
		//long timelastmqttfailedtry;		
		//char mqttserver[MQTTSERVERSIZE];
		//int mqttport;
		//char mqttuser[MQTTUSERSIZE];
		//char mqttpassword[MQTTPASSWORDSIZE];
		//PubSubClient client;			
	public:			
		Ioteey();
		bool timerMillisPassed(long timer, long nummillis);
		void readSettingsFromCharArray(const char* input); 
		bool timerReset(long timer);
		String getThingCode();
		String getThingNetworkName();
		String generateUUID();	
		void dumpSettings();	
		bool connected();	
		String getThingPassword();
		void setThingPassword(const char* password);
		String handleHeader(char* name, char* action);
		void handleSaveSettings();
		String handleFooter();
		String padding(int number, int numberofzeroes);
		HashMap* getSettings();
		void setDeviceList(char* devicelist);
		String renderDeviceParameters(int deviceid);
		char* getDeviceList();
		bool loadSettings();		
		void resetSettingsToEEPROM(bool hard);
		bool connectWIFI(String SSID,String password);
		bool connectAll();
		void process();
		void loadCallbackInfo(char* topic, byte* payload, unsigned int length);
		int digitalConsistentRead(int pin);				
		void startDefaultSetting();
		void setDefaultThingSetting(String settingkey, String settingvalue);
		void setDefaultDeviceSetting(int deviceid, String settingkey, String settingvalue);
		void setDefaultDeviceParameterSetting(int deviceid, int parameternumber, char* parameterid,  char* parametername, char* parametertype, char* parametervalue);	
		void endSettingDefault();
	
        //Thing
        void createThing(); 
		void setThingSetting(String settingkey,String settingvalue);					
        
        //Devices
	    int addDevice(const char* devicename, const char* devicetype);        
		int getDeviceCount();
		String getDeviceCode(int deviceid);
		String getDeviceCode();
		String getDeviceName(int deviceid);
		String getDeviceName();
		String getDeviceType(int deviceid);
		String getDeviceType();

		//Capabilities		
		int getCapabilityCount(int deviceid);
		int getCapabilityCount();
		int addCapability(int deviceid, const char* capability);
		int addCapability(const char* capability);
		String getCapability(int deviceid, int capabilityid);
		String getCapability(int capabilityid);
		
		//Commands
		int getCommandCount(int deviceid);
		int getCommandCount();
		int addCommand(int deviceid, const char* command);
		int addCommand(const char* command);		
		String getCommand(int deviceid, int commandid);
		String getCommand(int commandid);
		//Command values
		int getCommandValueCount(int commandid); 	
		int getCommandValueCount(int deviceid, int commandid); 
		int addCommandValue(int deviceid, int commandid, const char* commandvalue);		
		int addCommandValue(int commandid, const char* commandvalue);  
		String getCommandValue(int deviceid, int commandid, int commandvalueid); 
		String getCommandValue(int commandid, int commandvalueid);   
		//Command value parameters
		int getCommandValueParameterCount(int deviceid, int commandid, int commandvalueid);		
		int getCommandValueParameterCount(int commandid,int commandvalueid); 
		int addCommandValueParameter(int deviceid, int commandid, int commandvalueid, const char* commandvalueparameter);		
		int addCommandValueParameter(int commandid, int commandvalueid, const char* commandvalueparameter);  
		String getCommandValueParameter(int deviceid, int commandid, int commandvalueid, int commandvalueparameterid); 
		String getCommandValueParameter(int commandid, int commandvalueid, int commandvalueparameterid);   
		//LOCAL Command execution
		bool executeCommand(int deviceid, int commandid, const char* commandvalue, int commandparametercount, const char commandparameternames[][PARAMETERSIZE], const char commandparametervalues[][PARAMETERSIZE]);
		bool executeCommand(int commandid, const char* commandvalue, int commandparametercount, const char commandparameternames[][PARAMETERSIZE], const char commandparametervalues[][PARAMETERSIZE]);
		bool executeCommand(int commandid, const char* commandvalue);	
		void setExecCommandCallback(execCommandCallback *_callback);
		// TODO::::
		//REMOTE Command execution
		//
		//TRANSMISSIONMETHOD::THINGNAME::DEVICEID::A or C for Command or Attribute::ATTRIBUTENAME OR COMMANDNAME
		//TRANSMISSIONMETHOD => any additional info about the addressing separates by "/". For example, serial1 woud be "Serial/1"
		//serial/1::THING::1::C:ON
		//default::THING::1::C::ON	
		int getCommandId(int deviceid, const char* commandname);						
		int getCommandId(const char* commandname);					
		bool executeRemoteCommand(const char* destinationaddress, int destinationdeviceid, const char* commandname, const char* command, const char* commandparms);//finish this
		bool executeRemoteCommand(const char* destinationaddress, int destinationdeviceid, const char* commandname, const char* command);//finish this
		bool executeRemoteCommand(const char* destinationaddress, const char* commandname, const char* command, const char* commandparms);		//finish this
		bool executeRemoteCommand(const char* destinationaddress, const char* commandname, const char* command);			//finish this
		//REMOTE Command execution per transmission method
		bool executeHTTPRemoteCommand(const char* destinationname, int destinationport, int destinationdeviceid, const char* commandname, const char* command, const char* commandparms);//finish this
		bool executeHTTPRemoteCommand(const char* destinationname, int destinationdeviceid, const char* commandname, const char* command, const char* commandparms);//finish this
		bool executeHTTPRemoteCommand(const char* destinationname, int destinationdeviceid, const char* commandname, const char* command);	//finish this
		bool executeHTTPRemoteCommand(const char* destinationname, const char* commandname, const char* command);					//finish this
		bool executeSERIALRemoteCommand(int serialportnumber, int destinationdeviceid, const char* commandname, const char* command, const char* commandparms);//finish this
				
		void test(int a, int b);
				
		//Command Links
		int getCommandLinkCount(int deviceid);//finish this
		int getCommandLinkCount();//finish this
		int addCommandLink(int deviceid, int commandid, const char* destinationaddress);//finish this
		int addCommandLink(int commandid, const char* destinationaddress);//finish this
		String getCommandLink(int deviceid, int commandid);//finish this
		String getCommandLink(int commandid);//finish this
				
		//Attributes
		int getAttributeCount(int deviceid);
		int getAttributeCount();		
		int addAttribute(int deviceid, char* attributename);
		int addAttribute(char* attributename);
		String getAttribute(int deviceid, int attributeid);
		String getAttribute(int attributeid);
		//LOCAL Attribute setting and getting
		int getAttributeId(int deviceid, const char* attributeid);
		int getAttributeId(const char* attributename);
		void setAttributeValue(int deviceid, int attributeid, const char* value); //This should set locally but then also go through Attribute links and sync them
		void setAttributeValue(int attributesid, const char* value);			
		String getAttributeValue(int deviceid, int attributeid);
		String getAttributeValue(int attributeid);	
		//REMOTE Attribute setting and getting
		void setRemoteAttributeValue(const char* destinationaddress, int destinationdeviceid, const char* attributename, const char* value);//finish this
		void setRemoteAttributeValue(const char* destinationaddress, const char* attributename, const char* value);//finish this
		String getRemoteAttributeValue(const char* destinationaddress, int destinationdeviceid, const char* attributename);//finish this
		String getRemoteAttributeValue(const char* destinationaddress, const char* attributename);		//finish this	
		
		//Attribute Links
		int getAttributeLinkCount(int deviceid); //finish this
		int getAttributeLinkCount();//finish this
		int addAttributeLink(int deviceid, int attributeid, const char* destinationaddress);//finish this
		int addAttributeLink(int attributeid, const char* destinationaddress);//finish this
		String getAttributeLink(int deviceid, int attributeid);//finish this
		String getAttributeLink(int attributeid);//finish this
		//TRANSMISSIONMETHOD::THINGNAME::DEVICEID::A or C for Command or Attribute::ATTRIBUTENAME OR COMMANDNAME
		//TRANSMISSIONMETHOD => any additional info about the addressing separates by "/". For example, serial1 woud be "Serial/1"
		//http::THING::1::A::FABIO
		//serial/1::THING::1::A::FABIO

		//Device settings
		int getSettingCount(int deviceid);
		int getSettingCount();		
		int addSetting(int deviceid, char* settingname);
		int addSetting(char* settingname);
		String getSetting(int deviceid, int settingid);
		String getSetting(int settingid);
		//Device setting values
		int addSettingValue(int deviceid, int settingid, char* value);
		int addSettingValue(int settingid, char* value);
		String getSettingValue(int deviceid, int settingid);
		String getSettingValue(int settingid);
		bool deviceValueChanges(char* deviceid, int value); //determines if this value causes a change on the previous value of this device. Used to ensure status is only sent on change not on every iteration of value		
		bool deviceValueChanges(char* deviceid, char* value); //determines if this value causes a change on the previous value of this device. Used to ensure status is only sent on change not on every iteration of value				

		void handleGetAttribute(int deviceid, int attributeid);
		void handleProcessCommand(int deviceid, int commandid, String command);

		int getDeviceParameterCount(int deviceid);
		String getThingSetting(String settingkey);	
		String getDeviceSetting(int deviceid,String settingkey);
		String getDeviceParameterIDByNumber(int deviceid,int parameternumber);		
		String getDeviceParameterNameByNumber(int deviceid,int parameternumber);
		String getDeviceParameterTypeByNumber(int deviceid,int parameternumber);
		String getDeviceParameterValueByNumber(int deviceid, int parameternumber);
		String getDeviceParameterNameByID(int deviceid, char* id);
		String getDeviceParameterTypeByID(int deviceid, char* id);
		String getDeviceParameterValueByID(int deviceid, char* id);
				
		void setDeviceSetting(int deviceid,String settingkey,String settingvalue);
		void setDeviceParameterSetting(int deviceid, int parameternumber, char* parameterid,  char* parametername, char* parametertype, char* parametervalue);	
		void setDeviceParameterSetting(int deviceid, int parameternumber, char* parametervalue);	

		int getFromDeviceConnectionCount(int deviceid);		
		void setFromDeviceConnection(int deviceid, int connectionnumber, const char* destthingid, const char* destthingcode, const char connectiontype, const char destsensororactuator, int destsensororactuatorid, const char* destconnectiontypeparm);
				
		void initializeSettings(); //remove all references. Blank it out to start from scratch.
		void saveSettingsToEEPROM();
		String readSettingsIntoStringFromEEPROM();	
		void readSettingsFromEEPROM();	
		
		void printstatus(char* status);
		void printstatus(String status);		
		void printstatus(bool status);
		  
		//TODO: DO MQTT LATER
		//void buildTopicTopLevel(char* topic);
		//void buildTopicThing(char* topic);
		//void buildTopicDevices(char* topic);
		//void buildTopicThingVar(char* topic, char* var);
		//void buildTopicDevicesVar(char* topic, char* var);		
		//void sendMessage(const char* means, const char* meansparms, const char* thingcode,const char sensororactuator, char* sensororactuatorid,const char* message, void (*callback)(char* response, char* error));
		//bool initializeThingWithMQTT();
		//bool publishDeviceValueWithMQTT(char* deviceid,char* value);
		//bool initializeDeviceWithMQTT(char* deviceid,char* initialvalue);  //sends the intial value for the device		
		//bool connectMQTT(String mqttserver,String mqttport,String mqttuser,String mqttpassword);
		//bool findAndConnectMQTT();
		//bool startWithMQTT();
		//bool publishDeviceValue(char* deviceid,char* value);		
		//bool publishDeviceValueIfNeeded(char* deviceid,char* value);
		//bool publishDeviceValue(char* deviceid,int value);
		//bool publishDeviceValueIfNeeded(char* deviceid,int value);
		
};


