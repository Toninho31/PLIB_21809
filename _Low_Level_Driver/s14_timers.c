/*********************************************************************
*	TIMER modules (1, 2, 3, 4 et 5)
*	Author : Sébastien PERREAU
*
*	Revision history	:
*               15/11/2013              - Initial release
*********************************************************************/

#include "../PLIB.h"

extern const TIMER_REGISTERS * timerModules[];
const TIMER_REGISTERS * timerModules[] =
{
    (TIMER_REGISTERS *)_TMR1_BASE_ADDRESS,
    (TIMER_REGISTERS *)_TMR2_BASE_ADDRESS,
    (TIMER_REGISTERS *)_TMR3_BASE_ADDRESS,
    (TIMER_REGISTERS *)_TMR4_BASE_ADDRESS,
    (TIMER_REGISTERS *)_TMR5_BASE_ADDRESS
};

/*******************************************************************************
  Function:
    void TimerInit2345xUs(TIMER_MODULE mTimerModule, DWORD config, double periodUs);

  Description:
    This routine initialise the desire TIMER 2/3/4/5 module.

  Parameters:
    module      - Identifies the desired TIMER 2/3/4/5 module.

    config      - Configuration of the TIMER module.

    periodUs    - Set the desire period in Us.

  Returns:
    None.

  Example:
    <code>

    TimerInit2345xHz(TIMER2, T_OFF | T_SOURCE_INT | T_IDLE_CON | T_GATE_OFF, 98400);

    </code>
  *****************************************************************************/
void TimerInit2345xUs(TIMER_MODULE mTimerModule, DWORD config, double periodUs)
{
    TIMER_REGISTERS * timerRegister = (TIMER_REGISTERS *)timerModules[mTimerModule];
    DWORD vPRx = 100000;
    WORD vPrescale = 1;

    while(vPRx > 65535)
    {
        vPRx = (double)((periodUs * (PERIPHERAL_FREQ / 1000000L)) / vPrescale);
        if(vPrescale == 1) {
            vPrescale = 2;
            config |= T2_PS_1_1;
        }
        else if(vPrescale == 2) {
            vPrescale = 4;
            config |= T2_PS_1_2;
        }
        else if(vPrescale == 4) {
            vPrescale = 8;
            config |= T2_PS_1_4;
        }
        else if(vPrescale == 8) {
            vPrescale = 16;
            config |= T2_PS_1_8;
        }
        else if(vPrescale == 16) {
            vPrescale = 32;
            config |= T2_PS_1_16;
        }
        else if(vPrescale == 32) {
            vPrescale = 64;
            config |= T2_PS_1_32;
        }
        else if(vPrescale == 64) {
            vPrescale = 256;
            config |= T2_PS_1_64;
        }
        else if(vPrescale == 256) {
            vPrescale = 512;
            config |= T2_PS_1_256;
        }
        else
            break;
    }

    timerRegister->TxCON    = (config)&~(T_ON);
    timerRegister->TMRx     = 0x0000;
    timerRegister->PRx      = (vPRx);
    timerRegister->TxCON    = (config)&(T_ON);
}

/*******************************************************************************
  Function:
    double TimerGetPeriodUs(TIMER_MODULE mTimerModule);

  Description:
    This routine return the current period for the
    desire TIMER module.

  Parameters:
    module      - Identifies the desired TIMER module.

  Returns:
    double      - Desire period (in Us).

  Example:
    <code>

 ret = TimerGetPeriodUs(TIMER1);

    </code>
  *****************************************************************************/
double TimerGetPeriodUs(TIMER_MODULE mTimerModule)
{
    TIMER_REGISTERS * timerRegister = (TIMER_REGISTERS *)timerModules[mTimerModule];

    if(mTimerModule == TIMER1)
    {
        return (double)((timerRegister->PRx*pow(2, timerRegister->TxCONbits.TCKPS))*(1000000/PERIPHERAL_FREQ));
    }
    else
    {
        if(timerRegister->TxCONbits.TCKPS < 3)
        {
            return (double)((timerRegister->PRx*pow(8, timerRegister->TxCONbits.TCKPS))*(1000000/PERIPHERAL_FREQ));
        }
        else
        {
            return (double)((timerRegister->PRx*256.0)*(1000000/PERIPHERAL_FREQ));
        }
    }
}
