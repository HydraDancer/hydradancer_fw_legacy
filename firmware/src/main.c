/*
 * Small description of the project.
 */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"
#include "CH56x_debug_log.h"

#include "hspi.h"
#include "serdes.h"
#include "usb20.h"

// TODOOOOO: move logging to ep7
// TODOOOO: Add prefix for logging with usb_log()

// TODOOO: Add Halt support for endpoints (get_status()).
// TODOOO: Prefix all global Variables with g_
// TODOO: Add clock for debug (PFIC_Enable(SysTick) ?).
// TODOO: Add debgu over UART. Still useful ?
// TODOO: Add doxygen for every function.
// TODO: Homogenize var name to camelCase.
// TODO: Homogenize var comments.
// TODO: Add defaults to switches.
// TODO: Add idle state.

/* variables */
static bool g_isHost = false;
uint8_t HSPI_WORKARROUND = false;

static bool top_receivedUsbPacket = false;
static bool bottom_receivedHspiPacket = false;
static bool top_receivedSerdes = false;
static bool top_readyToTransmitUsbPacket = false;

uint16_t sizeEndp1Buff = 0;
const uint16_t capacityEndp1Buff = 4096;
__attribute__((aligned(16))) uint8_t endp1BuffRaw[4096];
uint8_t *endp1Buff = endp1BuffRaw;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int
main(void)
{
    bsp_gpio_init();
    bsp_init(FREQ_SYS);
    UART1_init(115200, FREQ_SYS);

    cfgDescrType = CfgDescr2EpDebug;
    speed = SpeedHigh;
    epMask = Ep1Mask | Ep7Mask;
    endpoint_clear(0x81);
    endpoint_clear(0x01);
    endpoint_clear(0x87);

    stDeviceDescriptor                     = stBoardTopDeviceDescriptor;
    stConfigurationDescriptor.base2EpDebug = stBoardTopConfigurationDescriptor;
    stInterfaceDescriptor                  = stBoardTopConfigurationDescriptor.itfDescr;
    stringDescriptors                      = boardTopStringDescriptors;

    /* Board sync. */
    int retCode;
    if (bsp_switch()) {
        g_isHost = true;
        usb_log("[TOP BOARD] Hello!\r\n");
        retCode = bsp_sync2boards(PA14, PA12, BSP_BOARD1);
    } else {
        stDeviceDescriptor.idProduct = 0x1338;
        g_isHost = false;
        usb_log("[BOTTOM BOARD] Hello!\r\n");
        retCode = bsp_sync2boards(PA14, PA12, BSP_BOARD2);
    }
    if (retCode) {
        usb_log("Synchronisation done (success)\r\n");
    } else {
        usb_log("Synchronisation error(timeout)\r\n");
    }


    /* USB Init. */
    U20_registers_init(speed);
    U20_endpoints_init(epMask);

    usb_log("USB init done\r\n");

    /* HSPI Init. */
    memset((void *)hspiDmaAddr0, '.', HSPI_DMA_LEN0);
    hspiDmaAddr0[HSPI_DMA_LEN0-1] = 0;
    memset((void *)hspiDmaAddr1, '.', HSPI_DMA_LEN1);
    hspiDmaAddr1[HSPI_DMA_LEN1-1] = 0;
    if (g_isHost) {
        HSPI_DoubleDMA_Init(HSPI_HOST, RB_HSPI_DAT8_MOD, (uint32_t)hspiDmaAddr0, (uint32_t)hspiDmaAddr1, HSPI_DMA_LEN);
    } else {
        HSPI_DoubleDMA_Init(HSPI_DEVICE, RB_HSPI_DAT8_MOD, (uint32_t)hspiDmaAddr0, (uint32_t)hspiDmaAddr1, 0);
    }
    usb_log("HSPI init done\r\n");

    /* SerDes Init. */
    PFIC_EnableIRQ(SERDES_IRQn);
    SerDes_EnableIT(ALL_INT_TYPE);
    if (g_isHost) {
        SerDes_Rx_Init(SDS_PLL_FREQ_1_20G);
        SerDes_DMA_Rx_CFG((uint32_t)serdesDmaAddr);
    } else {
        // SDS_TX_INT_FLG should not trigger an interrupt, it is handled by
        // SerDes_Wait_Txdone()
        SerDes_EnableIT(ALL_INT_TYPE & ~SDS_TX_INT_FLG);
        SerDes_Tx_Init(SDS_PLL_FREQ_1_20G);
        SerDes_DMA_Tx_CFG((uint32_t)serdesDmaAddr, SERDES_DMA_LEN, serdesCustomNumber);
    }
    usb_log("SerDes init done\r\n");

    usb_log("Init all done!\r\n");

    // HOW THIS EXAMLE WORKS :
    // - Host sends message over USB to top board
    // - Top board transmits that message to bottom board via HSPI
    // - Second board cypher the message received (with rot13 encryption)
    // - Second board sends the cyphered message back to top board via SerDes
    // - Top board sends the cyphered message back to host via USB

    if (g_isHost) {
        while (1) {
            // Wait to receive the message to cypher
            // See ep1_transceive_and_update() for how it is done
            usb_log("[HOST]   Waiting for the message from host (USB)\r\n");
            while (!g_top_receivedUsbPacket) { bsp_wait_ms_delay(10); }
            g_top_receivedUsbPacket = false;
            usb_log("[HOST]   Message received, transmitting via HSPI\r\n");

            // Fill and transmit data to second board via HSPI
            uint8_t *hspiBufferTx = hspi_get_buffer_next_tx();
            memcpy(hspiBufferTx, endp1Rbuff, min(HSPI_DMA_LEN, U20_UEP1_MAXSIZE));

            HSPI_DMA_Tx();

            // Wait to receive the anwser over SerDes
            usb_log("[HOST]   Waiting for SerDes response\r\n");
            while (!g_top_receivedSerdes) { bsp_wait_ms_delay(10); }
            g_top_receivedSerdes = false;
            usb_log("[HOST]   SerDes received, transmitting back to Host\r\n");

            // Prepare buffer to send back to host
            memcpy(endp1Buff, serdesDmaAddr, min(U20_UEP1_MAXSIZE, SERDES_DMA_LEN));
            sizeEndp1Buff = min(U20_UEP1_MAXSIZE, SERDES_DMA_LEN);
            g_top_readyToTransmitUsbPacket = true;

            // Wait for transmission over USB to be completede before being able
            // to cypher an other message
            while (g_top_readyToTransmitUsbPacket) { bsp_wait_ms_delay(10); }
            usb_log("[HOST]   Cyphered message sent back to host successfully\r\n");
        }
    } else {
        while (1) {
            // Wait to receive data from top board over HSPI
            usb_log("[DEVICE] Waiting for the message Host top board (HSPI)\r\n");
            while (!g_bottom_receivedHspiPacket) { bsp_wait_ms_delay(10); }
            g_bottom_receivedHspiPacket = false;
            usb_log("[DEVICE] HSPI Request received, cyphering\r\n");

            memcpy(serdesDmaAddr, hspi_get_buffer_rx(), min(SERDES_DMA_LEN, HSPI_DMA_LEN));
            // Apply ROT13 encryption over the received data
            rot13(serdesDmaAddr, SERDES_DMA_LEN);
            usb_log("[DEVICE] Cyphering done, sending back data to Host board via SerDes\r\n");

            // Send the cyphered data back to top board
            SerDes_DMA_Tx();
            SerDes_Wait_Txdone();
            usb_log("[HOST]   Cyphered message sent back to Host board successfully\r\n");
        }
    }

}


/*******************************************************************************
 * @fn     SERDES_IRQHandler
 *
 * @brief  SERDES Interrupt Handler.
 *
 * @return None
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void 
SERDES_IRQHandler(void)
{
    usb_log("SERDES IRQ Handler\r\n");
    switch (SerDes_StatusIT() & ALL_INT_TYPE) {
    case SDS_PHY_RDY_FLG:
        SerDes_ClearIT(SDS_PHY_RDY_FLG);
        break;
    // SDS_TX_INT_FLG == SDS_RX_ERR_FLG, depend if it is Tx or Rx.
    case SDS_TX_INT_FLG:
        SerDes_ClearIT(SDS_TX_INT_FLG);
        break;
    case SDS_RX_INT_FLG:
        g_top_receivedSerdes = true;
        SerDes_ClearIT(SDS_RX_INT_FLG);
        break;
    case SDS_RX_ERR_FLG | SDS_RX_INT_FLG:
        usb_log("SDS_RX_ERR_FLG | SDS_RX_INT_FLG\r\n");
        g_top_receivedSerdes = true;
        SerDes_ClearIT(SDS_RX_ERR_FLG | SDS_RX_INT_FLG);
        break;
    case SDS_FIFO_OV_FLG:
        SerDes_ClearIT(SDS_FIFO_OV_FLG);
        break;
    case SDS_COMMA_INT_FLG:
        SerDes_ClearIT(SDS_COMMA_INT_FLG);
        break;
    default:
        SerDes_ClearIT(ALL_INT_FLG);
        break;
    }
}


/*******************************************************************************
 * @fn     HSPI_IRQHandler
 *
 * @brief  HSPI Interrupt Handler.
 *
 * @return None
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void
HSPI_IRQHandler(void)
{
    uint8_t hspiRtxStatus;

    switch (R8_HSPI_INT_FLAG & HSPI_INT_FLAG) {
    case RB_HSPI_IF_T_DONE:
        hspiRtxStatus = hspi_get_rtx_status();
        if (hspiRtxStatus) {
            usb_log("[Interrupt HSPI]   Error transmitting: %s", hspiRtxStatus&RB_HSPI_CRC_ERR? "CRC_ERR" : "NUM_MIS");
        }

        // TODO: Find a cleaner solution for "acknowledgement" of the T_DONE.
        HSPI_WORKARROUND = true;
        R8_HSPI_INT_FLAG = RB_HSPI_IF_T_DONE;
        break;
    case RB_HSPI_IF_R_DONE:
        hspiRtxStatus = hspi_get_rtx_status();
        if (hspiRtxStatus) {
            usb_log("[Interrupt HSPI]   Error transmitting: %s", hspiRtxStatus&RB_HSPI_CRC_ERR? "CRC_ERR" : "NUM_MIS");
        }

        g_bottom_receivedHspiPacket = true;
        R8_HSPI_INT_FLAG = RB_HSPI_IF_R_DONE;
        break;
    case RB_HSPI_IF_FIFO_OV:
        R8_HSPI_INT_FLAG = RB_HSPI_IF_FIFO_OV;
        break;
    case RB_HSPI_IF_B_DONE:
        R8_HSPI_INT_FLAG = RB_HSPI_IF_B_DONE;
        break;
    default:
        cprintf("default\r\n");
        break;
    }
}


/*******************************************************************************
 * @fn     USBHS_IRQHandler
 *
 * @brief  USB2.0 Interrupt Handler.
 *
 * @return None
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void
USBHS_IRQHandler(void)
{
    static uint16_t bytesToWrite = 0;
    static uint8_t *pDataToWrite = NULL;

    static vuint8_t SetupReqType = 0;
    static vuint8_t SetupReq = 0;
    static vuint16_t SetupReqLen = 0;

    if (R8_USB_INT_FG & RB_USB_IF_ISOACT) {
        /* Unused. */
        R8_USB_INT_FG = RB_USB_IF_ISOACT;
    } else if (R8_USB_INT_FG & RB_USB_IF_SETUOACT) { // Setup interrupt.
        SetupReqType = UsbSetupBuf->bRequestType;
        SetupReq = UsbSetupBuf->bRequest;
        SetupReqLen = UsbSetupBuf->wLength;

        /* If bRequest != 0 it is a non standard request, thus not covered  by the spec. */
        if ((SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
            return;
        }

        switch(SetupReq) {
        case USB_GET_STATUS:
            endp0RTbuff[0] = 0x00;
            endp0RTbuff[1] = 0x00;
            bytesToWrite = 2;

            // Here should be the handling of the halt endpoint's command.
            break;
        case USB_CLEAR_FEATURE:
            switch (SetupReqType & USB_REQ_RECIP_MASK) {
            case USB_REQ_RECIP_DEVICE:
                /* Not implemented. */
                break;
            case USB_REQ_RECIP_INTERF:
                /* Not implemented. */
                break;
            case USB_REQ_RECIP_ENDP:
                endpoint_clear(UsbSetupBuf->wValue.bw.bb1);
                break;
            default:
                assert(0 && "ERROR: SETUP Interrupt USB_CLEAR_FEATURE invalid recipient");
                break;
            }
            break;
        case USB_SET_FEATURE:
            switch (SetupReqType & USB_REQ_RECIP_MASK) {
            case USB_REQ_RECIP_DEVICE:
                /* Not implemented. */
                assert(0 && "ERROR: SETUP Interrupt USB_SET_FEATURE (toward device) unimplemented");
                break;
            case USB_REQ_RECIP_INTERF:
                /* Not implemented. */
                assert(0 && "ERROR: SETUP Interrupt USB_SET_FEATURE (toward interface) unimplemented");
                break;
            case USB_REQ_RECIP_ENDP:
                switch (UsbSetupBuf->wValue.w) {
                case 0x0000: /* ENDPOINT_HALT */
                    endpoint_halt(UsbSetupBuf->wValue.bw.bb1);
                    break;
                default:
                    assert(0 && "ERROR: SETUP Interrupt USB_SET_FEATURE (toward endpoint) unimplemented");
                    break;
                }
                break;
            default:
                assert(0 && "ERROR: SETUP Interrupt USB_SET_FEATURE invalid recipient");
                break;
            }
            break;
        case USB_SET_ADDRESS:
            // NOTE: Address should not be set in this transaction but rather in the
            // following one (RB_USB_IF_TRANSFER IN).
            break;
        case USB_GET_DESCRIPTOR:
            fill_buffer_with_descriptor(UsbSetupBuf->wValue, &pDataToWrite, &bytesToWrite);
            break;
        case USB_SET_DESCRIPTOR:
            /* Unused. */
            break;
        case USB_GET_CONFIGURATION:
            /* We have only one configuration. */
            endp0RTbuff[0] = stConfigurationDescriptor.base.cfgDescr.bConfigurationValue;
            bytesToWrite = 1;
            break;
        case USB_SET_CONFIGURATION:
            /* As of now there is only one configuration. */
            break;
        case USB_GET_INTERFACE:
            /* We have only one interface. */
            endp0RTbuff[0] = stInterfaceDescriptor.bInterfaceNumber;
            bytesToWrite = 1;
            break;
        case USB_SET_INTERFACE:
            /* As of now there is only one interface. */
            break;
        case USB_SYNCH_FRAME:
            break;
        default:
            break;
        }

        if (SetupReqLen < bytesToWrite) {
            bytesToWrite = SetupReqLen;
        }

        uint8_t uisToken = UIS_TOKEN_OUT;
        if (SetupReqType & 0x80) {
            uisToken = UIS_TOKEN_IN;
        }
        ep0_transceive_and_update(uisToken, &pDataToWrite, &bytesToWrite);
        /* Packet type must cycle between DATA0 and DATA1. The request (the
         * first packet) is DATA0, thus the next packet must be DATA1 and so on.
         * So here the first packet is forced to 1. */
        R8_UEP0_TX_CTRL |= RB_UEP_T_TOG_1;

        R8_USB_INT_FG = RB_USB_IF_SETUOACT;
    } else if (R8_USB_INT_FG & RB_USB_IF_FIFOOV) {
        /* Unused. */
        R8_USB_INT_FG = RB_USB_IF_FIFOOV;
    } else if (R8_USB_INT_FG & RB_USB_IF_SUSPEND) {
        R8_USB_INT_FG = RB_USB_IF_SUSPEND;
    } else if (R8_USB_INT_FG & RB_USB_IF_TRANSFER) {
        uint8_t endpNum = R8_USB_INT_ST & RB_DEV_ENDP_MASK;
        uint8_t uisToken = (R8_USB_INT_ST & RB_DEV_TOKEN_MASK);

        switch (endpNum) {
        case 0:
            if (SetupReq == USB_SET_ADDRESS) {
                R8_USB_DEV_AD = UsbSetupBuf->wValue.bw.bb1;

                pDataToWrite = NULL;
                R16_UEP0_T_LEN = 0;
                R8_UEP0_TX_CTRL = 0;
                R8_UEP0_RX_CTRL = UEP_R_RES_ACK | RB_UEP_R_TOG_1;
            } else {
                ep0_transceive_and_update(uisToken, &pDataToWrite, &bytesToWrite);
            }
            break;
        case 1:
            ep1_transceive_and_update(uisToken, (uint8_t **)&endp1Buff, &sizeEndp1Buff);
            break;
        case 7:
            ep7_transmit_and_update(uisToken, (uint8_t **)&endp7LoggingBuff, &sizeEndp7LoggingBuff);
            break;
        }

        R8_USB_INT_FG = RB_USB_IF_TRANSFER; // Clear int flag
    } else if (R8_USB_INT_FG & RB_USB_IF_BUSRST) {
        U20_registers_init(speed);
        U20_endpoints_init(epMask);

        R8_USB_INT_FG = RB_USB_IF_BUSRST;
    }
}


/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   Example of basic HardFault Handler called if an exception occurs
 *
 * @return  none
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void
HardFault_Handler(void)
{
    cprintf("HardFault\r\n");
    cprintf(" SP=0x%08X\r\n", __get_SP());
    cprintf(" MIE=0x%08X\r\n", __get_MIE());
    cprintf(" MSTATUS=0x%08X\r\n", __get_MSTATUS());
    cprintf(" MCAUSE=0x%08X\r\n", __get_MCAUSE());
    bsp_wait_ms_delay(1000000);
}

