#include <stdbool.h>
#include <string.h>

#include "log.h"
#include "usb20.h"

#include "bbio.h"


/* macros */
#define _DESCRIPTOR_STORE_CAPACITY (4096)
#define _DESCRIPTOR_STRING_CAPACITY (10)

/* variables */

bool g_doesToeSupportCurrentDevice = false;

// TODO: Use structs instead of a variable for the array and a variable for the
// size, is it worth ?
uint8_t *g_bbioDescriptorDevice;
uint8_t *g_bbioDescriptorConfiguration;
uint8_t *g_bbioDescriptorsString[_DESCRIPTOR_STRING_CAPACITY];

uint16_t g_bbioDescriptorDeviceSize;
uint16_t g_bbioDescriptorConfigurationSize;
uint16_t g_bbioDescriptorsStringSizes[_DESCRIPTOR_STRING_CAPACITY];

/* internal variables */
static uint8_t _command          = 0;   // 0 : If not set, else the value of the enum BbioCommand
static uint8_t _subCommand       = 0;   // 0 : If not set, else the value of the enum BbioSubCommand
static uint8_t _descrStringIndex = 0;
static uint16_t _descrSize       = 0;

/* _descriptorsStore is our "free store", it is a memory pool dedicated to
 * descriptors the user will load
 * In the future we will have a dedicated memory allocator to manage this free
 * store
 * This free store is always hidden to the user, hence the underscore as a
 * prefix
 */
static uint8_t _descriptorsStore[_DESCRIPTOR_STORE_CAPACITY];
static uint8_t *_descriptorsStoreCursor = _descriptorsStore;


/* functions implementation */

/* @fn      bbio_command_decode
 *
 * @brief   Store the commands for the next call to bbio_command_handle
 *
 * @return  0 if Success, an error code else
 */
uint8_t
bbio_command_decode(uint8_t *command)
{
    // log_to_evaluator("bbio_command_decode()\r\n");
   /* Reminder of the structure of a bbio command :
    * command[0] = BbioCommand
    * command[1] = BbioSubCommand                   Valid only when BbioCommand = BbioSetDescr
    * command[2] = Index of the given descriptor    Valid only when BbioCommand = BbioSetDescr
    * command[3] = Size of descriptor (L)           Valid only when BbioCommand = BbioSetDescr
    * command[4] = Size of descriptor (H)           Valid only when BbioCommand = BbioSetDescr
    */
    // Reset internal variables.
    _command = 0;
    _subCommand = 0;
    _descrStringIndex = 0;
    _descrSize = 0;

        // Safeguard
    if (command[0] >= BbioMainMode && command[0] <= BbioResetDescr) {
        _command = command[0];
    } else {
        log_to_evaluator("ERROR: bbio_decode_command() unknown command\r\n");
        return 1;
    }

    if (_command == BbioSetDescr) {
        // Safeguard
        if (command[1] >= BbioSubSetDescrDevice && command[1] <= BbioSubSetDescrString) {
            _subCommand = command[1];
        } else {
            log_to_evaluator("ERROR: bbio_decode_command() unknown sub command\r\n");
            return 2;
        }
    }

    if (_subCommand == BbioSubSetDescrString) {
        // Safeguard
        if (command[2] >= _DESCRIPTOR_STRING_CAPACITY) {
            log_to_evaluator("ERROR: bbio_decode_command() string descriptor index out of range\r\n");
            return 3;
        }
        _descrStringIndex = command[2];
    }

    _descrSize = (command[4] << 8) | command[3]; // from 2 uint8_t to a uint16_t
    return 0;
}

/* @fn      bbio_command_handle
 *
 * @brief   Treat the command received at the previous call of
 *          bbio_command_decode
 *
 * @return  0 if Success, an error code else
 */
uint8_t
bbio_command_handle(uint8_t *bufferData)
{
    // log_to_evaluator("bbio_command_handle()\r\n");
    switch (_command) {
    case BbioMainMode:
        /* Not implemented yet */
        return 1;
    case BbioIdentifMode:
        /* Not implemented yet */
        return 2;
    case BbioSetDescr:
        bbio_command_set_descriptor_handle(bufferData);
        return 0;
    case BbioSetEndp:
        bbio_command_set_endpoints_handle(bufferData);
        return 0;
    case BbioConnect:
        g_descriptorConfigCustomSize = g_bbioDescriptorConfigurationSize;
        g_usb20Speed = SpeedHigh;

        // Filling structures "describing" our USB peripheral
        g_descriptorDevice  = g_bbioDescriptorDevice;
        g_descriptorConfig  = g_bbioDescriptorConfiguration;
        g_descriptorStrings = g_bbioDescriptorsString;

        g_doesToeSupportCurrentDevice = false;  // Reset the value
        usb20_registers_init(g_usb20Speed);
        return 0;
    case BbioGetStatus:
        return g_doesToeSupportCurrentDevice;
    case BbioDisconnect:
        g_doesToeSupportCurrentDevice = false;
        usb20_registers_deinit();
        return 0;
    case BbioResetDescr:
        g_descriptorDevice  = NULL;
        g_descriptorConfig  = NULL;
        g_descriptorStrings = NULL;

        g_bbioDescriptorDevice = NULL;
        g_bbioDescriptorConfiguration = NULL;
        for (uint8_t i = 0; i < _DESCRIPTOR_STRING_CAPACITY; ++i) {
            g_bbioDescriptorsString[i] = NULL;
        }

        memset(_descriptorsStore, 0, _DESCRIPTOR_STORE_CAPACITY);
        return 0;
    default:
        log_to_evaluator("ERROR: bbio_command_handle() unknown command\r\n");
        return 3;
    }
}

/* @fn      bbio_command_set_descriptor_handle
 *
 * @brief   Set the descriptor received
 *
 * @return  0 if Success, an error code else
 */
uint8_t
bbio_command_set_descriptor_handle(uint8_t *bufferData)
{
    // log_to_evaluator("bbio_command_set_descriptor_handle()\r\n");
    // Safeguards
    if (_descriptorsStoreCursor + _descrSize > _descriptorsStore + _DESCRIPTOR_STORE_CAPACITY) {
        log_to_evaluator("ERROR: bbio_handle_command() No space left in the descriptor store\r\n");
        return 1;
    }
    if (_descrStringIndex >= _DESCRIPTOR_STRING_CAPACITY) {
        log_to_evaluator("ERROR: bbio_handle_command() string descriptor index out of range\r\n");
        return 2;
    }

    if (_subCommand == BbioSubSetDescrDevice) { 
        // Device descriptor
        g_bbioDescriptorDevice     = _descriptorsStoreCursor;
        g_bbioDescriptorDeviceSize = _descrSize;
    }
    else if (_subCommand == BbioSubSetDescrConfig) { 
        // Config descriptor
        g_bbioDescriptorConfiguration     = _descriptorsStoreCursor;
        g_bbioDescriptorConfigurationSize = _descrSize;
    }
    else if (_subCommand == BbioSubSetDescrString) { 
        // String descriptor
        g_bbioDescriptorsString[_descrStringIndex]      = _descriptorsStoreCursor;
        g_bbioDescriptorsStringSizes[_descrStringIndex] = _descrSize;
    } else {
        log_to_evaluator("ERROR: bbio_handle_command() unknown sub command\r\n");
        return 3;
    }

    memcpy(_descriptorsStoreCursor, bufferData, _descrSize);
    _descriptorsStoreCursor += _descrSize;

    return 0;
}

/* @fn      bbio_command_set_endpoints_handle
 *
 * @brief   Set the descriptor received
 *
 * @return  0 if Success, an error code else
 */
uint8_t
bbio_command_set_endpoints_handle(uint8_t *bufferEndpoints)
{
    // log_to_evaluator("bbio_command_set_endpoints_handle()\r\n");
    // This function will enable endpoints with the right mode
    // (isochronous/bulk/interrupt)
    // The data is encoded in 1 byte :
    // 0b00yy Xxxx
    // yy correspond to the mode : (Not used as of now)
    // 01: isochronous
    // 10: bulk
    // 11: interrupt
    //
    // Xxxx correspond to the endpoint number
    // X: 0 for OUT, 1 for IN
    // xxx: the endpoint number (from 1 to 7)
    enum Endpoint endpointsIn  = 0;
    enum Endpoint endpointsOut = 0;

    while (*bufferEndpoints != 0) {
        if (*bufferEndpoints & 0b00001000) { // If IN
            switch (*bufferEndpoints & 0b00000111) {
            case 1:
                endpointsIn |= Ep1Mask;
                break;
            case 2:
                endpointsIn |= Ep2Mask;
                break;
            case 3:
                endpointsIn |= Ep3Mask;
                break;
            case 4:
                endpointsIn |= Ep4Mask;
                break;
            case 5:
                endpointsIn |= Ep5Mask;
                break;
            case 6:
                endpointsIn |= Ep6Mask;
                break;
            case 7:
                endpointsIn |= Ep7Mask;
                break;
            }
        } else { // Else OUT
            switch (*bufferEndpoints & 0b00000111) {
            case 1:
                endpointsOut |= Ep1Mask;
                break;
            case 2:
                endpointsOut |= Ep2Mask;
                break;
            case 3:
                endpointsOut |= Ep3Mask;
                break;
            case 4:
                endpointsOut |= Ep4Mask;
                break;
            case 5:
                endpointsOut |= Ep5Mask;
                break;
            case 6:
                endpointsOut |= Ep6Mask;
                break;
            case 7:
                endpointsOut |= Ep7Mask;
                break;
            }
        }

        // epilog
        ++bufferEndpoints;
    }

    usb20_endpoints_init(endpointsIn, endpointsOut);

    return 0;
}

