// devAoDAC128V.cc

// Original author: Jim Kowalkowski
// Date: 6/95
// Current author:  Mark Rivers
// Converted to MPF 9/4/99

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <alarm.h>
#include <recGbl.h>

#include <semLib.h>
#include <tickLib.h>
#include <taskLib.h>

extern "C" {
#include "dbAccess.h"
#include "dbDefs.h"
#include "link.h"
#include "epicsPrint.h"
#include "dbCommon.h"
#include "aoRecord.h"
#include "recSup.h"
}

#include "Message.h"
#include "Int32Message.h"
#include "DevMpf.h"

extern "C"
{
#ifdef NODEBUG
#define DEBUG(l,f,v...) ;
#else
#define DEBUG(l,f,v...) { if(l<=devAoDAC128VDebug) printf(f,## v); }
#endif
volatile int devAoDAC128VDebug = 0;
}

class aoDAC : public DevMpf
{
public:
        aoDAC(dbCommon*,DBLINK*);

        long startIO(dbCommon* pr);
        long completeIO(dbCommon* pr,Message* m);
        virtual long convert(dbCommon* pr, int pass);

        static long dev_init(void*);
private:
        int channel;
};

MAKE_LINCONV_DSET(devAoDAC128V,aoDAC::dev_init)

long aoDAC::dev_init(void* v)
{
        DEBUG(2,"aoDAC::dev_init(v)\n");
        aoRecord* ao = (aoRecord*)v;
        aoDAC *paoDAC = new aoDAC((dbCommon*)ao,&(ao->out));
        paoDAC->bind();
        // set linear conversion slope
        ao->eslo = (ao->eguf - ao->egul)/4095.0;
        return(MPF_NoConvert);
}

aoDAC::aoDAC(dbCommon* pr,DBLINK* l) : DevMpf(pr,l,false)
{
        vmeio* io = (vmeio*)&(l->value);

        DEBUG(2,"aoDAC::aoDAC(pr,l)\n");

        channel=io->signal;

        if(channel<0 || channel>7)
        {
            epicsPrintf("%s devAoDAC (init_record) Illegal OUT signal field (0-7)\n",
                pr->name);
        }
}

long aoDAC::startIO(dbCommon* pr)
{
        DEBUG(2,"aoDAC::StartIO(pr)\n");
        Int32Message *pim = new Int32Message;
        aoRecord* ao = (aoRecord*)pr;

        pim->address=channel;
        pim->value=ao->rval;
        return sendReply(pim);
}

long aoDAC::completeIO(dbCommon* pr,Message* m)
{
        aoRecord* ao = (aoRecord*)pr;
        Int32Message *pim = (Int32Message*)m;

        DEBUG(2,"aoDAC::CompleteIO(pr,m)\n");
        long rc=pim->status;

        if(rc==0)
                ao->udf=0;
        else
                recGblSetSevr(pr,READ_ALARM,INVALID_ALARM);

        delete m;

        return rc;
}

long aoDAC::convert(dbCommon* pr, int pass)
{
        DEBUG(2,"aoDAC::convert(pr, %d)\n", pass);
        aoRecord* ao = (aoRecord*)pr;

        if (pass==0) return(0);
        // set linear conversion slope
        ao->eslo = (ao->eguf - ao->egul)/4095.0;

        return 0;
}
