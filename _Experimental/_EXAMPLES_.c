/*********************************************************************
*	Author : Sébastien PERREAU
*
*	Revision history	:
*		02/06/2017		- Initial release
*                       - Add E_EEPROM example
* 
*   Description:
*   ------------ 
*********************************************************************/

#include "../PLIB.h"

void _EXAMPLE_SWITCH()
{
    SWITCH_DEF(sw1, SWITCH1, ACTIVE_LOW);
    static state_machine_t sm_example = {0};
    
    switch (sm_example.index)
    {
        case _SETUP:
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            
            switch (sw1.indice)
            {
                case 0:
                    mUpdateLedStatusD2(OFF);
                    mUpdateLedStatusD3(OFF);
                    break;
                case 1:
                    mUpdateLedStatusD2(fu_turn_indicator(ON, TICK_100MS, TICK_100MS));
                    break;
                case 2:
                    mUpdateLedStatusD2(fu_turn_indicator(ON, TICK_200MS, TICK_200MS));
                    break;
                case 3:
                    mUpdateLedStatusD2(fu_turn_indicator(ON, TICK_500MS, TICK_50MS));
                    break;
                case 4:
                    sw1.indice = 0;
                    break;
            }
            
            if (sw1.type_of_push == LONG_PUSH)
            {
                mUpdateLedStatusD2(BLINK);
                mUpdateLedStatusD3(BLINK_INV);
            }
            
            if (sw1.is_updated)
            {
                sw1.is_updated = false;
                // ...
            }
            
            fu_switch(&sw1);
            break;
    } 
}

void _EXAMPLE_ENCODER()
{
    ENCODER_DEF(encoder, __PE0, __PE1, ACTIVE_HIGH);
    static state_machine_t sm_example = {0};
    
    switch (sm_example.index)
    {
        case _SETUP:
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            
            if (encoder.indice > 0)
            {
                mUpdateLedStatusD2(fu_turn_indicator(ON, TICK_1MS*encoder.indice, TICK_1MS*encoder.indice));
                mUpdateLedStatusD3(OFF);
            }
            else if (encoder.indice < 0)
            {
                mUpdateLedStatusD2(OFF);
                mUpdateLedStatusD3(fu_turn_indicator(ON, -(TICK_1MS*encoder.indice), -(TICK_1MS*encoder.indice)));
            }
            else
            {
                mUpdateLedStatusD2(BLINK);
                mUpdateLedStatusD3(BLINK_INV);
            }
            
            fu_encoder(&encoder);
            break;
    }   
}

void _EXAMPLE_AVERAGE_AND_NTC()
{
    NTC_DEF(ntc_1, AN1, 25, 10000, 3380);
    NTC_DEF(ntc_2, AN2, 25, 10000, 3380);
    AVERAGE_DEF(avg_1, AN3, 30, TICK_1MS);
    static state_machine_t sm_example = {0};
    
    switch (sm_example.index)
    {
        case _SETUP:
            ADC10Init(AN1|AN2|AN3|AN15, ADC10_VREFP_VREFN);
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            fu_ntc(&ntc_1);
            fu_ntc(&ntc_2);
            fu_adc_average(&avg_1);
            
            if (ntc_1.temperature > 32.0)
            {
                // ...
            }
            break;
    }   
}

void _EXAMPLE_EEPROM()
{
    EEPROM_DEF(e_eeprom, SPI3A, csRD3, TICK_20MS, 150, 150);
    BUS_MANAGEMENT_DEF(bm_spi, &e_eeprom.spi_params.bus_management_params);
    static state_machine_t sm_example = {0};
    static uint64_t tickAntiFloodSw1 = 0;
    static uint64_t tickAntiFloodSw2 = 0;
    static uint64_t tickRead = 0;
    
    switch (sm_example.index)
    {
        case _SETUP:
            e_eeprom_check_for_erasing_memory(&e_eeprom, &bm_spi);
            sm_example.index = _MAIN;
            break;
            
        case _MAIN:
            if (!mGetIO(SWITCH1) && (mTickCompare(tickAntiFloodSw1) > TICK_200MS))
            {
                tickAntiFloodSw1 = mGetTick();
                if (!eEEPROMIsWriteInProgress(e_eeprom))
                {
                    // !! BE SURE TO HAVE ENOUGH SPACE INTO THE BUFFER !!
                    uint8_t i = 0;
                    e_eeprom.registers.dW.size = 5;
                    for(i = 0 ; i < (e_eeprom.registers.dW.size) ; i++)
                    {
                        e_eeprom.registers.dW.p[i] = 0x44+i;
                    }
                    eEEPROMWriteBytes(e_eeprom, 127);
                }
            }

            if (!mGetIO(SWITCH2) && (mTickCompare(tickAntiFloodSw2) > TICK_200MS))
            {
                tickAntiFloodSw2 = mGetTick();
                eEEPROMChipErase(e_eeprom);
            }

            if (mTickCompare(tickRead) >= TICK_100MS)
            {
                tickRead = mGetTick();
                eEEPROMReadBytes(e_eeprom, 127, 5);
            }

            if (!eEEPROMIsReadInProgress(e_eeprom))
            {
                if (e_eeprom.registers.dR.p[0] == 0x44)
                {
                    mUpdateLedStatusD2(ON);
                }
                else
                {
                    mUpdateLedStatusD2(OFF);
                }
            }
            
            fu_bus_management_task(&bm_spi);
            eEEPROMDeamon(&e_eeprom);
            break;
            
    }
}

void _EXAMPLE_MCP23S17()
{
    static MCP23S17_CONFIG e_mcp23s17;
    BUS_MANAGEMENT_DEF(bm, &e_mcp23s17.spi_params.bus_management_params);
    static bool isInitDone = false;
    static uint64_t tickAntiFloodSw1 = 0;
    
    if (!isInitDone)
    {
        eMCP23S17InitVar(SPI3A, csRD3, TICK_20MS, &e_mcp23s17);
        e_mcp23s17.write_registers.IODIRA = 0x00; 
        isInitDone = true;
    }
    else
    {
        if (!mGetIO(SWITCH1) && (mTickCompare(tickAntiFloodSw1) > TICK_200MS))
        {
            tickAntiFloodSw1 = mGetTick();
            
            e_mcp23s17.write_registers.OLATA = !e_mcp23s17.read_registers.GPIOA;
        }
        else
        {
            e_mcp23s17.write_registers.OLATA = 0x00;
        }
        
        if (GET_BIT(e_mcp23s17.read_registers.GPIOA, 0))
        {
            mUpdateLedStatusD2(ON);
        }
        else
        {
            mUpdateLedStatusD2(OFF);
        }
        
        fu_bus_management_task(&bm);
        eMCP23S17Deamon(&e_mcp23s17);
    }
}
