//dac128VServer.cc

/********************COPYRIGHT NOTIFICATION**********************************
This software was developed under a United States Government license
described on the COPYRIGHT_UniversityOfChicago file included as part
of this distribution.
****************************************************************************/
/*
    Original Author: Jim Kowalkowski
    Date: 2/15/95
    Current Authors: Mark Rivers
    Converted to MPF: 9/4/99

*/

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <epicsThread.h>
#include <iocsh.h>
#include <epicsExport.h>

#include "Message.h"
#include "Int32Message.h"
#include "mpfType.h"
#include "DAC128V.h"

class DAC128VServer {
public:
    DAC128V *pDAC128V;
    MessageServer *pMessageServer;
    static void dac128VServer(DAC128VServer *);
};


extern "C" int initDAC128V(
    const char *serverName, ushort_t carrier, ushort_t slot,
    int queueSize)
{
    DAC128V *pDAC128V = DAC128V::init(serverName, carrier, slot);
    if(!pDAC128V) return(-1);
    DAC128VServer *p = new DAC128VServer;
    p->pDAC128V = pDAC128V;
    p->pMessageServer = new MessageServer(serverName,queueSize);
    epicsThreadId threadId = epicsThreadCreate("dac128V", 
                             epicsThreadPriorityMedium, 10000,
                             (EPICSTHREADFUNC)DAC128VServer::dac128VServer, 
                             (void*) p);
    if(threadId == NULL)
        errlogPrintf("%s dac128VServer ThreadCreate Failure\n",
            p->pMessageServer->getName());
 
    return(0);
}

void DAC128VServer::dac128VServer(DAC128VServer *pDAC128VServer)
{
    while(true) {
        MessageServer *pMessageServer = pDAC128VServer->pMessageServer;
        DAC128V *pDAC128V = pDAC128VServer->pDAC128V;
        pMessageServer->waitForMessage();
        Message *inmsg;
        while((inmsg = pMessageServer->receive())) {
            if(inmsg->getType()!=messageTypeInt32) {
                printf("%s dac128VServer got illegal message type %d\n",
                    pMessageServer->getName(), inmsg->getType());
                delete inmsg;
            } else {
                Int32Message *pmessage = (Int32Message *)inmsg;
                pmessage->status = 0;
                int value = pmessage->value;
                int channel = pmessage->address;
                if(pDAC128V->setValue(value, channel)) pmessage->status= -1;
                pMessageServer->reply(pmessage);
            }
        }
    }
}

static const iocshArg initArg0 = { "Server name",iocshArgString};
static const iocshArg initArg1 = { "Carrier",iocshArgInt};
static const iocshArg initArg2 = { "Slot",iocshArgInt};
static const iocshArg initArg3 = { "queueSize",iocshArgInt};
static const iocshArg * const initArgs[4] = {&initArg0,
                                             &initArg1,
                                             &initArg2,
                                             &initArg3};
static const iocshFuncDef initFuncDef = {"initDAC128V",4,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    initDAC128V(args[0].sval, (int) args[1].sval, (int) args[2].sval, (int) args[3].sval);
}

void dac128VRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

epicsExportRegistrar(dac128VRegister);
