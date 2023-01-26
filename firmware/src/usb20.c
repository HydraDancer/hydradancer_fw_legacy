#include <stdarg.h>
#include <string.h>

#include "usb20.h"

/* variables */
uint16_t sizeEndp7LoggingBuff = 0;
const uint16_t capacityEndp7LoggingBuff = 4096;
__attribute__((aligned(16))) uint8_t endp7LoggingBuffRaw[4096];
uint8_t *endp7LoggingBuff = endp7LoggingBuffRaw;

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
array_addr_len(void **array)
{
    for (uint8_t i = 0;; ++i) {
        if (array[i] == NULL) {
            return i;
        }
    }
}

/* @fn      U20_registers_init
 *
 * @brief   Initialise registers and enable interrupt related to USB 2.0
 *
 * @return  None
 */
void
U20_registers_init(enum Speed sp)
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

/* @fn      U20_endpoints_init
 *
 * @brief   Initialise registers and buffers for endpoints, by default none of
 *          them is enabled (except ep0)
 *
 * @return  None
 */
void
U20_endpoints_init(enum Endpoint endpointsMask)
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

    if (endpointsMask & Ep1Mask) {
        R8_UEP4_1_MOD |= RB_UEP1_RX_EN | RB_UEP1_TX_EN;
        R8_UEP1_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
        R8_UEP1_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP1_TX_CTRL = UEP_T_RES_NAK;
        R8_UEP1_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsMask & Ep2Mask) {
        R8_UEP2_3_MOD |= RB_UEP2_RX_EN | RB_UEP2_TX_EN;
        R8_UEP2_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
        R8_UEP2_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP2_TX_CTRL = UEP_T_RES_NAK;
        R8_UEP2_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsMask & Ep3Mask) {
        R8_UEP2_3_MOD |= RB_UEP3_RX_EN | RB_UEP3_TX_EN;
        R8_UEP3_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
        R8_UEP3_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP3_TX_CTRL = UEP_T_RES_NAK;
        R8_UEP3_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsMask & Ep4Mask) {
        R8_UEP4_1_MOD |= RB_UEP4_RX_EN | RB_UEP4_TX_EN;
        R8_UEP4_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
        R8_UEP4_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP4_TX_CTRL = UEP_T_RES_NAK;
        R8_UEP4_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsMask & Ep5Mask) {
        R8_UEP5_6_MOD |= RB_UEP5_RX_EN | RB_UEP5_TX_EN;
        R8_UEP5_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
        R8_UEP5_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP5_TX_CTRL = UEP_T_RES_NAK;
        R8_UEP5_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsMask & Ep6Mask) {
        R8_UEP5_6_MOD |= RB_UEP6_RX_EN | RB_UEP6_TX_EN;
        R8_UEP6_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
        R8_UEP6_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP6_TX_CTRL = UEP_T_RES_NAK;
        R8_UEP6_RX_CTRL = UEP_R_RES_NAK;
    }

    if (endpointsMask & Ep7Mask) {
        R8_UEP7_MOD |= RB_UEP7_RX_EN | RB_UEP7_TX_EN;
        R8_UEP7_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
        R8_UEP7_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;
    } else {
        R8_UEP7_TX_CTRL = UEP_T_RES_NAK;
        R8_UEP7_RX_CTRL = UEP_R_RES_NAK;
    }
}

/* @fn      endpoint_clear
 *
 * @brief   Reset the given endpoint
 *
 * @warning It only reset one endpoint, do NOT give multiples !
 * 
 * @return  None
 */
void
endpoint_clear(uint8_t endpointToClear)
{
    switch (endpointToClear) {
    case 0x81: /* endpoint 1 IN. */
        R16_UEP1_T_LEN = 0;
        R8_UEP1_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x01: /* endpoint 1 OUT. */
        R8_UEP1_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x82: /* endpoint 2 IN. */
        R16_UEP2_T_LEN = 0;
        R8_UEP2_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x02: /* endpoint 2 OUT. */
        R8_UEP2_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x83: /* endpoint 3 IN. */
        R16_UEP3_T_LEN = 0;
        R8_UEP3_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x03: /* endpoint 3 OUT. */
        R8_UEP3_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x84: /* endpoint 4 IN. */
        R16_UEP4_T_LEN = 0;
        R8_UEP4_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x04: /* endpoint 4 OUT. */
        R8_UEP4_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x85: /* endpoint 5 IN. */
        R16_UEP5_T_LEN = 0;
        R8_UEP5_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x05: /* endpoint 5 OUT. */
        R8_UEP5_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x86: /* endpoint 6 IN. */
        R16_UEP6_T_LEN = 0;
        R8_UEP6_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x06: /* endpoint 6 OUT. */
        R8_UEP6_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    case 0x87: /* endpoint 7 IN. */
        R16_UEP7_T_LEN = 0;
        R8_UEP7_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
        break;
    case 0x07: /* endpoint 7 OUT. */
        R8_UEP7_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
        break;
    default:
        usb_log("ERROR: endpoint_clear() invalid argument");
        break;
    }
}

/* @fn      endpoint_halt
 *
 * @brief   Halt the given endpoint
 *
 * @warning It only halt one endpoint at a time, do NOT give multiples !
 * 
 * @return  None
 */
void
endpoint_halt(uint8_t endpointToHalt)
{
    switch(endpointToHalt) {
    case 0x81: /* Set endpoint 1 IN STALL. */
        R8_UEP1_TX_CTRL = (R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x01: /* Set endpoint 1 OUT STALL. */
        R8_UEP1_RX_CTRL = (R8_UEP1_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x82: /* Set endpoint 2 IN STALL. */
        R8_UEP2_TX_CTRL = (R8_UEP2_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x02: /* Set endpoint 2 OUT STALL. */
        R8_UEP2_RX_CTRL = (R8_UEP2_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x83: /* Set endpoint 3 IN STALL. */
        R8_UEP3_TX_CTRL = (R8_UEP3_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x03: /* Set endpoint 3 OUT STALL. */
        R8_UEP3_RX_CTRL = (R8_UEP3_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x84: /* Set endpoint 4 IN STALL. */
        R8_UEP4_TX_CTRL = (R8_UEP4_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x04: /* Set endpoint 4 OUT STALL. */
        R8_UEP4_RX_CTRL = (R8_UEP4_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x85: /* Set endpoint 5 IN STALL. */
        R8_UEP5_TX_CTRL = (R8_UEP5_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x05: /* Set endpoint 5 OUT STALL. */
        R8_UEP5_RX_CTRL = (R8_UEP5_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x86: /* Set endpoint 6 IN STALL. */
        R8_UEP6_TX_CTRL = (R8_UEP6_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x06: /* Set endpoint 6 OUT STALL. */
        R8_UEP6_RX_CTRL = (R8_UEP6_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    case 0x87: /* Set endpoint 7 IN STALL. */
        R8_UEP7_TX_CTRL = (R8_UEP7_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_STALL;
        break;
    case 0x07: /* Set endpoint 7 OUT STALL. */
        R8_UEP7_RX_CTRL = (R8_UEP7_RX_CTRL & ~RB_UEP_RRES_MASK) | UEP_R_RES_STALL;
        break;
    default:
        usb_log("ERROR: endpoint_halt() invalid argument");
        break;
    }
}

/* @fn      fill_buffer_with_descriptor
 *
 * @brief   Fill the given buffer with the requested descriptor
 *
 * @return  None
 */
void
fill_buffer_with_descriptor(UINT16_UINT8 descritorRequested, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    switch(descritorRequested.bw.bb0) {
    case USB_DESCR_TYP_DEVICE:
        *pBuffer = (uint8_t *)&stDeviceDescriptor;
        *pSizeBuffer = stDeviceDescriptor.bLength;
        break;
    case USB_DESCR_TYP_CONFIG:
        /* The .cfgDescr field is always the first, no matter the union's type.
         */
        *pBuffer = (uint8_t *)&stConfigurationDescriptor.base;
        *pSizeBuffer = stConfigurationDescriptor.base.cfgDescr.wTotalLength;
        break;
    case USB_DESCR_TYP_STRING: {
        uint8_t i = descritorRequested.bw.bb1;
        if (i >= 0 && i < array_addr_len((void **)stringDescriptors)) {
            *pBuffer = (uint8_t *)stringDescriptors[i];
            *pSizeBuffer = stringDescriptors[i][0];
        }
    }
    break;
    case USB_DESCR_TYP_INTERF:
        *pBuffer = (uint8_t *)&stInterfaceDescriptor;
        *pSizeBuffer = stInterfaceDescriptor.bLength;
        break;
    case USB_DESCR_TYP_ENDP:
        *pBuffer = (uint8_t *)&stEndpointDescriptor;
        *pSizeBuffer = stEndpointDescriptor.bLength;
        break;
    case USB_DESCR_TYP_HID:
        *pBuffer = (uint8_t *)&stHidDescriptor;
        *pSizeBuffer = stHidDescriptor.bLength;
        break;
    case USB_DESCR_TYP_REPORT:
        *pBuffer = (uint8_t *)reportDescriptor;
        /* TODO: support lengh of type uint16_t. */
        *pSizeBuffer = stHidDescriptor.wDescriptorLengthL;
        break;
    default:
        usb_log("ERROR: fill_buffer_with_descriptor() invalid descriptor requested");
        break;
    }
}

/* @fn      ep0_transceive_and_update
 *
 * @brief   Handle the "command" on endpoint 0 (mainly receive/transmit) and 
 *          update the buffer accordingly
 *
 * @return  None
 */
void
ep0_transceive_and_update(uint8_t uisToken, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    uint16_t bytesToWriteForCurrentTransaction = 0;

    switch (uisToken) {
    case UIS_TOKEN_OUT:
        /* Not implemented. */
        break;
    case UIS_TOKEN_SOF:
        /* Not implemented. */
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
        /* Not implemented. */
        break;
    default:
        usb_log("ERROR: ep0_transceive_and_update() invalid uisToken");
        break;
    }

    *pSizeBuffer -= bytesToWriteForCurrentTransaction;

    if (bytesToWriteForCurrentTransaction == 0) {    /* If it was the last transaction. */
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

/* @fn      ep7_transmit_and_update
 *
 * @brief   Handle the "command" on endpoint 7 (transmit debug) and update the
 *          buffer accordingly
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
            *(uint32_t *)pBuffer += U20_UEP7_MAXSIZE; /* Careful! We increase from the PREVIOUSLY read value. */
        } else {
            *pBuffer = bufferResetValue;

            R16_UEP7_T_LEN = 0;
            R8_UEP7_TX_CTRL ^= RB_UEP_T_TOG_1;
            R8_UEP7_TX_CTRL = (R8_UEP7_TX_CTRL & ~RB_UEP_TRES_MASK) | UEP_T_RES_ACK;
        }
        break;
        default:
            usb_log("ERROR: ep7_transmit_and_update default!");
            break;
    }
}

/* @fn      usb_log
 *
 * @brief   Function used to log data to the Host computer over USB
 *
 * @return  None
 */
void
usb_log(const char *fmt, ...)
{
    // Critical section, if we print something (outside of an interrrupt) and an
    // interrupt is called and do a print, then the first print is partially
    // overwritten.
    va_list ap;
    bsp_disable_interrupt();
    va_start(ap, fmt);
    uint16_t sizeLeft = capacityEndp7LoggingBuff - sizeEndp7LoggingBuff;
    
    if (sizeEndp7LoggingBuff >= capacityEndp7LoggingBuff) {
        usb_log("ERROR: Buffer already filled!");
        sizeLeft = 0;
    }
    
    int bytesWritten = vsnprintf(endp7LoggingBuff + sizeEndp7LoggingBuff, sizeLeft, fmt, ap);
    sizeEndp7LoggingBuff += bytesWritten;
    bsp_enable_interrupt();
}

