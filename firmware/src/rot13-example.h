#ifndef ROT13_EXAMPLE_H
#define ROT13_EXAMPLE_H

#include <stdbool.h>
#include <stdint.h>

/* variables */
extern bool g_top_receivedUsbPacket;
extern bool g_bottom_receivedHspiPacket;
extern bool g_top_receivedSerdes;
extern bool g_top_readyToTransmitUsbPacket;

/* functions declaration */
void rot13(uint8_t *buffer, uint16_t sizeBuffer);

#endif /* ROT13_EXAMPLE_H */
