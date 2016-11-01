extern "C" {
#include "user_interface.h"
}

#include "Ioteey.h"

Ioteey::Ioteey() {	
	this->wificonnected=false;
	this->mqttconnected=false;
	this->numfailedmqtttries=0;
	this->wifidefined = false;
	this->wifidefinedincorrectly = false;
	this->dnsactive = false;
};

void Ioteey::startDefaultSetting() {
	this->defaultsmodified=false;
};

String Ioteey::generateUUID() {
	byte uuidNumber[16];
	ESP8266TrueRandom.uuid(uuidNumber);
	String uuidStr = ESP8266TrueRandom.uuidToString(uuidNumber);	
	return uuidStr;
};

String Ioteey::buildThingKey(const char* element) {
	String ret = String("T00") + String(element);
	return ret;
};

String Ioteey::buildDeviceKey(int deviceid, const char* element) {
	String ret = String("D") + this->padding(deviceid,2) + String(element);
	return ret;
};

String Ioteey::buildDeviceCapabilityKey(int deviceid, int capabilityid, const char* element) {
	String ret = String("D") + this->padding(deviceid,2) + String(element) + this->padding(capabilityid,2);
	return ret;
};

String Ioteey::buildDeviceCommandKey(int deviceid, int commandid, const char* element) {
	String ret = String("D") + this->padding(deviceid,2) + String(element) + this->padding(commandid,2);
	return ret;
};

String Ioteey::buildDeviceCommandValueKey(int deviceid, int commandid, int commandvalueid, const char* element) {
	String ret = String("D") + this->padding(deviceid,2) + String(element) + this->padding(commandid,2) + this->padding(commandvalueid,2);
	return ret;
};

String Ioteey::buildDeviceCommandValueParameterKey(int deviceid, int commandid, int commandvalueid, int commandvalueparameterid, const char* element) {
	String ret = String("D") + this->padding(deviceid,2) + String(element) + this->padding(commandid,2) + this->padding(commandvalueid,2) + this->padding(commandvalueparameterid,2);
	return ret;
};

String Ioteey::buildDeviceAttributeKey(int deviceid, int attributeid, const char* element) {
	String ret = String("D") + this->padding(deviceid,2) + DEVICEATTRIBUTE + this->padding(attributeid,2) + String(element);
	return ret;
};

String Ioteey::buildDeviceSettingKey(int deviceid, int settingid, const char* element) {
	String ret = String("D") + this->padding(deviceid,2) + DEVICESETTING + this->padding(settingid,2) + String(element);
	return ret;
};

//What is used to broadcast mDNS 
String Ioteey::getThingNetworkName() {
	if(this->getDeviceCount()>1) {
		return String("Ioteey-Multidevice-") + this->getThingCode(); 
	} else {
		return String("Ioteey-") + this->getDeviceName(0) + String("-") + this->getThingCode().substring(0,8); 
	};
};


void Ioteey::createThing() { 	    
    if(this->settings.getItem(buildThingKey(THINGID))=="") { //This means this is a totall blank flash or one that has been "HARDUNSETALL"
		String thingid = this->generateUUID();
		//add that to THINGCODE if not set
		thingid.trim();
		this->settings.addItem(buildThingKey(THINGID),thingid);
		this->settings.addItem(buildThingKey(THINGCODE),thingid);
		this->saveSettingsToEEPROM();		
		delay(300);	
	};

    if(this->settings.getItem(buildThingKey(THINGCODE))=="") { 
		this->settings.addItem(buildThingKey(THINGCODE),this->settings.getItem(buildThingKey(THINGID)));
		this->saveSettingsToEEPROM();		
		delay(300);		
	};					
};

int Ioteey::getDeviceIdByName(const char* devicename) {
	// Compare without case sensitivity
	String a1 = String(devicename);
	a1.toUpperCase();
	String a2;
	
	int id = -1;
	for(int i=0;i<this->getDeviceCount();i++) {
		String p = buildDeviceKey(i, DEVICENAME);
	    String a2 = this->settings.getItem(p);
	    a2.toUpperCase();	
		if(a1==a2) {
			return i;
		};
	};
	return id;
};

int Ioteey::addDevice(const char* devicename, const char* devicetype) {
	int highestid = this->getDeviceCount();
	if(highestid==0) { //first device
		this->createThing();
	};
	int id = this->getDeviceIdByName(devicename);
	if(id>=0) {
		return id;
    } else {
		this->settings.addItem(buildDeviceKey(highestid,DEVICEID),String(highestid));
		this->settings.addItem(buildDeviceKey(highestid,DEVICENAME),String(devicename).substring(0,20));
		this->settings.addItem(buildDeviceKey(highestid,DEVICETYPE),String(devicetype).substring(0,20));
		return highestid;
	}
};  

int Ioteey::getDeviceCount() {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = buildDeviceKey(i, DEVICENAME);
		if(this->settings.getItem(p)=="") {
			finished = true;
		} else {
			i++;
		};
	};
	return i;
};

bool Ioteey::executeCommand(int deviceid, int commandid, const char* commandvalue, int commandparametercount, const char commandparameternames[][PARAMETERSIZE], const char commandparametervalues[][PARAMETERSIZE]) {
	this->execCommand("",deviceid,commandid,commandvalue,commandparametercount,commandparameternames,commandparametervalues); //execute the pointer function defined locally in the sketch
	// TODO::::::::::::
	// HERE IS WHERE YOU LOOP THROUGH THE COMMAND SYNCS AND EXECUTE REMOTE CALLS
	//for getCommandSyncCount(int deviceid)
	//	  get the synced items
	//    executeRemoteCommand(const char* destinationaddress, int destinationdeviceid, const char* commandname, const char* command, const char* commandparms)
	//end for
};

bool Ioteey::executeCommand(int commandid, const char* commandvalue, int commandparametercount, const char commandparameternames[][PARAMETERSIZE], const char commandparametervalues[][PARAMETERSIZE]) {
	return this->executeCommand(0,commandid,commandvalue,commandparametercount,commandparameternames,commandparametervalues);
};

bool Ioteey::executeCommand(int commandid, const char* commandvalue) {
	return this->executeCommand(0,commandid,commandvalue,0,NULL,NULL);
};

String Ioteey::getThingCode() {
	return this->settings.getItem("T0030");
};

String Ioteey::getThingPassword() {
	return this->settings.getItem("T0003");
};

void Ioteey::setThingPassword(const char* password) {
	if(this->getThingPassword()=="") {  //Only save password if blank. This is to work as a default.
		this->settings.addItem("T0003",String(password));
	};
};

String Ioteey::getDeviceCode(int deviceid) {	
	return this->getThingCode();
};

String Ioteey::getDeviceCode() {
	return this->getDeviceCode(0);
};

int Ioteey::getCapabilityCount(int deviceid) {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = buildDeviceCapabilityKey(deviceid, i, DEVICECAPABILITY);
		if(this->settings.getItem(p)=="") {
			finished = true;
		} else {
			i++;
		};
	};
	return i;	
};

int Ioteey::getCapabilityCount() {
	return this->getCapabilityCount(0);
};

int Ioteey::addCapability(int deviceid, const char* capability) {
	int highestid = this->getCapabilityCount(deviceid);
	this->settings.addItem(buildDeviceCapabilityKey(deviceid,highestid,DEVICECAPABILITY),String(capability));
	return highestid;	
};

int Ioteey::addCapability(const char* capability) {
	return this->addCapability(0, capability);
};

String Ioteey::getCapability(int deviceid, int capabilityid) {
	return this->settings.getItem(buildDeviceCapabilityKey(deviceid,capabilityid,DEVICECAPABILITY));
};

String Ioteey::getCapability(int capabilityid) {
	return this->getCapability(0,capabilityid);
};

int Ioteey::getCommandCount(int deviceid) {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = buildDeviceCommandKey(deviceid, i, DEVICECOMMAND);
		if(this->settings.getItem(p)=="") {
			finished = true;
		} else {
			i++;
		};
	};
	return i;	
};

int Ioteey::getCommandCount() {
	return this->getCommandCount(0);
};

int Ioteey::addCommand(int deviceid, const char* command) {
	int highestid = this->getCommandCount(deviceid);
	this->settings.addItem(buildDeviceCommandKey(deviceid,highestid,DEVICECOMMAND),String(command));
	return highestid;	
};

int Ioteey::addCommand(const char* command) {
	return this->addCommand(0,command);
};

String Ioteey::getCommand(int deviceid, int commandid) {
	return this->settings.getItem(buildDeviceCommandKey(deviceid,commandid,DEVICECOMMAND));
};

String Ioteey::getCommand(int commandid) {
	return this->getCommand(0,commandid);
};


int Ioteey::getCommandValueCount(int deviceid, int commandid) {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = buildDeviceCommandValueKey(deviceid, commandid, i, DEVICECOMMANDVALUE);
		if(this->settings.getItem(p)=="") {
			finished = true;
		} else {
			i++;
		};
	};
	return i;	
};

int Ioteey::getCommandValueParameterCount(int deviceid, int commandid,int commandvalueid) {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = buildDeviceCommandValueParameterKey(deviceid, commandid, commandvalueid, i, DEVICECOMMANDVALUEPARAMETER);
		if(this->settings.getItem(p)=="") {
			finished = true;
		} else {
			i++;
		};
	};
	return i;
};

int Ioteey::getCommandValueCount(int commandid) {
	return this->getCommandValueCount(0,commandid);
};

int Ioteey::getCommandValueParameterCount(int commandid, int commandvalueid) {
	return this->getCommandValueParameterCount(0,commandid,commandvalueid);
};

int Ioteey::addCommandValue(int deviceid, int commandid, const char* commandvalue) {
	int highestid = this->getCommandValueCount(deviceid, commandid);
	this->settings.addItem(buildDeviceCommandValueKey(deviceid,commandid,highestid,DEVICECOMMANDVALUE),String(commandvalue));
	return highestid;	
};

int Ioteey::addCommandValueParameter(int deviceid, int commandid, int commandvalueid, const char* commandvalueparameter) {
	int highestid = this->getCommandValueParameterCount(deviceid, commandid, commandvalueid);
	this->settings.addItem(buildDeviceCommandValueParameterKey(deviceid,commandid,commandvalueid,highestid,DEVICECOMMANDVALUEPARAMETER),String(commandvalueparameter));
	return highestid;	
};


int Ioteey::addCommandValue(int commandid, const char* commandvalue) {
	return this->addCommandValue(0,commandid,commandvalue);
};

int Ioteey::addCommandValueParameter(int commandid, int commandvalueid, const char* commandvalueparameter) {
	return this->addCommandValueParameter(0,commandid,commandvalueid,commandvalueparameter);
};

String Ioteey::getCommandValue(int deviceid, int commandid, int commandvalueid) {
	return this->settings.getItem(buildDeviceCommandValueKey(deviceid,commandid,commandvalueid,DEVICECOMMANDVALUE));
};

String Ioteey::getCommandValue(int commandid, int commandvalueid) {
	return this->getCommandValue(0,commandid,commandvalueid);
};

String Ioteey::getCommandValueParameter(int deviceid, int commandid, int commandvalueid, int commandvalueparameterid) {
	return this->settings.getItem(buildDeviceCommandValueParameterKey(deviceid,commandid,commandvalueid,commandvalueparameterid,DEVICECOMMANDVALUEPARAMETER));
};

String Ioteey::getCommandValueParameter(int commandid, int commandvalueid, int commandvalueparameterid) {
	return this->getCommandValueParameter(0,commandid,commandvalueid,commandvalueparameterid);
};

int Ioteey::getAttributeCount(int deviceid) {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = this->buildDeviceAttributeKey(deviceid, i, ATTRIBUTENAME);
		if(this->settings.getItem(p)=="") {
			finished = true;
		} else {
			i++;
		};
	};
	return i;	
};

int Ioteey::getAttributeCount() {
	return this->getAttributeCount(0);
};		

int Ioteey::addAttribute(int deviceid, char* attributename) {
	int highestid = this->getAttributeCount(deviceid);
	this->settings.addItem(buildDeviceAttributeKey(deviceid,highestid,ATTRIBUTENAME),String(attributename));
	return highestid;	
};

int Ioteey::addAttribute(char* attributename) {
	return this->addAttribute(0,attributename);
};

String Ioteey::getAttribute(int deviceid, int attributeid) {
	return this->settings.getItem(buildDeviceAttributeKey(deviceid,attributeid, ATTRIBUTENAME));
};

String Ioteey::getAttribute(int attributeid) {
	return this->getAttribute(0,attributeid);
};

String Ioteey::getAttributeValue(int deviceid, int attributeid) {
	return this->settings.getItem(buildDeviceAttributeKey(deviceid,attributeid, ATTRIBUTEVALUE));
};

String Ioteey::getAttributeValue(int attributeid) {
	return this->getAttributeValue(0,attributeid);
};

void Ioteey::setAttributeValue(int deviceid, int attributeid, const char* value) {
	this->settings.addItem(buildDeviceAttributeKey(deviceid,attributeid, ATTRIBUTEVALUE),String(value));
};

void Ioteey::setAttributeValue(int attributeid, const char* value) {
	this->setAttributeValue(0,attributeid,value);
};		

int Ioteey::getSettingCount(int deviceid) {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = buildDeviceSettingKey(deviceid, i, SETTINGNAME);
		if(this->settings.getItem(p)=="") {
			finished = true;
		} else {
			i++;
		};
	};
	return i;	
};

int Ioteey::getSettingCount() {
	return this->getSettingCount(0);
};
		
int Ioteey::addSetting(int deviceid, char* settingname) {
	int highestid = this->getSettingCount(deviceid);
	this->settings.addItem(buildDeviceSettingKey(deviceid,highestid,SETTINGNAME),String(settingname));
	return highestid;	
};

int Ioteey::addSetting(char* settingname) {
	return this->addSetting(0,settingname);
};

String Ioteey::getSetting(int deviceid, int settingid) {
	return this->settings.getItem(buildDeviceSettingKey(deviceid,settingid,SETTINGNAME));
};

String Ioteey::getSetting(int settingid) {
	return this->getSetting(0,settingid);
};

int Ioteey::addSettingValue(int deviceid, int settingid, char* value) {
	if(this->getSettingValue(deviceid,settingid)==String("")) { //only add if it has not been modified by user
		this->settings.addItem(buildDeviceSettingKey(deviceid,settingid,SETTINGVALUE),String(value));
	};	
	return settingid;
};

int Ioteey::addSettingValue(int settingid, char* value) {
	return this->addSettingValue(0,settingid, value);
};

String Ioteey::getSettingValue(int deviceid, int settingid) {
	return this->settings.getItem(buildDeviceSettingKey(deviceid,settingid,SETTINGVALUE));
};

String Ioteey::getSettingValue(int settingid) {
	return this->getSettingValue(0,settingid);
};

void Ioteey::setDefaultThingSetting(String settingkey, String settingvalue) {
	String fullsetting = String("T00");
	fullsetting = fullsetting + settingkey;

	if(this->settings.addItemIfNotExist(fullsetting,settingvalue)) {
		this->defaultsmodified=true;
	};
};

void Ioteey::setThingSetting(String settingkey,String settingvalue) {
	String fullsetting = String("T00");
	fullsetting = fullsetting + settingkey;
	this->settings.addItem(fullsetting,settingvalue);
};

String Ioteey::getThingSetting(String settingkey) {
	String fullsetting = String("T00");
	fullsetting = fullsetting + settingkey;
	return this->settings.getItem(fullsetting);	
};

void Ioteey::setExecCommandCallback(execCommandCallback *_callback) {
	this->execCommand = _callback;
};

String Ioteey::padding(int number, int numberofzeroes) {
	String d = String("0");
	for(int i=0;i<numberofzeroes-1;i++) {
		d = d + String("0");
	};	
	String f = String(number);
	d = d + f;
	
	String e = d.substring(d.length()-numberofzeroes,d.length());
	return e;	
};

void Ioteey::setDefaultDeviceSetting(int deviceid, String settingkey, String settingvalue) {	
	String fullsetting = "D" + this->padding(deviceid,2) + settingkey;
	
	if(this->settings.addItemIfNotExist(fullsetting,settingvalue)) {
		this->defaultsmodified=true;
	};
};

void Ioteey::setDeviceSetting(int deviceid,String settingkey,String settingvalue) {
	String fullsetting = "D" + this->padding(deviceid,2) + settingkey;
	
	this->settings.addItem(fullsetting,settingvalue);
};

String Ioteey::getDeviceSetting(int deviceid,String settingkey) {
	String fullsetting = "D" + this->padding(deviceid,2) + settingkey;
	
	return this->settings.getItem(fullsetting);
};

void Ioteey::setDefaultDeviceParameterSetting(int deviceid, int parameternumber, char* parameterid, char* parametername, char* parametertype, char* parametervalue) {	
	String fullsetting = "D" + this->padding(deviceid,2) + "31" + this->padding(parameternumber,2);
	
	String pid = fullsetting + "0";
	String pname = fullsetting + "1";
	String ptype = fullsetting + "2";
	String pvalue = fullsetting + "3";
	
	if(this->settings.addItemIfNotExist(pid,parameterid)) {
		this->defaultsmodified=true;
	};	
	if(this->settings.addItemIfNotExist(pname,parametername)) {
		this->defaultsmodified=true;
	};
	if(this->settings.addItemIfNotExist(ptype,parametertype)) {
		this->defaultsmodified=true;
	};
	if(this->settings.addItemIfNotExist(pvalue,parametervalue)) {
		this->defaultsmodified=true;
	};	
};

void Ioteey::endSettingDefault() {
	if(this->defaultsmodified) {
		this->saveSettingsToEEPROM();
	};
};

String Ioteey::renderDeviceParameters(int deviceid) {
	ioteeyhtmlhelper helper;
	String ret;
	
    ret = ret + helper.writeRecordHeading(this->getDeviceName(deviceid).c_str());
	
	for(int i=0;i<this->getDeviceParameterCount(deviceid);i++) {
		String code = "D" + this->padding(deviceid,2) + "31" + this->padding(i,2) + "3";
		ret = ret + helper.renderParameter(code, String("D"), deviceid, i, getDeviceParameterIDByNumber(deviceid,i), getDeviceParameterNameByNumber(deviceid,i), getDeviceParameterTypeByNumber(deviceid,i), getDeviceParameterValueByNumber(deviceid,i));
	};
	return ret;
};

String Ioteey::getDeviceName(int deviceid) {
	return this->settings.getItem(buildDeviceKey(deviceid,DEVICENAME));
};

String Ioteey::getDeviceName() {
	return this->getDeviceName(0);
};

String Ioteey::getDeviceType(int deviceid) {
	return this->settings.getItem(buildDeviceKey(deviceid,DEVICETYPE));
};

String Ioteey::getDeviceType() {
	return this->getDeviceType(0);
};

int Ioteey::getDeviceParameterCount(int deviceid) {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = String("D") + this->padding(deviceid,2) + "31" + this->padding(i,2) + "0";
		if(this->settings.getItem(p)=="") {
		    finished=true;
		} else {
			i++;		
		};
	};
	return i;
};

int Ioteey::getFromDeviceConnectionCount(int deviceid) {
	bool finished = false;
	int i = 0;
	while(!finished) {
		String p = String("D") + this->padding(deviceid,2) + DEVICECONNECTION + this->padding(i,2) + CONNTHINGCODEKEY;
		if(this->settings.getItem(p)=="") {
		    finished=true;
		} else {
			i++;		
		};
	};
	return i;
};

void Ioteey::setDeviceParameterSetting(int deviceid, int parameternumber, char* parameterid,  char* parametername, char* parametertype, char* parametervalue) {
	String fullsetting = "D" + this->padding(deviceid,2) + "31" + this->padding(parameternumber,2);
	
	String pid = fullsetting + "0";
	String pname = fullsetting + "1";
	String ptype = fullsetting + "2";
	String pvalue = fullsetting + "3";
	
	this->settings.addItem(pid,parameterid);
	this->settings.addItem(pname,parametername);
	this->settings.addItem(ptype,parametertype);
	this->settings.addItem(pvalue,parametervalue);
};

void Ioteey::setDeviceParameterSetting(int deviceid, int parameternumber, char* parametervalue) {
	String fullsetting = "D" + this->padding(deviceid,2) + "31" + this->padding(parameternumber,2);
	
	String pvalue = fullsetting + "3";
	
	this->settings.addItem(pvalue,parametervalue);
};

void Ioteey::setFromDeviceConnection(int deviceid, int connectionnumber, const char* destthingid, const char* destthingcode, const char connectiontype, const char destsensororactuator, int destsensororactuatorid, const char* destconnectiontypeparm) {
	String key = String("D") + this->padding(deviceid,2) + String(DEVICECONNECTION) + this->padding(connectionnumber,2);
	
	this->settings.addItem(key+String(CONNTHINGIDKEY),String(destthingid));
	this->settings.addItem(key+String(CONNTHINGCODEKEY),String(destthingcode));
	this->settings.addItem(key+String(CONNTYPEKEY),String(connectiontype));
	this->settings.addItem(key+String(CONNSENSORORACTUATORKEY),String(destsensororactuator));
	this->settings.addItem(key+String(CONNSENSORORACTUATORIDKEY),this->padding(destsensororactuatorid,2));
	this->settings.addItem(key+String(CONNTYPEPARAMETERKEY),String(destconnectiontypeparm));	
	
};

String Ioteey::getDeviceParameterIDByNumber(int deviceid,int parameternumber) {
	String p = String("D") + this->padding(deviceid,2) + "31" +  this->padding(parameternumber,2) + "0";
	return this->settings.getItem(p);
};

String Ioteey::getDeviceParameterNameByNumber(int deviceid,int parameternumber) {
	String p = String("D") + this->padding(deviceid,2) + "31" +  this->padding(parameternumber,2) + "1";
	return this->settings.getItem(p);
};

String Ioteey::getDeviceParameterTypeByNumber(int deviceid,int parameternumber) {
	String p = String("D") + this->padding(deviceid,2) + "31" +  this->padding(parameternumber,2) + "2";
	return this->settings.getItem(p);
};

String Ioteey::getDeviceParameterValueByNumber(int deviceid, int parameternumber) {
	String p = String("D") + this->padding(deviceid,2) + "31" +  this->padding(parameternumber,2) + "3";
	return this->settings.getItem(p);
};

String Ioteey::getDeviceParameterNameByID(int deviceid, char* id) {
	int pid=-1;
	for(int i=0;i<this->getDeviceParameterCount(deviceid);i++) {
		String item = this->getDeviceParameterIDByNumber(deviceid,i);
		if(item==String(id)) {
			pid = i;
			break;
		}	
	};
	if(pid>=0) {
		return this->getDeviceParameterNameByNumber(deviceid,pid);
	} else {
		return String("");
	};	
};

String Ioteey::getDeviceParameterTypeByID(int deviceid, char* id) {
	int pid=-1;
	for(int i=0;i<this->getDeviceParameterCount(deviceid);i++) {
		String item = this->getDeviceParameterIDByNumber(deviceid,i);
		if(item==String(id)) {
			pid = i;
			break;
		}	
	};
	if(pid>=0) {
		return this->getDeviceParameterTypeByNumber(deviceid,pid);
	} else {
		return String("");
	};
};

String Ioteey::getDeviceParameterValueByID(int deviceid, char* id) {
	int pid=-1;
	for(int i=0;i<this->getDeviceParameterCount(deviceid);i++) {
		String item = this->getDeviceParameterIDByNumber(deviceid,i);
		if(item==String(id)) {
			pid = i;
			break;
		}	
	};
	if(pid>=0) {
		return this->getDeviceParameterValueByNumber(deviceid,pid);
	} else {
		return String("");
	};
};

HashMap* Ioteey::getSettings() {
	return &this->settings;
};

String Ioteey::handleHeader(char* name, char* action) {	
  ioteeyhtmlhelper helper;
 
  String ret = String(""); 
  
  ret = ret + "<html><head><body>\n";
		ret = ret + helper.startFORM(name,action);
			ret = ret + helper.startTABLE("100%", "#", "0", "0", "0");
				ret = ret + helper.startROW();
					ret = ret + helper.startCOLUMN("#FFFFFF", "left", "top", "#", "#", "", true);
						ret = ret + helper.startTABLE("100%", "#", "1", "0", "0");
							ret = ret + helper.startROW();
								ret = ret + helper.startCOLUMN("#FFFFFF", "left", "top", "#", "#", "", true);
									ret = ret + helper.writeHeading();
								ret = ret + helper.endCOLUMN();
							ret = ret + helper.endROW();
							ret = ret + helper.startROW();
								ret = ret + helper.startCOLUMN("#FFFFFF", "left", "top", "#", "#", "", true);
									ret = ret + helper.startTABLE("100%", "#", "0", "0", "4");
	return ret;								
};

String Ioteey::handleFooter() {
  ioteeyhtmlhelper helper;
  
  String ret = String("");
  
									ret = ret + helper.endTABLE();
								ret = ret + helper.endCOLUMN();
							ret = ret + helper.endROW();
						ret = ret + helper.endTABLE();
					ret = ret + helper.endCOLUMN();
				ret = ret + helper.endROW();
			ret = ret + helper.endTABLE();
		ret = ret + helper.endFORM(); 
  ret = ret + "</body></head></html>\n";
  
  return ret;
  
;}

// "<domain>/" called to enter network settings
void Ioteey::handleRootAPConfig() {
  ioteeyhtmlhelper helper;

  char ssid[SSIDMAXSIZE];
  char password[SSIDPASSWORDMAXSIZE];
  
  strcpy(ssid,this->settings.getItem("T0082").c_str());
  strcpy(password,this->settings.getItem("T0083").c_str());
  
  String ret = String("");
  
  ret = this->handleHeader("ioteeyform","wifisave");
  ret = ret + helper.writeRecordHeading("Network Configuration for thing (REPLACE THIS WITH THE SAME HTML YOU ARE USING FOR INFORMATION)");
  ret = ret + helper.writeRecordHeading(this->getThingNetworkName().c_str());
  ret = ret + helper.writeTextInput("SSID","ssid",ssid,"30","text");
  ret = ret + helper.writeTextInput("Password","password",password,"30","password");
  ret = ret + helper.writeButtonRECORD("XXSave","Save","30");																									
  ret = ret + this->handleFooter();

  this->server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  this->server.sendHeader("Pragma", "no-cache");
  this->server.sendHeader("viewport","width=320,initial-scale=2");
  this->server.sendHeader("Expires", "-1");
  
  this->server.send ( 200, "text/html", ret );	
};

// "<domain>/wifisave" Called to save the network settings to join a thing to wifi
void Ioteey::handleWifiSave() {
  ioteeyhtmlhelper helper;
  	
  char ssid[SSIDMAXSIZE];
  char password[SSIDPASSWORDMAXSIZE];
  
  strcpy(ssid,server.arg("ssid").c_str());
  strcpy(password,server.arg("password").c_str());

  String ret = String("");

  ret = this->handleHeader("ioteeyform","");

  this->settings.addItem("T0082",String(ssid));
  this->settings.addItem("T0083",String(password));
  this->settings.addItem("T0088",String("0"));
  
  this->saveSettingsToEEPROM();

  ret = ret + helper.writeRecordHeading("Network Configuration successfully saved for thing");
  ret = ret + helper.writeRecordHeading(this->getThingNetworkName().c_str());
  ret = ret + "<br><br>Ioteey device is being reset and will join the " + ssid + " network. Your device should automatically switch to the " + ssid + " network. If it doesn't, switch it manually in your WIFI settings";
  																						
  ret = ret + this->handleFooter();

  this->server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  this->server.sendHeader("Pragma", "no-cache");
  this->server.sendHeader("viewport","width=320,initial-scale=2");
  this->server.sendHeader("Expires", "-1");

  this->server.send ( 200, "text/html", ret );	
  
  ESP.reset();

};

void Ioteey::handleInfo() {	
	char tmp[400];
	char tmp2[400];
	
	char* title  = "Ioteey Information";
			
  	yield();
  				  	
  	long memsize = 	strlen(this->htmltopbeforetitle) + 1 + \
					strlen(title) + 1 + \
					strlen(this->htmltopaftertitle) + 1 + \
					strlen(this->htmlbodybegin) + 1 + \
					strlen(this->containerbegin) + 1 + \
					strlen(this->containerend) + 1 + \
					strlen(this->htmlbodyend) + 1;
	
	memsize = (memsize*sizeof(char)) + 5000; //this 4000 hardcoding needs to change. See #1 below
	long remainingmem = memsize;
					
  	//allocate buffer size and build page
  	this->htmlbuffer = (char*)malloc(memsize);
  	
  	//#1 - TODO:  you need to add logic to go through all of the loops and count how much memroy is needed and then allocate it properly.
  	//However it is best to do this once you have the whole html ready so you do it once only.
  	
  	strcpy(this->htmlbuffer, this->htmltopbeforetitle);  	
  	strcat(this->htmlbuffer, title);   
  	strcat(this->htmlbuffer, this->htmltopaftertitle);
  	strcat(this->htmlbuffer, this->htmlbodybegin);
	strcat(this->htmlbuffer, this->containerbegin);
	strcat(this->htmlbuffer, this->fieldsetbegin);
	strcat(this->htmlbuffer, this->settingslink);
	strcat(this->htmlbuffer, this->labelbegin);	
	//Make this better. This 1. cannot be fixed time, use millis() to add to the latest knnown time so we have accurate time. 2. havea  setting for 
	//tiemzone and daylight or something so we can show in different zones. Default to UTC or something	
	sprintf(tmp,this->datetimestring,this->dateTime.year, this->dateTime.month, this->dateTime.day, this->dateTime.hour, this->dateTime.minute, this->dateTime.second);
	strcat(this->htmlbuffer, tmp);
	strcat(this->htmlbuffer, this->labelend);	
	strcat(this->htmlbuffer, this->labelbegin);
	strcat(this->htmlbuffer, this->getThingNetworkName().c_str());
	strcat(this->htmlbuffer, this->labelend);
	strcat(this->htmlbuffer, this->fieldsetend);
    strcat(this->htmlbuffer, this->containerend);

    for(int deviceid = 0; deviceid<this->getDeviceCount(); deviceid++) {  	
  	  strcat(this->htmlbuffer, this->containerbegin);
      strcat(this->htmlbuffer, this->fieldsetbegin);
      strcat(this->htmlbuffer, this->legendbegin);      
	  strcat(this->htmlbuffer, this->getDeviceName(deviceid).c_str());
	  strcat(this->htmlbuffer, this->legendend);
	  strcat(this->htmlbuffer, this->fieldsetbegin);
      strcat(this->htmlbuffer, this->legendbegin);
      strcat(this->htmlbuffer, this->attributes);
      strcat(this->htmlbuffer, this->legendend);
							  
	  for(int attributeid=0;attributeid<this->getAttributeCount(deviceid);attributeid++) {
		strcat(this->htmlbuffer, this->labelbegin);  
		strcat(this->htmlbuffer,this->getAttribute(deviceid,attributeid).c_str());	
		strcat(this->htmlbuffer, this->labelend);	
		strcat(this->htmlbuffer, this->valuebegin);
		strcat(this->htmlbuffer,this->getAttributeValue(deviceid,attributeid).c_str());
		strcat(this->htmlbuffer, this->valueend);
		yield();
	  };

	  strcat(this->htmlbuffer, this->fieldsetend);

	  strcat(this->htmlbuffer, this->fieldsetbegin);
      strcat(this->htmlbuffer, this->legendbegin);
      strcat(this->htmlbuffer, this->commands);
      strcat(this->htmlbuffer, this->legendend);
      	  	
	  for(int commandid=0;commandid<this->getCommandCount(deviceid);commandid++) {
	      strcat(this->htmlbuffer, this->divbegin);
	      strcat(this->htmlbuffer, this->labelbegin);  
		  strcat(this->htmlbuffer,this->getCommand(deviceid,commandid).c_str());
		  strcat(this->htmlbuffer, this->labelend);	
		  
		  for(int commandvalueid=0;commandvalueid<this->getCommandValueCount(deviceid,commandid);commandvalueid++) {			
				strcpy(tmp2,"onClick=\"location.href='/api/");
				strcat(tmp2,this->getDeviceName(deviceid).c_str());
				strcat(tmp2,"/commands/execute/");
				strcat(tmp2,this->getCommand(deviceid,commandid).c_str());
				strcat(tmp2,"/");
				strcat(tmp2,this->getCommandValue(deviceid,commandid,commandvalueid).c_str());
				strcat(tmp2,"?'+");
				for(int commandvalueparameterid=0;commandvalueparameterid<this->getCommandValueParameterCount(deviceid,commandid,commandvalueid);commandvalueparameterid++) {
					strcat(this->htmlbuffer, this->labelbegin);
					strcat(this->htmlbuffer, getCommandValueParameter(deviceid,commandid,commandvalueid,commandvalueparameterid).c_str());
					strcat(this->htmlbuffer, this->labelend);
					
					sprintf(tmp,this->input,this->buildDeviceCommandValueParameterKey(deviceid, commandid, commandvalueid, commandvalueparameterid,DEVICECOMMANDVALUEPARAMETER).c_str(),buildDeviceCommandValueParameterKey(deviceid, commandid, commandvalueid, commandvalueparameterid,DEVICECOMMANDVALUEPARAMETER).c_str(),"");
					strcat(this->htmlbuffer,tmp);
					
					strcat(tmp2,"'");
					strcat(tmp2,this->getCommandValueParameter(deviceid, commandid, commandvalueid, commandvalueparameterid).c_str());
					strcat(tmp2,"='+");
					strcat(tmp2, "document.getElementById('");
					strcat(tmp2,this->buildDeviceCommandValueParameterKey(deviceid, commandid, commandvalueid, commandvalueparameterid,DEVICECOMMANDVALUEPARAMETER).c_str());
					strcat(tmp2,"').value");
					if(commandvalueparameterid!=this->getCommandValueParameterCount(deviceid,commandid,commandvalueid)-1) {
						strcat(tmp2,"+'&'+");
					};
				};
				strcat(tmp2,";\"");
				sprintf(tmp,this->button,tmp2,"button",this->getCommandValue(deviceid,commandid,commandvalueid).c_str());	  		     
				strcat(this->htmlbuffer, tmp);
		  };
		  
	      strcat(this->htmlbuffer, this->divend);
		  yield();
	  }; 
	   	 
	  strcat(this->htmlbuffer, this->fieldsetend);

	  strcat(this->htmlbuffer, this->containerend);    
	 };
	 
    strcat(this->htmlbuffer, this->htmlbodyend);

  this->server.sendHeader("Connection", "close");
  
  this->server.send ( 200, "text/html", this->htmlbuffer );	

  yield();
	
  free(this->htmlbuffer);

};

// "<domain>/settings" Called when configuring settings on the thing after it is in a network
void Ioteey::handleSettings() {
	char tmp[400];
	
	char* title  = "Ioteey Settings";
			
  	yield();
  				  	
  	long memsize = 	strlen(this->htmltopbeforetitle) + 1 + \
					strlen(title) + 1 + \
					strlen(this->htmltopaftertitle) + 1 + \
					strlen(this->htmlbodybegin) + 1 + \
					strlen(this->containerbegin) + 1 + \
					strlen(this->containerend) + 1 + \
					strlen(this->htmlbodyend) + 1;
	
	memsize = (memsize*sizeof(char)) + 5000; //TODO: this 5000 hardcoding needs to change. See #1 below
	long remainingmem = memsize;
					
  	//allocate buffer size and build page
  	this->htmlbuffer = (char*)malloc(memsize);
  	
  	//#1 - TODO: you need to add logic to go through all of the loops and count how much memroy is needed and then allocate it properly.
  	//However it is best to do this once you have the whole html ready so you do it once only.
  	  	
  	strcpy(this->htmlbuffer, this->htmltopbeforetitle);  	
  	strcat(this->htmlbuffer, title);   
  	strcat(this->htmlbuffer, this->htmltopaftertitle);
  	strcat(this->htmlbuffer, this->htmlbodybegin);
	strcat(this->htmlbuffer, this->containerbegin);
	strcat(this->htmlbuffer, this->infolink);	
	
    strcat(this->htmlbuffer, this->containerend);
	
	sprintf(tmp,this->formbegin,this->postsavesetting);
	strcat(this->htmlbuffer,tmp);

    for(int deviceid = 0; deviceid<this->getDeviceCount(); deviceid++) {  	
  	  strcat(this->htmlbuffer, this->containerbegin);
      strcat(this->htmlbuffer, this->fieldsetbegin);
      strcat(this->htmlbuffer, this->legendbegin);      
	  strcat(this->htmlbuffer, this->getDeviceName(deviceid).c_str());
	  strcat(this->htmlbuffer, this->legendend);
							  
      for(int setting=0;setting<this->getSettingCount();setting++) {
		 strcat(this->htmlbuffer, this->labelbegin);
		 strcat(this->htmlbuffer, this->getSetting(deviceid,setting).c_str());
		 strcat(this->htmlbuffer, this->labelend);
		 strcat(this->htmlbuffer, this->valuebegin); 		 
		 sprintf(tmp,this->input,this->buildDeviceSettingKey(deviceid, setting, SETTINGVALUE).c_str(),this->buildDeviceSettingKey(deviceid, setting, SETTINGVALUE).c_str(),this->getSettingValue(deviceid,setting).c_str());
		 strcat(this->htmlbuffer,tmp);
		 strcat(this->htmlbuffer, this->valueend);
	  };	  	
	   	 
	  strcat(this->htmlbuffer, this->fieldsetend);

	  strcat(this->htmlbuffer, this->containerend);    
	 };
	 
	sprintf(tmp,this->button,"","submit",this->save); 
	strcat(this->htmlbuffer,tmp);
	strcat(this->htmlbuffer, this->formend); 
    strcat(this->htmlbuffer, this->htmlbodyend);

  this->server.sendHeader("Connection", "close");
  
  this->server.send ( 200, "text/html", this->htmlbuffer );	

  yield();
	
  free(this->htmlbuffer);

};


// "<domain>/savesettings" called by handleSettings to save thing info after it has been connected to network.
void Ioteey::handleSaveSettings() {
	char tmp[300];
	
	char* title  = "Ioteey Settings";
			
  	yield();
  				  	
  	long memsize = 	strlen(this->htmltopbeforetitle) + 1 + \
					strlen(title) + 1 + \
					strlen(this->htmltopaftertitle) + 1 + \
					strlen(this->htmlbodybegin) + 1 + \
					strlen(this->containerbegin) + 1 + \
					strlen(this->containerend) + 1 + \
					strlen(this->htmlbodyend) + 1;
	
	memsize = (memsize*sizeof(char)) + 5000; //TODO: this 5000 hardcoding needs to change. See #1 below
	long remainingmem = memsize;
					
  	//allocate buffer size and build page
  	this->htmlbuffer = (char*)malloc(memsize);
  	
  	//#1 - TODO: you need to add logic to go through all of the loops and count how much memroy is needed and then allocate it properly.
  	//However it is best to do this once you have the whole html ready so you do it once only.
  	  	
  	strcpy(this->htmlbuffer, this->htmltopbeforetitle);  	
  	strcat(this->htmlbuffer, title);   
  	strcat(this->htmlbuffer, this->htmltopaftertitle);
  	strcat(this->htmlbuffer, this->htmlbodybegin);
	strcat(this->htmlbuffer, this->containerbegin);
	
    strcat(this->htmlbuffer, this->containerend);
    strcat(this->htmlbuffer, this->containerbegin);
	strcat(this->htmlbuffer, "Thing Settings successfully saved for ");
	strcat(this->htmlbuffer, this->getThingNetworkName().c_str());	
	strcat(this->htmlbuffer, this->linebreak);
	strcat(this->htmlbuffer, this->linebreak);
	strcat(this->htmlbuffer, this->settingslink);																								
	strcat(this->htmlbuffer, this->infolink);	
	strcat(this->htmlbuffer, this->containerend);
    strcat(this->htmlbuffer, this->htmlbodyend);
  
	for(int i=0;i<server.args();i++) {
		if(server.argName(i) != String("submit")) {  
		  this->settings.addItem(server.argName(i),server.arg(i));
	    };
		yield();
	};
  
	this->saveSettingsToEEPROM();  
  
	this->server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	this->server.sendHeader("Pragma", "no-cache");
	this->server.sendHeader("viewport","width=320,initial-scale=2");
	this->server.sendHeader("Expires", "-1");

	this->server.send ( 200, "text/html", this->htmlbuffer );	
	free(this->htmlbuffer);
};

char* Ioteey::getDeviceList() {
	return this->devicelist;
};

bool Ioteey::loadSettings() {
	EEPROM.begin(MAXEEPROMWRITESIZE);
	
	this->readSettingsFromEEPROM();
    		
	if(this->settings.getItem("T0082")!="") {
	   this->wifidefined = true;
	};
	if(this->settings.getItem("T0088")=="1") {
		this->wifidefinedincorrectly=true;
	} else if (this->settings.getItem("T0088")=="0") {
		this->wifidefinedincorrectly=false;	
	} else {
		this->wifidefinedincorrectly=false;
	};
	
};

void Ioteey::handleGetAttribute(int deviceid, int attributeid) {
  
  this->server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  this->server.sendHeader("Pragma", "no-cache");
  this->server.sendHeader("Expires", "-1");
  this->server.send ( 200, "text/plain", this->getAttributeValue(deviceid,attributeid) );

};

void Ioteey::handleProcessCommand(int deviceid, int commandid, String command) {
  this->server.send ( 200, "text/html", String(command) );	
  
  //Get parameters from the call
  int totalparameters = 0;
  for(int i=0;i<server.args()&&i<PARAMETERARRAYSIZE;i++) {
	  strcpy(this->_commandparameternames[i], server.argName(i).c_str());
	  strcpy(this->_commandparametervalues[i], server.arg(i).c_str());
	  totalparameters++;
	  Serial.print(server.argName(i).c_str());
	  Serial.print(server.arg(i).c_str());
  };
	
  this->executeCommand(deviceid, commandid, command.c_str(), totalparameters,this->_commandparameternames,this->_commandparametervalues);  
  
};


bool Ioteey::connectAll() {
	bool error = false;
	
	if(this->wifidefined && !this->wifidefinedincorrectly) {
				this->dnsactive = false;
				
				WiFi.mode(WIFI_OFF);
				delay(100);
				WiFi.mode(WIFI_STA);

				if(this->connectWIFI(this->settings.getItem("T0082"),this->settings.getItem("T0083"))) {
					Serial.print("Setup on IP ");
					Serial.println(WiFi.localIP());
					
					//Go over all devices
					for(int i=0;i<this->getDeviceCount();i++) {
						//Go over the attribute URLs
						for(int j=0;j<this->getAttributeCount(i);j++) {
							String url = "/api/" + this->getDeviceName(i) + "/attributes/get/" + this->getAttribute(i,j);
							Serial.println(url);
							this->server.on(url.c_str(),std::bind(&Ioteey::handleGetAttribute, this, i ,j));
						};		
						//Go over command URLs
						for(int j=0;j<this->getCommandCount(i);j++) {
							Serial.println(this->getCommandValueCount(i,j));
							for(int k=0;k<this->getCommandValueCount(i,j);k++) {
								String url = "/api/" + this->getDeviceName(i) + "/commands/execute/" + this->getCommand(i,j) + "/" + this->getCommandValue(i,j,k);								
								Serial.println(url);
								this->server.on(url.c_str(),std::bind(&Ioteey::handleProcessCommand, this, i ,j, this->getCommandValue(i,j,k)));
							};
						};											
					};	
					this->server.on("/", std::bind(&Ioteey::handleInfo, this)); 
					this->server.on("/settings", std::bind(&Ioteey::handleSettings, this));
					this->server.on("/savesettings", std::bind(&Ioteey::handleSaveSettings, this));
					
					// Setup MDNS responder
					if(1) { //if mDNS is enabled
							if (!MDNS.begin(this->getThingNetworkName().c_str())) {
								Serial.print("Error setting up MDNS responder on ");
								Serial.print(this->getThingNetworkName().c_str());
								Serial.println(".local");
							} else {
								Serial.print("mDNS responder started on ");
								Serial.print(this->getThingNetworkName().c_str());
								Serial.println(".local");						
								// Add service to MDNS-SD
								MDNS.addService("http", "tcp", 80);															
							}
					}
					if(1) { // if UPnP is enabled
						char uuid[100];
						char *deviceURN = "urn:schemas-upnp-org:device:Ioteey:1";
						uint32_t chipId = ESP.getChipId();
						//                      8 2 6 6  E X  D
						sprintf(uuid, "38323636-4558-4dda-9188-cda0e6%02x%02x%02x",
						(uint16_t) ((chipId >> 16) & 0xff),
						(uint16_t) ((chipId >>  8) & 0xff), 
						(uint16_t)   chipId        & 0xff  );
  
						//TOO MUCH hardcoding. Fix this
						SSDP.begin(uuid, "urn:schemas-upnp-org:device:Ioteey:1", "model name", "model number");
						UPnP.begin(&this->server, uuid,  this->getDeviceName(0).c_str(), this->getDeviceType(0).c_str(), deviceURN, "model name", "model number", this->getThingNetworkName().c_str(), "123", "fabio man", "fabio.com", "model.com");					
					}
					//Get internet time	
					NTPtime NTPch("ch.pool.ntp.org");
					this->dateTime = NTPch.getNTPtime(-7.0, 0);	
					NTPch.printDateTime(this->dateTime);				
										
					// Web server start					
					this->server.begin(); 

				    if(!this->findAndConnectMQTT()) {
						this->timerReset(0); //start a timer to try to connect in the loop
					};	
				    		
				} else {
					//TODO: Technically this is supposed to be "T0088,"1" however, I think I want this trying indefinetely.
					this->settings.addItem("T0088","0"); //indicates WIFI was incorrectly defined. Go back to AP mode so user can fix it
					this->saveSettingsToEEPROM();	
					this->wifidefinedincorrectly=true;
					ESP.reset();
					return false;
				};
	} else {
		WiFi.mode(WIFI_OFF);
		delay(100);
		WiFi.mode(WIFI_AP);
		char ssid[SSIDMAXSIZE];
		char password[SSIDPASSWORDMAXSIZE];		
		strcpy(password,this->getThingPassword().c_str());
		strcpy(ssid,this->getThingNetworkName().c_str());
		Serial.println("");
		Serial.println("");
		Serial.print("Starting AP Network SSID: ");
		Serial.print(ssid);
		Serial.println("...");
		Serial.print("PASSWORD: ");
		Serial.print(password);
		Serial.println("...");
		WiFi.softAP(ssid, password);
		delay(200);	
		//When you are AP, fixed IP 192.168.4.1	
		IPAddress apIP(192, 168, 4, 1);
		IPAddress netMsk(255, 255, 255, 0);		
		WiFi.disconnect();
		delay(1000);
		WiFi.softAPConfig(apIP, apIP, netMsk);
		delay(2000);
							
		this->dnsactive = true;					
		this->dnsServer.setTTL(300);
		this->dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
		const byte DNS_PORT = 53;
		this->dnsServer.start(DNS_PORT, "ioteey.net", apIP);		
			
		delay(500); // Without delay I've seen the IP address blank
		this->server.on("/", std::bind(&Ioteey::handleRootAPConfig, this));
		this->server.on("/wifisave", std::bind(&Ioteey::handleWifiSave, this));
		this->server.begin(); // Web server start
		if(this->settings.getItem("T0088")=="1") {
			Serial.print("Too many attempts to connect to WIFI. If you believe you have the right settings, connect to the AP for the device and save the settings");
		} else {									
			Serial.print("No WIFI SSID defined. In WIFI STATION Mode for setup. Connect to network ");
		}
		Serial.print(ssid);			
		Serial.print(". Open browser on ioteey.net");
				
		return false; //returns false, so things are not initialized			
	};
};

bool Ioteey::connected() {
	return (WiFi.status() == WL_CONNECTED);
}

bool Ioteey::connectWIFI(String SSID,String password) {
	PubSubClient tclient(this->espClient);
	this->client = tclient;
	WiFi.mode(WIFI_STA);   

	int numtries = 0;
	int numwaits = 0;
		
	while((WiFi.status() != WL_CONNECTED)) 
	{
		if(numtries>2) {
			Serial.println("Unable to connect to WIFI. LAST ATTEMPT");
			return false;
		};
		
		WiFi.begin(SSID.c_str(), password.c_str());
	
		Serial.println("");
		Serial.print("connecting to WIFI '");
		Serial.print(SSID.c_str());
		Serial.print("' <");
		Serial.print(password.c_str());
		Serial.println(">");

		delay(5000);
		// Use the WiFi.status() function to check if the ESP8266
		// is connected to a WiFi network.
		while (WiFi.status() != WL_CONNECTED)
		{  
			Serial.print("Wait: ");
			Serial.println(numwaits+1);
			numwaits = numwaits + 1;
			if(numwaits>=2) {
				Serial.println("Unable to connect to WIFI");
				numwaits=0;
				break;
			};
			delay(2000);
		};
		numtries = numtries + 1;
	};
		
	this->wificonnected=true;
	this->ip = WiFi.localIP();
	Serial.println("WIFI connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());  	  
};

bool Ioteey::findAndConnectMQTT() {
  int n = MDNS.queryService("http", "tcp"); // Send out query for esp tcp services
  if (n == 0) {
    Serial.println("no services found");
  }
  else {
    for (int i = 0; i < n; ++i) {
      if(MDNS.hostname(i)=="IOTEEYSERVICE" && MDNS.port(i)==1883) {
	      Serial.println("Found IOTEEYSERVICE server.");	      
		  if(this->connectMQTT(MDNS.IP(i).toString(),String(MDNS.port(i)),this->settings.getItem("T0086"),this->settings.getItem("T0087"))) {
			//return true;
		  } else {
			//return false;
		  };	      	      	      
	  };
    }
  }
  Serial.println();		
};  

bool Ioteey::timerMillisPassed(long timer, long nummillis) {
	return (millis()-this->timerstart[timer])>nummillis;
};

bool Ioteey::timerReset(long timer) {
	this->timerstart[timer] = millis();
};

void Ioteey::process() {
	
	if(this->dnsactive) {
		this->dnsServer.processNextRequest();
	}
	
	// if not connected to mqtt then try every 30 seconds
	if(this->wificonnected && !this->mqttconnected && this->timerMillisPassed(0,30*1000)) {		
		this->findAndConnectMQTT();
		this->timerReset(0);
	};
	yield();

	//Keep trying to connect
	if(!this->wificonnected) {
		this->connectWIFI(this->settings.getItem("T0082"),this->settings.getItem("T0083"));	
	}

	//handle web calls
	this->server.handleClient();
	yield();

	// handle MQTT calls
	if(this->wificonnected && this->mqttconnected) {
		if(this->client.connected()) {
			this->client.loop();
		};
	};	
	yield();
	
	//Check if MQTT server went down every 30 seconds and degrade service
	if(this->mqttconnected && this->timerMillisPassed(1,30*1000)) {
		if (!this->client.connected()) {
			Serial.println("Lost connection with MQTT server. Trying to re-establish");
			this->reconnect();
		};	
		this->timerReset(1);
	};
	yield();
     
     //See if there are serial messages to process
     if(Serial.available()>0) {
	    String d = Serial.readString();	    
	    //Commands are preceeded by >>>. Check if it is a command first, otherwise ignore
	    if(d.substring(0,3)==">>>") {
			String command = d.substring(3,d.length()-1);		
			if(command.substring(0,4)=="HELP") {
				Serial.println("WIFISTATUS");
				Serial.println("MQTTSTATUS");
				Serial.println("WHOAMI");
				Serial.println("DUMPEEPROM");
				Serial.println("RESET");
				Serial.println("HARDRESET");
				Serial.println("HARDRESETALL");
				Serial.println("UNSETBUTNET");
				Serial.println("UNSET <var>");
				Serial.println("UNSETALL");
				Serial.println("REMOVEFROMNETWORK");
				Serial.println("SET <var>:<value>");
				Serial.println("GET <var>");
				Serial.println("AVAILMEM");
				Serial.println("MYIP");
				Serial.println("ERRORLOG");
			};
			if(command.substring(0,10)=="WIFISTATUS") {
				this->printstatus(this->wificonnected);	
				this->printstatus("OK");	
			};
			if(command.substring(0,10)=="MQTTSTATUS") {
				this->printstatus(this->mqttconnected);	
				this->printstatus("OK");	
			};
			if(command.substring(0,6)=="WHOAMI") { //sets info about the thing			
				this->printstatus("START");
				for (int i = 0; i < this->settings.size(); i++){
						String tmp = String(this->settings.keyAt(i)) + ":(" + String(this->settings.valueAt(i).length()) +  ") [" + this->settings.valueAt(i) + "]";
						this->printstatus(tmp);
						yield();
				}; 
				this->printstatus("OK");
			};
			if(command.substring(0,11)=="ERASECONFIG") {
				WiFi.softAPdisconnect(true);
				WiFi.disconnect(true);		
				Serial.println("Erasing all configuration");
				ESP.eraseConfig();		
				this->printstatus("OK");		
			}
			if(command.substring(0,10)=="DUMPEEPROM") {  // soft reset
				this->printstatus("OK");
				String res = this->readSettingsIntoStringFromEEPROM();
				Serial.print("(");
				Serial.print(res.length());
				Serial.print(") ");		
				Serial.print("[[[");		
				Serial.print(res);
				Serial.println("]]]");
			};			
			if(command.substring(0,5)=="RESET") {  // soft reset
				this->printstatus("OK");				
				ESP.reset();
			};
			if(command.substring(0,9)=="HARDRESET") {  // hard reset
				this->printstatus("OK");
			};			
			if(command.substring(0,12)=="HARDUNSETALL") {  // unset all values to blank as if it is original. This even resets teh Thing ID
				this->resetSettingsToEEPROM(true);		
				this->printstatus("OK");						
			};		
			if(command.substring(0,17)=="REMOVEFROMNETWORK") {
			    this->settings.remove("T0082");
			    this->settings.remove("T0083");
				this->settings.remove("T0088");
				this->saveSettingsToEEPROM();
				this->printstatus("OK");
			};		
			if(command.substring(0,8)=="UNSETALL") {  // unset all values to blank as if it is original. This even resets teh Thing ID
				this->resetSettingsToEEPROM(false);		
				this->printstatus("OK");						
			};		
			if(command.substring(0,11)=="UNSETBUTNET") {  // unset all values to blank as if it is original. This even resets teh Thing ID
				for(int i=0;i<this->settings.size();i++) {
					if( this->settings.keyAt(i)==String("T0003") ||\
					    this->settings.keyAt(i)==String("T0000") ||\
					    this->settings.keyAt(i)==String("T0030") ||\
					    this->settings.keyAt(i)==String("T0082") ||\
					    this->settings.keyAt(i)==String("T0083") ||\
					    this->settings.keyAt(i)==String("T0088") ) {
					} else {
						this->settings.remove(this->settings.keyAt(i));
					}
				};
				this->saveSettingsToEEPROM();		
				this->printstatus("OK");						
			};					
			if(command.substring(0,6)=="UNSET ") {  // set values
				String content = command.substring(6,command.length()-1);
				this->settings.remove(content);
				this->saveSettingsToEEPROM();	
				this->printstatus("OK");							
			};										
			if(command.substring(0,3)=="SET") {  // set values
				String content = command.substring(4,command.length()-1);
				this->readSettingsFromCharArray(content.c_str());
				this->saveSettingsToEEPROM();	
				this->printstatus("OK");							
			};		

			if(command.substring(0,3)=="GET") {  // set values
				String content = command.substring(4,command.length()-1);
				this->printstatus(this->settings.getItem(content));
				this->printstatus("OK");				
			};		
			if(command.substring(0,8)=="AVAILMEM") {
				uint32_t freex = system_get_free_heap_size();
				Serial.print("<<<");
				Serial.println(freex);
			};
			if(command.substring(0,4)=="MYIP") {
				Serial.print("<<<");
				if(this->wificonnected) {
					Serial.println(WiFi.localIP());
				} else {
					Serial.println("N/A");
				}
			};
								
			if(command.substring(0,3)=="ERRORLOG") {  //show error log entries stored in thing
				//TODO: THis needs some work
				this->printstatus("OK");	
			};						
		};
	 };
	
	 yield();  
};

void Ioteey::printstatus(String status) {
	Serial.print("<<<");
	Serial.println(status);
};

void Ioteey::printstatus(char* status) {
	Serial.print("<<<");
	Serial.println(status);
};

void Ioteey::printstatus(bool status) {
	Serial.print("<<<");
	Serial.println(status);
};

int Ioteey::digitalConsistentRead(int pin) {
	int starttest = millis();
	long lowcount=0;
	long highcount=0;
	while((millis()-starttest) < 300) {
		yield();
		if(digitalRead(pin)==HIGH) {
			highcount++;
		} else {
			lowcount++;
		};
	};
	if(highcount>lowcount) {
		return HIGH;
	} else {
		return LOW;
	};
};

bool Ioteey::deviceValueChanges(char* deviceid, char* value) {
	char tmp[50]="";
	strcat(tmp,"device-");
	strcat(tmp,deviceid);
	
	if(strcmp(this->getValue(tmp),value)==0) {
		return false;
	} else {
		return true;
	};
};

int Ioteey::getKeyIndex(char* key) {
	String intkey = String(key);
	int res = -1;
	for(int i=0;i<this->name->length();i++) {
		if(intkey.compareTo(this->name[i])==0) {
			res = i;
			break;
		};
	};
	return res;
};	

void Ioteey::saveValue(char* key, char* value) {
	int index = this->getKeyIndex(key);
	if(index>=0) {
		this->prevvalue[index] = String(value);
		this->lastupdatemillis[index] = millis();
	} else {
		//look for next available slot
	    for(int i=0;i<20;i++) {
			if(this->name[i].compareTo(String(""))==0) {
				this->name[i] = String(key);
				this->prevvalue[i] = String(value);
				this->lastupdatemillis[i] = millis();
				break;
			};
		};
	};
};

char* Ioteey::getValue(char* key) {
	int index = this->getKeyIndex(key);
	if(index>=0) {
		char res[20];
		this->prevvalue[index].toCharArray(res,20);
		return res;
	} else {
		return "";
	};
};

long Ioteey::getValueMillis(char* key) {
	int index = this->getKeyIndex(key);
	if(index>=0) {
		return this->lastupdatemillis[index];
	} else {
		return 0;
	};
};

bool Ioteey::publishDeviceValueIfNeeded(char* deviceid,char* value) {
	if(this->deviceValueChanges(deviceid,value)) {
		Serial.print("Value changed to ");
		Serial.println(value);
		if(this->mqttconnected) {
			return this->publishDeviceValue(deviceid,value);
		} else {
			return false;
		};
	} else {
		if(this->mqttconnected) {
			//check if we are approaching the keepalive time. We don't want an OFFLINE notice, so we update anyways even if it is with same value
			char key[50]="";
			strcat(key,"device-");
			strcat(key,deviceid);
			long differenceinsecs = (millis() - this->getValueMillis(key)) / 1000;	
			long threshold = this->keepalive - 10; //10 seconds to spare
			if(differenceinsecs >= threshold) { //do this 10 seconds before the keep alive
				Serial.print("getValueMillis(key)): ");
				Serial.println(this->getValueMillis(key));
				Serial.print("differenceinsecs: ");
				Serial.println(differenceinsecs);
				Serial.print("threshold: ");
				Serial.println(threshold);				
				//Serial.println("Value updated to same because of risk or keepalive issues");
				return this->publishDeviceValue(deviceid,value);			
			} else {
				return false;
			};
		};
	};
};

void Ioteey::dumpSettings() {
   for(int i=0;i<this->settings.size();i++) {
	    Serial.print(i);
	    Serial.print(" - ");
	    Serial.print("Key (");
	    Serial.print(this->settings.keyAt(i).length());
	    Serial.print(") ");
	    Serial.print(this->settings.keyAt(i));
	    Serial.print("[0]");
	    Serial.print(" Value (");
	    Serial.print(this->settings.valueAt(i).length());
	    Serial.print(") ");
	    Serial.print(this->settings.valueAt(i));
	    Serial.println("[0]");
	    yield();
	};	
};

void Ioteey::resetSettingsToEEPROM(bool hard) {
   if(hard) { //put the thing id back
	  EEPROM.write(0,'\0');
	  EEPROM.commit();
	  this->initializeSettings();
   } else {
	  String res = this->settings.getItem("T0000");
	  EEPROM.write(0,'\0');
	  EEPROM.commit();
	  this->initializeSettings();
	  this->settings.addItem("T0000",res);
	  this->saveSettingsToEEPROM(); 
   };
};

//This will only save T items to EEPROM. Devices, attributes, etc are loaded dynamically from the sketch
void Ioteey::saveSettingsToEEPROM() {
   int memused = 0;
   int i = 0;
   String str;
   for(i=0;i<this->settings.size();i++) {  
	 //Either save items that start with T or items that are device settings (3,5=DEVICET SETTING and last digit is 1) and make sure only non empty values are saved  
	 if( (  this->settings.keyAt(i).c_str()[0]=='T' ||\ 
	        (\
	           this->settings.keyAt(i).substring(3,5)==String(DEVICESETTING) &&\
	           this->settings.keyAt(i).c_str()[7]=='1'\
	        )\
	     )  &&\
	     (\
	       this->settings.valueAt(i)!=String("")\
	     )\
	   ) {
		//Write key
		for(int j=0;j<this->settings.keyAt(i).length();j++) {
			if(memused < MAXEEPROMWRITESIZE) {
				EEPROM.write(memused,this->settings.keyAt(i).charAt(j));
			} else {
				Serial.print("Maximum memory allocation of ");
				Serial.print(MAXEEPROMWRITESIZE);
				Serial.println(" bytes exceeded in EEPROM");
				return;
			};	
			memused++;
			if(memused % 100) {
				yield();
			};
		};  
		if(memused < MAXEEPROMWRITESIZE) {
			EEPROM.write(memused,':');
		} else {
			Serial.print("Maximum memory allocation of ");
			Serial.print(MAXEEPROMWRITESIZE);
			Serial.println(" bytes exceeded in EEPROM");
			return;
		};			
		memused++;
		//Write value
		for(int j=0;j<this->settings.valueAt(i).length();j++) {
			EEPROM.write(memused,this->settings.valueAt(i).charAt(j));
			memused++;
			if(memused % 100) {
				yield();
			};
		}; 
		if(i!=this->settings.size()-1) { 
			//Write mark
			if(memused < MAXEEPROMWRITESIZE) {
				EEPROM.write(memused,'|');
			} else {
				Serial.print("Maximum memory allocation of ");
				Serial.print(MAXEEPROMWRITESIZE);
				Serial.println(" bytes exceeded in EEPROM");
				return;
			};				
			memused++;	 
			if(memused < MAXEEPROMWRITESIZE) {
				EEPROM.write(memused,'|');
			} else {
				Serial.print("Maximum memory allocation of ");
				Serial.print(MAXEEPROMWRITESIZE);
				Serial.println(" bytes exceeded in EEPROM");
				return;
			};				
			memused++;	 
			if(memused < MAXEEPROMWRITESIZE) {
				EEPROM.write(memused,'|');
			} else {
				Serial.print("Maximum memory allocation of ");
				Serial.print(MAXEEPROMWRITESIZE);
				Serial.println(" bytes exceeded in EEPROM");
				return;
			};				
			memused++;	 	 
		};	
     };
   };
   if(memused < MAXEEPROMWRITESIZE) {
		EEPROM.write(memused,'\0');
   } else {
		Serial.print("Maximum memory allocation of ");
		Serial.print(MAXEEPROMWRITESIZE);
		Serial.println(" bytes exceeded in EEPROM");
		return;
   };				

   EEPROM.commit();
};

String Ioteey::readSettingsIntoStringFromEEPROM() { 
   char t[MAXEEPROMWRITESIZE+1];
   
   int i;
   for(i=0;i<MAXEEPROMWRITESIZE;i++) {
		t[i] = EEPROM.read(i);
		if(i % 100 == 0) {
			yield();
		};
   }
   t[i+1] = '\0';
   return String(t);
};	

//In the future, try to combine the next two methods into one to avoid redundancy
void Ioteey::readSettingsFromCharArray(const char* input) { 
   char key[MAXKEYSIZE];
   char value[MAXVALUESIZE];
   char c;
   int memlocation = 0;
   char state='K'; //K=key,S=keyvalueseparator,V=value,1=end1,2=end2,3=end3,4=END
   int index=0;
   key[0] = '\0';
   value[0] = ' \0';
   

   while(true) {
      c = input[memlocation];
      if(c=='\0') {
		state = 'E';
	  };
      if(state=='K') { 
		 if(c==':') {
			state='V';
			index=0;
			value[index]='\0';
		 } else {
			if(index<MAXKEYSIZE) { 
			  key[index] = c;
			  key[index+1] = '\0';
			};  
			index++;
		 };		
	  } else if(state=='V') {
		 if(c=='|') {
			state='1';		
		 } else {
			if(index<MAXVALUESIZE) {  
			  value[index] = c;
			  value[index+1] = '\0';
			};  
			index++;		
		 };
	  } else if(state=='1') {
		 if(c=='|') {
			state='2';
		 };
	  } else if(state=='2') {
		 if(c=='|') {
			 state='3';
		 };
	  } else if(state=='3') {
		 this->settings.addItem(String(key),String(value));
		 state='K';
		 index=0;
		 key[index] = c;
		 key[index+1] = '\0';
		 index++;
		 value[0] = '\0';		 
	  } else if(state=='E') {
		 this->settings.addItem(String(key),String(value));
		 break;
	  };
      memlocation++;
      if(memlocation>MAXEEPROMWRITESIZE) {
		break;
	  };      
	  yield();
   };
};	


void Ioteey::readSettingsFromEEPROM() { 
   char key[MAXKEYSIZE];
   char value[MAXVALUESIZE];
   char c;
   int memlocation = 0;
   char state='K'; //K=key,S=keyvalueseparator,V=value,1=end1,2=end2,3=end3,4=END
   int index=0;
   key[0] = '\0';
   value[0] = ' \0';
   

   while(true) {
      c = EEPROM.read(memlocation);
      if(c=='\0') {
		state = 'E';
	  };
      if(state=='K') { 
		 if(c==':') {
			state='V';
			index=0;
			value[index]='\0';
		 } else {
			if(index<MAXKEYSIZE) { 
			  key[index] = c;
			  key[index+1] = '\0';
			};  
			index++;
		 };		
	  } else if(state=='V') {
		 if(c=='|') {
			state='1';		
		 } else {
			if(index<MAXVALUESIZE) {  
			  value[index] = c;
			  value[index+1] = '\0';
			};  
			index++;		
		 };
	  } else if(state=='1') {
		 if(c=='|') {
			state='2';
		 };
	  } else if(state=='2') {
		 if(c=='|') {
			 state='3';
		 };
	  } else if(state=='3') {
		 this->settings.addItem(String(key),String(value));
		 state='K';
		 index=0;
		 key[index] = c;
		 key[index+1] = '\0';
		 index++;
		 value[0] = '\0';		 
	  } else if(state=='E') {
		 this->settings.addItem(String(key),String(value));
		 break;
	  };
      memlocation++;
      if(memlocation>MAXEEPROMWRITESIZE) {
		break;
	  };      
	  yield();
   };
};	

void Ioteey::initializeSettings() {
   this->settings.resetMap();
};


//bool Ioteey::initializeDevice(int deviceid,int initialvalue) {
	//char buf[10];
	
	//sprintf (buf, "%i", initialvalue);	
	//return this->initializeDevice(deviceid,buf);
//};
  	 			
bool Ioteey::publishDeviceValue(char* deviceid,int value) {
	char buf[10];
	
	sprintf (buf, "%i", value);
	return this->publishDeviceValue(deviceid,buf);
};

bool Ioteey::deviceValueChanges(char* deviceid, int value) {
	char buf[10];
	
	sprintf (buf, "%i", value);
	return this->deviceValueChanges(deviceid,buf);
};
 	
bool Ioteey::publishDeviceValueIfNeeded(char* deviceid,int value) {
	char buf[10];
	
	sprintf (buf, "%i", value);
	return this->publishDeviceValueIfNeeded(deviceid,buf);
};

bool Ioteey::publishDeviceValue(char* deviceid,char* value) {
  if(this->mqttconnected) {	
		this->publishDeviceValueWithMQTT(deviceid,value);
  };	
};







//////////////////////////////////////////////////////////////////////////////
//  MQTT RELATED METHODS
// TODO: ALL COMMENTED BECAUSE YOU NEED TO FINISH THIS
//////////////////////////////////////////////////////////////////////////////

void Ioteey::buildTopicTopLevel(char* topic) {
	strcpy(topic,"things/");
	strcat(topic,this->getThingCode().c_str());
	strcat(topic,"/");
};

void Ioteey::buildTopicThing(char* topic) {
	this->buildTopicTopLevel(topic);
	strcat(topic,"thing/");
};

void Ioteey::buildTopicDevices(char* topic) {
	this->buildTopicTopLevel(topic);
	strcat(topic,"devices/");
};

void Ioteey::buildTopicThingVar(char* topic, char* var) {
	this->buildTopicThing(topic);
	strcat(topic,var);
	Serial.println(topic);
};

void Ioteey::buildTopicDevicesVar(char* topic, char* var) {
	this->buildTopicDevices(topic);
	strcat(topic,var);
	Serial.println(topic);
};

bool Ioteey::initializeThingWithMQTT() {
	char lastwilltopic[100];
	this->buildTopicThingVar(lastwilltopic,"status");
  
	this->client.publish(lastwilltopic,"ONLINE", true);
  
	char keepalivetopic[100] = "";
	this->buildTopicThingVar(keepalivetopic,"keepalive");
	
	char buf[10];
	
	sprintf (buf, "%i", keepalive);
		
	this->client.publish(keepalivetopic,buf,true);
};

//bool Ioteey::connectMQTT(String thingid, String mqttserver,String mqttport,String mqttuser,String mqttpassword,void (*callback)(char*, byte*, unsigned int)) {
bool Ioteey::connectMQTT(String mqttserver,String mqttport,String mqttuser,String mqttpassword) {		
	strcpy(this->mqttserver,mqttserver.c_str());
	strcpy(this->mqttuser,mqttuser.c_str());
	strcpy(this->mqttpassword,mqttpassword.c_str());	
	this->mqttport = mqttport.toInt();
    	
	Serial.print("Connecting to server: ");
	Serial.print(this->mqttserver);
	Serial.print("  ");
	Serial.println(this->mqttport);
	this->client.setServer(this->mqttserver, this->mqttport);
	delay(200);
	if (!this->client.connected()) {
		if(!this->reconnect()) {
			Serial.println("Cannot connect to MQTT");
			return false;
		} else {
			Serial.print("Connected successfully to MQTT at ");
			Serial.println(mqttserver);
			this->mqttconnected=true;
		};
	}
	this->client.loop();
	delay(200); 	
	
	char topic[100] = "";
	this->buildTopicDevicesVar(topic, "+");
	strcat(topic,"/command");
	
	this->client.subscribe(topic);
	//this->client.setCallback(callback);
	this->timerReset(1);
	return true;
};

bool Ioteey::publishDeviceValueWithMQTT(char* deviceid, char* value) {
	char topic[100];
	this->buildTopicDevicesVar(topic,deviceid);
	strcat(topic,"/");
  
	char valuetopic[100];
	strcpy(valuetopic,topic);
	strcat(valuetopic,"value");

	this->client.publish(valuetopic,value, true);
	
	char key[50]="";
	strcat(key,"device-");
	strcat(key,deviceid);
	this->saveValue(key, value);
};

bool Ioteey::initializeDeviceWithMQTT(char* deviceid, char* initialvalue) {
	if(strcmp(this->devicelist,"")!=0) {
		strcat(this->devicelist,",");
	}	
	strcat(this->devicelist,deviceid);

	char topic[100] = "";
	this->buildTopicDevicesVar(topic, deviceid);	
	strcat(topic,"/");
  
    this->publishDeviceValue(deviceid,initialvalue);
    
	char statustopic[100] = "";
	strcat(statustopic,topic);
	strcat(statustopic,"status");
	
	this->client.publish(statustopic,"ONLINE", true);
	 	
	char errortopic[100] = "";
	strcat(errortopic,topic);	
	strcat(errortopic,"error");

	this->client.publish(errortopic,"", true);
	
	return true;
};

bool Ioteey::startWithMQTT() {
	char devicestopic[100];
    this->buildTopicDevicesVar(devicestopic, "list"); 
  
	this->client.publish(devicestopic,this->devicelist, true); 	
};

bool Ioteey::reconnect() {
  char topic[100];
  this->buildTopicThingVar(topic, "status");

  this->numfailedmqtttries = 0;
  	
  if (!client.connected()) {	
		while(this->numfailedmqtttries<2) {
			Serial.println("Attempting MQTT connection...");
			// Attempt to connect
			// If you do not want to use a username and password, change next line to
			if (client.connect(this->getThingNetworkName().c_str(),topic,0,true,"OFFLINE")) {
				Serial.println("connected");
				this->mqttconnected=true;
				this->numfailedmqtttries=0;
				return this->mqttconnected;
			} else {
				this->mqttconnected=false;
				this->numfailedmqtttries = this->numfailedmqtttries+1;
				this->timelastmqttfailedtry = millis();
				Serial.print("failed, rc=");
				Serial.println(client.state());
			};   
			delay(3000);
		};
		
  };
  return this->mqttconnected;
};

void Ioteey::loadCallbackInfo(char* topic, byte* payload, unsigned int length) {
	//strcpy(this->topic,topic);
	//this->payload = payload;
	//this->length = length;
	
	//String stringtopic = String(topic);

	//int indexthingconstant = stringtopic.indexOf("/");
	//int indexdeviceid = stringtopic.indexOf("/",indexthingconstant+1);
	//int indexsensorconstant = stringtopic.indexOf("/",indexdeviceid+1);
	//int indexsensorid = stringtopic.indexOf("/",indexsensorconstant+1);

	//String deviceid = stringtopic.substring(indexthingconstant+1,indexdeviceid);
	//String sensorid = stringtopic.substring(indexsensorconstant+1,indexsensorid);
	
	//char sensoridarray[100];
	
	//sensorid.toCharArray(sensoridarray,100);
	
	//int i;
	//char command[100] = "";
	//for(i=0;i<length;i++) {
	//	command[i] = (char)payload[i];
	//};
	//command[i+1] = '\0';
	
	//this->commandcallback(sensoridarray,command);
	
};

