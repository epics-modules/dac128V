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

*/

#include <vxWorks.h>
#include <iv.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
 
#include "DAC128V.h"
#include "IndustryPackModule.h"

#define SYSTRAN_ID 0x45
#define SYSTRAN_DAC128V 0x69


DAC128V * DAC128V::init(
    const char *moduleName, const char *carrierName, const char *siteName)
{
    IndustryPackModule *pIPM = IndustryPackModule::createIndustryPackModule(
        moduleName,carrierName,siteName);
    if(!pIPM) return(0);
    unsigned char manufacturer = pIPM->getManufacturer();
    unsigned char model = pIPM->getModel();
    if(manufacturer!=SYSTRAN_ID) {
        printf("initDAC128V manufacturer 0x%x not SYSTRAN_ID\n",
            manufacturer);
        return(0);
    }
    if(model!=SYSTRAN_DAC128V) {
       printf("initDAC128V model 0x%x not a SYSTRAN_DAC128V\n",model);
       return(0);
    }
    // lastChan and maxValue could be set by looking at "model" in the future
    // if models with more channels or more bits are available
    int LastChan = 7;
    int MaxValue = 4095;
    DAC128V *pDAC128V = new DAC128V(pIPM, LastChan, MaxValue);
    return(pDAC128V);
}

DAC128V::DAC128V(
    IndustryPackModule *pIndustryPackModule, int lastChan, int maxValue)
: pIPM(pIndustryPackModule), lastChan(lastChan), maxValue(maxValue)
{
    regs = (unsigned short *) pIPM->getMemBaseIO();
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
