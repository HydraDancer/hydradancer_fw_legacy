#include "CH56xSFR.h"
#include "CH56x_common.h"
#include "CH56x_debug_log.h"

#include "CH56x_usb30_util.h"
#include "util_debug.h"

// #define DBG1 1
#define ERROR 1

/* Global define */
#define U20_MAXPACKET_LEN       512
#define U20_UEP0_MAXSIZE        8


// TODOO: Add clock for debug (PFIC_Enable(SysTick) ?).
// TODOO: Add debgu over UART.
// TODO: Homogenize var name to camelCase.
// TODO: Homogenize var comments.
// TODO: Add defaults to switches


/* See USB spec. rev. 2.0 p260 for more details. */
USB_DEV_DESCR st_device_descriptor = {
    .bLength = sizeof(USB_DEV_DESCR),
    .bDescriptorType = USB_DESCR_TYP_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x0,    /* Defined in the interface descriptor. */
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 8,
    .idVendor = 0x1337,
    .idProduct = 0x1337,
    .bcdDevice = 0x4200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};


uint8_t hid_report_descriptor_keyboard[] = {
	0x05, 0x01,		// Usage Page (Generic Desktop)
	0x09, 0x06,		// Usage (Keyboard)
	0xA1, 0x01,		// Collection (Application)
	0x95, 0x04,		// Report Count (4)
	0x75, 0x08,		// Report Size (8)
	0x15, 0x00,		// Logical Minimum (0)
	0x25, 0x65,		// Logical Maximum(101)
	0x05, 0x07,		// Usage Page (Key Codes)
	0x19, 0x00,		// Usage Minimum (0)
	0x29, 0x65,		// Usage Maximum (101)
	0x81, 0x00,		// Input (Data, Array   ; Key arrays (4 bytes)
	0xC0,		// End Collection
};


/* WARNING: Be careful, wTotalLength is hardcoded here. */
USB_CFG_DESCR_KEYBOARD st_configuration_descriptor_keyboard = {
    .cfg_descr = {
        .bLength = sizeof(USB_CFG_DESCR),
        .bDescriptorType = USB_DESCR_TYP_CONFIG,
        .wTotalLength = sizeof(USB_CFG_DESCR_KEYBOARD),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 4,
        .bmAttributes = 0x80,
        .MaxPower = 0x64,
    },
    .itf_descr = {
        .bLength = sizeof(USB_ITF_DESCR),
        .bDescriptorType = USB_DESCR_TYP_INTERF,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 1,
        /* Device class is not declared in the device descriptor but should
         * rather be declared in the interface descriptor.
         * - .bInterfaceClass : Class code (here 3 for HID).
         * - .bInterfaceSubClass : Does it support boot protocol ? (Here we
         *   don't).
         * - .bInterfaceProtocol : Keyboard, Mouse, ... (Here 1 for keyboard).
         * More details at :
         * https://www.usb.org/sites/default/files/hid1_11.pdf */
        .bInterfaceClass = USB_DEV_CLASS_HID,
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x01,
        .iInterface = 5,
    },
    .hid_descr = {
        /* See https://www.usb.org/sites/default/files/hid1_11.pdf (p. 22). */
        .bLength = sizeof(USB_HID_DESCR),
        .bDescriptorType = USB_DESCR_TYP_HID,
        .bcdHIDL = 11,
        .bcdHIDH = 1,
        .bCountryCode = 8,
        .bNumDescriptors = 1,
        .bDescriptorTypeX = USB_DESCR_TYP_REPORT,
        .wDescriptorLengthL = sizeof(hid_report_descriptor_keyboard), /* TODO: Only works when size is less than 2**8 ! */
        .wDescriptorLengthH = 0,
    },
    .endp_descr = {
        .bLength = sizeof(USB_ENDP_DESCR),
        .bDescriptorType = USB_DESCR_TYP_ENDP,
        .bEndpointAddress = 0x81,               /* In endpoint (MSB set to 1). */
        .bmAttributes = USB_ENDP_TYPE_INTER,    /* Transfer type. */
        .wMaxPacketSizeL = 8,
        .wMaxPacketSizeH = 0,
        .bInterval = 1,                         /* Polling interval, 1 for isochronous, else 0. */
    },
};


// TODO: Refactor
uint8_t string_langID[] =
{
	0x04, // length of this descriptor
	0x03, /* USB Descriptor Type String */
	0x09, // Language ID 0 low byte
	0x04  // Language ID 0 high byte
};

uint8_t string_descriptor_manufacturer[] = {
    26,                    // .bLength
    USB_DESCR_TYP_STRING,   // .bDescriptorType
    'M', 0x00,
    'a', 0x00,
    'n', 0x00,
    'u', 0x00,
    'f', 0x00,
    'a', 0x00,
    'c', 0x00,
    't', 0x00,
    'u', 0x00,
    'r', 0x00,
    'e', 0x00,
    'r', 0x00,
};

uint8_t string_descriptor_product[] = {
    18,                    // .bLength
    USB_DESCR_TYP_STRING,   // .bDescriptorType
    'P', 0x0,
    'r', 0x0,
    'o', 0x0,
    'd', 0x0,
    'u', 0x0,
    'c', 0x0,
    't', 0x0,
    't', 0x0,
};

uint8_t string_descriptor_serial_number[] = {
    28,                    // .bLength
    USB_DESCR_TYP_STRING,   // .bDescriptorType
    'S', 0x0,
    'e', 0x0,
    'r', 0x0,
    'i', 0x0,
    'a', 0x0,
    'l', 0x0,
    '-', 0x0,
    'n', 0x0,
    'u', 0x0,
    'm', 0x0,
    'b', 0x0,
    'e', 0x0,
    'r', 0x0,
};


uint8_t string_descriptor_config[] = {
    14,                    // .bLength
    USB_DESCR_TYP_STRING,   // .bDescriptorType
    'C', 0x0,
    'o', 0x0,
    'n', 0x0,
    'f', 0x0,
    'i', 0x0,
    'g', 0x0,
};

uint8_t string_descriptor_interface[] = {
    20,                    // .bLength
    USB_DESCR_TYP_STRING,   // .bDescriptorType
    'I', 0x0,
    'n', 0x0,
    't', 0x0,
    'e', 0x0,
    'r', 0x0,
    'f', 0x0,
    'a', 0x0,
    'c', 0x0,
    'e', 0x0,
};


uint8_t *string_descriptors[] = {
    string_langID,
    string_descriptor_manufacturer,
    string_descriptor_product,
    string_descriptor_serial_number,
    string_descriptor_config,
    string_descriptor_interface,
};


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

#ifdef ERROR
    cprintf("ERROR\r\n");
#endif
#ifdef TEST
    cprintf("TEST\r\n");
#endif
#ifdef DBG1
    cprintf("DBG1\r\n");
#endif
#ifdef DBG1
    cprintf("Main ...\r\n");
#endif

    
    // TODOO: Move to init function.
    // USB2 & USB3 Init
	// USB2 & USB3 are managed in LINK_IRQHandler()/TMR0_IRQHandler()/USBHS_IRQHandler()/USBSS_IRQHandler()
	R32_USB_CONTROL = 0;
	PFIC_EnableIRQ(USBHS_IRQn);
	// PFIC_EnableIRQ(USBSS_IRQn);
	PFIC_EnableIRQ(LINK_IRQn);

	// PFIC_EnableIRQ(TMR0_IRQn);
	// R8_TMR0_INTER_EN = RB_TMR_IE_CYC_END;
	// TMR0_TimerInit(67000000); // USB3.0 connection failure timeout about 0.56 seconds

      
    /* Init as device according to the doc (p. 105) */
    R8_USB_CTRL = 0;
    R8_USB_DEV_AD = 0;
    R8_USB_INT_FG = 0;

    R8_USB_CTRL &= ~RB_USB_HOST_MODE;   // Not useful as the field is already 0.
    R8_USB_CTRL &= ~(RB_USB_RESET_SIE | RB_USB_CLR_ALL);   // Not useful as the field is already 0.

    R8_USB_CTRL |= (RB_USB_INT_BUSY | RB_USB_DMA_EN);
    R8_USB_CTRL |= UCST_LS;
    R8_USB_CTRL |= RB_DEV_PU_EN;

    R8_USB_INT_EN = RB_USB_IE_ISOACT | RB_USB_IE_SETUPACT | RB_USB_IE_FIFOOV | RB_USB_IE_SUSPEND | RB_USB_IE_TRANS | RB_USB_IE_BUSRST ;

    /* Init as device's endpoints according to the doc (p. 105) */
    /* TODO: Init EP1 (or any other ep required for keyboard) */
    R8_UEP4_1_MOD = RB_UEP1_RX_EN | RB_UEP1_TX_EN;
    R8_UEP2_3_MOD = 0;
    R8_UEP5_6_MOD = 0;
    R8_UEP7_MOD   = 0;

	R16_UEP0_MAX_LEN = U20_UEP0_MAXSIZE;
	R16_UEP1_MAX_LEN = U20_MAXPACKET_LEN;

	R32_UEP0_RT_DMA = (uint32_t)(uint8_t *)endp0RTbuff;
	R32_UEP1_TX_DMA = (uint32_t)(uint8_t *)endp1Tbuff;
	R32_UEP1_RX_DMA = (uint32_t)(uint8_t *)endp1Rbuff;

	R16_UEP0_T_LEN = 0;
	R8_UEP0_TX_CTRL = 0;
	R8_UEP0_RX_CTRL = 0;

	R16_UEP1_T_LEN = 0;
	R8_UEP1_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_0;
	R8_UEP1_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_0;

    /* NAK all unused endpoints. */
	R16_UEP2_T_LEN = 0;
	R8_UEP2_TX_CTRL = UEP_T_RES_NAK;
	R8_UEP2_RX_CTRL = UEP_R_RES_NAK;

	R16_UEP3_T_LEN = 0;
	R8_UEP3_TX_CTRL = UEP_T_RES_NAK;
	R8_UEP3_RX_CTRL = UEP_R_RES_NAK;

	R16_UEP4_T_LEN = 0;
	R8_UEP4_TX_CTRL = UEP_T_RES_NAK ;
	R8_UEP4_RX_CTRL = UEP_R_RES_NAK;

	R16_UEP5_T_LEN = 0;
	R8_UEP5_TX_CTRL = UEP_T_RES_NAK;
	R8_UEP5_RX_CTRL = UEP_R_RES_NAK;

	R16_UEP6_T_LEN = 0;
	R8_UEP6_TX_CTRL = UEP_T_RES_NAK;
	R8_UEP6_RX_CTRL = UEP_R_RES_NAK;

	R16_UEP7_T_LEN = 0;
	R8_UEP7_TX_CTRL = UEP_T_RES_NAK;
	R8_UEP7_RX_CTRL = UEP_R_RES_NAK;

#ifdef DBG1
    cprintf("Main done!\r\n");
#endif

    while (1) { }
}

__attribute__((interrupt("WCH-Interrupt-fast"))) void
LINK_IRQHandler(void)
{
#ifdef ERROR
    // TODO: Fix bsp_get_tick() not working.
    // cprintf("\r\nCalled:\tUSBHS_IRQHandler()\t%08X\r\n", bsp_get_tick());
    cprintf("LINK Handler!\r\n");
#endif
}

/*******************************************************************************
 * @fn     USBHS_IRQHandler
 *DBG1
 * @brief  USB2.0 Interrupt Handler.
 *
 * @return None
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void
USBHS_IRQHandler(void)
{
#ifdef ERROR
    if ((R8_USB_INT_FG & RB_USB_IF_ISOACT)){
        cprintf("Isochornous transfer!\r\n");
        cprintf_st_cusp(UsbSetupBuf);
    }
#endif
    static uint16_t bytesToWrite = 0;
    static uint8_t *pDataToWrite = NULL;

    static vuint8_t SetupReqType = 0;
    static vuint8_t SetupReq = 0;
    static vuint16_t SetupReqLen = 0; // Data length.

    uint8_t int_flag;
    int_flag = R8_USB_INT_FG;

    if (int_flag & RB_USB_IF_ISOACT)
	{
        /* Not enabled in R8_USB_INT_EN, should never trigger. */
    }
    else if (int_flag & RB_USB_IF_SETUOACT) // Setup interrupt.
	{
		SetupReqType = UsbSetupBuf->bRequestType;
		SetupReq = UsbSetupBuf->bRequest;
		SetupReqLen = UsbSetupBuf->wLength; // Data length.

        if ((SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
        {
            if ((SetupReqType & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
                if (SetupReq == HID_SET_IDLE) {
                    // TODOO: Implememt Idle state.
                    return;
                }
                // Build the Interrupt packet for keyboard.
            }
            /* If bRequest != 0 it is a non standard request, thus not covered  by the spec. */
            return;
        }
        switch(SetupReq) {
        case USB_GET_STATUS:
            endp0RTbuff[0] = 0x00;
            endp0RTbuff[1] = 0x00;
            bytesToWrite = 2;

            switch(UsbSetupBuf->wIndex.w) {
            case 0x81:
                if( ( R8_UEP1_TX_CTRL & RB_UEP_TRES_MASK ) == UEP_T_RES_STALL ) {
                    endp0RTbuff[0] = 0x01;
                    bytesToWrite = 1;
                }
                break;
            case 0x01:
                if( ( R8_UEP1_RX_CTRL & RB_UEP_RRES_MASK ) == UEP_R_RES_STALL ) {
                    endp0RTbuff[0] = 0x01;
                    bytesToWrite = 1;
                }
                break;
            }
            break;
        case USB_CLEAR_FEATURE:
            /* As of now, only endpoints (not device) can be cleared. */
            /* Clear endpoint. */
            if ( (UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP ) {
                switch (UsbSetupBuf->wValue.bw.bb1) {
                case 0x81: /* endpoint 1 IN */
                    R16_UEP1_T_LEN = 0;
                    R8_UEP1_TX_CTRL = UEP_T_RES_NAK | RB_UEP_T_TOG_0;
                    break;
                case 0x01: /* endpoint 1 OUT */
                    R8_UEP1_RX_CTRL = UEP_T_RES_ACK | RB_UEP_R_TOG_0;
                    break;
                default:
                    /* As of now, only ep1 is used. */
                    break;
                }
            }
            break;
        case USB_SET_FEATURE:
            /* See USB spec. rev. 2.0 p258 for more details. */
            if ( (UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP ) {
                switch (UsbSetupBuf->wValue.w) {
                case 0x0000: /* ENDPOINT_HALT */
                    switch( ( UsbSetupBuf->wIndex.w ) & 0xff ) {
                    case 0x81: /* Set endpoint 1 IN STALL */
                        R8_UEP1_TX_CTRL = ( R8_UEP1_TX_CTRL & ~RB_UEP_TRES_MASK ) | UEP_T_RES_STALL;
                        break;
                    case 0x01: /* Set endpoint 1 OUT STALL */
                        R8_UEP1_RX_CTRL = ( R8_UEP1_RX_CTRL & ~RB_UEP_RRES_MASK ) | UEP_R_RES_STALL;
                        break;
                    default:
                        break;
                    }
                    break;
                default:
                    /* Error case, nothing else should be supported. */
                    break;
                }
            }
            break;
        case USB_SET_ADDRESS:
            // NOTE: Address should not be set in this transaction but rather in the
            // following one (RB_USB_IF_TRANSFER IN). R8_USB_DEV_AD =
            // UsbSetupBuf->wValue.bw.bb1;
            break;
        case USB_GET_DESCRIPTOR:
            /* See USB spec. rev. 2.0 p253 for more details. */
            switch(UsbSetupBuf->wValue.bw.bb0) {
            case USB_DESCR_TYP_DEVICE:
                pDataToWrite = (uint8_t *)&st_device_descriptor;
                bytesToWrite = sizeof(st_device_descriptor);
              break;
            case USB_DESCR_TYP_CONFIG:
                pDataToWrite = (uint8_t *)&st_configuration_descriptor_keyboard;
                bytesToWrite = sizeof(st_configuration_descriptor_keyboard);
                break;
            case USB_DESCR_TYP_STRING:
                {
                    cprintf("Descriptor Required: %d\r\n", UsbSetupBuf->wValue.bw.bb1);
                    uint8_t i = UsbSetupBuf->wValue.bw.bb1;
                    if (i >= 0 && i < (sizeof(string_descriptors)/sizeof(string_descriptors[0]))) {
                        pDataToWrite = (uint8_t *)string_descriptors[i];
                        bytesToWrite = string_descriptors[i][0];
                    }
                }
                break;
            case USB_DESCR_TYP_INTERF:
                pDataToWrite = (uint8_t *)&st_configuration_descriptor_keyboard.itf_descr;
                bytesToWrite = sizeof(st_configuration_descriptor_keyboard.itf_descr);
                break;
            case USB_DESCR_TYP_ENDP:
                pDataToWrite = (uint8_t *)&st_configuration_descriptor_keyboard.endp_descr;
                bytesToWrite = sizeof(st_configuration_descriptor_keyboard.endp_descr);
                break;
            case USB_DESCR_TYP_HID:
                pDataToWrite = (uint8_t *)&st_configuration_descriptor_keyboard.hid_descr;
                bytesToWrite = sizeof(st_configuration_descriptor_keyboard.hid_descr);
                break;
            case USB_DESCR_TYP_REPORT:
#ifdef ERROR
    cprintf("USB_DESCR_TYP_REPORT\r\n");
#endif
                pDataToWrite = (uint8_t *)&hid_report_descriptor_keyboard;
                bytesToWrite = sizeof(hid_report_descriptor_keyboard);
                break;
            default:
                break;
            }
            break;
        case USB_SET_DESCRIPTOR:
            break;
        case USB_GET_CONFIGURATION:
            /* We have only one configuration. */
            endp0RTbuff[0] = st_configuration_descriptor_keyboard.cfg_descr.bConfigurationValue; 
            bytesToWrite = 1;
            break;
        case USB_SET_CONFIGURATION:
            /* As of now there is only one configuration. */
            break;
        case USB_GET_INTERFACE:
            /* We have only one interface. */
            endp0RTbuff[0] = st_configuration_descriptor_keyboard.itf_descr.bInterfaceNumber; 
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

        /* Necessary for the first get_descriptor(configuration) */
        if (SetupReqLen < bytesToWrite) {
            bytesToWrite = SetupReqLen;
        }

        uint16_t bytesToWriteForCurrentTransaction = bytesToWrite;
        if (bytesToWriteForCurrentTransaction >= U20_UEP0_MAXSIZE) {
            bytesToWriteForCurrentTransaction = U20_UEP0_MAXSIZE;
        }

        if (pDataToWrite && bytesToWriteForCurrentTransaction > 0) {
            if (UsbSetupBuf->bRequestType & 0x80) { /* IN Transaction. */
                memcpy(endp0RTbuff, pDataToWrite, bytesToWriteForCurrentTransaction);
                pDataToWrite += bytesToWriteForCurrentTransaction;
            }
        }

        bytesToWrite -= bytesToWriteForCurrentTransaction;
        if (bytesToWriteForCurrentTransaction == 0) {    /* If it was the last transaction. */
            pDataToWrite = NULL;
        }

        R16_UEP0_T_LEN = bytesToWriteForCurrentTransaction;
        R8_UEP0_TX_CTRL = UEP_T_RES_ACK | RB_UEP_T_TOG_1; // Data process or state process
        R8_UEP0_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_1;

        R8_USB_INT_FG = RB_USB_IF_SETUOACT; // Clear int flag
    }
    else if (int_flag & RB_USB_IF_FIFOOV)
	{
        /* Not enabled in R8_USB_INT_EN, should never trigger. */
    }
    else if (int_flag & RB_USB_IF_HST_SOF)
	{
        /* Not enabled in R8_USB_INT_EN, should never trigger. */
    }
    else if (int_flag & RB_USB_IF_SUSPEND)
	{
        R8_USB_INT_FG = RB_USB_IF_SUSPEND;
    }
    else if (int_flag & RB_USB_IF_TRANSFER)
	{
        // TODO: Add code, this should be used to transfer data that does not
        // fit in 1 transaction.
        uint32_t endpNum = R8_USB_INT_ST & 0xf;
        uint32_t rxToken = (R8_USB_INT_ST & RB_DEV_TOKEN_MASK);
        uint16_t bytesToWriteForCurrentTransaction;

        switch (endpNum) {
        case 0:
            // TODO: Refactor.
            switch (rxToken) {
            case UIS_TOKEN_OUT:
                break;
            case UIS_TOKEN_SOF:
                break;
            case UIS_TOKEN_IN:
                bytesToWriteForCurrentTransaction = bytesToWrite;
                if (bytesToWriteForCurrentTransaction >= U20_UEP0_MAXSIZE) {
                    bytesToWriteForCurrentTransaction = U20_UEP0_MAXSIZE;
                }
                bytesToWrite -= bytesToWriteForCurrentTransaction;

                switch(SetupReq) {
                case USB_GET_DESCRIPTOR:
                if (pDataToWrite && bytesToWriteForCurrentTransaction > 0) {
                    if (SetupReqType & 0x80) { /* IN Transaction. */
                        memcpy(endp0RTbuff, pDataToWrite, bytesToWriteForCurrentTransaction);
                        pDataToWrite += bytesToWriteForCurrentTransaction;
                    }
                }
                break;
            case USB_SET_ADDRESS:
                R8_USB_DEV_AD = UsbSetupBuf->wValue.bw.bb1;
                bytesToWriteForCurrentTransaction = 0;
                break;
            }

            if (bytesToWriteForCurrentTransaction == 0) {    /* If it was the last transaction. */
                pDataToWrite = NULL;

                R16_UEP0_T_LEN = 0;                               // Clear send length
                R8_UEP0_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_1; // The state process is OUT
                R8_UEP0_TX_CTRL = 0;                              // Clear send controller
            } else {
                R16_UEP0_T_LEN = bytesToWriteForCurrentTransaction;
                R8_UEP0_TX_CTRL ^= RB_UEP_T_TOG_1;
                R8_UEP0_TX_CTRL = ( R8_UEP0_TX_CTRL &~RB_UEP_TRES_MASK )| UEP_T_RES_ACK ;
            }

                break;
            case UIS_TOKEN_SETUP:
                break;
            }
            break;
        case 1:
            // TODO: Refactor.
            // ...
            if (rxToken == UIS_TOKEN_IN) {
#ifdef ERROR
    cprintf("Polling for input!\r\n");
#endif

                // static uint8_t keyboard_payload[] = { 0x05, 0x0c, 0x17, 0x08, 0x28 };
                static uint8_t keyboard_payload[] = { 0x17, 0x08, 0x16, 0x17, 0x28 };
                static uint8_t i = 0;
                uint8_t modulus = 10;

                // Keyboard input crafting.
                uint8_t output[] = { 0x00, 0x00, 0x00, 0x00 };
                if (i%modulus == 0) {
                    output[0] = keyboard_payload[(i%(modulus*sizeof(keyboard_payload)))/modulus];
                }
                i++;


                if (SetupReqType & 0x80) { /* IN Transaction. */
#ifdef ERROR
    cprintf("Polling for input IN!\r\n");
#endif
                    memcpy(endp1Tbuff, output, sizeof(output));
                    R16_UEP1_T_LEN = sizeof(output);
                    R8_UEP1_TX_CTRL ^= RB_UEP_T_TOG_1;
                    R8_UEP1_TX_CTRL = ( R8_UEP1_TX_CTRL &~RB_UEP_TRES_MASK )| UEP_T_RES_ACK ;
                }
            }
            break;
        }

        R8_USB_INT_FG = RB_USB_IF_TRANSFER; // Clear int flag
    }
    else if (int_flag & RB_USB_IF_BUSRST)
	{
        // TODO
 		// R8_USB_CTRL = 0x06; 
		// R8_USB_INT_EN = 0x00; 
		// R8_USB_DEV_AD = 0x00; 
		// R16_USB_FRAME_NO = 0x0000; 
		// R8_USB_SUSPEND = 0x00; 
		// R8_USB_SPD_TYPE = 0x00; 
		// R8_USB_MIS_ST = 0x20; 
		// R8_USB_INT_FG = 0x00; 

        R8_USB_CTRL = 0;
        R8_USB_DEV_AD = 0;
        R8_USB_INT_FG = 0;

        R8_USB_CTRL &= ~RB_USB_HOST_MODE;   // Not useful as the field is already 0.
        R8_USB_CTRL &= ~(RB_USB_RESET_SIE | RB_USB_CLR_ALL);   // Not useful as the field is already 0.

        R8_USB_CTRL |= (RB_USB_INT_BUSY | RB_USB_DMA_EN);
        R8_USB_CTRL |= UCST_LS;
        R8_USB_CTRL |= RB_DEV_PU_EN;

        R8_USB_INT_EN = RB_USB_IE_SETUPACT | RB_USB_IE_TRANS | RB_USB_IE_SUSPEND  |RB_USB_IE_BUSRST ;


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
__attribute__((interrupt("WCH-Interrupt-fast"))) void HardFault_Handler(void)
{
	cprintf("HardFault\r\n");
	cprintf(" SP=0x%08X\r\n", __get_SP());
	cprintf(" MIE=0x%08X\r\n", __get_MIE());
	cprintf(" MSTATUS=0x%08X\r\n", __get_MSTATUS());
	cprintf(" MCAUSE=0x%08X\r\n", __get_MCAUSE());
	bsp_wait_ms_delay(1000000);
}

