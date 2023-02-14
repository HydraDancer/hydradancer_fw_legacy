#ifndef USB20_ENDPOINTS_H
#define USB20_ENDPOINTS_H

#include <stddef.h>
#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"

#include "hspi.h"
#include "usb20.h"


/* variables */
extern uint16_t sizeEndp6LoggingBuff;
extern const uint16_t capacityEndp6LoggingBuff;
extern uint8_t *endp6LoggingBuff;

extern uint16_t sizeEndp7LoggingBuff;
extern const uint16_t capacityEndp7LoggingBuff;
extern uint8_t *endp7LoggingBuff;

extern bool g_doesToeSupportCurrentDevice;

/* functions declaration */

/*******************************************************************************
 * Function Name  : epX_transceive_and_update
 * Description    : Template for endpoint handler
 *                  To use copy and paste it bellow, rename the following
 *                  "variables":
 *                  - R16_UEP0_T_LEN to R16_UEPx_T_LEN where x = desired endpoint
 *                  - R8_UEP0_TX_CTRL to R8_UEPx_TX_CTRL where x = desired endpoint
 *                  - R8_UEP0_RX_CTRL to R8_UEPx_RX_CTRL where x = desired endpoint
 *                  - U20_UEP0_MAXSIZE to a variable corresponding the the maximum
 *                    size for a packet on this endpoint
 *                  You also need to rename this function (replace X with the
 *                  endpoint number desired) and add it to USBHS_IRQHandler in
 *                  IF_TRANSFER (see ep1_transceive_and_update() as an example)
 * Warning        : Handlers for endpoint 0 and 7 are already defined
 *                  - Ep0 is used for initialising USB device
 *                  - ep7 is dedicated to logging
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to transceive and
 *                    the associated size
 *                  The parameters can be changed, but using a temporary buffer
 *                  with a "cursor" can be useful when dealing with a message
 *                  longer than one transaction
 * Return         : None
 *******************************************************************************/
void epX_transceive_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer);

/*******************************************************************************
 * Function Name  : ep1_transceive_and_update_host
 * Description    : Handle the "command" on endpoint 1 (mainly receive/transmit)
 *                  and update the buffer accordingly
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to transceive and
 *                    the associated size
 * Return         : None
 *******************************************************************************/
void ep1_transceive_and_update_host(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer);

/*******************************************************************************
 * Function Name  : epX_handler_toe
 * Description    : Dummy endpoint handler for ToE board
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - the endpoint requested
 * Return         : None
 *******************************************************************************/
void epX_handler_toe(uint8_t uisToken, uint8_t endpoint);

/*******************************************************************************
 * Function Name  : ep6_transmit_and_update
 * Description    : Handle the "command" on endpoint 6 (transmit debug of top
 *                  board) and update the buffer accordingly
 *
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to transceive and
 *                    the associated size
 * Return         : None
 *******************************************************************************/
void ep6_transmit_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer);

/*******************************************************************************
 * Function Name  : ep7_transmit_and_update
 * Description    : Handle the "command" on endpoint 7 (transmit debug of
 *                  bottom board) and update the buffer accordingly
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to transceive and
 *                    the associated size
 * Return         : None
 *******************************************************************************/
void ep7_transmit_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer);


#endif /* USB20_ENDPOINTS_H */
