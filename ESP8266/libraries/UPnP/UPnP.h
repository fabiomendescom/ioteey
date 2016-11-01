/*
 * ESP8266 Simple UPnP Framework
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
 *  * THIS WAS MODIFIED BY FABIO FROM ITS ORIGINAL VERSION!!!! USE THIS
 * I KEPT THE ORIGINAL COPYRIGHTS ABOVE TO GIVE CREDIT TO THE AUTHORS
 * 
 */
#ifndef	__UPnP_H_
#define	__UPnP_H_

#define	ENABLE_SPIFFS
#define	ENABLE_LED_SERVICE
// #define	ENABLE_DHT_SERVICE
#define	ENABLE_BMP_SERVICE
#define ENABLE_OTA
#define ENABLE_SNTP

#include <ESP8266WebServer.h>

//FABIO
//#include "SSDP.h"
//#include "WiFiUdp.h"
//#include "UPnPService.h"
//#include "UPnP/WebServer.h"
//#include "UPnP/Configuration.h"

#define	N_SERVICES	4

class UPnPClass {
  public:
    UPnPClass();
    ~UPnPClass();

    void begin(ESP8266WebServer *http, const char* uuid, const char* ioteeyname, const char* ioteeytype, const char* deviceURN, const char* modelName, const char* modelNumber, const char* friendlyName, const char* serialNumber, const char* manufacturer, const char* manufacturerURL, const char* modelURL);

    void setSchemaURL(const char *url);
    void setHTTPPort(uint16_t port);
    void setName(const char *name);
    void setURL(const char *url);
    void setSerialNumber(const char *serialNumber);
    void setModelName(const char *name);
    void setModelNumber(const char *num);
    void setModelURL(const char *url);
    void setManufacturer(const char *name);
    void setManufacturerURL(const char *url);
    void setIoteeyName(const char *ioteeyname);
    void setIoteeyType(const char *ioteeytype);

    void schema();
    void scpd();
//    void SendSCPD();

	//FABIO
    //void addService(UPnPService *service);

    static const char *mimeTypeXML;
    static const char *mimeTypeText;
    static const char *envelopeHeader;
    static const char *envelopeTrailer;

    //void EventHandler();

  private:
    //UPnPDevice *device;
    ESP8266WebServer *http;
    char _uuid[100];
    uint16_t _deviceport ;
    char _ioteeyname[100];
    char _ioteeytype[100];
    char _schemaURL[100];
    char _modelName[100];
    char _modelNumber[100]; 
    char _friendlyName[100];
    char _serialNumber[100];
    char _presentationURL[100];
    char _manufacturer[100];
    char _manufacturerURL[100];
    char _modelURL[100];
    char _deviceURN[100];      

  protected:
	//FABIO
    //UPnPService **services;
    int nservices, maxservices;
};

extern UPnPClass UPnP;
//extern const char *_http_header;
//extern void staticSendSCPD();
//extern void staticEventHandler();

#endif
