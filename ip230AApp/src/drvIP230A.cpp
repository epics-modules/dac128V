/* drvIP230A.cpp
 * Driver for AcroMag IP230A using asynPortDriver base class
*/

#include <asynPortDriver.h>
#include <drvIpac.h>
#include <iocsh.h>
#include <epicsExport.h>

#define ACROMAG_ID 0xA3
#define ACROMAG_IP230A 0x18
#define MAX_CHANNELS 8

static const char *driverName = "IP230A";

/** This is the class definition for the IP230A class
  */
class IP230A : public asynPortDriver {
public:
    IP230A(const char *portName, int carrier, int slot);

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    virtual asynStatus getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
    virtual void report(FILE *fp, int details);

protected:
    int IP230A_Data;          /**< (asynInt32, r/w) DAC output value in device units */
    int IP230A_DoubleData;    /**< (asynFloat64, r/w) DAC output value in device units but double */
    
private:
    int lastChan;
    int maxValue;
    volatile unsigned short* regs;    
};


#define IP230ADataString        "DATA"
#define IP230ADoubleDataString  "DOUBLE_DATA"

IP230A::IP230A(const char *portName, int carrier, int slot)
    : asynPortDriver(portName, MAX_CHANNELS,
          asynInt32Mask | asynFloat64Mask | asynDrvUserMask,
          asynInt32Mask | asynFloat64Mask, 
          ASYN_MULTIDEVICE, 1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=1, autoConnect=1 */
          0, 0)  /* Default priority and stack size */
{
    static const char *functionName = "IP230A";

    createParam(IP230ADataString,       asynParamInt32,   &IP230A_Data);
    createParam(IP230ADoubleDataString, asynParamFloat64, &IP230A_DoubleData);
    
    if (ipmValidate(carrier, slot, ACROMAG_ID, ACROMAG_IP230A) != 0) {
       asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
            "%s:%s: module not found in carrier %d slot %d\n",
            driverName, functionName, carrier, slot);
    } else {
        this->regs = (unsigned short *) ipmBaseAddr(carrier, slot, ipac_addrIO);
    }

    /* lastChan and maxValue could be set by looking at "model" in the future
     * if models with more channels or more bits are available */
    this->lastChan = 7;
    this->maxValue = 65535;
   /* enable single conversion for IP230A DAC Module */
   this->regs[0] = 0x0100;
}

asynStatus IP230A::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int channel;
    static const char *functionName = "writeInt32";

    this->getAddress(pasynUser, &channel);
    if(value<0 || value>this->maxValue || channel<0 || channel>this->lastChan)
        return(asynError);
    setIntegerParam(channel, IP230A_Data, value);
    /* DAC channels at 0x10 offset from base address */
    this->regs[0x10 + channel] = value;
    /* write to the Start Convert bit to initialize d-a conversion  */
    this->regs[0x0E] = 0x0001;
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
              "%s:%s, port %s, wrote %d to channel %d\n",
              driverName, functionName, this->portName, value, channel);
    return(asynSuccess);
}

asynStatus IP230A::getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high)
{
    *low = 0;
    *high = this->maxValue;
    return(asynSuccess);
}

asynStatus IP230A::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    return(this->writeInt32(pasynUser, (epicsInt32) value));
}

asynStatus IP230A::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
    int channel;
    static const char *functionName = "readInt32";

    this->getAddress(pasynUser, &channel);
    if(channel<0 || channel>this->lastChan) return(asynError);
    *value=this->regs[channel];
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
              "%s:%s, port %s, read %d from channel %d\n",
              driverName, functionName, this->portName, *value, channel);
    return(asynSuccess);
}

asynStatus IP230A::readFloat64(asynUser *pasynUser, epicsFloat64 *value)
{
    epicsInt32 ivalue;
    asynStatus status;

    status = this->readInt32(pasynUser, &ivalue);
    *value = (epicsFloat64)ivalue;
    return(status);
}

/* Report  parameters */
void IP230A::report(FILE *fp, int details)
{
    asynPortDriver::report(fp, details);
    fprintf(fp, "  Port: %s, address %p\n", this->portName, this->regs);
    if (details >= 1) {
        fprintf(fp, "  lastChan=%d, maxValue=%d\n", 
                this->lastChan, this->maxValue);
    }
}

/** Configuration command, called directly or from iocsh */
extern "C" int initIP230A(const char *portName, int carrier, int slot)
{
    new IP230A(portName, carrier, slot);
    return(asynSuccess);
}


static const iocshArg initArg0 = { "Port name",iocshArgString};
static const iocshArg initArg1 = { "Carrier",iocshArgInt};
static const iocshArg initArg2 = { "Slot",iocshArgInt};
static const iocshArg * const initArgs[3] = {&initArg0,
                                             &initArg1,
                                             &initArg2};
static const iocshFuncDef initFuncDef = {"initIP230A",3,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    initIP230A(args[0].sval, args[1].ival, args[2].ival);
}

void drvIP230ARegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
epicsExportRegistrar(drvIP230ARegister);
}
