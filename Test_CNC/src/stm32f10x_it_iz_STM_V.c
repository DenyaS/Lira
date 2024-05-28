/**
  ******************************************************************************
  * @file    src/stm32f10x_it.c 
  * @author  SDK-SYSTEMS
  * @version V1.0.1
  * @date    SEP-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 SDK-SYSTEMS ODESSA </center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_usart.h"
#include "includes.h"

void TimingDelay_Decrement(void);
/** @addtogroup Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
  uint8_t Sen1_state;
  uint8_t Sen1_old_state = 0;
  uint8_t Sen2_state;
  uint8_t Sen2_old_state = 0;
  uint8_t Sen3_state;
  uint8_t Sen3_old_state = 0;
  uint8_t Sen4_state;
  uint8_t Sen4_old_state = 0;

  /* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  TickCnt++;        // ������� ����
}

/** [duke] 
  * @brief      ���������� ���������� �������� ������� TIM6
  *                 ==�������� LCD==
  *                 ==UART0==
  *                 ==UART1==
  * @param      None
  * @retval     None
  */
void TIM6_DAC_IRQHandler(void)
{
  // Disable general interrupts
  //disableInterrupts();  

  // ��� ��� ���� ���������� ���������� � ��� ���, ����� ���������,
  // ��������� �� ���������� �� ������������ �������� ������� TIM6.
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
    // ������� ��� ��������������� ����������
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

    // ����������� ��������� ������ V0 (������������� LCD MTC-S16204 
    GPIO_Write(MTC_GPIO_PORT_V0, GPIO_ReadOutputData(MTC_GPIO_PORT_V0) ^ MTC_V0);
  }
    
  // Enable general interrupts
  //enableInterrupts();   
}


/** [duke] 
  * @brief      ���������� ���������� �������� ������� TIM7
  *                 ==����������==
  *                 ==�������==
  * @param      None
  * @retval     None
  */
void TIM7_IRQHandler(void)
{
  uint16_t d;
  uint8_t e;

  // Disable general interrupts
  //disableInterrupts();  

  // ��������, ��������� �� ���������� �� ������������ �������� ������� TIM7.
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
  {
    // ������� ��� ��������������� ����������
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    
    //======================= ��������� ���������� =============================
    if (TimerKeyboard == 0x00) { 
      //d = GPIO_ReadInputData(KEYBOARD_GPIO_PORT) & 0x00f0;
      /** ��������! �� �������� PB4 - Button "UP":
      ��������� ������������� ���������������� � ����� STM32VLD ����� PB4*/
      d = GPIO_ReadInputData(KEYBOARD_GPIO_PORT) & 0x00e0;
      // �������� - ��������� ������ [UP] (�.�. ������ � �����)
      e = GPIO_ReadInputDataBit(GPIOC, KEY_UP);
      if(d) {
        // �������� �� 5 ��� ��������� �������
        STimerBacklight = 5;
        if(!fl_KeyOn) { // klavisha ne byla najata
          fl_KeyOn = 1; 
          PreviKey = d; 
          TimerKeyboard = 5;   // ����� ��������� �������
        }
        else {
          if(PreviKey == d) { // klavisha eshe najata
            if (!fl_KeyFixed) { 
              LastKey = d;
              fl_KeyFixed = 1;
            }
            TimerKeyboard = 10;    // vremya na otpuskanie klavishi
          } 
          else { // uspeli najat' druguyu klavishu
            PreviKey = d;           
            TimerKeyboard = 1;     
          }
        }
      } 
      else if (!d && !e) {
        fl_KeyOn = 0; 
        fl_KeyFixed = 0;
      }
      /* ----- �������� - ��������� ������ [UP] (�.�. ������ � �����) ------ */
      if(e) {
        // �������� �� 5 ��� ��������� �������
        STimerBacklight = 5;
        if(!fl_KeyOn) { // klavisha ne byla najata
          fl_KeyOn = 1; 
          PreviKey = UP; 
          TimerKeyboard = 5;   // ����� ��������� �������
        }
        else {
          if(PreviKey == UP) { // klavisha eshe najata
            if (!fl_KeyFixed) { 
              LastKey = UP;
              fl_KeyFixed = 1;
            }
            TimerKeyboard = 10;    // vremya na otpuskanie klavishi
          } 
        }
      } 
      /* -------------------------------------------------------------- */
    }
    
    //====================== ��������� �������� ===============================
    // ��������� ������� Sen1 
    Sen1_state = SEN_1_STATE;
    if ((Sen1_state == 1) && (Sen1_old_state == 0)) {
      // ������ ������ "������� �������� �����"
      fl_OpenFrontDoor = 1;
    }
    Sen1_old_state = Sen1_state;

    // ��������� ������� Sen2 
    Sen2_state = SEN_2_STATE;
    if ((Sen2_state == 1) && (Sen2_old_state == 0)) {
      // ������ ������ "������� ������ �����"
      fl_OpenRearDoor = 1;
    }
    Sen2_old_state = Sen2_state;
    
    // ��������� ������� Sen3 
    Sen3_state = SEN_3_STATE;
    if ((Sen3_state == 1) && (Sen3_old_state == 0)) {
      // �������� ������ �������� �����
      fl_OpenCar = 1;
    }
    Sen3_old_state = Sen3_state;
      
    // ��������� ������� Sen4 
    Sen4_state = SEN_4_STATE;
    if ((Sen4_state == 1) && (Sen4_old_state == 0)) {
      // ...
      _NOP();
    }
    Sen4_old_state = Sen4_state;
  }
  
  // Enable general interrupts
  //enableInterrupts();   
}

/** [duke] 
  * @brief      ���������� ���������� �� SPI1
  * @param      None
  * @retval     None
  */
void SPI1_IRQHandler (void) 
{
  // Disable general interrupts
  //disableInterrupts();  

  if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == SET) 
  { // ���������� ������� ��������� ����� ?
    _NOP(); // ... ������ ��� ����
    // Clear the SPIx CRC Error flag
    SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_TXE);
  }
  
  if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET) 
  { // ���������� ������� ������� ����� ?
    //data_SPI = SPI1->DR; // ������ �� ��� ������
    _NOP(); // ... ������ ��� ����
    //SPI1->DR = data_SPI; // � ���������� ������� �� ��� �������
    // Clear the SPIx CRC Error flag
    SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
  }

  /*
  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) == SET) 
  { // ���������� ������� ��������� ����� ?
    _NOP(); // ... ������ ��� ����
    // Clear the SPIx CRC Error flag
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_TXE);
  }

  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET) 
  { // ���������� ������� ������� ����� ?
    _NOP(); // ... ������ ��� ����
    // Clear the SPIx CRC Error flag
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
  }
  */
  // Enable general interrupts
  //enableInterrupts();   
}

/** [duke] 
  * @brief      ���������� ���������� UART0
  * @param      None
  * @retval     None
  */
void USART1_IRQHandler (void) 
{
  uchar c;
  
  // Disable general interrupts
  disableInterrupts(); //====DEBUG HANG====
  
  //---------- ���������� �� ���������� �������� ����� UART0 (USART1) -----------
  if(USART_GetITStatus(USART1, USART_IT_TC) == SET)  
  { // ���� ����� � �������� (Transmission complete interrupt)
    if(UARTx[0].SendPack)
    {
      if(UARTx[0].TxCnt) 
      { // ���� ����� ��� �� �������
#ifdef __BYTESTUFFING__ // ��������� ������������� �� ��������
// ��� �������� ���� 0x7e ���������� �� ������������������ 0x7d 0x5e
// � ���� 0x7d ���������� �� ������������������ 0x7d 0x5d
        if(fl_ByteStuf) 			
        {	
          fl_ByteStuf = 0;			            // ���������� ���� �������������
          USART1->DR = *UARTx[0].Ptr++ ^ 0x20;	// ����������� �� ������������ ���� ����� XOR, 
                                                // �������� �� 0x7e -> 0x5e, �� 0x7d -> 0x5d
                                                // �������� ��������� [Ptr]
          UARTx[0].TxCnt--;                     // ��������� ������� ������������ ���� [TxCnt]
          goto complete;						// ����� �������� �����			
        } 
        else 
        { 
          if((*UARTx[0].Ptr == 0x7e) || (*UARTx[0].Ptr == 0x7d))  // ���� ����� �������� ���� 0x7e ��� 0x7d
          {
            fl_ByteStuf = 1;		// ���������� ���� ������������
            USART1->DR = 0x7d;		// � ��������� ������ �� 0x7d
                                    // ������� ���� �� ������, ��������� �� �������� 
			goto complete;          // ����� - ����� ����� ���� �� ������
          } 
        }
#endif  // ����� ��������� ������������� �� ��������
        UARTx[0].TxCnt--;				// �������������� ������� ���� ��� ��������� ������
		USART1->DR = *UARTx[0].Ptr++;	// ���������� � ������� [DR] ������������ ����
                                        // �������� ��������� [Ptr]      
      }
      else
      { // ����� ������� 
        UARTx[0].SendPack = 0;
      }
    }
  } //---- �������� ����� ��������� ----

  //---------- ���������� �� ���������� ����� ����� UART0 (USART1) -----------
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  { // Receive Data register not empty interrupt
	//c = USART1->DR;						// ������ ������ �� ��������
    c = USART_ReceiveData(USART1);
	if(UARTx[0].RxComplete) { goto complete; }
	if(UARTx[0].ReceivePack) 
    { // ����� ������ �� �������
      UARTx[0].Timer = 20;     // 20 ms
#ifdef __BYTESTUFFING__  // ��������� ������������� �� �����
// ��� ����� ������������������ 0x7d 0x5e ���������� �� ���� 0x7e
// � ������������������ 0x7d 0x5d ���������� �� ���� 0x7d
      if(fl_ByteStuf)			// ���� ���������� ���� ������������� 
      {
        fl_ByteStuf = 0;	    // �� ���������� ���
		c = c ^ 0x20;		    // ����������� �� �������� ���� ����� XOR
                                // � �������� �� 0x5e -> 0x7e, �� 0x5d -> 0x7d
      } 
      else 
      {
        if(c == 0x7d)		    // ���� ������� ���� 0x7d, �� �������� ��������� �������������
		{ 
          fl_ByteStuf = 1;	    // ������������� ���� �������������
          goto complete;		// � ������� - ���� ���� ������ �� ��������������
								// ��������� ����� ���� 0x5e, ���� 0x5d
		}
      }
#endif  // ����� ��������� ������������� �� �����
      // ����� ������ = ������ ������ (���������� � ����� 2) + 
      // + ADR_DST + ADR_SRC + SIZE + CMD + 2 ����� CRC16
      if(UARTx[0].RxCnt == 2) { UARTx[0].RxSize = c + 6; }  
      if(UARTx[0].RxSize >= UART_BUF_SIZE) 
      { // ���� ������ ������ ��������� ������ ������
        //UARTx[0].ReceivePack = 0;		// ����������� ����
		UARTx[0].Timer = 0;
      }
      UARTx[0].InBuf[UARTx[0].RxCnt++] = c;     // ����������� RxCnt
      if(UARTx[0].RxCnt >= UARTx[0].RxSize) 
      {
        //UARTx[0].ReceivePack = 0;		// ����������� ���� ������
        //UARTx[0].RxComplete = 1;
        UARTx[0].Timer = 0;
      }
	} 
    else 
    { // ������� ���� ������ ������ - �������� ��������� ����� �����
      if(c == FLAG_PACKET) 
      {
        UARTx[0].ReceivePack = 1;					
		UARTx[0].RxSize	= HEAD_PACKET;          
		UARTx[0].RxCnt = 0;
		UARTx[0].Timer = 20;       // 20 ms
      }
	}
    if (UARTx[0].Timer == 0) {
      UARTx[0].ReceivePack = 0;		// ����������� ���� ������
      UARTx[0].RxComplete = 1;      // ����� ������
      checkPacket_UARTx(0);
    }
  } //---- ���� ����� �������� ----

complete:   
  // ���������� ��� ���������� TC (��� RXNE ������������ ����� ������ Data Register)
  USART_ClearITPendingBit(USART1, USART_IT_TC);        
    
  // Enable general interrupts
  enableInterrupts();   //====DEBUG HANG====
}

/** [duke] 
  * @brief      ���������� ���������� UART1
  * @param      None
  * @retval     None
  */
void USART2_IRQHandler (void) 
{
  uchar c;
  
  // Disable general interrupts
  disableInterrupts();  //====DEBUG HANG====
  
  //---------- ���������� �� ���������� �������� ����� -----------
  if(USART_GetITStatus(USART2, USART_IT_TC) == SET)  
  { 
    if(UARTx[1].SendPack)
    {
      if(UARTx[1].TxCnt) 
      { 
#ifdef __BYTESTUFFING__ 
        if(fl_ByteStuf) 			
        {	
          fl_ByteStuf = 0;			            
          USART2->DR = *UARTx[1].Ptr++ ^ 0x20;	 
          UARTx[1].TxCnt--;                     
          goto complete;									
        } 
        else 
        { 
          if((*UARTx[1].Ptr == 0x7e) || (*UARTx[1].Ptr == 0x7d))  
          {
            fl_ByteStuf = 1;		
            USART2->DR = 0x7d;		
			goto complete;          
          } 
        }
#endif  // ����� ��������� ������������� �� ��������
        UARTx[1].TxCnt--;				
		USART2->DR = *UARTx[1].Ptr++;	
      }
      else
      { // ����� ������� 
        UARTx[1].SendPack = 0;
        off(TX485);           // ��������� ���������� RS-485  
      }
    }
  } //---- �������� ����� ��������� ----

  //---------- ���������� �� ���������� ����� ����� UART1 (USART2) -----------
  if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
  { 
    c = USART_ReceiveData(USART2);
	if(UARTx[1].RxComplete) { goto complete; }
	if(UARTx[1].ReceivePack) 
    { // ����� ������ �� �������
      UARTx[1].Timer = 20;     
#ifdef __BYTESTUFFING__  // ��������� ������������� �� �����
      if(fl_ByteStuf)			 
      {
        fl_ByteStuf = 0;	    
		c = c ^ 0x20;		    
      } 
      else 
      {
        if(c == 0x7d)		    
		{ 
          fl_ByteStuf = 1;	    
          goto complete;		
		}
      }
#endif  // ����� ��������� ������������� �� �����
      if(UARTx[1].RxCnt == 2) { UARTx[1].RxSize = c + 6; }  
      if(UARTx[1].RxSize >= UART_BUF_SIZE) 
      { // ���� ������ ������ ��������� ������ ������
		UARTx[1].Timer = 0;
      }
      UARTx[1].InBuf[UARTx[1].RxCnt++] = c;     // ����������� RxCnt
      if(UARTx[1].RxCnt >= UARTx[1].RxSize) 
      {
        UARTx[1].Timer = 0;
      }
	} 
    else 
    { 
      if(c == FLAG_PACKET) 
      {
        UARTx[1].ReceivePack = 1;					
		UARTx[1].RxSize	= HEAD_PACKET;          
		UARTx[1].RxCnt = 0;
		UARTx[1].Timer = 20;       // 20 ms
      }
	}
    if (UARTx[1].Timer == 0) {
      UARTx[1].ReceivePack = 0;		// ����������� ���� ������
      UARTx[1].RxComplete = 1;      // ����� ������
      checkPacket_UARTx(1);
    }
  } //---- ���� ����� �������� ----

complete:   
  // ���������� ��� ���������� TC (��� RXNE ������������ ����� ������ Data Register)
  USART_ClearITPendingBit(USART2, USART_IT_TC);        

  // Enable general interrupts
  enableInterrupts();   //====DEBUG HANG====
}

/** [duke] 
  * @brief      ���������� ���������� I2C1
  * @param      None
  * @retval     None
  */
void I2C1_EV_IRQHandler (void)
{
  /*
  // Disable general interrupts
  disableInterrupts();

  // Enable general interrupts
  enableInterrupts();
  */
}

/**
  * @brief  This function handles External lines 5-9 interrupt request.
  *         Line8 - interrupt from Radio MRF49XA
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{

  // Disable general interrupts
  disableInterrupts();  //====DEBUG HANG====

  uchar Bt;
  /* ������� ���������� �� ����������� MRF49XA (Line 8) */
  if(EXTI_GetITStatus(RF_EXTI_LINE) != RESET)
  {
    ENABLE_RF();      // CS active
    
    // �������� ���������
    on(LED_RF);
    GPIO_ResetBits(OUTPUTS_GPIO_PORT, LED_WORK);

    // ==== DEBUG ====
    //getRadioStatus(CS_YES);
    //if (RF.STSREG.TXRXFIFO)
    if (GPIO_ReadInputDataBit(RSDO) == 1)
    { // ����� ��������: ������� ����������� ����� � ����� ���������� ����� ��� ��������
      // ����� �����: ���� �������� ������ � FIFO, ������ ���� �� FIFO
      if (Radio.TransmitMode)
      {
//------------------ ����� �������� ---------------------------
        if(Radio.TxCnt < Radio.TxLen) 
        { // �� ��� ����� ������ ��� ��������
          RF.TXBREG.TXDB = Radio.TxBuf[Radio.TxCnt++];
          sendData_MRF49XA(RF.TXBREG.HalfWord, CS_YES);
        }
        else { // ��� ����� ������ ��������
          Radio.Transmit = 0;
          setRadioMode(RECEIVE);      // ������������� ����� �����
        }  
      }
//------------------ ����� ������ -----------------------------
      else          // Radio.TransmitMode == 0
      {
        Bt = readFIFO_MRF49XA();
        if (Radio.Receive) { // ����� �����������
          if(Radio.RxCnt < Radio.RxLen) { // ���� ��� �� ���������� �����
            Radio.RxBuf[Radio.RxCnt++] = Bt;  // ���������� �������� ���� � �����
          }                                   // �������������� ������� ����
          else {  // ��� ����� ������ ��������
            Radio.Receive = 0;				// ��������� ���� ������
            Radio.ReceiveComplete = 1;		// ����� ������
            // ��������� FIFO
            sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
            // ==== DEBUG ====
            // �������� ��������� ������   
            checkPacket_MRF49XA();
            NumsOfIPackets++;
            //================
          }
        }
        else { // ����� ��� �� ������� ����������� 
          if (Bt <= DATASIZE) { // ������ ������ ���� ������ (DSize)
            Radio.DisChange = 1;
            Radio.Receive = 1;		// ������������� ���� ����� ������
            Radio.RxLen = (uint)Bt + sizeof(PackHeader_TypeDef) + 2; // ������� ���������� ���� � ������
            Radio.RxCnt = 0;
            Radio.RxBuf[Radio.RxCnt++] = Bt; // ���������� � ����� � �������������� �������
          }
          else { // �������� �������� ������� ���� ������ ������
            Radio.DisChange = 0;
            sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);  // ������ FIFO
            RF_SYNC_RESTART();        // ������������� �������� ������������������������
          }
        }
      }
    }
    else {    // ����������� �� ����� �� � �����, �� � ��������
      DISABLE_RF();
    }
    // ���������� �� MRF49XA ���������: ���� ������ ��� �������   
    getRadioStatus(CS_YES);

    // ��������� ���������
    off(LED_RF);
    GPIO_SetBits(OUTPUTS_GPIO_PORT, LED_WORK);

    // Clear the RF_IRO EXTI line pending bit 
    EXTI_ClearITPendingBit(RF_EXTI_LINE);
  }

  // Enable general interrupts
  enableInterrupts();   //====DEBUG HANG====
}
        
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 


/**
  * @brief  �������� - ���������� �� ���������� MRF49XA
  */ 
/*
// ������� ���������� �� ����������� MRF49XA (Line 8) 
  if(EXTI_GetITStatus(RF_EXTI_LINE) != RESET)
  {

	// ���� ��� ������� ���������� �� ����� - �����
    if (!signal(IRQ, I)) goto exticomplete; // �����? ���� �� ���� ������, ������ ������ ���
	// ������ ������� ������� �����������
    getRadioStatus(1);

extitransmit:
//---------------------- � ������ �������� -------------------------------
	if (Radio.TransmitMode) 
    {
      // ������� ���������� ���� ������ (�� ��� ������� � ������� ��������)
      if(fl_StartPacket) {  
        fl_StartPacket = 0;
        sendData_MRF49XA(RF.TXBREG.HalfWord);
        goto extitransmit;
      } 
      if (RF.STSREG.TXRXFIFO)
      { // ������� ����������� ����� � ����� ���������� ����� ��� ��������
        if (Radio.TxCnt >= Radio.TxLen) {
          // ��� ����� ������ ��������
          // ������������� ����� �����
          setRadioMode(RECEIVE);
          goto exticomplete;
		}

#ifdef BYTESTUFFING // ��������� ������������� �� ��������
// ��� �������� ���� 0x7e ���������� �� ������������������ 0x7d 0x5e
// � ���� 0x7d ���������� �� ������������������ 0x7d 0x5d
        if(fl_ByteStuf) 			
        {	
          fl_ByteStuf = 0;			         // ���������� ���� �������������
          // ����������� �� ������������ ���� ����� XOR                                      
          // �������� �� 0x7e -> 0x5e, �� 0x7d -> 0x5d 
          Radio.TxBuf[Radio.TxCnt] ^= 0x20;	 
        } 
        else 
        { 
          if((Radio.TxBuf[Radio.TxCnt] == 0x7e) 
             || (Radio.TxBuf[Radio.TxCnt] == 0x7d))  // ���� ����� �������� ���� 0x7e ��� 0x7d
          {
            fl_ByteStuf = 1;		// ���������� ���� ������������
            RF.TXBREG.TXDB = 0x7d;	// � ��������� ������ �� 0x7d
                                    // ������� ���� �� ������ 
			sendData_MRF49XA(RF.TXBREG.HalfWord);
            goto extitransmit;		// ������� - ����� ����� ���� �� ������
          } 
        }
#endif  // ����� ��������� ������������� �� ��������

		RF.TXBREG.TXDB = Radio.TxBuf[Radio.TxCnt++];
		sendData_MRF49XA(RF.TXBREG.HalfWord);
      }
      goto extitransmit;
	}

//------------------------ � ������ ����� ---------------------------------
	else if (RF.STSREG.TXRXFIFO) 
    { // ���� �������� ������ � FIFO, ������ ���� �� FIFO
      Bt = readData_MRF49XA();
      if (Radio.Receive) {
        if (Radio.RxCnt >= Radio.RxLen) {
          Radio.Receive = 0;				// ��������� ���� ������
          Radio.ReceiveComplete = 1;		// ����� ������
          // ������������� �������� �����������������������
          RF_SYNC_RESTART();
		}
		Radio.RxBuf[Radio.RxCnt++] = Bt;
      } 
      else {
        if (Bt <= DATASIZE) {
          Radio.Receive = 1;				// ������������� ���� ����� ������
          Radio.RxLen = (uint)Bt + sizeof(PackHeader_TypeDef) + 2;
          Radio.RxCnt = 0;
          Radio.RxBuf[Radio.RxCnt++] = Bt;
		} 
        else {
          // ������������� �������� �����������������������
          RF_SYNC_RESTART();
		}
      }
	}

exticomplete:   // ���������� ��������� - ����� ������� ��� ������    

    off (LED);
    // Clear the RF_IRO EXTI line pending bit 
    EXTI_ClearITPendingBit(RF_EXTI_LINE);
  }
*/

/******************* (C) COPYRIGHT 2013 SDK-SYSTEMS *****END OF FILE****/
