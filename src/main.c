#include "CH56xSFR.h"
#include "CH56x_common.h"

#include "CH56x_usb30_util.h"


/* Global define */
#define U20_MAXPACKET_LEN       512
#define U20_UEP0_MAXSIZE        64


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
    .bMaxPacketSize0 = 64,
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
	0x95, 0x06,		// Report Count (4)
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
        .bmAttributes = USB_ENDP_TYPE_ISOCH,    /* Transfer type. */
        .wMaxPacketSizeL = 64,
        .wMaxPacketSizeH = 0,
        .bInterval = 1,                         /* Polling interval, 1 for isochronous, else 0. */
    },
};


uint8_t string_descriptor_manufacturer[] = {
    0x1A,                    // .bLength
    USB_DESCR_TYP_STRING,   // .bDescriptorType
    'M', 0x0,
    'a', 0x0,
    'n', 0x0,
    'u', 0x0,
    'f', 0x0,
    'a', 0x0,
    'c', 0x0,
    't', 0x0,
    'u', 0x0,
    'r', 0x0,
    'e', 0x0,
    'r', 0x0,
};

uint8_t string_descriptor_product[] = {
    0x10,                    // .bLength
    USB_DESCR_TYP_STRING,   // .bDescriptorType
    'P', 0x0,
    'r', 0x0,
    'o', 0x0,
    'd', 0x0,
    'u', 0x0,
    'c', 0x0,
    't', 0x0,
};

uint8_t string_descriptor_serial_number[] = {
    0x1C,                    // .bLength
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
    0x0D,                    // .bLength
    USB_DESCR_TYP_STRING,   // .bDescriptorType
    'C', 0x0,
    'o', 0x0,
    'n', 0x0,
    'f', 0x0,
    'i', 0x0,
    'g', 0x0,
};

uint8_t string_descriptor_interface[] = {
    0x14,                    // .bLength
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
    NULL,   /* Array must start at 1. */
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
#if DEBUG
    cprintf("Main ...\n");
#endif
    bsp_init(FREQ_SYS);
    // TODOO: Move to init function.
      
    /* Init as device according to the doc (p. 105) */
    R8_USB_CTRL = 0;
    R8_USB_DEV_AD = 0;
    R8_USB_INT_FG = 0;

    R8_USB_CTRL &= ~RB_USB_HOST_MODE;   // Not useful as the field is already 0.
    R8_USB_CTRL &= ~(RB_USB_RESET_SIE | RB_USB_CLR_ALL);   // Not useful as the field is already 0.

    R8_USB_CTRL |= (RB_USB_INT_BUSY | RB_USB_DMA_EN);
    R8_USB_CTRL |= UCST_HS;
    R8_USB_CTRL |= RB_DEV_PU_EN;

    R8_USB_INT_EN = RB_USB_IE_SETUPACT | RB_USB_IE_TRANS | RB_USB_IE_SUSPEND  |RB_USB_IE_BUSRST ;

    /* Init as device's endpoints according to the doc (p. 105) */
    /* No endpoints other than ep0 enabled */
    /* TODO: Init EP1 (or any other ep required for keyboard) */
    R8_UEP4_1_MOD = 0;
    R8_UEP2_3_MOD = 0;
    R8_UEP5_6_MOD = 0;
    R8_UEP7_MOD   = 0;

	R16_UEP0_MAX_LEN = 64;
	R32_UEP0_RT_DMA = (uint32_t)(uint8_t *)endp0RTbuff;

	R16_UEP0_T_LEN = 0;
	R8_UEP0_TX_CTRL = 0;
	R8_UEP0_RX_CTRL = 0;




#if DEBUG
    cprintf("Main done!\n");
#endif

    while (1) { }
}


/*******************************************************************************
 * @fn     USBSS_IRQHandler
 *
 * @brief  USB3.0 Interrupt Handler.
 *
 * @return None
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void
USBHS_IRQHandler(void)
{
#if DEBUG
    cprintf("Called:\tUSBHS_IRQHandler()\n");
#endif

    static uint16_t bytesToWrite = 0;
    static uint8_t *pDataToWrite = NULL;

    uint8_t int_flag;
    int_flag = R8_USB_INT_FG;

    if (int_flag & RB_USB_IF_ISOACT)
	{
        /* Not enabled in R8_USB_INT_EN, should never trigger. */
#if DEBUG
    cprintf("Interrupt flag:\tRB_USB_IF_ISOACT\n");
#endif
    }
    else if (int_flag & RB_USB_IF_SETUOACT) // Setup interrupt.
	{
#if DEBUG
    cprintf("Interrupt flag:\tRB_USB_IF_SETUOACT\n");
#endif
		vuint8_t SetupReqType = UsbSetupBuf->bRequestType;
		vuint8_t SetupReq = UsbSetupBuf->bRequest;
		vuint16_t SetupReqLen = UsbSetupBuf->wLength; // Data length.

        if ((SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
        {
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
            R8_USB_DEV_AD = UsbSetupBuf->wValue.bw.bb1;
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
                    uint8_t i = UsbSetupBuf->wValue.bw.bb1;
                    if (i > 0 && i < (sizeof(string_descriptors)/sizeof(string_descriptors[0])))
                        pDataToWrite = (uint8_t *)string_descriptors[i];
                        bytesToWrite = sizeof(string_descriptors[i]);
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
            case USB_DESCR_TYP_QUALIF:
                break;
            case USB_DESCR_TYP_SPEED:
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
        }

        uint16_t bytesToWriteForCurrentTransaction = bytesToWrite;
        if (bytesToWriteForCurrentTransaction >= U20_MAXPACKET_LEN) {
            bytesToWriteForCurrentTransaction = U20_MAXPACKET_LEN;
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
#if DEBUG
    cprintf("Interrupt flag:\tRB_USB_IF_FIFOOV\n");
#endif
        /* Not enabled in R8_USB_INT_EN, should never trigger. */
    }
    else if (int_flag & RB_USB_IF_HST_SOF)
	{
#if DEBUG
    cprintf("Interrupt flag:\tRB_USB_IF_HST_SOF\n");
#endif
        /* Not enabled in R8_USB_INT_EN, should never trigger. */
    }
    else if (int_flag & RB_USB_IF_SUSPEND)
	{
#if DEBUG
    cprintf("Interrupt flag:\TRB_USB_IF_SUSPEND\n");
#endif
    }
    else if (int_flag & RB_USB_IF_TRANSFER)
	{
#if DEBUG
    cprintf("Interrupt flag:\tRB_USB_IF_TRANSFER\n");
#endif
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
                if (bytesToWriteForCurrentTransaction >= U20_MAXPACKET_LEN) {
                    bytesToWriteForCurrentTransaction = U20_MAXPACKET_LEN;
                }
                bytesToWrite -= bytesToWriteForCurrentTransaction;

                if (pDataToWrite && bytesToWriteForCurrentTransaction > 0) {
                    if (UsbSetupBuf->bRequestType & 0x80) { /* IN Transaction. */
                        memcpy(endp0RTbuff, pDataToWrite, bytesToWriteForCurrentTransaction);
                        pDataToWrite += bytesToWriteForCurrentTransaction;
                    }
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
            break;
        }

        R8_USB_INT_FG = RB_USB_IF_TRANSFER; // Clear int flag
    }
    else if (int_flag & RB_USB_IF_BUSRST)
	{
#if DEBUG
    cprintf("Interrupt flag:\tRB_USB_IF_BUSRST\n");
#endif
        /* Init as device according to the doc (p. 105) */
        R8_USB_CTRL = 0;
        R8_USB_DEV_AD = 0;
        R8_USB_INT_FG = 0;

        R8_USB_CTRL &= ~RB_USB_HOST_MODE;   // Not useful as the field is already 0.
        R8_USB_CTRL &= ~(RB_USB_RESET_SIE | RB_USB_CLR_ALL);   // Not useful as the field is already 0.

        R8_USB_CTRL |= (RB_USB_INT_BUSY | RB_USB_DMA_EN);
        R8_USB_CTRL |= UCST_HS;
        R8_USB_CTRL |= RB_DEV_PU_EN;

        R8_USB_INT_EN = RB_USB_IE_SETUPACT | RB_USB_IE_TRANS | RB_USB_IE_SUSPEND  |RB_USB_IE_BUSRST ;

        /* Init as device's endpoints according to the doc (p. 105) */
        /* No endpoints other than ep0 enabled */
        /* TODO: Init EP1 (or any other ep required for keyboard) */
        R8_UEP4_1_MOD = 0;
        R8_UEP2_3_MOD = 0;
        R8_UEP5_6_MOD = 0;
        R8_UEP7_MOD   = 0;

        R16_UEP0_MAX_LEN = 64;
        R32_UEP0_RT_DMA = (uint32_t)(uint8_t *)endp0RTbuff;

        R16_UEP0_T_LEN = 0;
        R8_UEP0_TX_CTRL = 0;
        R8_UEP0_RX_CTRL = 0;
    }
}

