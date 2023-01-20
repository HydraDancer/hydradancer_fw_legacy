#ifndef USB20_H
#define USB20_H

#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"

/* macros */
#define U20_MAXPACKET_LEN (512)                                                                     
#define U20_UEP0_MAXSIZE  (64)  // Change accordingly to USB mode (Here HS).
#define U20_UEP1_MAXSIZE  (512) // Change accordingly to USB mode (Here HS).
#define UsbSetupBuf       ((PUSB_SETUP)endp0RTbuff)                                                 

/* enums */
enum Speed { SpeedLow = UCST_LS, SpeedFull = UCST_FS, SpeedHigh = UCST_HS };
enum Endpoint {
    Ep1Mask = 1 << 0,
    Ep2Mask = 1 << 1,
    Ep3Mask = 1 << 2,
    Ep4Mask = 1 << 3,
    Ep5Mask = 1 << 4,
    Ep6Mask = 1 << 5,
    Ep7Mask = 1 << 6,
};
enum ConfigurationDescriptorType { CfgDescrBase, CfgDescrWithHid, CfgDescr2Ep };

typedef union {
    uint16_t w;
    struct BW {
        uint8_t bb1; /* Low byte. */
        uint8_t bb0;
    } bw;
} UINT16_UINT8;

typedef struct __PACKED {
    uint8_t       bRequestType;
    uint8_t       bRequest;
    UINT16_UINT8  wValue;
    UINT16_UINT8  wIndex;
    uint16_t      wLength;
} *PUSB_SETUP;

typedef struct __PACKED _USB_CONFIG_DESCR_FULL_BASE {
    USB_CFG_DESCR  cfgDescr;
    USB_ITF_DESCR  itfDescr;
    USB_ENDP_DESCR endpDescr;
} USB_CFG_DESCR_FULL_BASE, *PUSB_CFG_DESCR_FULL_BASE;

typedef struct __PACKED _USB_CONFIG_DESCR_FULL_HID {
    USB_CFG_DESCR  cfgDescr;
    USB_ITF_DESCR  itfDescr;
    USB_HID_DESCR  hidDescr;
    USB_ENDP_DESCR endpDescr;
} USB_CFG_DESCR_FULL_HID, *PUSB_CFG_DESCR_FULL_HID;

typedef struct __PACKED _USB_CONFIG_DESCR_FULL_2_ENDPOINTS {
    USB_CFG_DESCR  cfgDescr;
    USB_ITF_DESCR  itfDescr;
    USB_ENDP_DESCR endpDescr1In;
    USB_ENDP_DESCR endpDescr1Out;
} USB_CFG_DESCR_FULL_2_ENDPOINTS, *PUSB_CFG_DESCR_FULL_2_ENDPOINTS;

typedef union {
    USB_CFG_DESCR_FULL_BASE base;
    USB_CFG_DESCR_FULL_HID withHid;
    USB_CFG_DESCR_FULL_2_ENDPOINTS base2Ep;
} USB_CFG_DESCR_FULL, *PUSB_CFG_DESCR_FULL;

/* variables */
enum ConfigurationDescriptorType cfgDescrType;
enum Speed speed;
enum Endpoint epMask;

USB_DEV_DESCR stDeviceDescriptor;
USB_CFG_DESCR_FULL stConfigurationDescriptor;
USB_ITF_DESCR stInterfaceDescriptor;
USB_ENDP_DESCR stEndpointDescriptor;
USB_HID_DESCR stHidDescriptor;
uint8_t *reportDescriptor;
uint8_t **stringDescriptors;

extern uint16_t sizeEndp1LoggingBuff;
extern const uint16_t capacityEndp1LoggingBuff;
extern uint8_t *endp1LoggingBuff;

extern uint8_t endp0RTbuff[]; // Endpoint 0 data transceiver buffer.
extern uint8_t endp1Rbuff[];  // Endpoint 1 data receiver buffer.
extern uint8_t endp1Tbuff[];  // Endpoint 1 data transmitter buffer.
extern uint8_t endp2Rbuff[];  // Endpoint 2 data receiver buffer.
extern uint8_t endp2Tbuff[];  // Endpoint 2 data transmitter buffer.
extern uint8_t endp3Rbuff[];  // Endpoint 3 data receiver buffer.
extern uint8_t endp3Tbuff[];  // Endpoint 3 data transmitter buffer.
extern uint8_t endp4Rbuff[];  // Endpoint 4 data receiver buffer.
extern uint8_t endp4Tbuff[];  // Endpoint 4 data transmitter buffer.
extern uint8_t endp5Rbuff[];  // Endpoint 5 data receiver buffer.
extern uint8_t endp5Tbuff[];  // Endpoint 5 data transmitter buffer.
extern uint8_t endp6Rbuff[];  // Endpoint 6 data receiver buffer.
extern uint8_t endp6Tbuff[];  // Endpoint 6 data transmitter buffer.
extern uint8_t endp7Rbuff[];  // Endpoint 7 data receiver buffer.
extern uint8_t endp7Tbuff[];  // Endpoint 7 data transmitter buffer.

/* configuration, allows nested code to access above variables */
#include "usb20-config.h"

/* functions declaration */

/*******************************************************************************
 * Function Name  : U20_registers_init
 * Description    : Initialise registers and enable interrupt related to USB 2.0
 * Input          : Desired USB 2.0 speed, see enum Speed
 * Return         : None
 *******************************************************************************/
void U20_registers_init(enum Speed sp);

/*******************************************************************************
 * Function Name  : U20_endpoints_init
 * Description    : Initialise registers and buffers for endpoints, by default
 *                  none of them is enabled (except ep0)
 * Input          : A bitmask of the endpoints to initialise and enable
 * Return         : None
 *******************************************************************************/
void U20_endpoints_init(enum Endpoint endpointsMask);

/*******************************************************************************
 * Function Name  : endpoint_clear
 * Description    : Reset the given endpoint
 * Warning        : It only reset one endpoint, do NOT give multiples !
 * Input          : The endpoint to reset
 * Return         : None
 *******************************************************************************/
void endpoint_clear(uint8_t endpointToClear);

/*******************************************************************************
 * Function Name  : endpoint_halt
 * Description    : Halt the given endpoint
 * Warning        : It only halt one endpoint, do NOT give multiples !
 * Input          : The endpoint to halt
 * Return         : None
 *******************************************************************************/
void endpoint_halt(uint8_t endpointToHalt);

/*******************************************************************************
 * Function Name  : fill_buffer_with_descriptor
 * Description    : Fill the given buffer with the requested descriptor
 * Input          : - descritorRequested is the wValue field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to populate and the
 *                    size we wrote
 * Return         : None
 *******************************************************************************/
void fill_buffer_with_descriptor(UINT16_UINT8 descritorRequested, uint8_t **pBuffer, uint16_t *pSizeBuffer);

/*******************************************************************************
 * Function Name  : ep0_transceive_and_update
 * Description    : Handle the "command" on endpoint 0 (mainly receive/transmit)
 *                  and update the buffer accordingly
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to transceive and
 *                    the associated size
 * Return         : None
 *******************************************************************************/
void ep0_transceive_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer);

/*******************************************************************************
 * Function Name  : ep1_transmit_keyboard
 * Description    : Handle the "command" on endpoint 1 when behaving as a
 *                  keyboard Here it means anwsering to interrupt request with
 *                  the appropriate key sequence
 * Input          : None
 * Return         : None
 *******************************************************************************/
void ep1_transmit_keyboard(void);

/*******************************************************************************
 * Function Name  : ep1_transceive_and_update
 * Description    : Handle the "command" on endpoint 1 (mainly receive/transmit)
 *                  and update the buffer accordingly
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to transceive and
 *                    the associated size
 * Return         : None
 *******************************************************************************/
void ep1_transceive_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer);

/*******************************************************************************
 * Function Name  : ep1_log
 * Description    : Function used to log data to the Host computer over USB
 * Input          : Variadic function, same arguments as you would give to
 *                  printf()
 * Return         : None
 *******************************************************************************/
void ep1_log(const char *fmt, ...);


#endif /* USB20_H */
