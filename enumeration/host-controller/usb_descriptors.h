#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H


/* enums */
struct Device_t {
    char *s_name;
    unsigned char *descriptorDevice;
    unsigned char *descriptorConfig;
};

/* variables */

char _vendorName[] = "Vendor specific";

unsigned char _vendorDescriptorDevice[] = {
    18,     // bLength
    1,      // bDescriptorType
    0x00,   // bcdUSB (low)
    0x02,   // bcdUSB (high)
    0x00,   // bDeviceClass (Defined in the interface descriptor)
    0x00,   // bDeviceSubClass
    0x00,   // bDeviceProtocol
    64,     // bMaxPacketSize0
    0x34,   // idVendor (low)
    0x12,   // idVendor (high)
    0xCD,   // idProduct (low)
    0xAB,   // idProduct (high)
    0x00,   // bcdDevice (low)
    0x42,   // bcdDevice (high)
    0x00,   // iManufacturer
    0x00,   // iProduct
    0x00,   // iSerialNumber
    1,      // bNumConfigurations
};

unsigned char _vendorDescriptorConfig[] = {
    //  Descriptor Config
	0x09, // bLength
	0x02, // bDescriptorType
	0x19, // wTotalLengthL
	0x00, // wTotalLengthH
	0x01, // bNumInterfaces
	0x01, // bConfigurationValue
	0x00, // iConfiguration
	0x80, // bmAttributes
	0x64, // MaxPower
    //  Descriptor Interace
	0x09, // bLength
	0x04, // bDescriptorType
	0x00, // bInterfaceNumber
	0x00, // bAlternateSetting
	0x01, // bNumEndpoint
	0xFF, // bInterfaceClass
	0xFF, // bInterfaceSubClass
	0xFF, // bInterfaceProtocol
	0x00, // iInterface
    //  Descriptor Endpoint
	0x07, // bLength
	0x05, // bDescriptorType
	0x01, // bEndpointAddress (OUT)
	0x02, // bmAttributes
	0x00, // wMaxPacketSizeL
	0x02, // wMaxPacketSizeH
	0x00, // bInterval
};

struct Device_t g_deviceVendor = { _vendorName, _vendorDescriptorDevice, _vendorDescriptorConfig };

#endif /* USB_DESCRIPTORS_H */
