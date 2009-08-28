/* drvDac128V.cpp
 * Driver for Systran DAC-128V using asynPortDriver base class
*/

#include <asynPortDriver.h>
#include <drvIpac.h>
#include <iocsh.h>
#include <epicsExport.h>

#define SYSTRAN_ID 0x45
#define SYSTRAN_DAC128V 0x69
#define MAX_CHANNELS 8

static const char *driverName = "DAC128V";

/** This is the class definition for the DAC128V class
  */
class DAC128V : public asynPortDriver {
public:
    DAC128V(const char *portName, int carrier, int slot);

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    virtual asynStatus getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
    virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
                                     const char **pptypeName, size_t *psize);
    virtual void report(FILE *fp, int details);
    
private:
    int lastChan;
    int maxValue;
    volatile unsigned short* regs;    
};

/** Enumeration of parameters that affect the behaviour of the driver. 
  * These are the values that asyn will place in pasynUser->reason when the
  * standard asyn interface methods are called. */
typedef enum
{
    /*    Name               asyn interface          access   Description  */
    DAC128V_Data      /**< (asynInt32, asynFloat64,    r/w) DAC output value in device units */
} DAC128VDriverParam_t;

static asynParamString_t DAC128VParamString[] = {
    {DAC128V_Data,   "DATA"}
};

#define NUM_PARAMS (sizeof(DAC128VParamString)/sizeof(DAC128VParamString[0]))

DAC128V::DAC128V(const char *portName, int carrier, int slot)
    : asynPortDriver(portName, MAX_CHANNELS-1, NUM_PARAMS, 
          asynInt32Mask | asynFloat64Mask | asynDrvUserMask,
          asynInt32Mask | asynFloat64Mask, 
          ASYN_MULTIDEVICE, 1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=1, autoConnect=1 */
          0, 0)  /* Default priority and stack size */
{
    static const char *functionName = "DAC128V";

    if (ipmValidate(carrier, slot, SYSTRAN_ID, SYSTRAN_DAC128V) != 0) {
       asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
            "%s:%s: module not found in carrier %d slot %d\n",
            driverName, functionName, carrier, slot);
    } else {
        this->regs = (unsigned short *) ipmBaseAddr(carrier, slot, ipac_addrIO);
    }

    /* lastChan and maxValue could be set by looking at "model" in the future
     * if models with more channels or more bits are available */
    this->lastChan = 7;
    this->maxValue = 4095;
}

asynStatus DAC128V::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int channel;
    static const char *functionName = "writeInt32";

    this->getAddress(pasynUser, functionName, &channel);
    if(value<0 || value>this->maxValue || channel<0 || channel>this->lastChan)
        return(asynError);
    this->regs[channel] = value;
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
              "%s:%s, port %s, wrote %d to channel %d\n",
              driverName, functionName, this->portName, value, channel);
    return(asynSuccess);
}

asynStatus DAC128V::getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high)
{
    *low = 0;
    *high = this->maxValue;
    return(asynSuccess);
}

asynStatus DAC128V::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
   return(this->writeInt32(pasynUser, (epicsInt32) value));
}

asynStatus DAC128V::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
    int channel;
    static const char *functionName = "readInt32";

    this->getAddress(pasynUser, functionName, &channel);
    if(channel<0 || channel>this->lastChan) return(asynError);
    *value=this->regs[channel];
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
              "%s:%s, port %s, read %d from channel %d\n",
              driverName, functionName, this->portName, *value, channel);
    return(asynSuccess);
}

asynStatus DAC128V::readFloat64(asynUser *pasynUser, epicsFloat64 *value)
{
    epicsInt32 ivalue;
    asynStatus status;

    status = this->readInt32(pasynUser, &ivalue);
    *value = (epicsFloat64)ivalue;
    return(status);
}

/** Sets pasynUser->reason to one of the enum values for the DAC128VDriverParam_t values
  * if the drvInfo field matches one the strings defined above.
  * Simply calls asynPortDriver::drvUserCreateParam with the parameter table for this driver.
  * \param[in] pasynUser pasynUser structure that driver modifies
  * \param[in] drvInfo String containing information about what driver function is being referenced
  * \param[out] pptypeName Location in which driver puts a copy of drvInfo.
  * \param[out] psize Location where driver puts size of param 
  * \return Returns asynSuccess if a matching string was found, asynError if not found. */
asynStatus DAC128V::drvUserCreate(asynUser *pasynUser,
                                            const char *drvInfo, 
                                            const char **pptypeName, size_t *psize)
{
    asynStatus status;
    //const char *functionName = "drvUserCreate";
    status = this->drvUserCreateParam(pasynUser, drvInfo, pptypeName, psize, 
                                      DAC128VParamString, NUM_PARAMS);
    return(status);    
}

/* Report  parameters */
void DAC128V::report(FILE *fp, int details)
{
    asynPortDriver::report(fp, details);
    fprintf(fp, "  Port: %s, address %p\n", this->portName, this->regs);
    if (details >= 1) {
        fprintf(fp, "  lastChan=%d, maxValue=%d\n", 
                this->lastChan, this->maxValue);
    }
}

/** Configuration command, called directly or from iocsh */
extern "C" int initDAC128V(const char *portName, int carrier, int slot)
{
    new DAC128V(portName, carrier, slot);
    return(asynSuccess);
}


static const iocshArg initArg0 = { "Port name",iocshArgString};
static const iocshArg initArg1 = { "Carrier",iocshArgInt};
static const iocshArg initArg2 = { "Slot",iocshArgInt};
static const iocshArg * const initArgs[3] = {&initArg0,
                                             &initArg1,
                                             &initArg2};
static const iocshFuncDef initFuncDef = {"initDAC128V",3,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    initDAC128V(args[0].sval, args[1].ival, args[2].ival);
}

void drvDac128VRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
epicsExportRegistrar(drvDac128VRegister);
}
