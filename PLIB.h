#ifndef __DEF_INCLUDES_PLIB
#define	__DEF_INCLUDES_PLIB

#include <xc.h>		// xc pic32 header
#include <plib.h>	// xc pic32 libraries
#include <math.h>
#include <lega-c/machine/types.h>
#include "ctype.h"

#include "defines.h"

#include "_Low_Level_Driver/s08_interrupt_mapping.h"
#include "_Low_Level_Driver/s12_ports.h"
#include "_Low_Level_Driver/s14_timers.h"
#include "_Low_Level_Driver/s16_output_compare.h"
#include "_Low_Level_Driver/s17_adc.h"
#include "_Low_Level_Driver/s21_uart.h"
#include "_Low_Level_Driver/s23_spi.h"
#include "_Low_Level_Driver/s24_i2c.h"
#include "_Low_Level_Driver/s34_can.h"
#include "_Low_Level_Driver/s35_ethernet_TCPIP.h"
#include "_Low_Level_Driver/s35_ethernet_OSI-2_DataLinkLayer.h"
#include "_Low_Level_Driver/s35_ethernet_OSI-3_NetworkLayer.h"
#include "_Low_Level_Driver/s35_ethernet_OSI-4_TransportLayer.h"
#include "_Low_Level_Driver/s35_ethernet_OSI-5_ApplicationLayer.h"
#include "_Low_Level_Driver/s35_ethernet_Applications.h"

#include "_High_Level_Driver/utilities.h"
#include "_High_Level_Driver/string_advance.h"
#include "_High_Level_Driver/one_wire_communication.h"
#include "_High_Level_Driver/lin.h"
#include "_High_Level_Driver/ble.h"

#include "_External_Components/e_eeprom.h"
#include "_External_Components/e_mcp23s17.h"
#include "_External_Components/e_ws2812b.h"
#include "_External_Components/e_qt2100.h"
#include "_External_Components/e_amis30621.h"
#include "_External_Components/e_tmc429.h"

#include "_Experimental/_EXAMPLES_.h"

uint8_t _ledStatus;

#endif
