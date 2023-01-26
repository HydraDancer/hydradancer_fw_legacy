#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

unsigned char g_descriptorDevice[] = {
    18,     // bLength
    1,      // bDescriptorType
    0x00,   // bcdUSB (low)
    0x20,   // bcdUSB (high)
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

#endif /* USB_DESCRIPTORS_H */
