/*********************************************************************
*	SPI modules
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		16/10/2015		- Initial release
* 
*   Informations:
*   -------------
*   The user can read a data store in a buffer at any time. 
*   Interruptions are disable and acquisitions are made all the time.
*   For example if AN1, AN9 and AN15 are enable then the user can 
*   retrieve the data at any time with the routine ADC10Read(channel).
*********************************************************************/

#include "../PLIB.h"

void ADC10Init(UINT channels, ADC10_VOLTAGE_REF vref)
{
    BYTE i = 0;
    BYTE numberOfSamplesBetweenInterrupts = 0;
    
    for(i = 0 ; i < 16 ; i++)
    {
        if((channels >> i) & 0x0001)
        {
            numberOfSamplesBetweenInterrupts++;
        }
    }
    
    AD1CON1CLR = _AD1CON1_ON_MASK;
    AD1CSSL = channels;     // Select input channel to scan
    TRISB |= channels;    // Set IO as input pin
    AD1PCFG = ~channels;    // Set IO as analog pin
    AD1CHS = 0; // Ignore these bits because ADC_SCAN_ON in AD1CON2.
    AD1CON3 = ADC_CONV_CLK_INTERNAL_RC | ADC_SAMPLE_TIME_15;
    AD1CON2 = vref | ADC_SCAN_ON | (numberOfSamplesBetweenInterrupts << 2);
    AD1CON1 = ADC_MODULE_ON | ADC_FORMAT_INTG16 | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON;
}

WORD ADC10Read(WORD channel)
{
    BYTE i, j;
    for(i = 0, j = 0 ; i < 16 ; i++)
    {
        if((AD1CSSL >> i) & 0x0001)
        {
            j++;
        }
        if((channel >> i) & 0x0001)
        {
            return (*(&ADC1BUF0+((j-1) * 4)));
        }
    }
    return 0;
}
