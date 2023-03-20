#ifndef USB20_H
#define USB20_H

#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"
#include "CH56x_usb30_devbulk_LIB.h"

#include "usb20-endpoints.h"

/* macros */
#define U20_MAXPACKET_LEN (512)                                                                     
#define U20_UEP0_MAXSIZE  (64)  // Change accordingly to USB mode (Here HS)
#define U20_UEP1_MAXSIZE  (512) // Change accordingly to USB mode (Here HS)
#define U20_UEP6_MAXSIZE  (512) // Change accordingly to USB mode (Here HS)
#define U20_UEP7_MAXSIZE  (512) // Change accordingly to USB mode (Here HS)
#define UsbSetupBuf       ((PUSB_SETUP)endp0RTbuff)                                                 

/* enums */
enum Speed { SpeedLow = UCST_LS, SpeedFull = UCST_FS, SpeedHigh = UCST_HS };
enum Endpoint {
    Ep0Mask = 1 << 0,
    Ep1Mask = 1 << 1,
    Ep2Mask = 1 << 2,
    Ep3Mask = 1 << 3,
    Ep4Mask = 1 << 4,
    Ep5Mask = 1 << 5,
    Ep6Mask = 1 << 6,
    Ep7Mask = 1 << 7,
};


/* variables */
enum Speed g_usb20Speed;
enum Endpoint g_usb20EpInMask;
enum Endpoint g_usb20EpOutMask;

// If this variable is != 0 then use this size rather than .wTotalLength
extern uint16_t g_descriptorConfigCustomSize;
extern uint8_t *g_descriptorDevice;
extern uint8_t *g_descriptorConfig;
extern uint8_t *g_descriptorHidReport;
extern uint8_t *g_descriptorHubReport;
extern uint8_t **g_descriptorStrings;

extern uint8_t endp0RTbuff[]; // Endpoint 0 data transceiver buffer
extern uint8_t endp1Rbuff[];  // Endpoint 1 data receiver buffer
extern uint8_t endp1Tbuff[];  // Endpoint 1 data transmitter buffer
extern uint8_t endp2Rbuff[];  // Endpoint 2 data receiver buffer
extern uint8_t endp2Tbuff[];  // Endpoint 2 data transmitter buffer
extern uint8_t endp3Rbuff[];  // Endpoint 3 data receiver buffer
extern uint8_t endp3Tbuff[];  // Endpoint 3 data transmitter buffer
extern uint8_t endp4Rbuff[];  // Endpoint 4 data receiver buffer
extern uint8_t endp4Tbuff[];  // Endpoint 4 data transmitter buffer
extern uint8_t endp5Rbuff[];  // Endpoint 5 data receiver buffer
extern uint8_t endp5Tbuff[];  // Endpoint 5 data transmitter buffer
extern uint8_t endp6Rbuff[];  // Endpoint 6 data receiver buffer
extern uint8_t endp6Tbuff[];  // Endpoint 6 data transmitter buffer
extern uint8_t endp7Rbuff[];  // Endpoint 7 data receiver buffer
extern uint8_t endp7Tbuff[];  // Endpoint 7 data transmitter buffer

/* configuration, allows nested code to access above variables */
#include "usb20-config.h"

/* functions declaration */

/*******************************************************************************
 * Function Name  : usb20_registers_init
 * Description    : Initialize registers and enable interrupt related to USB 2.0
 * Input          : Desired USB 2.0 speed, see enum Speed
 * Return         : None
 *******************************************************************************/
void usb20_registers_init(enum Speed sp);

/*******************************************************************************
 * Function Name  : usb20_registers_init
 * Description    : Deinitialize registers and related to USB 2.0 (act like a
 *                  physical disconnect)
 * Input          : Desired USB 2.0 speed, see enum Speed
 * Return         : None
 *******************************************************************************/
void usb20_registers_deinit(void);

/*******************************************************************************
 * Function Name  : usb20_endpoints_init
 * Description    : Initialise registers and buffers for endpoints, by default
 *                  none of them is enabled (except ep0)
 * Input          : A bitmask of the endpoints to initialise and enable
 * Return         : None
 *******************************************************************************/
void usb20_endpoints_init(enum Endpoint endpointsInMask, enum Endpoint endpointsOutMask);

/*******************************************************************************
 * Function Name  : usb20_endpoint_ack
 * Description    : Set response to ACK for the given endpoint
 * Warning        : It only ACK one endpoint, do NOT give multiples !
 * Input          : The endpoint to reset
 * Return         : None
 *******************************************************************************/
void usb20_endpoint_ack(uint8_t endpointToACK);


/*******************************************************************************
 * Function Name  : usb20_endpoint_nak
 * Description    : Set response to NAK for the given endpoint
 * Warning        : It only NAK one endpoint, do NOT give multiples !
 * Input          : The endpoint to reset
 * Return         : None
 *******************************************************************************/
void usb20_endpoint_nak(uint8_t endpointToNAK);

/*******************************************************************************
 * Function Name  : usb20_endpoint_halt
 * Description    : Halt the given endpoint
 * Warning        : It only halt one endpoint, do NOT give multiples !
 * Input          : The endpoint to halt
 * Return         : None
 *******************************************************************************/
void usb20_endpoint_halt(uint8_t endpointToHalt);

/*******************************************************************************
 * Function Name  : usb20_fill_buffer_with_descriptor
 * Description    : Fill the given buffer with the requested descriptor
 * Input          : - descritorRequested is the wValue field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to populate and the
 *                    size we wrote
 * Return         : None
 *******************************************************************************/
void usb20_fill_buffer_with_descriptor(UINT16_UINT8 descritorRequested, uint8_t **pBuffer, uint16_t *pSizeBuffer);

/*******************************************************************************
 * Function Name  : usb20_ep0_transceive_and_update
 * Description    : Handle the "command" on endpoint 0 (mainly receive/transmit)
 *                  and update the buffer accordingly
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to transceive and
 *                    the associated size
 * Return         : None
 *******************************************************************************/
void usb20_ep0_transceive_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer);

/*******************************************************************************
 * Function Name  : usb20_log
 * Description    : Function used to log data to the Host computer over USB
 * Input          : Variadic function, same arguments as you would give to
 *                  printf()
 *                  endp is an enum corresponding to the endpoint to log to
 * Return         : None
 *******************************************************************************/
void usb20_log(enum Endpoint endp, const char *fmt, ...);

/*******************************************************************************
 * Function Name  : usb20_vlog
 * Description    : Function used to log data to the Host computer over USB
 * Input          : A format string and the associated va_list
 *                  endp is an enum corresponding to the endpoint to log to
 * Return         : None
 *******************************************************************************/
void usb20_vlog(enum Endpoint endp, const char *fmt, va_list ap);



#endif /* USB20_H */
