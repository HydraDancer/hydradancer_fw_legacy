#ifndef CONFIG_H
#define CONFIG_H

/* variables */
static USB_DEV_DESCR stKeyboardDeviceDescriptor = {
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

static uint8_t keyboardReportDescriptor[] = {
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
	0x81, 0x00,		// Input (Data, Array)   ; Key arrays (4 bytes)
	0xC0,		    // End Collection
};

static USB_CFG_DESCR_FULL_HID stKeyboardConfigurationDescriptor = {
    .cfgDescr = {
        .bLength = sizeof(USB_CFG_DESCR),
        .bDescriptorType = USB_DESCR_TYP_CONFIG,
        .wTotalLength = sizeof(USB_CFG_DESCR_FULL_HID),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 4,
        .bmAttributes = 0x80,
        .MaxPower = 0x64,
    },
    .itfDescr = {
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
    .hidDescr = {
        /* See https://www.usb.org/sites/default/files/hid1_11.pdf (p. 22). */
        .bLength = sizeof(USB_HID_DESCR),
        .bDescriptorType = USB_DESCR_TYP_HID,
        .bcdHIDL = 0x11,
        .bcdHIDH = 0x01,
        .bCountryCode = 8,
        .bNumDescriptors = 1,
        .bDescriptorTypeX = USB_DESCR_TYP_REPORT,
        .wDescriptorLengthL = sizeof(keyboardReportDescriptor), /* TODO: Only works when size is less than 2**8 ! */
        .wDescriptorLengthH = 0,
    },
    .endpDescr = {
        .bLength = sizeof(USB_ENDP_DESCR),
        .bDescriptorType = USB_DESCR_TYP_ENDP,
        .bEndpointAddress = 0x81,               /* In endpoint (MSB set to 1). */
        .bmAttributes = USB_ENDP_TYPE_INTER,    /* Transfer type. */
        .wMaxPacketSizeL = 8,
        .wMaxPacketSizeH = 0,
        .bInterval = 1,                         /* Polling interval, 1 for isochronous, else 0. */
    },
};

static uint8_t stringLangID[] =
{
	0x04, /* length of this descriptor */
	0x03, /* USB Descriptor Type String */
	0x09, /* Language ID 0 low byte */
	0x04  /* Language ID 0 high byte */
};

static uint8_t stringDescriptorManufacturer[] = {
    26,                     /* .bLength */
    USB_DESCR_TYP_STRING,   /* .bDescriptorType */
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

static uint8_t stringDescriptorProduct[] = {
    18,                     /* .bLength */
    USB_DESCR_TYP_STRING,   /* .bDescriptorType */
    'P', 0x0,
    'r', 0x0,
    'o', 0x0,
    'd', 0x0,
    'u', 0x0,
    'c', 0x0,
    't', 0x0,
    't', 0x0,
};

static uint8_t stringDescriptorSerialNumber[] = {
    28,                     /* .bLength */
    USB_DESCR_TYP_STRING,   /* .bDescriptorType */
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


static uint8_t stringDescriptorConfig[] = {
    14,                     /* .bLength */
    USB_DESCR_TYP_STRING,   /* .bDescriptorType */
    'C', 0x0,
    'o', 0x0,
    'n', 0x0,
    'f', 0x0,
    'i', 0x0,
    'g', 0x0,
};

static uint8_t stringDescriptorInterface[] = {
    20,                     /* .bLength */
    USB_DESCR_TYP_STRING,   /* .bDescriptorType */
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

static uint8_t *keyboardStringDescriptors[] = {
    stringLangID,
    stringDescriptorManufacturer,
    stringDescriptorProduct,
    stringDescriptorSerialNumber,
    stringDescriptorConfig,
    stringDescriptorInterface,
    NULL /* Must be NULL-terminated as we need to determine its length. */
};

#endif /* CONFIG_H */

