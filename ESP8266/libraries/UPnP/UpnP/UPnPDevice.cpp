/*
 * ESP8266 Simple Service Discovery
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
 */
#include "Arduino.h"
#include "UPnPDevice.h"
#include "debug.h"

#undef DEBUG_UPNP
// #define DEBUG_UPNP	Serial

UPnPDevice::UPnPDevice() {
  strcpy(_schemaURL,"ssdp/schema.xml");
  //_schemaURL = (char *)"ssdp/schema.xml";
  strcpy(_friendlyName," ");
  strcpy(_serialNumber," ");
  strcpy(_presentationURL," ");
  strcpy(_manufacturer," ");
  strcpy(_manufacturerURL," ");
  strcpy(_modelURL," ");
  strcpy(_deviceURN," ");  
}

UPnPDevice::~UPnPDevice() {
#ifdef DEBUG_UPNP
  DEBUG_UPNP.printf("UPnPDevice DTOR\n");
#endif
}

void UPnPDevice::setSchemaURL(char *url) {
  strcpy(_schemaURL,url);
}

void UPnPDevice::setHTTPPort(uint16_t port){
  _port = port;
}

void UPnPDevice::setName(char *name){
  strcpy(_friendlyName,name);
}

void UPnPDevice::setURL(char *url){
  strcpy(_presentationURL,url);
}

void UPnPDevice::setSerialNumber(char *serialNumber){
  strcpy(_serialNumber,serialNumber);
}

void UPnPDevice::setModelName(char *name){
  strcpy(_modelName,name);
}

void UPnPDevice::setModelNumber(char *num){
  strcpy(_modelNumber,num);
}

void UPnPDevice::setModelURL(char *url){
  strcpy(_modelURL,url);
}

void UPnPDevice::setManufacturer(char *name){
  strcpy(_manufacturer,name);
}

void UPnPDevice::setManufacturerURL(char *url){
  strcpy(_manufacturerURL,url);
}

void UPnPDevice::setDeviceURN(char *urn){
  strcpy(_deviceURN,urn);
}

void UPnPDevice::setPort(uint16_t port) {
	_port = port;
}

uint16_t UPnPDevice::getPort() {
	return _port;
}

char *UPnPDevice::getSchemaURL() {
	return _schemaURL;
}

char *UPnPDevice::getPresentationURL() {
	return _presentationURL;
}

char *UPnPDevice::getSerialNumber() {
	return _serialNumber;
}

char *UPnPDevice::getModelName() {
	return _modelName;
}

char *UPnPDevice::getModelNumber() {
	return _modelNumber;
}

char *UPnPDevice::getModelURL() {
	return _modelURL;
}

char *UPnPDevice::getManufacturer() {
	return _manufacturer;
}

char *UPnPDevice::getManufacturerURL() {
	return _manufacturerURL;
}

char *UPnPDevice::getUuid() {
	return _uuid;
}

char *UPnPDevice::getFriendlyName() {
	return _friendlyName;
}

char *UPnPDevice::getDeviceURN() {
	return _deviceURN;
}
