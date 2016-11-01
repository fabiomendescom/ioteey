/*
 * ESP8266 Simple Service Discovery
 *
 * Copyright (c) 2015 Hristo Gochkov
 * Copyright (c) 2015, 2016 Danny Backx
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
 */
#ifndef	__UPnPDevice_H_
#define	__UPnPDevice_H_

#define LWIP_OPEN_SRC
#include <functional>
#include "debug.h"

#define UPnP_UUID_SIZE              37
// #define UPnP_SCHEMA_URL_SIZE        64
#define UPnP_FRIENDLY_NAME_SIZE     64
#define UPnP_SERIAL_NUMBER_SIZE     32
// #define UPnP_PRESENTATION_URL_SIZE  128
#define UPnP_MODEL_NAME_SIZE        64
#define UPnP_MODEL_URL_SIZE         128
#define UPnP_MODEL_VERSION_SIZE     32
#define UPnP_MANUFACTURER_SIZE      64
// #define UPnP_MANUFACTURER_URL_SIZE  128

class UPnPDevice {
  public:
    UPnPDevice();
    ~UPnPDevice();
    void setSchemaURL(char *url);
    void setHTTPPort(uint16_t port);
    void setName(char *name);
    void setURL(char *url);
    void setSerialNumber(char *serialNumber);
    void setModelName(char *name);
    void setModelNumber(char *num);
    void setModelURL(char *url);
    void setManufacturer(char *name);
    void setManufacturerURL(char *url);
    void setDeviceURN(char *urn);

    void setPort(uint16_t port);
    uint16_t getPort();

    void setFriendlyName(const char *fn);
    char *getFriendlyName();

    char *getSchemaURL();
    char *getPresentationURL();
    char *getSerialNumber();
    char *getModelName();
    char *getModelNumber();
    char *getModelURL();
    char *getManufacturer();
    char *getManufacturerURL();
    char *getUuid();
    char *getDeviceURN();

    char *_uuid;
    char *_modelName;
    char *_modelNumber;

  private:

  protected:
    uint16_t _port;
    char _schemaURL[100];
    char _friendlyName[100];
    char _serialNumber[100];
    char _presentationURL[100];
    char _manufacturer[100];
    char _manufacturerURL[100];
    char _modelURL[100];
    char _deviceURN[100];

};

#endif
