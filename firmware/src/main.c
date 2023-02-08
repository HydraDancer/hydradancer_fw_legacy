/*
 * This firmware aims to provide a bridge for enumeration and fuzzing of
 * usb stack.
 * Part of the logic is abstracted in other files, everything related to
 * interrupt handling and USB endpoint behaviour is done in this file.
 *
 * - For Interrupt handling see the associated handler. For example the handler
 * related to HSPI is HSPI_IRQHandler().
 * - For USB endpoint behaviour see epX_transceive_and_update() from
 *   src/usb20-endpoints.h .
 */
#include <stdbool.h>
#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"
#include "CH56x_debug_log.h"

#include "bbio.h"
#include "hspi.h"
#include "log.h"
#include "serdes.h"
#include "usb20-endpoints.h"
#include "usb20.h"


// TODOO: Add clock for debug (PFIC_Enable(SysTick) ?)

/* variables */
static bool g_isHost = false;

uint16_t sizeEndp1Buff = 0;
const uint16_t capacityEndp1Buff = 4096;
__attribute__((aligned(16))) uint8_t endp1BuffRaw[4096];
uint8_t *endp1Buff = endp1BuffRaw;


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int
main(void)
{
    bsp_gpio_init();
    bsp_init(FREQ_SYS);
    UART1_init(115200, FREQ_SYS);


    /* USB Init */
    if (bsp_switch()) {
        g_usb20Speed = SpeedHigh;
        g_usb20EpInMask  = Ep1Mask | Ep6Mask| Ep7Mask;
        g_usb20EpOutMask = Ep1Mask;

        // Filling structures "describing" our USB peripheral
        g_descriptorDevice  = (uint8_t *)&stBoardTopDeviceDescriptor;
        g_descriptorConfig  = (uint8_t *)&stBoardTopConfigurationDescriptor;
        g_descriptorStrings = boardTopStringDescriptors;

        usb20_registers_init(g_usb20Speed);
        usb20_endpoints_init(g_usb20EpInMask, g_usb20EpOutMask);
    }

    /* Board sync */
    int retCode;
    if (bsp_switch()) {
        g_isHost = true;
        retCode = bsp_sync2boards(PA14, PA12, BSP_BOARD1);
    } else {
        g_isHost = false;
        retCode = bsp_sync2boards(PA14, PA12, BSP_BOARD2);
    }

    /* HSPI Init */
    memset((void *)hspiDmaAddr0, 0, HSPI_DMA_LEN0);
    hspiDmaAddr0[HSPI_DMA_LEN0-1] = 0;
    memset((void *)hspiDmaAddr1, 0, HSPI_DMA_LEN1);
    hspiDmaAddr1[HSPI_DMA_LEN1-1] = 0;
    if (g_isHost) {
        HSPI_DoubleDMA_Init(HSPI_HOST, RB_HSPI_DAT8_MOD, (uint32_t)hspiDmaAddr0, (uint32_t)hspiDmaAddr1, HSPI_DMA_LEN);
    } else {
        HSPI_DoubleDMA_Init(HSPI_DEVICE, RB_HSPI_DAT8_MOD, (uint32_t)hspiDmaAddr0, (uint32_t)hspiDmaAddr1, 0);
    }

    /* SerDes Init */
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
        // reconfigured before each transaction, used to identify the "kind" of
        // the transaction
        // SerDes_DMA_Tx_CFG((uint32_t)serdesDmaAddr, SERDES_DMA_LEN, serdesCustomNumber);
    }



    if (g_isHost) {
        log_to_evaluator("Init all done!\r\n");
        while (1) {
            // BBIO commands are passed directly to the bottom board,
            // There is no logic in the top board
            // See usb20_ep1_transceive_and_update_host()
        }
    } else {
        log_to_evaluator("Init all done!\r\n");
        while (1) { 
            bsp_uled_on();
            bsp_wait_ms_delay(500);
            bsp_uled_off();
            bsp_wait_ms_delay(500);
        }
    }

}


/*******************************************************************************
 * @fn     SERDES_IRQHandler
 *
 * @brief  SERDES Interrupt Handler
 *
 * @return None
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void 
SERDES_IRQHandler(void)
{
    uint16_t logLen;
    switch (SerDes_StatusIT() & ALL_INT_TYPE) {
    case SDS_PHY_RDY_FLG:
        SerDes_ClearIT(SDS_PHY_RDY_FLG);
        break;
    // SDS_TX_INT_FLG == SDS_RX_ERR_FLG, depend if it is Tx or Rx
    case SDS_TX_INT_FLG:
        SerDes_ClearIT(SDS_TX_INT_FLG);
        break;
    case SDS_RX_INT_FLG | SDS_RX_ERR_FLG:
        log_to_evaluator("SDS_RX_INT_FLG | SDS_RX_ERR_FLG\r\n");
        // No breaks, the handling is the same for both interrupts
    case SDS_RX_INT_FLG:

        switch(SDS->SDS_DATA0 & SerdesMagicNumberMask) {
        case SerdesMagicNumberLog:
            // Handle log received from bottom board
            logLen = strnlen(serdesDmaAddr, SERDES_DMA_LEN);
            if (sizeEndp7LoggingBuff + logLen <= capacityEndp7LoggingBuff) {
                memcpy(endp7LoggingBuff + sizeEndp7LoggingBuff, serdesDmaAddr, logLen);
                sizeEndp7LoggingBuff += logLen;
            }
            break;
        case SerdesMagicNumberRetCode:
            break;
        default:
            log_to_evaluator("ERROR: SERDES_IRQHandler() unknown magic number\r\n");
            break;
        }

        SerDes_ClearIT(SerDes_StatusIT() & ALL_INT_TYPE);
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
 * @brief  HSPI Interrupt Handler
 *
 * @return None
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void
HSPI_IRQHandler(void)
{
    // Bbio commands are in 2 parts
    // 1) Bbio instruction, see specs for more details
    // 2) Datas associated to the instruction previously received
    // Thus the following static var is used to track which part we are in
    static uint8_t currentStep = 0;

    uint8_t hspiRtxStatus;
    uint8_t *hspiRxBuffer;

    switch (R8_HSPI_INT_FLAG & HSPI_INT_FLAG) {
    case RB_HSPI_IF_T_DONE:
        hspiRtxStatus = hspi_get_rtx_status();
        if (hspiRtxStatus) {
            log_to_evaluator("[Interrupt HSPI]   Error transmitting: %s", hspiRtxStatus&RB_HSPI_CRC_ERR? "CRC_ERR" : "NUM_MIS");
        }
        R8_HSPI_INT_FLAG = RB_HSPI_IF_T_DONE;
        break;
    case RB_HSPI_IF_R_DONE:
        hspiRtxStatus = hspi_get_rtx_status();
        hspiRxBuffer = hspi_get_buffer_rx();
        if (hspiRtxStatus) {
            log_to_evaluator("[Interrupt HSPI]   Error receiving: %s", hspiRtxStatus&RB_HSPI_CRC_ERR? "CRC_ERR" : "NUM_MIS");
        }

        // Business logic goes here
        log_to_evaluator("Received HSPI\r\n");

        if (currentStep == 0) {
            bbio_command_decode(hspiRxBuffer);

            // Epilog
            currentStep ^= 1;
        } else if (currentStep == 1) {
            bbio_command_handle(hspiRxBuffer);

            // Epilog
            currentStep ^= 1;
        } else {
            log_to_evaluator("ERROR: Bottom board HSPI Handler current step: %x\r\n", currentStep);
        }

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
 * @brief  USB2.0 Interrupt Handler
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
        /* Unused */
        R8_USB_INT_FG = RB_USB_IF_ISOACT;
    } else if (R8_USB_INT_FG & RB_USB_IF_SETUOACT) { // Setup interrupt
        SetupReqType = UsbSetupBuf->bRequestType;
        SetupReq = UsbSetupBuf->bRequest;
        SetupReqLen = UsbSetupBuf->wLength;

        /* If bRequest != 0 it is a non standard request, thus not covered  by the spec */
        if ((SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
            return;
        }

        switch(SetupReq) {
        case USB_GET_STATUS:
            endp0RTbuff[0] = 0x00;
            endp0RTbuff[1] = 0x00;
            bytesToWrite = 2;

            // Here should be the handling of the halt endpoint's command
            break;
        case USB_CLEAR_FEATURE:
            switch (SetupReqType & USB_REQ_RECIP_MASK) {
            case USB_REQ_RECIP_DEVICE:
                /* Not implemented */
                break;
            case USB_REQ_RECIP_INTERF:
                /* Not implemented */
                break;
            case USB_REQ_RECIP_ENDP:
                usb20_endpoint_clear(UsbSetupBuf->wValue.bw.bb1);
                break;
            default:
                log_to_evaluator("ERROR: SETUP Interrupt USB_CLEAR_FEATURE invalid recipient");
                break;
            }
            break;
        case USB_SET_FEATURE:
            switch (SetupReqType & USB_REQ_RECIP_MASK) {
            case USB_REQ_RECIP_DEVICE:
                /* Not implemented */
                log_to_evaluator("ERROR: SETUP Interrupt USB_SET_FEATURE (toward device) unimplemented");
                break;
            case USB_REQ_RECIP_INTERF:
                /* Not implemented */
                log_to_evaluator("ERROR: SETUP Interrupt USB_SET_FEATURE (toward interface) unimplemented");
                break;
            case USB_REQ_RECIP_ENDP:
                switch (UsbSetupBuf->wValue.w) {
                case 0x0000: /* ENDPOINT_HALT */
                    usb20_endpoint_halt(UsbSetupBuf->wValue.bw.bb1);
                    break;
                default:
                    log_to_evaluator("ERROR: SETUP Interrupt USB_SET_FEATURE (toward endpoint) unimplemented");
                    break;
                }
                break;
            default:
                log_to_evaluator("ERROR: SETUP Interrupt USB_SET_FEATURE invalid recipient");
                break;
            }
            break;
        case USB_SET_ADDRESS:
            // NOTE: Address should not be set in this transaction but rather in the
            // following one (RB_USB_IF_TRANSFER IN)
            break;
        case USB_GET_DESCRIPTOR:
            usb20_fill_buffer_with_descriptor(UsbSetupBuf->wValue, &pDataToWrite, &bytesToWrite);
            break;
        case USB_SET_DESCRIPTOR:
            /* Unused */
            break;
        case USB_GET_CONFIGURATION:
            /* We have only one configuration, hardcoded for now */
            endp0RTbuff[0] = 1;
            bytesToWrite = 1;
            break;
        case USB_SET_CONFIGURATION:
            /* As of now there is only one configuration */
            break;
        case USB_GET_INTERFACE:
            /* We have only one interface, hardcoded for now */
            endp0RTbuff[0] = 0;
            bytesToWrite = 1;
            break;
        case USB_SET_INTERFACE:
            /* As of now there is only one interface */
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
        usb20_ep0_transceive_and_update(uisToken, &pDataToWrite, &bytesToWrite);
        /* Packet type must cycle between DATA0 and DATA1
         * The request (the first packet) is DATA0, thus the next packet must be
         * DATA1 and so on
         * So here the first packet is forced to 1
         */
        R8_UEP0_TX_CTRL |= RB_UEP_T_TOG_1;

        R8_USB_INT_FG = RB_USB_IF_SETUOACT;
    } else if (R8_USB_INT_FG & RB_USB_IF_FIFOOV) {
        /* Unused */
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
                usb20_ep0_transceive_and_update(uisToken, &pDataToWrite, &bytesToWrite);
            }
            break;
        case 1:
            if (g_isHost) {
                ep1_transceive_and_update_host(uisToken, (uint8_t **)&endp1Buff, &sizeEndp1Buff);
            } else {
                ep1_transceive_and_update_target(uisToken, (uint8_t **)&endp1Buff, &sizeEndp1Buff);
            }
            break;
        case 6:
            ep6_transmit_and_update(uisToken, (uint8_t **)&endp6LoggingBuff, &sizeEndp6LoggingBuff);
            break;
        case 7:
            ep7_transmit_and_update(uisToken, (uint8_t **)&endp7LoggingBuff, &sizeEndp7LoggingBuff);
            break;
        default:
            log_to_evaluator("ERROR: USBHS_IRQHandler() endpoint requested (%d) has no handler associated\r\n", endpNum);
            break;
        }

        R8_USB_INT_FG = RB_USB_IF_TRANSFER; // Clear int flag
    } else if (R8_USB_INT_FG & RB_USB_IF_BUSRST) {
        usb20_registers_init(g_usb20Speed);
        usb20_endpoints_init(g_usb20EpInMask, g_usb20EpOutMask);

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

