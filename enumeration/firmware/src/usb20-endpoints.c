#include "usb20-endpoints.h"

#include "log.h"


#include "rot13-example.h"  // Included only for the rot13 example, TODO: remove

/* variables */
uint16_t sizeEndp6LoggingBuff = 0;
const uint16_t capacityEndp6LoggingBuff = 4096;
__attribute__((aligned(16))) uint8_t endp6LoggingBuffRaw[4096] __attribute__((section(".DMADATA")));
uint8_t *endp6LoggingBuff = endp6LoggingBuffRaw;

uint16_t sizeEndp7LoggingBuff = 0;
const uint16_t capacityEndp7LoggingBuff = 4096;
__attribute__((aligned(16))) uint8_t endp7LoggingBuffRaw[4096] __attribute__((section(".DMADATA")));
uint8_t *endp7LoggingBuff = endp7LoggingBuffRaw;

/* To know if a device is supported by the ToE, getting the descriptors queried
 * is not enough
 * We wait for a request on an endpoint, if a request is sent to us after the
 * "authentication" (everything that happen on ep0) on an endpoint, we consider
 * that our current device is supported
 */
bool g_doesToeSupportCurrentDevice = false;


/* functions implementation */

/*******************************************************************************
 * @fn      epX_transceive_and_update
 *
 * @brief   Template for endpoint handler
 *          To use copy and paste it bellow, rename the following "variables":
 *          - R16_UEP0_T_LEN to R16_UEPx_T_LEN where x = desired endpoint
 *          - R8_UEP0_TX_CTRL to R8_UEPx_TX_CTRL where x = desired endpoint
 *          - R8_UEP0_RX_CTRL to R8_UEPx_RX_CTRL where x = desired endpoint
 *          - U20_UEP0_MAXSIZE to a variable corresponding the the maximum size
 *            for a packet on this endpoint
 *          You also need to rename this function (replace X with the endpoint
 *          number desired) and add it to USBHS_IRQHandler in IF_TRANSFER (see
 *          ep1_transceive_and_update() as an example)
 *
 *          The parameters can be changed, but using a temporary buffer with a
 *          "cursor" can be useful when dealing with a message longer than one
 *          transaction
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
/* @fn      ep1_transceive_and_update_host
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
        /* We NAK IN transactions until we receive the return value
         * from bbio_*()
         */
        usb20_endpoint_nak(0x81);

        // Transmit data to second board via HSPI
        memcpy(hspi_get_buffer_next_tx(), endp1Rbuff, min(HSPI_DMA_LEN, U20_UEP1_MAXSIZE));
        HSPI_DMA_Tx();

        R8_UEP1_RX_CTRL ^= RB_UEP_R_TOG_1;
        R8_UEP1_RX_CTRL = (R8_UEP1_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_ACK;
        break;
    case UIS_TOKEN_IN:
        // TODO: Sort this and fix this
        R16_UEP1_T_LEN = 1;
        R8_UEP1_TX_CTRL ^= RB_UEP_T_TOG_1;
        R8_UEP1_TX_CTRL = (R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;

        break;
    default:
        log_to_evaluator("ERROR: ep1_transceive_and_update default!");
        break;
    }
}

/*******************************************************************************
/* @fn      epX_handler_toe
 *
 * @brief   Dummy endpoint handler for ToE board
 *
 * @return  None
 */
void
epX_handler_toe(uint8_t uisToken, uint8_t endpoint)
{
    switch (uisToken) {
        case UIS_TOKEN_OUT:
            switch (endpoint) {
            case 1:
                R16_UEP1_T_LEN = 0;
                R8_UEP1_TX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP1_TX_CTRL = (R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 2:
                R16_UEP2_T_LEN = 0;
                R8_UEP2_TX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP2_TX_CTRL = (R8_UEP2_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 3:
                R16_UEP3_T_LEN = 0;
                R8_UEP3_TX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP3_TX_CTRL = (R8_UEP3_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 4:
                R16_UEP4_T_LEN = 0;
                R8_UEP4_TX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP4_TX_CTRL = (R8_UEP4_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 5:
                R16_UEP5_T_LEN = 0;
                R8_UEP5_TX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP5_TX_CTRL = (R8_UEP5_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 6:
                R16_UEP6_T_LEN = 0;
                R8_UEP6_TX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP6_TX_CTRL = (R8_UEP6_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 7:
                R16_UEP7_T_LEN = 0;
                R8_UEP7_TX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP7_TX_CTRL = (R8_UEP7_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            default:
                log_to_evaluator("ERROR: epX_handler_toe() invalid endpoint (OUT)\r\n");
                return;
            }
        break;
    case UIS_TOKEN_IN:
            switch (endpoint) {
            case 1:
                R8_UEP1_RX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP1_RX_CTRL = (R8_UEP1_RX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 2:
                R8_UEP2_RX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP2_RX_CTRL = (R8_UEP2_RX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 3:
                R8_UEP3_RX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP3_RX_CTRL = (R8_UEP3_RX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 4:
                R8_UEP4_RX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP4_RX_CTRL = (R8_UEP4_RX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 5:
                R8_UEP5_RX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP5_RX_CTRL = (R8_UEP5_RX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 6:
                R8_UEP6_RX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP6_RX_CTRL = (R8_UEP6_RX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            case 7:
                R8_UEP7_RX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP7_RX_CTRL = (R8_UEP7_RX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
                break;
            default:
                log_to_evaluator("ERROR: epX_handler_toe() invalid endpoint (IN)\r\n");
                return;
            }
        break;
    default:
        log_to_evaluator("ERROR: epX_handler_toe() default!\r\n");
        return;
    }
    g_doesToeSupportCurrentDevice = true;
}

/* @fn      ep6_transmit_and_update
 *
 * @brief   Handle the "command" on endpoint 6 (transmit debug of bottom board)
 *          and update the buffer accordingly
 *
 * @return  None
 */
void
ep6_transmit_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    static uint8_t *bufferResetValue = NULL;
    if (bufferResetValue == NULL) {
        bufferResetValue = *pBuffer;
    }

    switch (uisToken) {
    case UIS_TOKEN_IN:
        if (*pSizeBuffer != 0x0000) {
            uint16_t sizeCurrentTransaction = min(*pSizeBuffer, U20_UEP6_MAXSIZE);
            memcpy(endp6Tbuff, *pBuffer, sizeCurrentTransaction);

            R16_UEP6_T_LEN = sizeCurrentTransaction;
            R8_UEP6_TX_CTRL ^= RB_UEP_T_TOG_1;
            R8_UEP6_TX_CTRL = (R8_UEP6_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;

            *pSizeBuffer -= sizeCurrentTransaction;
            *(uint32_t *)pBuffer += U20_UEP6_MAXSIZE; /* Careful! We increase from the PREVIOUSLY read value */
        } else {
            *pBuffer = bufferResetValue;

            R16_UEP6_T_LEN = 0;
            R8_UEP6_TX_CTRL ^= RB_UEP_T_TOG_1;
            R8_UEP6_TX_CTRL = (R8_UEP6_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
        }
        break;
        default:
            log_to_evaluator("ERROR: ep6_transmit_and_update default!");
            break;
    }
}

/* @fn      ep7_transmit_and_update
 *
 * @brief   Handle the "command" on endpoint 7 (transmit debug of bottom board)
 *          and update the buffer accordingly
 *
 * @return  None
 */
void
ep7_transmit_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    static uint8_t *bufferResetValue = NULL;
    if (bufferResetValue == NULL) {
        bufferResetValue = *pBuffer;
    }

    switch (uisToken) {
    case UIS_TOKEN_IN:
        if (*pSizeBuffer != 0x0000) {
            uint16_t sizeCurrentTransaction = min(*pSizeBuffer, U20_UEP7_MAXSIZE);
            memcpy(endp7Tbuff, *pBuffer, sizeCurrentTransaction);

            R16_UEP7_T_LEN = sizeCurrentTransaction;
            R8_UEP7_TX_CTRL ^= RB_UEP_T_TOG_1;
            R8_UEP7_TX_CTRL = (R8_UEP7_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;

            *pSizeBuffer -= sizeCurrentTransaction;
            *(uint32_t *)pBuffer += U20_UEP7_MAXSIZE; /* Careful! We increase from the PREVIOUSLY read value */
        } else {
            *pBuffer = bufferResetValue;

            R16_UEP7_T_LEN = 0;
            R8_UEP7_TX_CTRL ^= RB_UEP_T_TOG_1;
            R8_UEP7_TX_CTRL = (R8_UEP7_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
        }
        break;
        default:
            log_to_evaluator("ERROR: ep7_transmit_and_update default!");
            break;
    }
}

