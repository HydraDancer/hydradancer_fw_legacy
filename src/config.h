#ifndef CONFIG_H
#define CONFIG_H

/* variables */
static USB_DEV_DESCR stBoardTopDeviceDescriptor = {
    .bLength = sizeof(USB_DEV_DESCR),
    .bDescriptorType = USB_DESCR_TYP_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,   /* Defined in the interface descriptor. */
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

static USB_CFG_DESCR_FULL_2_ENDPOINTS stBoardTopConfigurationDescriptor = {
    .cfgDescr = {
        .bLength = sizeof(USB_CFG_DESCR),
        .bDescriptorType = USB_DESCR_TYP_CONFIG,
        .wTotalLength = sizeof(USB_CFG_DESCR_FULL_2_ENDPOINTS),
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
        .bNumEndpoints = 2,
        .bInterfaceClass = USB_DEV_CLASS_VEN_SPEC,
        .bInterfaceSubClass = 0xff,
        .bInterfaceProtocol = 0xff,
        .iInterface = 5,
    },
    .endpDescr1In = {
        .bLength = sizeof(USB_ENDP_DESCR),
        .bDescriptorType = USB_DESCR_TYP_ENDP,
        .bEndpointAddress = 0x81,               /* In endpoint (MSB set to 1). */
        .bmAttributes = USB_ENDP_TYPE_BULK,     /* Transfer type. */
        .wMaxPacketSizeL = 8,                   /* Packet size as defined by the spec for USB LS. */
        .wMaxPacketSizeH = 0,
        .bInterval = 0,                         /* Polling interval, 1 for isochronous, else 0. */
    },
    .endpDescr1Out = {
        .bLength = sizeof(USB_ENDP_DESCR),
        .bDescriptorType = USB_DESCR_TYP_ENDP,
        .bEndpointAddress = 0x01,               /* In endpoint (MSB set to 1). */
        .bmAttributes = USB_ENDP_TYPE_BULK,     /* Transfer type. */
        .wMaxPacketSizeL = 8,                   /* Packet size as defined by the spec for USB LS. */
        .wMaxPacketSizeH = 0,
        .bInterval = 0,                         /* Polling interval, 1 for isochronous, else 0. */
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
    16,                     /* .bLength */
    USB_DESCR_TYP_STRING,   /* .bDescriptorType */
    'P', 0x0,
    'r', 0x0,
    'o', 0x0,
    'd', 0x0,
    'u', 0x0,
    'c', 0x0,
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

static uint8_t *boardTopStringDescriptors[] = {
    stringLangID,
    stringDescriptorManufacturer,
    stringDescriptorProduct,
    stringDescriptorSerialNumber,
    stringDescriptorConfig,
    stringDescriptorInterface,
    NULL /* Must be NULL-terminated as we need to determine its length. */
};

#endif /* CONFIG_H */

