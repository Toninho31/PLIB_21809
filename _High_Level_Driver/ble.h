#ifndef __BLE_H
#define __BLE_H

#define _VSD_VERSIONS			"2.001"
#define _NORDIC_SDK_VERSION		"nRF5_SDK_15.0.0_a53641a"

typedef enum
{
    ID_RESERVED_ = 0x00,
	ID_DEVICE_NAME = 0x01,                	// PIC -> BLE  (initialization sequence)
	ID_DEVICE_VSD_VERSION = 0x02,         	// PIC <- BLE  (initialization sequence)
	ID_DEVICE_NORDIC_SDK_VERSION = 0x03,  	// PIC <- BLE  (initialization sequence)
	ID_SERVICE_SCENARIO = 0x40,            	// PIC <-> BLE
	ID_DEVICE_RESET = 0xff               	// PIC -> BLE  Reset the BLE PICKIT & PIC Reset itself after BLE PICKIT ACK received
} BLE_SERVICES;

typedef enum
{
    UART_NO_MESSAGE,
    UART_ACK_MESSAGE,
    UART_NACK_MESSAGE,
    UART_NEW_MESSAGE,
    UART_OTHER_MESSAGE
} BLE_UART_MESSAGE_TYPE;

typedef struct
{
    BLE_UART_MESSAGE_TYPE message_type;
    bool dma_tx_in_progress;
	bool transmit_in_progress;
	bool receive_in_progress;
	uint8_t buffer[256];
	uint8_t index;
	uint64_t tick;
} ble_uart_t;

typedef struct
{
	uint8_t id;
	uint8_t type;
	uint8_t length;
	uint8_t data[251];
} ble_serial_message_t;

typedef union
{
    struct
    {
        unsigned set_name:1;
        unsigned get_vsd_version:1;
        unsigned get_nordic_sdk_version:1;
        unsigned exe_send_reset:1;
    };
    struct
    {
        uint32_t w;
    };
} ble_client_flags_t;

typedef union
{
    struct
    {
        unsigned ret_vsd_version:1;
        unsigned ret_nordic_sdk_version:1;
    };
    struct
    {
        uint32_t w;
    };
} ble_server_flags_t;

typedef union
{
    struct
    {
        unsigned is_connected_to_a_central:1;
        unsigned ble_is_in_advertising_mode:1;
    };
    struct
    {
        uint32_t w;
    };
} ble_server_status_flags_t;

typedef struct
{
    char vsd_version[5];
    char nordic_sdk_version[30];
    char device_name[20];
} ble_device_infos_t;

typedef struct
{
    uint8_t index;
    uint32_t counter;
} ble_service_scenario_t;

typedef struct
{
	ble_uart_t                      uart;
	ble_serial_message_t            message;
	ble_client_flags_t        		flagsC;     // Client is PIC or other uC
	ble_server_flags_t        		flagsS;     // Server is BLE PICKIT
	ble_server_status_flags_t		status;     // Server is BLE PICKIT
    // Services / Characteristics / Others
    ble_device_infos_t              infos;
    ble_service_scenario_t          scenario;
} ble_params_t;

#define BLE_DEVICE_INFOS_INSTANCE(_name)                \
{                                                       \
    .vsd_version = {_VSD_VERSIONS},                     \
    .nordic_sdk_version = {_NORDIC_SDK_VERSION},   		\
    .device_name = {_name}                              \
}

#define BLE_PARAMS_INSTANCE(_name)                      \
{                                                       \
	.uart = {0},                                        \
	.message = {0},                                     \
	.flagsC = {{0}},                                    \
	.flagsS = {{0}},                                    \
	.status = {{0}},									\
	.infos = BLE_DEVICE_INFOS_INSTANCE(_name),          \
	.scenario = {0},                                    \
}

#define BLE_DEF(_var, _name)    \
static ble_params_t _var = BLE_PARAMS_INSTANCE(_name)

typedef void (*p_function)(uint8_t *buffer);

void ble_init(ble_params_t * p_ble_params);
void ble_stack_tasks();

#endif
