/*
 * Copyright (c) 2015, 2016 Danny Backx
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
#ifndef	__UPnPService_H_
#define	__UPnPService_H_

#include "debug.h"
//#include "UPnP/WebServer.h"
//#include "UPnP/WebClient.h"
//#include "UPnPSubscriber.h"
#include "StateVariable.h"
//#include "UPnP/Configuration.h"
//#include <FS.h>

#define	N_ACTIONS			4
#define	N_VARIABLES			4
#define	SUBSCRIBER_ALLOC_INCREMENT	4

class UPnPService;
typedef void (UPnPService::*MemberActionFunction)();
typedef void (*ActionFunction)();

typedef struct {
  const char *name;
  UPnPService *sensor;
  ActionFunction handler;
  MemberActionFunction mhandler;
  const char *xml;
} Action;

class UPnPService {
  public:
    UPnPService(const char *name, const char *serviceType, const char *serviceId);
    UPnPService();
    ~UPnPService();

    // Use the XML to publish a callable action
    // If it is called (via a UPnP query to our web server), call the handler function.
    // There are two types of handler functions : a member function, or a static function.
    void addAction(const char *name, ActionFunction handler, const char *xml);
    void addAction(const char *name, MemberActionFunction handler, const char *xml);

    // Define a state variable
    void addStateVariable(const char *name, const char *datatype, boolean sendEvents);
    void VariableChanged(const char *name, const char *value);
    char *getActionListXML();
    char *getStateVariableListXML();
    char *getServiceXML();
    void begin();
    Action *findAction(const char *);
    StateVariable *lookupVariable(char *name);

    void EventHandler();
    static void ControlHandler();

    int nvariables, maxvariables;
    StateVariable **variables;

    int nactions;
    Action *actions;

    const char *serviceName;
    const char *serviceId;
    const char *serviceType;

    //UPnPSubscriber *Subscribe();
    //void Subscribe(UPnPSubscriber *ns);
    //void Unsubscribe();
    //void Unsubscribe(char *uuid);
    //void Unsubscribe(UPnPSubscriber *sp);

    void SendNotify(UPnPSubscriber *s, const char *varName);
    void SendNotify();
    // void SendNotify(StateVariable &sv);
    void SendNotify(const char *varName);

    void SendSCPD(WiFiClient client);
    void ReadConfiguration(const char *name, Configuration *config);

  private:
    UPnPSubscriber **subscriber;
    int nsubscribers, maxsubscribers;
    int ReadLine(File f);
    char *line;

    Configuration *config;

  protected:

};

#endif
