/*
 * ESP8266 Simple UPnP framework
 *   There's no separate class for multiple devices,
 *   also currently we allocate only one service,
 *   because we assume one (IoT) device is all we do.
 *
 * Copyright (c) 2015 Hristo Gochkov
 * Copyright (c) 2015 Danny Backx
 * 
 * Original (Arduino) version by Filippo Sallemi, July 23, 2014.
 * Can be found at: https://github.com/nomadnt/uSSDP
 * 
 * License (MIT license):
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 * 
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * 
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 * 
 * 
 * THIS WAS MODIFIED BY FABIO FROM ITS ORIGINAL VERSION!!!! USE THIS
 * I KEPT THE ORIGINAL COPYRIGHTS ABOVE TO GIVE CREDIT TO THE AUTHORS
 * 
 */
#include "Arduino.h"
#include "UPnP.h"
#include "debug.h"
//#include "UPnP/WebServer.h"
//#include "UPnP/Headers.h"

//extern WebServer HTTP;

// Choose one
#undef	DEBUG_UPNP
// #define	DEBUG_UPNP	Serial

UPnPClass UPnP;	// FIXME

UPnPClass::UPnPClass() {
  //FABIO
  //services = 0;
}

UPnPClass::~UPnPClass() {
#ifdef DEBUG_UPNP
  DEBUG_UPNP.printf("UPnPClass DTOR\n");
#endif
}

void UPnPClass::begin(ESP8266WebServer *http, const char* uuid, const char* ioteeyname, const char* ioteeytype, const char* deviceURN, const char* modelName, const char* modelNumber, const char* friendlyName, const char* serialNumber, const char* manufacturer, const char* manufacturerURL, const char* modelURL) {
  this->http = http;
  String url = "/" + String("description.xml");
  this->http->on(url.c_str(),std::bind(&UPnPClass::schema, this));
  this->http->on("/SwitchPower1.xml",std::bind(&UPnPClass::scpd, this));

  strcpy(_uuid,uuid) ;
  _deviceport = 80 ;
  strcpy(_schemaURL,"description.xml");
  strcpy(_modelName,modelName);
  strcpy(_modelNumber,modelNumber); 
  strcpy(_friendlyName,friendlyName);
  strcpy(_serialNumber,serialNumber);
  strcpy(_manufacturer,manufacturer);
  strcpy(_manufacturerURL,manufacturerURL);
  strcpy(_modelURL,modelURL);
  strcpy(_deviceURN,deviceURN);      
  strcpy(_ioteeyname,ioteeyname);
  strcpy(_ioteeytype,ioteeytype);
}

const char *_http_header =
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/xml\r\n"
  "Connection: close\r\n"
  "Access-Control-Allow-Origin: *\r\n"
  "\r\n";

static const char *_upnp_device_template_1 =
  "<?xml version=\"1.0\"?>"
  "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
    "<specVersion>"
      "<major>1</major>"
      "<minor>0</minor>"
    "</specVersion>"
    "<URLBase>http://%s:%u/</URLBase>" /* WiFi.localIP(), _port */
    "<device>"
      "<deviceType>%s</deviceType>"
      "<friendlyName>%s</friendlyName>"
      "<presentationURL>%s</presentationURL>"
      "<serialNumber>%s</serialNumber>"
      "<modelName>%s</modelName>"
      "<modelNumber>%s</modelNumber>"
      "<modelURL>%s</modelURL>"
      "<manufacturer>%s</manufacturer>"
      "<manufacturerURL>%s</manufacturerURL>"
      "<UDN>uuid:%s</UDN>"
      "<serviceList>";

static const char *_upnp_device_template_2 =
      "</serviceList>"
    "</device>"
  "</root>\r\n"
  "\r\n";

const char *UPnPClass::mimeTypeXML = "text/xml; charset=\"utf-8\"";
const char *UPnPClass::mimeTypeText = "text/plain; charset=\"utf-8\"";
const char *UPnPClass::envelopeHeader = 
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
    "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n"
    "<s:body>\r\n";
const char *UPnPClass::envelopeTrailer = 
    "</s:body>\r\n"    
    "</s:Envelope>\r\n";
    
void UPnPClass::scpd() {
  String ret;
  ret = ret + "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
  ret = ret + "<scpd xmlns=\"urn:schemas-upnp-org:device-1-0\">";
  ret = ret + "  <specVersion>";
  ret = ret + "    <major>1</major>";
  ret = ret + "    <minor>0</minor>";
  ret = ret + "  </specVersion>";
  ret = ret + "  <actionList>";
  ret = ret + "    <action>";
  ret = ret + "      <name>SetTarget</name>";
  ret = ret + "      <argumentList>";
  ret = ret + "        <argument>";
  ret = ret + "          <name>NewTargetValue</name>";
  ret = ret + "          <relatedStateVariable>Target</relatedStateVariable>";
  ret = ret + "          <direction>in</direction>";
  ret = ret + "        </argument>";
  ret = ret + "      </argumentList>";
  ret = ret + "    </action>";
  ret = ret + "    <action>";
  ret = ret + "      <name>GetTarget</name>";
  ret = ret + "      <argumentList>";
  ret = ret + "        <argument>";
  ret = ret + "          <name>RetTargetValue</name>";
  ret = ret + "          <relatedStateVariable>Target</relatedStateVariable>";
  ret = ret + "          <direction>out</direction>";
  ret = ret + "        </argument>";
  ret = ret + "      </argumentList>";
  ret = ret + "    </action>";
  ret = ret + "    <action>";
  ret = ret + "      <name>GetStatus</name>";
  ret = ret + "      <argumentList>";
  ret = ret + "        <argument>";
  ret = ret + "          <name>ResultStatus</name>";
  ret = ret + "          <relatedStateVariable>Status</relatedStateVariable>";
  ret = ret + "          <direction>out</direction>";
  ret = ret + "        </argument>";
  ret = ret + "      </argumentList>";
  ret = ret + "    </action>";
  ret = ret + "  </actionList>";
  ret = ret + "  <serviceStateTable>";
  ret = ret + "    <stateVariable sendEvents=\"no\">";
  ret = ret + "      <name>Target</name>";
  ret = ret + "      <dataType>boolean</dataType>";
  ret = ret + "      <defaultValue>0</defaultValue>";
  ret = ret + "    </stateVariable>";
  ret = ret + "    <stateVariable sendEvents=\"yes\">";
  ret = ret + "      <name>Status</name>";
  ret = ret + "      <dataType>boolean</dataType>";
  ret = ret + "      <defaultValue>0</defaultValue>";
  ret = ret + "    </stateVariable>";
  ret = ret + "  </serviceStateTable>";
  ret = ret + "</scpd>";
  
  this->http->setContentLength(ret.length());
  this->http->send(200, "text/xml", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.  
  this->http->sendContent(ret);
  
  this->http->handleClient();  
}; 

// Called by HTTP server when our description XML is queried
void UPnPClass::schema() {
  IPAddress ip = WiFi.localIP();
  
  String ret = String("");
  ret = ret + "<?xml version=\"1.0\"?>";
  ret = ret + "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">";
  ret = ret +   "<specVersion>";
  ret = ret +     "<major>1</major>";
  ret = ret +     "<minor>0</minor>";
  ret = ret +   "</specVersion>";
  ret = ret +   "<device>";
  ret = ret +     "<deviceType>";
  ret = ret + String(_deviceURN);
  ret = ret +     "</deviceType>";
  ret = ret +     "<friendlyName>";
  ret = ret + String(_friendlyName);
  ret = ret +     "</friendlyName>";
  ret = ret +     "<serialNumber>";
  ret = ret + String(_serialNumber);
  ret = ret +     "</serialNumber>";
  ret = ret +     "<modelName>";
  ret = ret + String(_modelName);
  ret = ret +     "</modelName>";
  ret = ret +     "<modelNumber>";
  ret = ret + String(_modelNumber);
  ret = ret +     "</modelNumber>";
  ret = ret +     "<modelURL>";
  ret = ret + String(_modelURL);
  ret = ret +     "</modelURL>";
  ret = ret +     "<manufacturer>";
  ret = ret + String(_manufacturer);
  ret = ret +     "</manufacturer>";
  ret = ret +     "<manufacturerURL>";
  ret = ret + String(_manufacturerURL);
  ret = ret +     "</manufacturerURL>";
  ret = ret +     "<UDN>";
  ret = ret + String("uuid:") + String(_uuid);
  ret = ret +     "</UDN>";
  ret = ret +     "<IoteeyType>";
  ret = ret + String(_ioteeytype);  
  ret = ret +     "</IoteeyType>";  
  ret = ret +     "<IoteeyName>";
  ret = ret + String(_ioteeyname);  
  ret = ret +     "</IoteeyName>";  
  ret = ret +     "<IoteeyCode>";
  ret = ret + String(_uuid);  
  ret = ret +     "</IoteeyCode>";    
  
  //ret = ret +     "<serviceList>";    
  //ret = ret +     "   <service>";   
  //ret = ret +     "      <serviceType>urn:schemas-upnp-org:service:SwitchPower:1</serviceType>";
  //ret = ret +     "      <serviceId>urn:upnp-org:serviceId:SwitchPower:1</serviceId>";
  //ret = ret +     "      <SCPDURL>/SwitchPower1.xml</SCPDURL>";
  //ret = ret +     "      <controlURL>/SwitchPower/Control</controlURL>";
  //ret = ret +     "      <eventSubURL>/SwitchPower/Event</eventSubURL>";  
  //ret = ret +     "   </service>";  
  //ret = ret +     "</serviceList>";

  ret = ret +   "</device>";
  ret = ret + "</root>";

  //this->http->setContentLength(ret.length());
  this->http->send(200, "text/xml",ret); // Empty content inhibits Content-length header so we have to close the socket ourselves.  
  //this->http->sendContent(ret);
  
  this->http->handleClient();
 
  //FABIO
  //if (services) {
    //for (int i=0; i<nservices; i++) {
      // Need to free
      //char *tmp = services[i]->getServiceXML();
      //client.print(tmp);
      //free(tmp);
    //}
  //}
}

/*
void UPnPClass::addService(UPnPService *srv) {
  if (nservices == maxservices) {
    maxservices += N_SERVICES;
    services = (UPnPService **)realloc(services, maxservices * sizeof(UPnPService *));
  }
  services[nservices++] = srv;
}
*/

/*
 * This function is a pass-through for the member function just below.
 */
//void staticSendSCPD() {
//#ifdef DEBUG_UPNP
//  DEBUG_UPNP.println("staticSendSCPD");
//#endif

//  UPnP.SendSCPD();
//}

/*
 * Use the URL that the web server received, isolate the UPnPService name from it,
 * find that service, and call its SendSCPD method.
 */
//void UPnPClass::SendSCPD() {
//#ifdef DEBUG_UPNP
  //FABIO
  //DEBUG_UPNP.printf("SendSCPD(%s)\n", HTTP.httpUri());
//#endif
//FABIO
/*
  // Find out which UPnPService this was called for
  // The URL here is e.g. "/LEDService/scpd.xml"
  const char *url = HTTP.httpUri();
  const char *name = url+1;
  const char *p;

  for (p=name; *p && *p != '/'; p++) ;
  if (*p == '\0')
    return;	// silently

  int len = (p-name);
  for (int i=0; i<nservices; i++)
    if (strncmp(name, services[i]->serviceName, len) == 0) {
#ifdef DEBUG_UPNP
      DEBUG_UPNP.printf("SendSCPD : service %d, %s\n", i, services[i]->serviceName);
#endif

      // Call it !
      services[i]->SendSCPD(HTTP.client());
      return;
    }
    */ 
//}

// This is a pass-through for UPnPClass::EventHandler, called from UPnPService.
//void staticEventHandler() {
//  UPnP.EventHandler();
//}

//void UPnPClass::EventHandler() {
//#ifdef DEBUG_UPNP
  //FABIO
  //DEBUG_UPNP.printf("UPnPClass::EventHandler(%s)\n", HTTP.httpUri());
//#endif

//FABIO
/*
  // Find out which UPnPService this was called for
  // The URL here is e.g. "/LEDService/scpd.xml"
  const char *url = HTTP.httpUri();
  const char *name = url+1;
  const char *p;

  for (p=name; *p && *p != '/'; p++) ;
  if (*p == '\0')
    return;	// silently

  int len = (p-name);
*/
  //FABIO
  //for (int i=0; i<nservices; i++)
   // if (strncmp(name, services[i]->serviceName, len) == 0) {
//#ifdef DEBUG_UPNP
     // DEBUG_UPNP.printf("UPnPClass::EventHandler : service %d, %s\n", i, services[i]->serviceName);
//#endif

      // Call it !
      //services[i]->EventHandler();
      //return;
   // }
//}
