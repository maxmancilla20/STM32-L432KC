/****************************************************************************************************
 * @file           : Can.c
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The CAN Driver handles initialization, transmission, and reception of CAN messages.
 *                   This module abstracts the CAN peripheral for the AUTOSAR MCAL layer.
 ***************************************************************************************************/

#include "stm32l4xx.h"
#include "Can.h"
#include "Can_Cfg.h"

#define GPIOAEN             (1U<<0)
#define RCC_APB1ENR_CAN1EN  (1U<<25)
#define CAN_AF             0x09 /*(1001)*/

void can_gpio_init(void)
{
    /* Enable Clock Access APB1*/
    //RCC->AHB2ENR |= GPIOAEN;

    /* Set PA11 and PA12 to alternate function mode*/
    GPIOA->MODER &= ~(1U << 22); /* Clear bit 22 */
    GPIOA->MODER |= (1U << 23); /* Set bit 23 */

    GPIOA->MODER &= ~(1U << 24); /* Clear bit 24 */    
    GPIOA->MODER |= (1U << 25); /* Set bit 25 */

    /* Set PA11 and PA12 alternate function type to CAN (AF9) */
    GPIOA->AFR[1] |= (CAN_AF << 12); /* Set pin PA11 alternate function */
    GPIOA->AFR[1] |= (CAN_AF << 16); /* Set pin PA12 alternate function */

    /* Enable CAN RX0 Interrupt*/
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
}

void can_params_init(uint8_t mode)
{
    /* Enable Clock Access to CAN1  */
    RCC->APB1ENR1 |= RCC_APB1ENR_CAN1EN;

    /* Enter Initialization Mode */
    CAN1->MCR |= CAN_MCR_INRQ; /* Request initialization mode */
    while (!(CAN1->MSR & CAN_MSR_INAK)); /* Wait until initialization mode is active */

    /* Exit sleep mode */
    CAN1->MCR &= ~CAN_MCR_SLEEP; /* Exit sleep mode */
    while (CAN1->MSR & CAN_MSR_SLAK); /* Wait until sleep mode is exited */

    /* Confirue timing parameters including baudrate by configuring time segment 1 and 2 and prescaler */
    //CAN1->BTR = (CAN_BTR_SJW_0) | (CAN_BTR_TS1_2) | (CAN_BTR_TS2_0) | (CAN_BTR_TS2_1) | (CAN_BTR_BRP_0) | (CAN_BTR_BRP_1); /* Set baudrate to 500 kbps with 16 MHz clock */
    
    CAN1->BTR = (1 << CAN_BTR_TS1_Pos) | (0 << CAN_BTR_TS1_Pos) | (1 << CAN_BTR_BRP_Pos);
    /* Select Mode */
    if(mode)
    {
        /* Normal Mode*/
        CAN1->BTR &= ~ (1U << 30); /* Enable automatic bus-off management */
    }
    else
    {
        /* Loopback mode*/
        CAN1->BTR |= (1U << 30);/* Disable automatic bus-off management */
    }
}

void can_start(void)
{
    /* Exit Initialization Mode */
    CAN1->MCR &= ~CAN_MCR_INRQ; /* Request normal mode */
    while (CAN1->MSR & CAN_MSR_INAK); /* Wait until normal mode is active */

    /* Enable Interrupt for FIFO0 message pending*/
    CAN1->IER |= CAN_IER_FMPIE0;
}


/**
  * @brief  Add a message to the first free Tx mailbox and activate the
  *         corresponding transmission request.
  * @param  pHeader pointer to a CAN_TxHeaderTypeDef structure.
  * @param  aData array containing the payload of the Tx frame.
  * @param  pTxMailbox pointer to a variable where the function will return
  *         the TxMailbox used to store the Tx message.
  *         This parameter can be a value of @arg CAN_Tx_Mailboxes.
  * @retval  status
  */
uint8_t can_add_tx_message(can_tx_header_typedef *pHeader, uint8_t aData[], uint32_t *pTxMailbox)
{
    // Variable to hold the selected transmit mailbox

    uint32_t trasmitmailbox;

    // Read the Trasnmit Status Register

    uint32_t tsr = READ_REG(CAN1->TSR);

    //Check that at least one Tx mailbox is emnpty

    if(((tsr & CAN_TSR_TME0) != 0U) ||
       ((tsr & CAN_TSR_TME1) != 0U) ||
       ((tsr & CAN_TSR_TME2) != 0U))
    {
        trasmitmailbox = (tsr & CAN_TSR_CODE) >> CAN_TSR_CODE_Pos;

        /* Check trasnmitmailbox validity */
        if(trasmitmailbox > 2U)
        {
            return 1; /* Invalid mailbox index */
        }

        /* Store the Tx mailbox */
        *pTxMailbox = (uint32_t)1 << trasmitmailbox;

        /* Set up the ID */
        if(pHeader->ide == CAN_ID_STD)
        { 
            /* Standard ID */
            CAN1->sTxMailBox[trasmitmailbox].TIR = ((pHeader->std_id << CAN_TI0R_STID_Pos) | pHeader->rtr);
        }
        else
        { 
            /* Extended ID */
            CAN1->sTxMailBox[trasmitmailbox].TIR = ((pHeader->ext_id << CAN_TI0R_EXID_Pos) | pHeader->ide | pHeader->rtr);
        }

        /* Set up the DLC */
        CAN1->sTxMailBox[trasmitmailbox].TDTR = (pHeader->dlc);

        /* Set up the Trasmit Global Time mode */
        if(pHeader->trasmit_global_time == 1)
        {
            SET_BIT(CAN1->sTxMailBox[trasmitmailbox].TDTR, CAN_TDT0R_TGT);
        }

        /* Set up the data field */
                    /* More significant bytes */
        WRITE_REG(CAN1->sTxMailBox[trasmitmailbox].TDHR,
                  ((uint32_t)aData[7] << CAN_TDH0R_DATA7_Pos) |
                  ((uint32_t)aData[6] << CAN_TDH0R_DATA6_Pos) |
                  ((uint32_t)aData[5] << CAN_TDH0R_DATA5_Pos) |
                  ((uint32_t)aData[4] << CAN_TDH0R_DATA4_Pos));
        
                  /* Less significant bytes */
        WRITE_REG(CAN1->sTxMailBox[trasmitmailbox].TDLR,
                  ((uint32_t)aData[3] << CAN_TDL0R_DATA3_Pos) |
                  ((uint32_t)aData[2] << CAN_TDL0R_DATA2_Pos) |
                  ((uint32_t)aData[1] << CAN_TDL0R_DATA1_Pos) |
                  ((uint32_t)aData[0] << CAN_TDL0R_DATA0_Pos));

        /* Request transmission */
        SET_BIT(CAN1->sTxMailBox[trasmitmailbox].TIR, CAN_TI0R_TXRQ);

        return 0; /* Message added successfully */
    }

    return 0; /* No empty mailbox available */
}

/**
  * @brief  Get an CAN frame from the Rx FIFO zone into the message RAM.
  * @param  RxFifo Fifo number of the received message to be read.
  * @param  pHeader pointer to a can_rx_header_typedef structure where the header
  *         of the Rx frame will be stored.
  * @param  aData array where the payload of the Rx frame will be stored.
  * @retval  status
  */
uint8_t can_get_rx_message(uint32_t RxFifo, can_rx_header_typedef *pHeader, uint8_t aData[])
{

    /* Check the Rx FIFO */
    if (RxFifo == CAN_RX_FIFO0) /* Rx element is assigned to Rx FIFO 0 */
    {
      /* Check that the Rx FIFO 0 is not empty */
      if ((CAN1->RF0R & CAN_RF0R_FMP0) == 0U)
      {
        return 1;
      }
    }
    else /* Rx element is assigned to Rx FIFO 1 */
    {
      /* Check that the Rx FIFO 1 is not empty */
      if ((CAN1->RF1R & CAN_RF1R_FMP1) == 0U)
      {
        return 1;
      }
    }

    // Read the header information from the FIFO mailbox
    // Extract identifier, DLC, timestamp, etc.

    pHeader->ide = CAN_RI0R_IDE & CAN1->sFIFOMailBox[RxFifo].RIR;

    if (pHeader->ide == CAN_ID_STD)
    {
      pHeader->std_id = (CAN_RI0R_STID & CAN1->sFIFOMailBox[RxFifo].RIR) >> CAN_TI0R_STID_Pos;
    }
    else
    {
      pHeader->ext_id = ((CAN_RI0R_EXID | CAN_RI0R_STID) & CAN1->sFIFOMailBox[RxFifo].RIR) >> CAN_RI0R_EXID_Pos;
    }

    pHeader->rtr = (CAN_RI0R_RTR & CAN1->sFIFOMailBox[RxFifo].RIR);
    pHeader->dlc = (CAN_RDT0R_DLC & CAN1->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_DLC_Pos;
    pHeader->filter_match_index = (CAN_RDT0R_FMI & CAN1->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_FMI_Pos;
    pHeader->timestamp = (CAN_RDT0R_TIME & CAN1->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_TIME_Pos;

    // Extract the data from the FIFO mailbox
    aData[0] = (uint8_t)((CAN_RDL0R_DATA0 & CAN1->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA0_Pos);
    aData[1] = (uint8_t)((CAN_RDL0R_DATA1 & CAN1->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA1_Pos);
    aData[2] = (uint8_t)((CAN_RDL0R_DATA2 & CAN1->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA2_Pos);
    aData[3] = (uint8_t)((CAN_RDL0R_DATA3 & CAN1->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA3_Pos);
    aData[4] = (uint8_t)((CAN_RDH0R_DATA4 & CAN1->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA4_Pos);
    aData[5] = (uint8_t)((CAN_RDH0R_DATA5 & CAN1->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA5_Pos);
    aData[6] = (uint8_t)((CAN_RDH0R_DATA6 & CAN1->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA6_Pos);
    aData[7] = (uint8_t)((CAN_RDH0R_DATA7 & CAN1->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA7_Pos);

    /* Release the FIFO */
    if (RxFifo == CAN_RX_FIFO0) /* Rx element is assigned to Rx FIFO 0 */
    {
      /* Release RX FIFO 0 */
      SET_BIT(CAN1->RF0R, CAN_RF0R_RFOM0);
    }
    else /* Rx element is assigned to Rx FIFO 1 */
    {
      /* Release RX FIFO 1 */
      SET_BIT(CAN1->RF1R, CAN_RF1R_RFOM1);
    }

  return 0;// Message read successfully
}

void can_filter_config(uint16_t std_id)
{
	/*Set filter intiailization mode*/
	CAN1->FMR |= CAN_FMR_FINIT;

	///*Set the slave filter to start from 20, only available for 2 CANs*/
	//CAN1->FMR &=~(CAN_FMR_CAN2SB_Msk);
	//CAN1->FMR |=(20 << CAN_FMR_CAN2SB_Pos);

	/*****Filter activation sequence****/
	/*Deactive filter 13*/
	CAN1->FA1R &=~(CAN_FA1R_FACT13);

	/*Set 32-bit scale configuration*/
	CAN1->FS1R  |= CAN_FS1R_FSC13;

	/*Configure filter mode to identifier mask mode*/
	CAN1->FM1R &=~CAN_FM1R_FBM13;

	/*Set identifier*/
	CAN1->sFilterRegister[13].FR1 =
    ((uint32_t)(std_id & 0x7FFU) << CAN_TI0R_STID_Pos);

	CAN1->sFilterRegister[13].FR2 = CAN_TI0R_STID_Msk;

	/*Assign filter 13 to FIFO0*/
	CAN1->FFA1R &=~(CAN_FFA1R_FFA13);

	/*Activate filter 13*/
	CAN1->FA1R |= (CAN_FA1R_FACT13);

	/*Clear filter intialization mode*/
	CAN1->FMR &= ~CAN_FMR_FINIT;

}
