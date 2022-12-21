/*
 * Small description of the project.
 */
#include <assert.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"
#include "CH56x_debug_log.h"

// TODOOO: Add Halt support for endpoints (get_status()).
// TODOO: Add clock for debug (PFIC_Enable(SysTick) ?).
// TODOO: Add debgu over UART.
// TODO: Homogenize var name to camelCase.
// TODO: Homogenize var comments.
// TODO: Add defaults to switches.
// TODO: Add idle state.


/* macros */
#define U20_MAXPACKET_LEN       512
#define U20_UEP0_MAXSIZE        8
#define UsbSetupBuf ((PUSB_SETUP)endp0RTbuff)

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
enum ConfigurationDescriptorType { CfgDescrBase, CfgDescrWithHid };

typedef union {
	uint16_t w;
	struct BW {
		uint8_t bb1; // low byte
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

typedef union {
    USB_CFG_DESCR_FULL_BASE base;
    USB_CFG_DESCR_FULL_HID withHid;
} USB_CFG_DESCR_FULL, *PUSB_CFG_DESCR_FULL;

/* function declarations */
static uint8_t array_addr_len(void **array);
static void U20_init(enum Speed sp);
static void U20_endpoints_init(enum Endpoint endpointsMask);
static void endpoint_clear(uint8_t endpointToClear);
static void endpoint_halt(uint8_t endpointToHalt);
static void fill_buffer_with_descriptor(UINT16_UINT8 descritorRequested, uint8_t **pBuffer, uint16_t *pSizeBuffer);
static uint16_t ep0_transceive_and_update(uint8_t bDirection, uint8_t **buffer, uint16_t *sizeBuffer);
static void ep1_transmit_keyboard(void);

/* variables */
static enum ConfigurationDescriptorType cfgDescrType = CfgDescrBase;
static enum Speed speed = SpeedLow;
static enum Endpoint epMask = 0;
static USB_DEV_DESCR stDeviceDescriptor;
static USB_CFG_DESCR_FULL stConfigurationDescriptor;
static USB_ITF_DESCR stInterfaceDescriptor;
static USB_ENDP_DESCR stEndpointDescriptor;
static USB_HID_DESCR stHidDescriptor;
static uint8_t *reportDescriptor;
static uint8_t **stringDescriptors;

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

/* configuration, allows nested code to access above variables */
#include "config.h"

/* function implemtations */
static uint8_t
array_addr_len(void **array)
{
    for (uint8_t i = 0;; ++i) {
        if (array[i] == NULL) {
            return i;
        }
    }
}

static void
U20_init(enum Speed sp)
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

static void
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

static void
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
        assert(0 && "ERROR: endpoint_clear() invalid argument");
        break;
    }
}

static void
endpoint_halt(uint8_t endpointToHalt)
{
    switch(endpointToHalt) {
    case 0x81: /* Set endpoint 1 IN STALL. */
        R8_UEP1_TX_CTRL = ( R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK ) | UEP_T_RES_STALL;
        break;
    case 0x01: /* Set endpoint 1 OUT STALL. */
        R8_UEP1_RX_CTRL = ( R8_UEP1_RX_CTRL & ~RB_UEP_RRES_MASK ) | UEP_R_RES_STALL;
        break;
    case 0x82: /* Set endpoint 2 IN STALL. */
        R8_UEP2_TX_CTRL = ( R8_UEP2_TX_CTRL & ~RB_UEP_TRES_MASK ) | UEP_T_RES_STALL;
        break;
    case 0x02: /* Set endpoint 2 OUT STALL. */
        R8_UEP2_RX_CTRL = ( R8_UEP2_RX_CTRL & ~RB_UEP_RRES_MASK ) | UEP_R_RES_STALL;
        break;
    case 0x83: /* Set endpoint 3 IN STALL. */
        R8_UEP3_TX_CTRL = ( R8_UEP3_TX_CTRL & ~RB_UEP_TRES_MASK ) | UEP_T_RES_STALL;
        break;
    case 0x03: /* Set endpoint 3 OUT STALL. */
        R8_UEP3_RX_CTRL = ( R8_UEP3_RX_CTRL & ~RB_UEP_RRES_MASK ) | UEP_R_RES_STALL;
        break;
    case 0x84: /* Set endpoint 4 IN STALL. */
        R8_UEP4_TX_CTRL = ( R8_UEP4_TX_CTRL & ~RB_UEP_TRES_MASK ) | UEP_T_RES_STALL;
        break;
    case 0x04: /* Set endpoint 4 OUT STALL. */
        R8_UEP4_RX_CTRL = ( R8_UEP4_RX_CTRL & ~RB_UEP_RRES_MASK ) | UEP_R_RES_STALL;
        break;
    case 0x85: /* Set endpoint 5 IN STALL. */
        R8_UEP5_TX_CTRL = ( R8_UEP5_TX_CTRL & ~RB_UEP_TRES_MASK ) | UEP_T_RES_STALL;
        break;
    case 0x05: /* Set endpoint 5 OUT STALL. */
        R8_UEP5_RX_CTRL = ( R8_UEP5_RX_CTRL & ~RB_UEP_RRES_MASK ) | UEP_R_RES_STALL;
        break;
    case 0x86: /* Set endpoint 6 IN STALL. */
        R8_UEP6_TX_CTRL = ( R8_UEP6_TX_CTRL & ~RB_UEP_TRES_MASK ) | UEP_T_RES_STALL;
        break;
    case 0x06: /* Set endpoint 6 OUT STALL. */
        R8_UEP6_RX_CTRL = ( R8_UEP6_RX_CTRL & ~RB_UEP_RRES_MASK ) | UEP_R_RES_STALL;
        break;
    case 0x87: /* Set endpoint 7 IN STALL. */
        R8_UEP7_TX_CTRL = ( R8_UEP7_TX_CTRL & ~RB_UEP_TRES_MASK ) | UEP_T_RES_STALL;
        break;
    case 0x07: /* Set endpoint 7 OUT STALL. */
        R8_UEP7_RX_CTRL = ( R8_UEP7_RX_CTRL & ~RB_UEP_RRES_MASK ) | UEP_R_RES_STALL;
        break;
    default:
        assert(0 && "ERROR: endpoint_halt() invalid argument");
        break;
    }
}

static void
fill_buffer_with_descriptor(UINT16_UINT8 descritorRequested, uint8_t **pBuffer, uint16_t *pSizeBuffer)
{
    switch(descritorRequested.bw.bb0) {
        case USB_DESCR_TYP_DEVICE:
            *pBuffer = (uint8_t *)&stDeviceDescriptor;
            *pSizeBuffer = stDeviceDescriptor.bLength;
            break;
        case USB_DESCR_TYP_CONFIG:
            switch (cfgDescrType) {
                case CfgDescrBase:
                    *pBuffer = (uint8_t *)&stConfigurationDescriptor.base;
                    *pSizeBuffer = stConfigurationDescriptor.base.cfgDescr.wTotalLength;
                    break;
                case CfgDescrWithHid:
                    *pBuffer = (uint8_t *)&stConfigurationDescriptor.withHid;
                    *pSizeBuffer = stConfigurationDescriptor.withHid.cfgDescr.wTotalLength;
                    break;
            }
            break;
        case USB_DESCR_TYP_STRING:
            {
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
            assert(0 && "ERROR: fill_buffer_with_descriptor() invalid descriptor requested");
            break;
    }
}

static uint16_t
ep0_transceive_and_update(uint8_t uisToken, uint8_t **buffer, uint16_t *sizeBuffer)
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
        bytesToWriteForCurrentTransaction = *sizeBuffer;
        if (bytesToWriteForCurrentTransaction >= U20_UEP0_MAXSIZE) {
            bytesToWriteForCurrentTransaction = U20_UEP0_MAXSIZE;
        }

        if (*buffer && bytesToWriteForCurrentTransaction > 0) {
            memcpy(endp0RTbuff, *buffer, bytesToWriteForCurrentTransaction);
        }
        break;
    case UIS_TOKEN_SETUP:
        /* Not implemented. */
        break;
    default:
        assert(0 && "ERROR: ep0_transceive_and_update() invalid uisToken");
        break;
    }

    *sizeBuffer -= bytesToWriteForCurrentTransaction;

    if (bytesToWriteForCurrentTransaction == 0) {    /* If it was the last transaction. */
        *buffer = NULL;

        R16_UEP0_T_LEN = 0;
        R8_UEP0_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_1;
        R8_UEP0_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_1;
    } else {
        *buffer += bytesToWriteForCurrentTransaction;

        R16_UEP0_T_LEN = bytesToWriteForCurrentTransaction;
        R8_UEP0_TX_CTRL ^= RB_UEP_T_TOG_1;
        R8_UEP0_TX_CTRL = ( R8_UEP0_TX_CTRL &~RB_UEP_TRES_MASK )| UEP_T_RES_ACK;
    }
}

static void
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
    R8_UEP1_TX_CTRL = ( R8_UEP1_TX_CTRL &~RB_UEP_TRES_MASK )| UEP_T_RES_ACK ;
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int
main(void)
{
    bsp_init(FREQ_SYS);
    UART1_init(115200, FREQ_SYS);

    cfgDescrType = CfgDescrWithHid;
    speed = SpeedLow;
    epMask = Ep1Mask;

    stDeviceDescriptor    = stKeyboardDeviceDescriptor;
    stInterfaceDescriptor = stKeyboardConfigurationDescriptor.itfDescr;
    stEndpointDescriptor  = stKeyboardConfigurationDescriptor.endpDescr;
    stHidDescriptor       = stKeyboardConfigurationDescriptor.hidDescr;
    reportDescriptor      = keyboardReportDescriptor;
    stringDescriptors     = keyboardStringDescriptors;

    switch (cfgDescrType) {
        case CfgDescrBase:
            // No base descritor available for now.
            // stConfigurationDescriptor.base    = stKeyboardConfigurationDescriptor;
            break;
        case CfgDescrWithHid:
            stConfigurationDescriptor.withHid = stKeyboardConfigurationDescriptor;
            break;
    }

    U20_init(speed);
    U20_endpoints_init(epMask);

    while (1) { }
}

/*******************************************************************************
 * @fn     USBHS_IRQHandler
 *
 * @brief  USB2.0 Interrupt Handler.
 *
 * @return None
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void
USBHS_IRQHandler(void)
{
    static uint16_t bytesToWrite = 0;
    static uint8_t *pDataToWrite = NULL;

    static vuint8_t SetupReqType = 0;
    static vuint8_t SetupReq = 0;
    static vuint16_t SetupReqLen = 0;

    if (R8_USB_INT_FG & RB_USB_IF_ISOACT)
	{
        /* Unused. */
        R8_USB_INT_FG = RB_USB_IF_ISOACT;
    }
    else if (R8_USB_INT_FG & RB_USB_IF_SETUOACT) // Setup interrupt.
	{
		SetupReqType = UsbSetupBuf->bRequestType;
		SetupReq = UsbSetupBuf->bRequest;
		SetupReqLen = UsbSetupBuf->wLength;

        /* If bRequest != 0 it is a non standard request, thus not covered  by the spec. */
        if ((SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
            return;
        }

        switch(SetupReq) {
        case USB_GET_STATUS:
            endp0RTbuff[0] = 0x00;
            endp0RTbuff[1] = 0x00;
            bytesToWrite = 2;

            // Here should be the handling of the halt endpoint's command.
            break;
        case USB_CLEAR_FEATURE:
            switch (SetupReqType & USB_REQ_RECIP_MASK) {
            case USB_REQ_RECIP_DEVICE:
                /* Not implemented. */
                break;
            case USB_REQ_RECIP_INTERF:
                /* Not implemented. */
                break;
            case USB_REQ_RECIP_ENDP:
                endpoint_clear(UsbSetupBuf->wValue.bw.bb1);
                break;
            default:
                assert(0 && "ERROR: SETUP Interrupt USB_CLEAR_FEATURE invalid recipient");
                break;
            }
            break;
        case USB_SET_FEATURE:
            switch (SetupReqType & USB_REQ_RECIP_MASK) {
            case USB_REQ_RECIP_DEVICE:
                /* Not implemented. */
                    assert(0 && "ERROR: SETUP Interrupt USB_SET_FEATURE (toward device) unimplemented");
                break;
            case USB_REQ_RECIP_INTERF:
                /* Not implemented. */
                    assert(0 && "ERROR: SETUP Interrupt USB_SET_FEATURE (toward interface) unimplemented");
                break;
            case USB_REQ_RECIP_ENDP:
                switch (UsbSetupBuf->wValue.w) {
                case 0x0000: /* ENDPOINT_HALT */
                    endpoint_halt(UsbSetupBuf->wValue.bw.bb1);
                    break;
                default:
                    assert(0 && "ERROR: SETUP Interrupt USB_SET_FEATURE (toward endpoint) unimplemented");
                    break;
                }
                break;
            default:
                assert(0 && "ERROR: SETUP Interrupt USB_SET_FEATURE invalid recipient");
                break;
            }
            break;
        case USB_SET_ADDRESS:
            // NOTE: Address should not be set in this transaction but rather in the
            // following one (RB_USB_IF_TRANSFER IN).
            break;
        case USB_GET_DESCRIPTOR:
            fill_buffer_with_descriptor(UsbSetupBuf->wValue, &pDataToWrite, &bytesToWrite);
            break;
        case USB_SET_DESCRIPTOR:
                /* Unused. */
            break;
        case USB_GET_CONFIGURATION:
            /* We have only one configuration. */
            endp0RTbuff[0] = stConfigurationDescriptor.base.cfgDescr.bConfigurationValue; 
            bytesToWrite = 1;
            break;
        case USB_SET_CONFIGURATION:
            /* As of now there is only one configuration. */
            break;
        case USB_GET_INTERFACE:
            /* We have only one interface. */
            endp0RTbuff[0] = stInterfaceDescriptor.bInterfaceNumber; 
            bytesToWrite = 1;
            break;
        case USB_SET_INTERFACE:
            /* As of now there is only one interface. */
            break;
        case USB_SYNCH_FRAME:
            break;
        default:
            break;
        }

        if (SetupReqLen < bytesToWrite) {
            bytesToWrite = SetupReqLen;
        }

        uint8_t uisToken = UIS_TOKEN_OUT;
        if (SetupReqType & 0x80) {
            uisToken = UIS_TOKEN_IN;
        }
        ep0_transceive_and_update(uisToken, &pDataToWrite, &bytesToWrite);
        /* Packet type must cycle between DATA0 and DATA1. The request (the
         * first packet) is DATA0, thus the next packet must be DATA1 and so on.
         * So here the first packet is forced to 1. */
        R8_UEP0_TX_CTRL |= RB_UEP_T_TOG_1;

        R8_USB_INT_FG = RB_USB_IF_SETUOACT;
    }
    else if (R8_USB_INT_FG & RB_USB_IF_FIFOOV)
	{
        /* Unused. */
        R8_USB_INT_FG = RB_USB_IF_FIFOOV;
    }
    else if (R8_USB_INT_FG & RB_USB_IF_SUSPEND)
	{
        R8_USB_INT_FG = RB_USB_IF_SUSPEND;
    }
    else if (R8_USB_INT_FG & RB_USB_IF_TRANSFER)
	{
        uint32_t endpNum = R8_USB_INT_ST & RB_DEV_ENDP_MASK;
        uint32_t rxToken = (R8_USB_INT_ST & RB_DEV_TOKEN_MASK);
        uint16_t bytesToWriteForCurrentTransaction;



        switch (endpNum) {
        case 0:
            if (SetupReq == USB_SET_ADDRESS) {
                R8_USB_DEV_AD = UsbSetupBuf->wValue.bw.bb1;

                pDataToWrite = NULL;
                R16_UEP0_T_LEN = 0;
                R8_UEP0_TX_CTRL = 0;
                R8_UEP0_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_1;
                } else {
                    ep0_transceive_and_update(rxToken, &pDataToWrite, &bytesToWrite);
                }
            break;
        case 1:
            ep1_transmit_keyboard();
            break;
        }

        R8_USB_INT_FG = RB_USB_IF_TRANSFER; // Clear int flag
    }
    else if (R8_USB_INT_FG & RB_USB_IF_BUSRST)
	{
        U20_init(speed);
        U20_endpoints_init(epMask);

        R8_USB_INT_FG = RB_USB_IF_BUSRST;
    }
}


/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   Example of basic HardFault Handler called if an exception occurs
 *
 * @return  none
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void
HardFault_Handler(void)
{
	cprintf("HardFault\r\n");
	cprintf(" SP=0x%08X\r\n", __get_SP());
	cprintf(" MIE=0x%08X\r\n", __get_MIE());
	cprintf(" MSTATUS=0x%08X\r\n", __get_MSTATUS());
	cprintf(" MCAUSE=0x%08X\r\n", __get_MCAUSE());
	bsp_wait_ms_delay(1000000);
}

