#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include <stddef.h>


/* enums */
struct Device_t {
    char *s_name;
    unsigned char *descriptorDevice;
    unsigned char *descriptorConfig;
    unsigned char *descriptorHidReport;
};

/* variables */
extern struct Device_t g_deviceGeneric;
extern struct Device_t g_deviceAudio;
extern struct Device_t g_deviceCdc;
extern struct Device_t g_deviceKeyboard;
extern struct Device_t g_deviceImage;
extern struct Device_t g_devicePrinter;
extern struct Device_t g_deviceMassStorage;
extern struct Device_t g_deviceSmartCard;
extern struct Device_t g_devicePersonalHealthcare;
extern struct Device_t g_deviceVideo;
extern struct Device_t g_deviceDFU;
extern struct Device_t g_deviceFTDI;

extern struct Device_t *g_devices[];

#endif /* USB_DESCRIPTORS_H */
