//DAC128V.h

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

    17-May-2000 MLR Added getValue() method

*/

#ifndef dac128VH
#define dac128VH

class IndustryPackModule;

class DAC128V
{
public:
    static DAC128V * init(
        const char *moduleName, const char *carrierName, const char *siteName);
    int setValue(int value, int channel);
    int getValue(int *value, int channel);
private:
    DAC128V(IndustryPackModule *pIndustryPackModule, int lastChan, 
                                                     int maxValue);
    IndustryPackModule *pIPM;
    volatile unsigned short* regs;
    int lastChan;
    int maxValue;
};

#endif //dac128VH
