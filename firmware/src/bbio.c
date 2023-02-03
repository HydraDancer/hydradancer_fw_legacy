#include <string.h>

#include "log.h"
#include "usb20.h"

#include "bbio.h"

// TODOOO: Add enable endpoints feature
// TODOOO: Add reset feature
// TODO: Use return value to indicate termination state (0 success or error code)


/* macros */
#define _DESCRIPTOR_STORE_SIZE (4096)
#define _DESCRIPTOR_STRING_CAPACITY (10)

/* variables */

/* _descriptorsStore is our "free store", it is a memory pool dedicated to
 * descriptors the user will load.
 * In the future we will have a dedicated memory allocator to manage this free
 * store.
 * This free store is always hidden to the user, hence the underscore as a
 * prefix.
 */
static uint8_t _descriptorsStore[_DESCRIPTOR_STORE_SIZE];
static uint8_t *_descriptorsStoreCursor = _descriptorsStore;

// TODO: Use structs instead of a variable for the array and a variable for the
// size
uint8_t *g_bbioDescriptorDevice;
uint8_t *g_bbioDescriptorConfiguration;
uint8_t *g_bbioDescriptorsString[_DESCRIPTOR_STRING_CAPACITY];

uint16_t g_bbioDescriptorDeviceSize;
uint16_t g_bbioDescriptorConfigurationSize;
uint16_t g_bbioDescriptorsStringSizes[_DESCRIPTOR_STRING_CAPACITY];

/* internal variables */
// Internals variables used to share data between bbio_decode_command and
// bbio_handle_command

// Shares the command
// 0 : Not set
// 1 : Set descriptor
// 2 : Set endpoint
// 3 : Connect
static uint8_t _command = 0;

// Shares the sub command
// 0 : Not set
// 1 : Device
// 2 : Config
// 3 : String
static uint8_t _subCommand = 0;

// Shares the index of the string descriptor to set
static uint8_t _descrStringIndex = 0;

// Shares the size of the incomming descriptor
static uint16_t _descrSize = 0;

/* functions implementation */
void
bbio_command_decode(uint8_t *command)
{
    log_to_evaluator("bbio_command_decode()\r\n");
   /* Reminder of the structure of a bbio command :
    * command[0] = BbioCommand
    * command[1] = BbioSubCommand                   Valid only when BbioCommand = BbioSetDescr
    * command[2] = Index of the given descriptor    Valid only when BbioCommand = BbioSetDescr
    * command[3] = Size of descriptor (L)           Valid only when BbioCommand = BbioSetDescr
    * command[4] = Size of descriptor (H)           Valid only when BbioCommand = BbioSetDescr
    */
    // Reset internal variables.
    _subCommand = 0;
    _descrStringIndex = 0;
    _descrSize = 0;

    // TODO: Replace if/else with a switch/case for readability
    if (command[0] == BbioMainMode) {
        // Not implemented yet
    } else if (command[0] == BbioIdentifMode) {
        // Not implemented yet
    } else if (command[0] == BbioSetDescr) {
        // Decode the sub command
        _command = 1;
        if (command[1] == BbioSubSetDescrDevice) {
            // Device descriptor
            _subCommand = 1;
        } else if (command[1] == BbioSubSetDescrConfig) {
            // Config descriptor
            _subCommand = 2;
        } else if (command[1] == BbioSubSetDescrString) {
            // String descriptor
            _subCommand = 3;
            if (command[2] >= _DESCRIPTOR_STRING_CAPACITY) {
                log_to_evaluator("ERROR: bbio_decode_command() string descriptor index out of range\r\n");
                return;
            }
            _descrStringIndex = command[2];
        } else {
            log_to_evaluator("ERROR: bbio_decode_command() unknown sub command\r\n");
            return;
        }
        _descrSize = (command[4] << 8) | command[3];

    } else if (command[0] == BbioSetEndp) {
        // Endpoints configuration
        _command = 2;
    } else if (command[0] == BbioConnect) {
        // Connect the USB device
        _command = 3;
    } else {
        log_to_evaluator("ERROR: bbio_decode_command() unknown command\r\n");
        return;
    }
}

// Return 0 if success, 1 else
uint8_t
bbio_command_handle(uint8_t *bufferData)
{
    log_to_evaluator("bbio_command_handle()\r\n");
    switch (_command) {
        case 0:
            return 1;
        case 1: // Set descriptor
            bbio_sub_command_handle(bufferData);
            return 0;
        case 2: // Set endpoints
            bbio_command_endpoints_handle(bufferData);
            return 0;
        case 3: // Connect
            {
                g_descriptorConfigCustomSize = g_bbioDescriptorConfigurationSize;
                speed = SpeedHigh;

                // TODO: Currently hardcoded, handle it properly

                // Filling structures "describing" our USB peripheral.
                g_descriptorDevice  = g_descriptorDevice;
                g_descriptorConfig  = g_descriptorConfig;
                g_descriptorStrings = g_descriptorStrings;

                U20_registers_init(speed);
                U20_endpoints_init(Ep1Mask, 0);
            }
            return 0;
    }
}

void
bbio_sub_command_handle(uint8_t *bufferData)
{
    // Safeguards
    if (_descriptorsStoreCursor + _descrSize > _descriptorsStore + _DESCRIPTOR_STORE_SIZE) {
        log_to_evaluator("ERROR: bbio_handle_command() No space left inf the descriptor store\r\n");
        return;
    }
    if (_descrStringIndex >= _DESCRIPTOR_STRING_CAPACITY) {
        log_to_evaluator("ERROR: bbio_handle_command() string descriptor index out of range\r\n");
        return;
    }

    if (_subCommand == 1) { 
        // Device descriptor
        g_bbioDescriptorDevice     = _descriptorsStoreCursor;
        g_bbioDescriptorDeviceSize = _descrSize;
    }
    else if (_subCommand == 2) { 
        // Config descriptor
        g_bbioDescriptorConfiguration     = _descriptorsStoreCursor;
        g_bbioDescriptorConfigurationSize = _descrSize;
    }
    else if (_subCommand == 3) { 
        // String descriptor
        g_bbioDescriptorsString[_descrStringIndex]      = _descriptorsStoreCursor;
        g_bbioDescriptorsStringSizes[_descrStringIndex] = _descrSize;
    } else {
        log_to_evaluator("ERROR: bbio_handle_command() unknown sub command\r\n");
        return;
    }

    memcpy(_descriptorsStoreCursor, bufferData, _descrSize);
    _descriptorsStoreCursor += _descrSize;
}

// This function will enable endpoints with the right mode
// (isochronous/bulk/interrupt)
// The data is encoded in 1 byte :
// 0b00yy Xxxx
// yy correspond to the mode :
// 01: isochronous
// 10: bulk
// 11: interrupt
//
// Xxxx correspond to the endpoint number
// X: 0 for OUT, 1 for IN
// xxx: the endpoint number (from 1 to 7)
void
bbio_command_endpoints_handle(uint8_t *bufferEndpoints)
{
    while (*bufferEndpoints != 0) {
        // TODO: Populate

        // epilog
        ++bufferEndpoints;
    }
}
