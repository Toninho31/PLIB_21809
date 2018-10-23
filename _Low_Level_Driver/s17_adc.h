#ifndef __DEF_ADC10
#define	__DEF_ADC10

#define AN0              0x0001
#define AN1              0x0002
#define AN2              0x0004
#define AN3              0x0008
#define AN4              0x0010
#define AN5              0x0020
#define AN6              0x0040
#define AN7              0x0080
#define AN8              0x0100
#define AN9              0x0200
#define AN10             0x0400
#define AN11             0x0800
#define AN12             0x1000
#define AN13             0x2000
#define AN14             0x4000
#define AN15             0x8000

typedef enum _ADC10_VOLTAGE_REF
{
	ADC10_VDD_VSS = 0,
    ADC10_VREFP_VSS = (1 << 13),
    ADC10_VDD_VREFN = (2 << 13),
    ADC10_VREFP_VREFN = (3 << 13)
}ADC10_VOLTAGE_REF;

void ADC10Init(UINT channels, ADC10_VOLTAGE_REF vref);
WORD ADC10Read(WORD channel);

#endif
