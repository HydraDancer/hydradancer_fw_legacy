#include "usb20-endpoints.h"

#include "log.h"

/* functions implementation */

/*******************************************************************************
 * @fn      epX_transceive_and_update
 *
 * @brief   Template for endpoint handler.
 *          To use copy and paste it bellow, rename the following "variables":
 *          - R16_UEP0_T_LEN to R16_UEPx_T_LEN where x = desired endpoint
 *          - R8_UEP0_TX_CTRL to R8_UEPx_TX_CTRL where x = desired endpoint
 *          - R8_UEP0_RX_CTRL to R8_UEPx_RX_CTRL where x = desired endpoint
 *          - U20_UEP0_MAXSIZE to a variable corresponding the the maximum size
 *            for a packet on this endpoint
 *          You also need to rename this function (replace X with the endpoint
 *          number desired) and add it to USBHS_IRQHandler in IF_TRANSFER (see
 *          ep1_transceive_and_update() as an example).
 *
 *          The parameters can be changed, but using a temporary buffer with a
 *          "cursor" can be useful when dealing with a message longer than one
 *          transaction.
 *
 * @warning Handlers for endpoint 0 and 7 are already defined
 *          - Ep0 is used for initialising USB device
 *          - ep7 is dedicated to logging
 *
 * @input   - uisToken is the bmRequestType field of the Setup Packet
 *          - pBuffer and pSizeBuffer are the buffer to transceive and the
 *            associated size

 * @return  None
 */
void
epX_transceive_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    static uint8_t *bufferResetValue = NULL;
    if (bufferResetValue == NULL) {
        bufferResetValue = *pBuffer;
    }

    switch (uisToken) {
    case UIS_TOKEN_OUT:
        // TODOOO: Handle transfer where there is more than one transaction.
        R16_UEP0_T_LEN = 0;
        R8_UEP0_TX_CTRL ^= RB_UEP_T_TOG_1;
        R8_UEP0_TX_CTRL = (R8_UEP0_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
        R8_UEP0_RX_CTRL ^= RB_UEP_R_TOG_1;
        R8_UEP0_RX_CTRL = (R8_UEP0_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_ACK;
        break;
    case UIS_TOKEN_IN:
        if (*pSizeBuffer) {
            uint16_t sizeCurrentTransaction = min(*pSizeBuffer, U20_UEP0_MAXSIZE);
            memcpy(endp1Tbuff, *pBuffer, sizeCurrentTransaction);

            R16_UEP0_T_LEN = sizeCurrentTransaction;
            R8_UEP0_TX_CTRL ^= RB_UEP_T_TOG_1;
            R8_UEP0_TX_CTRL = (R8_UEP0_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;

            *pSizeBuffer -= sizeCurrentTransaction;
            *(uint32_t *)pBuffer += U20_UEP0_MAXSIZE; /* Careful! We increase from the PREVIOUSLY read value */
        } else {
            *pBuffer = bufferResetValue;

            g_top_readyToTransmitUsbPacket = false; // Part of the rot13 example

            R16_UEP0_T_LEN = 0;
            R8_UEP0_TX_CTRL ^= RB_UEP_T_TOG_1;
            R8_UEP0_TX_CTRL = (R8_UEP0_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
        }
        break;
    case UIS_TOKEN_SOF:
        break;
    case UIS_TOKEN_SETUP:
        /* Only used with ep0, should not trigger here */
        break;
        default:
            log_to_evaluator("ERROR: epX_transceive_and_update default!");
            break;
    }
}


/*******************************************************************************
/* @fn      ep1_transmit_keyboard
 *
 * @brief   Handle the "command" on endpoint 1 when behaving as a keyboard
 *          Here it means anwsering to interrupt request with the appropriate
 *          key sequence
 *          Not used, only given as a reference
 *
 * @return  None
 */
void
ep1_transmit_keyboard(void)
{
    /* Link to USB HID Keyboard scan codes :
     * https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2 */
    static uint8_t keyboard_payload[] = { 0x17, 0x08, 0x16, 0x17, 0x28 };
    static uint8_t i = 0;
    uint8_t modulus = 4;

    /* Keyboard input crafting. */
    uint8_t output[] = { 0x00, 0x00, 0x00, 0x00 };
    if (i%modulus == 0) {
        output[0] = keyboard_payload[i/modulus];
    }
    i++;
    if (i == (modulus*sizeof(keyboard_payload))-1) {
        i = 0;
    }

    memcpy(endp1Tbuff, output, sizeof(output));
    R16_UEP1_T_LEN = sizeof(output);
    R8_UEP1_TX_CTRL ^= RB_UEP_T_TOG_1;
    R8_UEP1_TX_CTRL = (R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
}


/*******************************************************************************
/* @fn      ep1_transceive_and_update
 *
 * @brief   Handle the "command" on endpoint 1 (mainly receive/transmit) and 
 *          update the buffer accordingly
 *
 * @return  None
 */
void
ep1_transceive_and_update_host(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    static uint8_t *bufferResetValue = NULL;
    if (bufferResetValue == NULL) {
        bufferResetValue = *pBuffer;
    }

    switch (uisToken) {
    case UIS_TOKEN_OUT:
        log_to_evaluator("Received USB request (OUT)\r\n");
        // Transmit data to second board via HSPI
        memcpy(hspi_get_buffer_next_tx(), endp1Rbuff, min(HSPI_DMA_LEN, U20_UEP1_MAXSIZE));
        HSPI_DMA_Tx();

        R16_UEP1_T_LEN = 0;
        R8_UEP1_TX_CTRL ^= RB_UEP_T_TOG_1;
        R8_UEP1_TX_CTRL = (R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
        R8_UEP1_RX_CTRL ^= RB_UEP_R_TOG_1;
        R8_UEP1_RX_CTRL = (R8_UEP1_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_ACK;
        break;
    case UIS_TOKEN_IN:
        // Not used, only transmitting from host to device board
        break;
    default:
        log_to_evaluator("ERROR: ep1_transceive_and_update default!");
        break;
    }
}


/*******************************************************************************
/* @fn      ep1_transceive_and_update
 *
 * @brief   Handle the "command" on endpoint 1 (mainly receive/transmit) and 
 *          update the buffer accordingly
 *
 * @return  None
 */
void
ep1_transceive_and_update_target(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    static uint8_t *bufferResetValue = NULL;
    if (bufferResetValue == NULL) {
        bufferResetValue = *pBuffer;
    }

    switch (uisToken) {
    case UIS_TOKEN_OUT:
        // Not implemented yet

        R16_UEP1_T_LEN = 0;
        R8_UEP1_TX_CTRL ^= RB_UEP_T_TOG_1;
        R8_UEP1_TX_CTRL = (R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
        R8_UEP1_RX_CTRL ^= RB_UEP_R_TOG_1;
        R8_UEP1_RX_CTRL = (R8_UEP1_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_ACK;
        break;
    case UIS_TOKEN_IN:
        // Not implemented yet
        break;
    default:
        log_to_evaluator("ERROR: ep1_transceive_and_update default!");
        break;
    }
}

