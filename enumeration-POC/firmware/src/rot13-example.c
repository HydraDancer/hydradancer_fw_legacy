#include "rot13-example.h"

/* variables */
bool g_top_receivedUsbPacket = false;
bool g_bottom_receivedHspiPacket = false;
bool g_top_receivedSerdes = false;
bool g_top_readyToTransmitUsbPacket = false;

/* functions implementation */
void
rot13(uint8_t *buffer, uint16_t sizeBuffer)
{
    for (uint16_t i = 0; i < sizeBuffer; ++i) {
        if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            buffer[i] += 13;
            if (buffer[i] > 'Z') {
                buffer[i] -= 26;
            }
        }
        if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            buffer[i] += 13;
            if (buffer[i] > 'z') {
                buffer[i] -= 26;
            }
        }
    }
}

