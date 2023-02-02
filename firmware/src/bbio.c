#include <string.h>

#include "log.h"
#include "usb20.h"

#include "bbio.h"

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
static uint8_t *_descriptorsStoreCursor;

// TODO: Use structs instead of a variable for the array and a variable for the
// size
uint8_t *g_descriptorDevice;
uint8_t *g_descriptorConfiguration;
uint8_t *g_descriptorsString[_DESCRIPTOR_STRING_CAPACITY];

uint16_t g_descriptorDeviceSize;
uint16_t g_descriptorConfigurationSize;
uint16_t g_descriptorsStringSizes[_DESCRIPTOR_STRING_CAPACITY];

/* internal variables */
// Internals variables used to share data between bbio_decode_command and
// bbio_handle_command

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
bbio_decode_command(uint8_t *command)
{
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

    if (command[0] == BbioMainMode) {
        // Not implemented yet
    } else if (command[0] == BbioIdentifMode) {
        // Not implemented yet
    } else if (command[0] == BbioSetDescr) {
        // Decode the sub command
        if (command[1] == BbioSubSetDescrDevice) {
            // Device descriptor
            _subCommand = 1;
        }
        else if (command[1] == BbioSubSetDescrConfig) {
            // Config descriptor
            _subCommand = 2;
        }
        else if (command[1] == BbioSubSetDescrString) {
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

    } else {
        log_to_evaluator("ERROR: bbio_decode_command() unknown command\r\n");
        return;
    }
}

void
bbio_handle_command(uint8_t *bufferData)
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
        g_descriptorDevice     = _descriptorsStoreCursor;
        g_descriptorDeviceSize = _descrSize;
    }
    else if (_subCommand == 2) { 
        // Config descriptor
        g_descriptorConfiguration     = _descriptorsStoreCursor;
        g_descriptorConfigurationSize = _descrSize;
    }
    else if (_subCommand == 3) { 
        // String descriptor
        g_descriptorsString[_descrStringIndex]      = _descriptorsStoreCursor;
        g_descriptorsStringSizes[_descrStringIndex] = _descrSize;
    } else {
        log_to_evaluator("ERROR: bbio_handle_command() unknown sub command\r\n");
        return;
    }

    memcpy(_descriptorsStoreCursor, bufferData, _descrSize);
    _descriptorsStoreCursor += _descrSize;
}

