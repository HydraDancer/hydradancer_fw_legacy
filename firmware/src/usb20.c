#include <stdarg.h>
#include <string.h>

#include "log.h"

#include "usb20.h"


/* variables */
//
// If this variable is != 0 then use this size rather than .wTotalLength
uint16_t g_descriptorConfigCustomSize = 0;

uint8_t *g_descriptorDevice = NULL;
uint8_t *g_descriptorConfig = NULL;
uint8_t **g_descriptorStrings = NULL;

__attribute__((aligned(16))) uint8_t endp0RTbuff[512] __attribute__((section(".DMADATA"))); // Endpoint 0 data transceiver buffer.
__attribute__((aligned(16))) uint8_t endp1Rbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 1 data recceiver buffer.
__attribute__((aligned(16))) uint8_t endp1Tbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 1 data transmitter buffer.
__attribute__((aligned(16))) uint8_t endp2Rbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 2 data recceiver buffer.
__attribute__((aligned(16))) uint8_t endp2Tbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 2 data transmitter buffer.
__attribute__((aligned(16))) uint8_t endp3Rbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 3 data recceiver buffer.
__attribute__((aligned(16))) uint8_t endp3Tbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 3 data transmitter buffer.
__attribute__((aligned(16))) uint8_t endp4Rbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 4 data recceiver buffer.
__attribute__((aligned(16))) uint8_t endp4Tbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 4 data transmitter buffer.
__attribute__((aligned(16))) uint8_t endp5Rbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 5 data recceiver buffer.
__attribute__((aligned(16))) uint8_t endp5Tbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 5 data transmitter buffer.
__attribute__((aligned(16))) uint8_t endp6Rbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 6 data recceiver buffer.
__attribute__((aligned(16))) uint8_t endp6Tbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 6 data transmitter buffer.
__attribute__((aligned(16))) uint8_t endp7Rbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 7 data recceiver buffer.
__attribute__((aligned(16))) uint8_t endp7Tbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 7 data transmitter buffer.


/* functions implementation */

/* @fn      array_addr_len
 *
 * @brief   Get the length of an array of pointers
 *          Only used internally
 *
 * @return  Return the length of an array of pointers
 */
static uint8_t
_array_addr_len(void **array)
{
    for (uint8_t i = 0;; ++i) {
        if (array[i] == NULL) {
            return i;
        }
    }
}

/* @fn      usb20_registers_init
 *
 * @brief   Initialise registers and enable interrupt related to USB 2.0
 *
 * @return  None
 */
void
usb20_registers_init(enum Speed sp)
{
    R32_USB_CONTROL = 0;
    PFIC_EnableIRQ(USBHS_IRQn);
    PFIC_EnableIRQ(LINK_IRQn);

    /* Init as device according to the doc (p. 105) */
    R8_USB_CTRL = 0;
    R8_USB_DEV_AD = 0;
    R8_USB_INT_FG = 0;

    R8_USB_CTRL |= (RB_USB_INT_BUSY | RB_USB_DMA_EN);
    R8_USB_CTRL |= RB_DEV_PU_EN;
    R8_USB_CTRL |= sp;

    R8_USB_INT_EN = RB_USB_IE_ISOACT | RB_USB_IE_SETUPACT | RB_USB_IE_FIFOOV
                    | RB_USB_IE_SUSPEND | RB_USB_IE_TRANS | RB_USB_IE_BUSRST;
}

/* @fn      usb20_endpoints_init
 *
 * @brief   Initialise registers and buffers for endpoints, by default none of
 *          them is enabled (except ep0)
 *
 * @return  None
 */
void
usb20_endpoints_init(enum Endpoint endpointsInMask, enum Endpoint endpointsOutMask)
{
    R8_UEP4_1_MOD = 0;
    R8_UEP2_3_MOD = 0;
    R8_UEP5_6_MOD = 0;
    R8_UEP7_MOD   = 0;

    R16_UEP0_MAX_LEN = U20_UEP0_MAXSIZE;
    R16_UEP1_MAX_LEN = U20_MAXPACKET_LEN;
    R16_UEP2_MAX_LEN = U20_MAXPACKET_LEN;
    R16_UEP3_MAX_LEN = U20_MAXPACKET_LEN;
    R16_UEP4_MAX_LEN = U20_MAXPACKET_LEN;
    R16_UEP5_MAX_LEN = U20_MAXPACKET_LEN;
    R16_UEP6_MAX_LEN = U20_MAXPACKET_LEN;
    R16_UEP7_MAX_LEN = U20_MAXPACKET_LEN;

    R32_UEP0_RT_DMA = (uint32_t)(uint8_t *)endp0RTbuff;
    R32_UEP1_TX_DMA = (uint32_t)(uint8_t *)endp1Tbuff;
    R32_UEP1_RX_DMA = (uint32_t)(uint8_t *)endp1Rbuff;
    R32_UEP2_TX_DMA = (uint32_t)(uint8_t *)endp2Tbuff;
    R32_UEP2_RX_DMA = (uint32_t)(uint8_t *)endp2Rbuff;
    R32_UEP3_TX_DMA = (uint32_t)(uint8_t *)endp3Tbuff;
    R32_UEP3_RX_DMA = (uint32_t)(uint8_t *)endp3Rbuff;
    R32_UEP4_TX_DMA = (uint32_t)(uint8_t *)endp4Tbuff;
    R32_UEP4_RX_DMA = (uint32_t)(uint8_t *)endp4Rbuff;
    R32_UEP5_TX_DMA = (uint32_t)(uint8_t *)endp5Tbuff;
    R32_UEP5_RX_DMA = (uint32_t)(uint8_t *)endp5Rbuff;
    R32_UEP6_TX_DMA = (uint32_t)(uint8_t *)endp6Tbuff;
    R32_UEP6_RX_DMA = (uint32_t)(uint8_t *)endp6Rbuff;
    R32_UEP7_TX_DMA = (uint32_t)(uint8_t *)endp7Tbuff;
    R32_UEP7_RX_DMA = (uint32_t)(uint8_t *)endp7Rbuff;

    R16_UEP0_T_LEN = 0;
    R8_UEP0_TX_CTRL = 0;
    R8_UEP0_RX_CTRL = 0;

    /* Endpoints IN */
    if (endpointsInMask & Ep1Mask) {
        R8_UEP4_1_MOD |= RB_UEP1_TX_EN;
        R8_UEP1_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
    } else {
        R8_UEP1_TX_CTRL = UEP_T_RES_NAK;
    }

    if (endpointsInMask & Ep2Mask) {
        R8_UEP2_3_MOD |= RB_UEP2_TX_EN;
        R8_UEP2_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
    } else {
        R8_UEP2_TX_CTRL = UEP_T_RES_NAK;
    }

    if (endpointsInMask & Ep3Mask) {
        R8_UEP2_3_MOD |= RB_UEP3_TX_EN;
        R8_UEP3_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
    } else {
        R8_UEP3_TX_CTRL = UEP_T_RES_NAK;
    }

    if (endpointsInMask & Ep4Mask) {
        R8_UEP4_1_MOD |= RB_UEP4_TX_EN;
        R8_UEP4_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
    } else {
        R8_UEP4_TX_CTRL = UEP_T_RES_NAK;
    }

    if (endpointsInMask & Ep5Mask) {
        R8_UEP5_6_MOD |= RB_UEP5_TX_EN;
        R8_UEP5_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
    } else {
        R8_UEP5_TX_CTRL = UEP_T_RES_NAK;
    }

    if (endpointsInMask & Ep6Mask) {
        R8_UEP5_6_MOD |= RB_UEP6_TX_EN;
        R8_UEP6_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
    } else {
        R8_UEP6_TX_CTRL = UEP_T_RES_NAK;
    }

    if (endpointsInMask & Ep7Mask) {
        R8_UEP7_MOD |= RB_UEP7_TX_EN;
        R8_UEP7_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
    } else {
        R8_UEP7_TX_CTRL = UEP_T_RES_NAK;
    }

    /* Endpoints OUT */
    if (endpointsOutMask & Ep1Mask) {
        R8_UEP4_1_MOD |= RB_UEP1_RX_EN;
        R8_UEP1_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP1_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsOutMask & Ep2Mask) {
        R8_UEP2_3_MOD |= RB_UEP2_TX_EN;
        R8_UEP2_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP2_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsOutMask & Ep3Mask) {
        R8_UEP2_3_MOD |= RB_UEP3_TX_EN;
        R8_UEP3_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP3_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsOutMask & Ep4Mask) {
        R8_UEP4_1_MOD |= RB_UEP4_TX_EN;
        R8_UEP4_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP4_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsOutMask & Ep5Mask) {
        R8_UEP5_6_MOD |= RB_UEP5_TX_EN;
        R8_UEP5_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP5_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsOutMask & Ep6Mask) {
        R8_UEP5_6_MOD |= RB_UEP6_TX_EN;
        R8_UEP6_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP6_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsOutMask & Ep7Mask) {
        R8_UEP7_MOD |= RB_UEP7_TX_EN;
        R8_UEP7_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP7_RX_CTRL = UEP_R_RES_NAK;
    }
}

/* @fn      usb20_endpoint_clear
 *
 * @brief   Reset the given endpoint
 *
 * @warning DEPRECATED !
 *
 * @warning It only reset one endpoint, do NOT give multiples !
 * 
 * @return  None
 */
void
usb20_endpoint_clear(uint8_t endpointToClear)
{
    switch (endpointToClear) {
    case 0x81: /* endpoint 1 IN */
        R16_UEP1_T_LEN = 0;
        R8_UEP1_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x01: /* endpoint 1 OUT */
        R8_UEP1_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x82: /* endpoint 2 IN */
        R16_UEP2_T_LEN = 0;
        R8_UEP2_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x02: /* endpoint 2 OUT */
        R8_UEP2_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x83: /* endpoint 3 IN */
        R16_UEP3_T_LEN = 0;
        R8_UEP3_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x03: /* endpoint 3 OUT */
        R8_UEP3_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x84: /* endpoint 4 IN */
        R16_UEP4_T_LEN = 0;
        R8_UEP4_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x04: /* endpoint 4 OUT */
        R8_UEP4_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x85: /* endpoint 5 IN */
        R16_UEP5_T_LEN = 0;
        R8_UEP5_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x05: /* endpoint 5 OUT */
        R8_UEP5_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x86: /* endpoint 6 IN */
        R16_UEP6_T_LEN = 0;
        R8_UEP6_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x06: /* endpoint 6 OUT */
        R8_UEP6_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x87: /* endpoint 7 IN */
        R16_UEP7_T_LEN = 0;
        R8_UEP7_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x07: /* endpoint 7 OUT */
        R8_UEP7_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    default:
        log_to_evaluator("ERROR: endpoint_clear() invalid argument");
        break;
    }
}

/* @fn      usb20_endpoint_halt
 *
 * @brief   Halt the given endpoint
 *
 * @warning It only halt one endpoint at a time, do NOT give multiples !
 * 
 * @return  None
 */
void
usb20_endpoint_halt(uint8_t endpointToHalt)
{
    switch(endpointToHalt) {
    case 0x81: /* Set endpoint 1 IN STALL */
        R8_UEP1_TX_CTRL = (R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x01: /* Set endpoint 1 OUT STALL */
        R8_UEP1_RX_CTRL = (R8_UEP1_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x82: /* Set endpoint 2 IN STALL */
        R8_UEP2_TX_CTRL = (R8_UEP2_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x02: /* Set endpoint 2 OUT STALL */
        R8_UEP2_RX_CTRL = (R8_UEP2_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x83: /* Set endpoint 3 IN STALL */
        R8_UEP3_TX_CTRL = (R8_UEP3_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x03: /* Set endpoint 3 OUT STALL */
        R8_UEP3_RX_CTRL = (R8_UEP3_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x84: /* Set endpoint 4 IN STALL */
        R8_UEP4_TX_CTRL = (R8_UEP4_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x04: /* Set endpoint 4 OUT STALL */
        R8_UEP4_RX_CTRL = (R8_UEP4_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x85: /* Set endpoint 5 IN STALL */
        R8_UEP5_TX_CTRL = (R8_UEP5_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x05: /* Set endpoint 5 OUT STALL */
        R8_UEP5_RX_CTRL = (R8_UEP5_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x86: /* Set endpoint 6 IN STALL */
        R8_UEP6_TX_CTRL = (R8_UEP6_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x06: /* Set endpoint 6 OUT STALL */
        R8_UEP6_RX_CTRL = (R8_UEP6_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x87: /* Set endpoint 7 IN STALL */
        R8_UEP7_TX_CTRL = (R8_UEP7_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x07: /* Set endpoint 7 OUT STALL */
        R8_UEP7_RX_CTRL = (R8_UEP7_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    default:
        log_to_evaluator("ERROR: endpoint_halt() invalid argument");
        break;
    }
}

/* @fn      usb20_fill_buffer_with_descriptor
 *
 * @brief   Fill the given buffer with the requested descriptor
 *
 * @return  None
 */
void
usb20_fill_buffer_with_descriptor(UINT16_UINT8 descritorRequested, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    switch(descritorRequested.bw.bb0) {
    case USB_DESCR_TYP_DEVICE:
        *pBuffer = g_descriptorDevice;
        *pSizeBuffer = ((USB_DEV_DESCR *)g_descriptorDevice)->bLength;
        break;
    case USB_DESCR_TYP_CONFIG:
        /* The .cfgDescr field is always the first, no matter the union's type
         */
        *pBuffer = g_descriptorConfig;
        // If the descriptor type is custom we can not trust its .wTotalLength
        // field
            if (g_descriptorConfigCustomSize != 0) {
                *pSizeBuffer = g_descriptorConfigCustomSize;
            } else {
                *pSizeBuffer = ((USB_CFG_DESCR *)g_descriptorConfig)->wTotalLength;
            }
        break;
    case USB_DESCR_TYP_STRING: {
        uint8_t i = descritorRequested.bw.bb1;
        if (i >= 0 && i < _array_addr_len((void **)g_descriptorStrings)) {
            *pBuffer = g_descriptorStrings[i];
            *pSizeBuffer = g_descriptorStrings[i][0];
        }
    }
    break;
    case USB_DESCR_TYP_INTERF:
        /* Not supported for now */
        // *pBuffer = (uint8_t *)&stInterfaceDescriptor;
        // *pSizeBuffer = stInterfaceDescriptor.bLength;
        break;
    case USB_DESCR_TYP_ENDP:
        /* Not supported for now */
        // *pBuffer = (uint8_t *)&stEndpointDescriptor;
        // *pSizeBuffer = stEndpointDescriptor.bLength;
        break;
    case USB_DESCR_TYP_HID:
        /* Not supported yet, it should already be sent with the configuration
         * descriptor */
        // *pBuffer = (uint8_t *)&stHidDescriptor;
        // *pSizeBuffer = stHidDescriptor.bLength;
        break;
    case USB_DESCR_TYP_REPORT:
        // TODOOO: Implement
        // *pBuffer = (uint8_t *)reportDescriptor;
        // *pSizeBuffer = stHidDescriptor.wDescriptorLengthL;
        break;
    default:
        log_to_evaluator("ERROR: fill_buffer_with_descriptor() invalid descriptor requested");
        break;
    }
}

/* @fn      usb20_ep0_transceive_and_update
 *
 * @brief   Handle the "command" on endpoint 0 (mainly receive/transmit) and 
 *          update the buffer accordingly
 *
 * @return  None
 */
void
usb20_ep0_transceive_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    uint16_t bytesToWriteForCurrentTransaction = 0;

    switch (uisToken) {
    case UIS_TOKEN_OUT:
        /* Not implemented */
        break;
    case UIS_TOKEN_SOF:
        /* Not implemented */
        break;
    case UIS_TOKEN_IN:
        bytesToWriteForCurrentTransaction = *pSizeBuffer;
        if (bytesToWriteForCurrentTransaction >= U20_UEP0_MAXSIZE) {
            bytesToWriteForCurrentTransaction = U20_UEP0_MAXSIZE;
        }

        if (*pBuffer && bytesToWriteForCurrentTransaction > 0) {
            memcpy(endp0RTbuff, *pBuffer, bytesToWriteForCurrentTransaction);
        }
        break;
    case UIS_TOKEN_SETUP:
        /* Not implemented */
        break;
    default:
        log_to_evaluator("ERROR: ep0_transceive_and_update() invalid uisToken");
        break;
    }

    *pSizeBuffer -= bytesToWriteForCurrentTransaction;

    if (bytesToWriteForCurrentTransaction == 0) {    /* If it was the last transaction */
        *pBuffer = NULL;

        R16_UEP0_T_LEN = 0;
        R8_UEP0_TX_CTRL ^= RB_UEP_T_TOG_1;
        R8_UEP0_TX_CTRL = (R8_UEP0_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
        R8_UEP0_RX_CTRL ^= RB_UEP_R_TOG_1;
        R8_UEP0_RX_CTRL = (R8_UEP0_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_ACK;
    } else {
        *pBuffer += bytesToWriteForCurrentTransaction;

        R16_UEP0_T_LEN = bytesToWriteForCurrentTransaction;
        R8_UEP0_TX_CTRL ^= RB_UEP_T_TOG_1;
        R8_UEP0_TX_CTRL = (R8_UEP0_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
    }
}

/* @fn      usb20_log
 *
 * @brief   Function used to log data to the Host computer over USB
 *
 * @return  None
 */
void
usb20_log(enum Endpoint endp, const char *fmt, ...)
{
    va_list ap;

    uint8_t *loggingbuff = NULL;
    uint16_t *pSizeLoggingBuff = NULL;
    uint16_t capLoggingBuff = 0;

    uint16_t sizeLeft;
    int bytesWritten;

    switch (endp) {
    case Ep6Mask:
        loggingbuff = endp6LoggingBuff;
        pSizeLoggingBuff = &sizeEndp6LoggingBuff;
        capLoggingBuff = capacityEndp6LoggingBuff;
        break;
    case Ep7Mask:
        loggingbuff = endp7LoggingBuff;
        pSizeLoggingBuff = &sizeEndp7LoggingBuff;
        capLoggingBuff = capacityEndp7LoggingBuff;
        break;
    default:
        log_to_evaluator("ERROR: usb20_log() cannot log on given endpoint (%x)\r\n", endp);
        return;
    }

    // Critical section, if we print something (outside of an interrrupt) and an
    // interrupt is called and do a print, then the first print is partially
    // overwritten
    bsp_disable_interrupt();
    va_start(ap, fmt);
    sizeLeft = capLoggingBuff - *pSizeLoggingBuff;
    
    if (*pSizeLoggingBuff >= capLoggingBuff) {
        log_to_evaluator("ERROR: usb20_log() buffer already filled!");
        sizeLeft = 0;
    }
    
    bytesWritten = vsnprintf(loggingbuff + *pSizeLoggingBuff, sizeLeft, fmt, ap);
    *pSizeLoggingBuff += bytesWritten;
    bsp_enable_interrupt();
}
/* @fn      usb20_vlog
 *
 * @brief   Function used to log data to the Host computer over USB, takes a
 *          va_list as second argument
 *
 * @return  None
 */
void
usb20_vlog(enum Endpoint endp, const char *fmt, va_list ap)
{
    uint8_t *loggingbuff = NULL;
    uint16_t *pSizeLoggingBuff = NULL;
    uint16_t capLoggingBuff = 0;

    uint16_t sizeLeft;
    int bytesWritten;

    switch (endp) {
    case Ep6Mask:
        loggingbuff = endp6LoggingBuff;
        pSizeLoggingBuff = &sizeEndp6LoggingBuff;
        capLoggingBuff = capacityEndp6LoggingBuff;
        break;
    case Ep7Mask:
        loggingbuff = endp7LoggingBuff;
        pSizeLoggingBuff = &sizeEndp7LoggingBuff;
        capLoggingBuff = capacityEndp7LoggingBuff;
        break;
    default:
        log_to_evaluator("ERROR: usb20_log() cannot log on given endpoint (%x)\r\n", endp);
        return;
    }

    // Critical section, if we print something (outside of an interrrupt) and an
    // interrupt is called and do a print, then the first print is partially
    // overwritten
    bsp_disable_interrupt();
    sizeLeft = capLoggingBuff - *pSizeLoggingBuff;
    
    if (*pSizeLoggingBuff >= capLoggingBuff) {
        log_to_evaluator("ERROR: usb20_log() buffer already filled!");
        sizeLeft = 0;
    }
    
    bytesWritten = vsnprintf(loggingbuff + *pSizeLoggingBuff, sizeLeft, fmt, ap);
    *pSizeLoggingBuff += bytesWritten;
    bsp_enable_interrupt();
}

// TODO: Add description
// WARNING: Currently there is big limitations on this function :
// - Some descriptors are not supported
// - Can only have one descriptor of each supported type
void
usb20_descriptor_set(const uint8_t *newDescriptor)
{
    uint8_t *pTargetDescr = NULL;
    uint8_t targetSize  = 0;
    uint8_t bLength = newDescriptor[0];
    uint8_t bDescriptorType = newDescriptor[1];

    switch (bDescriptorType) {
	case USB_DESCR_TYP_DEVICE:
        pTargetDescr = g_descriptorDevice;
        targetSize = sizeof(USB_DEV_DESCR);
		break;
	case USB_DESCR_TYP_CONFIG:
        pTargetDescr = g_descriptorConfig;
        // TODOO: Fix the size
        targetSize = sizeof(USB_CFG_DESCR);
		break;
	case USB_DESCR_TYP_INTERF:
        /* To Be Done */
        // pTargetDescr = (uint8_t *)&stConfigurationDescriptor.base.itfDescr;
        // targetSize = sizeof(USB_ITF_DESCR);
		break;
	case USB_DESCR_TYP_ENDP:
        /* To Be Done */
        // pTargetDescr = (uint8_t *)&stConfigurationDescriptor.base.endpDescr;
        // targetSize = sizeof(USB_ENDP_DESCR);
		break;
	case USB_DESCR_TYP_STRING:
        // Special case, will be handled later
	case USB_DESCR_TYP_HID:
        // Special case, will be handled later
	case USB_DESCR_TYP_REPORT:
        // Special case, will be handled later
	case USB_DESCR_TYP_QUALIF:
	case USB_DESCR_TYP_SPEED:
	case USB_DESCR_TYP_OTG:
	case USB_DESCR_TYP_PHYSIC:
	case USB_DESCR_TYP_CS_INTF:
	case USB_DESCR_TYP_CS_ENDP:
	case USB_DESCR_TYP_HUB:
        // Not supported yet
    default:
        log_to_evaluator("ERROR: usb20_descriptor_set() bDescriptorType %x not supported", bDescriptorType);
        return;
    }

    memset(pTargetDescr, 0, targetSize);
    log_to_evaluator("targetSize: %d, bLength: %d\r\n", targetSize, bLength);
    memcpy(pTargetDescr, newDescriptor, min(targetSize, bLength));
}

