//dac128V.cc

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

    17-MAY-2000  MLR  Added getValue() method
    27-MAY-2003  MLR  Converted to R3.14, used IPAC calls directly.

*/

#include <stdio.h>
#include <drvIpac.h>
 
#include "DAC128V.h"

#define SYSTRAN_ID 0x45
#define SYSTRAN_DAC128V 0x69


DAC128V * DAC128V::init(ushort_t carrier, ushort_t slot)
{
    if (ipmValidate(carrier, slot, SYSTRAN_ID, SYSTRAN_DAC128V) != 0) {
       printf("initDAC128V module not found in slot\n");
       return(0);
    }
    // lastChan and maxValue could be set by looking at "model" in the future
    // if models with more channels or more bits are available
    int LastChan = 7;
    int MaxValue = 4095;
    DAC128V *pDAC128V = new DAC128V(carrier, slot, LastChan, MaxValue);
    return(pDAC128V);
}

DAC128V::DAC128V(
    ushort_t carrier, ushort_t slot, int lastChan, int maxValue)
: lastChan(lastChan), maxValue(maxValue)
{
    regs = (unsigned short *) ipmBaseAddr(carrier, slot, ipac_addrIO);
}

int DAC128V::setValue(int value, int channel)
{
    if(value<0 || value>maxValue || channel<0 || channel>lastChan) return(-1);
    regs[channel] = value;
    return(0);
}

int DAC128V::getValue(int *value, int channel)
{
    if(channel<0 || channel>lastChan) return(-1);
    *value=regs[channel];
    return(0);
}
