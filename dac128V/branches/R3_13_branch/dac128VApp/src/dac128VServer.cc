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

#include <vxWorks.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <taskLib.h>

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


static char taskname[] = "dac128V";
extern "C" DAC128V *initDAC128V(
    const char *moduleName, const char *carrierName, const char *siteName,
    int queueSize)
{
    DAC128V *pDAC128V = DAC128V::init(moduleName,carrierName,siteName);
    if(!pDAC128V) return(0);
    DAC128VServer *pDAC128VServer = new DAC128VServer;
    pDAC128VServer->pDAC128V = pDAC128V;
    pDAC128VServer->pMessageServer = new MessageServer(moduleName,queueSize);
    int taskId = taskSpawn(taskname,100,VX_FP_TASK,2000,
        (FUNCPTR)DAC128VServer::dac128VServer,(int)pDAC128VServer,
        0,0,0,0,0,0,0,0,0);
    if(taskId==ERROR)
        printf("%s dac128VServer taskSpawn Failure\n",
            pDAC128VServer->pMessageServer->getName());
    return(pDAC128V);
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
