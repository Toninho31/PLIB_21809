

#include "../PLIB.h"

static ble_params_t * p_ble;

static void _set_name(uint8_t *buffer);
static void _get_vsd_version(uint8_t *buffer);
static void _get_nordic_sdk_version(uint8_t *buffer);
static void _exe_send_reset(uint8_t *buffer);
static uint8_t vsd_send_request(p_function ptr);

void ble_init(ble_params_t * p_ble_params)
{   
    DmaChnOpen(DMA_CHANNEL2, DMA_CHN_PRI0, DMA_OPEN_MATCH);
    DmaChnSetEvEnableFlags(DMA_CHANNEL2, DMA_EV_BLOCK_DONE);	// enable the transfer done interrupt, when all buffer transferred
    IRQInit(IRQ_DMA2, IRQ_ENABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
    DmaChnSetEventControl(DMA_CHANNEL2, DMA_EV_START_IRQ(_UART4_TX_IRQ));
    
    UARTConfigure(UART4, UART_ENABLE_PINS_TX_RX_ONLY | UART_ENABLE_HIGH_SPEED);
    UARTSetLineControl(UART4, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART4, PERIPHERAL_FREQ, 1000000);
    UARTSetFifoMode(UART4, UART_INTERRUPT_ON_TX_DONE | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTEnable(UART4, UART_ENABLE | UART_PERIPHERAL | UART_RX | UART_TX);
    
    IRQInit(IRQ_U4E, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
    IRQInit(IRQ_U4TX, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_3, IRQ_SUB_PRIORITY_LEVEL_1);
    IRQInit(IRQ_U4RX, IRQ_DISABLED, IRQ_PRIORITY_LEVEL_5, IRQ_SUB_PRIORITY_LEVEL_1);
    
    p_ble = p_ble_params;
    p_ble->flagsC.set_name = true;
}

void __ISR(_DMA_2_VECTOR, IPL3SOFT) DmaHandler2(void)
{
    p_ble->uart.dma_tx_in_progress = false;
    IRQClearFlag(IRQ_DMA2);
}

void ble_stack_tasks()
{    
    p_ble->uart.transmit_in_progress = (p_ble->uart.dma_tx_in_progress | !U4STAbits.TRMT);
    if (U4STAbits.URXDA)
    {
        p_ble->uart.receive_in_progress = true;
        p_ble->uart.tick = mGetTick();
        p_ble->uart.buffer[p_ble->uart.index] = (uint8_t) (U4RXREG);
        p_ble->uart.index++;
    }
    
    if (mTickCompare(p_ble->uart.tick) >= TICK_300US)
    {
        if (	(p_ble->uart.index == 3) && \
                (p_ble->uart.buffer[0] == 'A') && \
                (p_ble->uart.buffer[1] == 'C') && \
                (p_ble->uart.buffer[2] == 'K'))
        {
            p_ble->uart.message_type = UART_ACK_MESSAGE;
        }
        else if (	(p_ble->uart.index == 4) && \
                    (p_ble->uart.buffer[0] == 'N') && \
                    (p_ble->uart.buffer[1] == 'A') && \
                    (p_ble->uart.buffer[2] == 'C') && \
                    (p_ble->uart.buffer[3] == 'K'))
        {
            p_ble->uart.message_type = UART_NACK_MESSAGE;
        }
        else if (	(p_ble->uart.index > 5) && \
                    ((p_ble->uart.buffer[1] == 'W') || \
					(p_ble->uart.buffer[1] == 'R') || \
					(p_ble->uart.buffer[1] == 'D')))
        {
            p_ble->uart.message_type = UART_NEW_MESSAGE;
        }
        else
        {
            p_ble->uart.message_type = UART_OTHER_MESSAGE;
        }
        p_ble->uart.index = 0;
        p_ble->uart.receive_in_progress = false;
    }
    
    if (p_ble->uart.message_type == UART_NEW_MESSAGE)
    {
        uint8_t i;
        uint16_t crc_calc, crc_uart;
    
        p_ble->uart.message_type = UART_NO_MESSAGE;
        
        crc_calc = fu_crc_16_ibm(p_ble->uart.buffer, p_ble->uart.buffer[2]+3);
        crc_uart = (p_ble->uart.buffer[p_ble->uart.buffer[2]+3] << 8) + (p_ble->uart.buffer[p_ble->uart.buffer[2]+4] << 0);

        if (crc_calc == crc_uart)
        {
            p_ble->message.id = p_ble->uart.buffer[0];
            p_ble->message.type = p_ble->uart.buffer[1];
            p_ble->message.length = p_ble->uart.buffer[2];
            for (i = 0 ; i < p_ble->message.length ; i++)
            {
                p_ble->message.data[i] = p_ble->uart.buffer[3+i];
            }
            DmaChnSetTxfer(DMA_CHANNEL2, "ACK", (void*)&U4TXREG, 3, 1, 1);
            DmaChnStartTxfer(DMA_CHANNEL2, DMA_WAIT_NOT, 0);
            p_ble->uart.dma_tx_in_progress = true;
        }
        else
        {
            p_ble->message.id = ID_RESERVED_;
            DmaChnSetTxfer(DMA_CHANNEL2, "NACK", (void*)&U4TXREG, 4, 1, 1);
            DmaChnStartTxfer(DMA_CHANNEL2, DMA_WAIT_NOT, 0);
            p_ble->uart.dma_tx_in_progress = true;
        }
        memset(p_ble->uart.buffer, 0, sizeof(p_ble->uart.buffer));
       
        switch (p_ble->message.id)
        {
            case ID_DEVICE_VSD_VERSION:
                if (p_ble->message.type == 'R')
                {
                    for (i = 0 ; i < p_ble->message.length ; i++)
                    {
                        p_ble->infos.vsd_version[i] = p_ble->message.data[i];
                    }
                }
                break;
                
            case ID_DEVICE_NORDIC_SDK_VERSION:
                if (p_ble->message.type == 'R')
                {
                    for (i = 0 ; i < p_ble->message.length ; i++)
                    {
                        p_ble->infos.nordic_sdk_version[i] = p_ble->message.data[i];
                    }
                }
                break;
                
            case ID_SERVICE_SCENARIO:
                if (p_ble->message.type == 'D')
                {
                    p_ble->scenario.index = p_ble->message.data[0];
                    p_ble->scenario.counter++;
                }
                break;

            default:
                break;

        }
    }
    
    if (p_ble->flagsC.w > 0)
    {
        if (p_ble->flagsC.set_name)
        {
            if (!vsd_send_request(_set_name))
            {
                p_ble->flagsC.set_name = false;
            }
        }
        else if (p_ble->flagsC.get_vsd_version)
        {
            if (!vsd_send_request(_get_vsd_version))
            {
                p_ble->flagsC.get_vsd_version = false;
            }
        }
        else if (p_ble->flagsC.get_nordic_sdk_version)
        {
            if (!vsd_send_request(_get_nordic_sdk_version))
            {
                p_ble->flagsC.get_nordic_sdk_version = false;
            }
        }
        else if (p_ble->flagsC.exe_send_reset)
        {
            if (!vsd_send_request(_exe_send_reset))
            {
                SoftReset();
            }
        }
    }
    
}

static void _set_name(uint8_t *buffer)
{
    uint8_t i = 0;
	uint16_t crc = 0;

	buffer[0] = ID_DEVICE_NAME;
	buffer[1] = 'W';
	
	for (i = 0 ; i < 20 ; i++)
    {
        if (p_ble->infos.device_name[i] == '\0')
        {
            break;
        }
        buffer[3+i] = p_ble->infos.device_name[i];
    }
    buffer[2] = i;
    
	crc = fu_crc_16_ibm(buffer, buffer[2]+3);
	buffer[buffer[2]+3] = (crc >> 8) & 0xff;
	buffer[buffer[2]+4] = (crc >> 0) & 0xff;
}

static void _get_vsd_version(uint8_t *buffer)
{
	uint16_t crc = 0;

	buffer[0] = ID_DEVICE_VSD_VERSION;
	buffer[1] = 'R';
	buffer[2] = 0;
	crc = fu_crc_16_ibm(buffer, buffer[2]+3);
	buffer[buffer[2]+3] = (crc >> 8) & 0xff;
	buffer[buffer[2]+4] = (crc >> 0) & 0xff;
}

static void _get_nordic_sdk_version(uint8_t *buffer)
{
	uint16_t crc = 0;

	buffer[0] = ID_DEVICE_NORDIC_SDK_VERSION;
	buffer[1] = 'R';
	buffer[2] = 0;
	crc = fu_crc_16_ibm(buffer, buffer[2]+3);
	buffer[buffer[2]+3] = (crc >> 8) & 0xff;
	buffer[buffer[2]+4] = (crc >> 0) & 0xff;
}

static void _exe_send_reset(uint8_t *buffer)
{
	uint16_t crc = 0;

	buffer[0] = ID_DEVICE_RESET;
	buffer[1] = 'W';
    buffer[2] = 1;
    buffer[3] = 23;
	crc = fu_crc_16_ibm(buffer, buffer[2]+3);
	buffer[buffer[2]+3] = (crc >> 8) & 0xff;
	buffer[buffer[2]+4] = (crc >> 0) & 0xff;
}

static uint8_t vsd_send_request(p_function ptr)
{
    static state_machine_t sm;
	static uint8_t buffer[256] = {0};

	switch (sm.index)
	{

        case 0:
            
            if (!p_ble->uart.transmit_in_progress && !p_ble->uart.receive_in_progress)
            {
                sm.index++;
                sm.tick = mGetTick();
            }
            break;
            
		case 1:

			(*ptr)(buffer);
            
            DmaChnSetTxfer(DMA_CHANNEL2, buffer, (void*)&U4TXREG, buffer[2]+5, 1, 1);
            DmaChnStartTxfer(DMA_CHANNEL2, DMA_WAIT_NOT, 0);
            p_ble->uart.dma_tx_in_progress = true;

			sm.index++;
			sm.tick = mGetTick();
			break;

		case 2:

            if (!p_ble->uart.transmit_in_progress)
            {
                memset(buffer, 0, sizeof(buffer));
                sm.index++;
                sm.tick = mGetTick();
            }
			break;

		case 3:

            if (p_ble->uart.message_type == UART_ACK_MESSAGE)
            {
                p_ble->uart.message_type = UART_NO_MESSAGE;
                sm.index = 0;
            }
            else if (p_ble->uart.message_type == UART_NACK_MESSAGE)
            {
                p_ble->uart.message_type = UART_NO_MESSAGE;
                sm.index = 1;
            }
            else if (mTickCompare(sm.tick) >= TICK_10MS)
            {
                sm.index = 1;
            }
			break;

		default:
            sm.index = 0;
			break;

	}

	return sm.index;
}
 