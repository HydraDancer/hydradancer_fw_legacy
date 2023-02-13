#ifndef USB20_ENDPOINTS_H
#define USB20_ENDPOINTS_H

#include <stddef.h>
#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"

#include "usb20.h"

#include "rot13-example.h" // Included only for the example, used in ep1_transceive_and_update()

/* functions declaration */

/*******************************************************************************
 * Function Name  : epX_transceive_and_update
 * Description    : Template for endpoint handler.
 *                  To use copy and paste it bellow, rename the following
 *                  "variables":
 *                  - R16_UEP0_T_LEN to R16_UEPx_T_LEN where x = desired endpoint
 *                  - R8_UEP0_TX_CTRL to R8_UEPx_TX_CTRL where x = desired endpoint
 *                  - R8_UEP0_RX_CTRL to R8_UEPx_RX_CTRL where x = desired endpoint
 *                  - U20_UEP0_MAXSIZE to a variable corresponding the the maximum
 *                    size for a packet on this endpoint
 *                  You also need to rename this function (replace X with the
 *                  endpoint number desired) and add it to USBHS_IRQHandler in
 *                  IF_TRANSFER (see ep1_transceive_and_update() as an example).
 * Warning        : Handlers for endpoint 0 and 7 are already defined
 *                  - Ep0 is used for initialising USB device
 *                  - ep7 is dedicated to logging
 * Input          : - uisToken is the bmRequestType field of the Setup Packet
 *                  - pBuffer and pSizeBuffer are the buffer to transceive and
 *                    the associated size
 *                  The parameters can be changed, but using a temporary buffer
 *                  with a "cursor" can be useful when dealing with a message
 *                  longer than one transaction.
 * Return         : None
 *******************************************************************************/
void epX_transceive_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer);

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
 * Function Name  : ep1_transmit_keyboard
 * Description    : NOT USED, only given as a reference
 *                  Handle the "command" on endpoint 1 when behaving as a
 *                  keyboard, here it means anwsering to interrupt request with
 *                  the appropriate key sequence
 * Input          : None
 * Return         : None
 *******************************************************************************/
void ep1_transmit_keyboard(void);


#endif /* USB20_ENDPOINTS_H */
