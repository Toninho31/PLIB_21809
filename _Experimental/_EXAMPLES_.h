#ifndef __DEF_EXAMPLES
#define __DEF_EXAMPLES

typedef enum
{
    _SETUP = 0,
    _MAIN
} _EXAMPLE_STATUS;

void _EXAMPLE_SWITCH();
void _EXAMPLE_ENCODER();
void _EXAMPLE_AVERAGE_AND_NTC();
void _EXAMPLE_EEPROM();
void _EXAMPLE_MCP23S17();

#endif
