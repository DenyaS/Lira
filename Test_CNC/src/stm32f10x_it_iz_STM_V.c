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
  TickCnt++;        // Считаем тики
}

/** [duke] 
  * @brief      Обработчик прерывания базового таймера TIM6
  *                 ==Контраст LCD==
  *                 ==UART0==
  *                 ==UART1==
  * @param      None
  * @retval     None
  */
void TIM6_DAC_IRQHandler(void)
{
  // Disable general interrupts
  //disableInterrupts();  

  // Так как этот обработчик вызывается и для ЦАП, нужно проверять,
  // произошло ли прерывание по переполнению счётчика таймера TIM6.
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
    // Очищаем бит обрабатываемого прерывания
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

    // Инвертиреум состояние вывода V0 (контрастность LCD MTC-S16204 
    GPIO_Write(MTC_GPIO_PORT_V0, GPIO_ReadOutputData(MTC_GPIO_PORT_V0) ^ MTC_V0);
  }
    
  // Enable general interrupts
  //enableInterrupts();   
}


/** [duke] 
  * @brief      Обработчик прерывания базового таймера TIM7
  *                 ==Клавиатура==
  *                 ==Датчики==
  * @param      None
  * @retval     None
  */
void TIM7_IRQHandler(void)
{
  uint16_t d;
  uint8_t e;

  // Disable general interrupts
  //disableInterrupts();  

  // Проверка, произошло ли прерывание по переполнению счётчика таймера TIM7.
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
  {
    // Очищаем бит обрабатываемого прерывания
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    
    //======================= Обработка клавиатуры =============================
    if (TimerKeyboard == 0x00) { 
      //d = GPIO_ReadInputData(KEYBOARD_GPIO_PORT) & 0x00f0;
      /** Внимание! Не работает PB4 - Button "UP":
      ошибочное использование задействованного в плате STM32VLD порта PB4*/
      d = GPIO_ReadInputData(KEYBOARD_GPIO_PORT) & 0x00e0;
      // Отдельно - обработка кнопки [UP] (т.к. ошибся в схеме)
      e = GPIO_ReadInputDataBit(GPIOC, KEY_UP);
      if(d) {
        // Включаем на 5 сек подсветку дисплея
        STimerBacklight = 5;
        if(!fl_KeyOn) { // klavisha ne byla najata
          fl_KeyOn = 1; 
          PreviKey = d; 
          TimerKeyboard = 5;   // время удержания клавиши
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
      /* ----- Отдельно - обработка кнопки [UP] (т.к. ошибся в схеме) ------ */
      if(e) {
        // Включаем на 5 сек подсветку дисплея
        STimerBacklight = 5;
        if(!fl_KeyOn) { // klavisha ne byla najata
          fl_KeyOn = 1; 
          PreviKey = UP; 
          TimerKeyboard = 5;   // время удержания клавиши
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
    
    //====================== Обработка датчиков ===============================
    // Обработка датчика Sen1 
    Sen1_state = SEN_1_STATE;
    if ((Sen1_state == 1) && (Sen1_old_state == 0)) {
      // Нажата кнопка "Открыть переднюю дверь"
      fl_OpenFrontDoor = 1;
    }
    Sen1_old_state = Sen1_state;

    // Обработка датчика Sen2 
    Sen2_state = SEN_2_STATE;
    if ((Sen2_state == 1) && (Sen2_old_state == 0)) {
      // Нажата кнопка "Открыть заднюю дверь"
      fl_OpenRearDoor = 1;
    }
    Sen2_old_state = Sen2_state;
    
    // Обработка датчика Sen3 
    Sen3_state = SEN_3_STATE;
    if ((Sen3_state == 1) && (Sen3_old_state == 0)) {
      // Сработал датчик открытия двери
      fl_OpenCar = 1;
    }
    Sen3_old_state = Sen3_state;
      
    // Обработка датчика Sen4 
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
  * @brief      Обработчик прерываний от SPI1
  * @param      None
  * @retval     None
  */
void SPI1_IRQHandler (void) 
{
  // Disable general interrupts
  //disableInterrupts();  

  if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == SET) 
  { // Прерывание вызвано передачей байта ?
    _NOP(); // ... Делаем что надо
    // Clear the SPIx CRC Error flag
    SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_TXE);
  }
  
  if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET) 
  { // Прерывание вызвано приемом байта ?
    //data_SPI = SPI1->DR; // Читаем то что пришло
    _NOP(); // ... Делаем что надо
    //SPI1->DR = data_SPI; // И отправляем обратно то что приняли
    // Clear the SPIx CRC Error flag
    SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
  }

  /*
  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) == SET) 
  { // Прерывание вызвано передачей байта ?
    _NOP(); // ... Делаем что надо
    // Clear the SPIx CRC Error flag
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_TXE);
  }

  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET) 
  { // Прерывание вызвано приемом байта ?
    _NOP(); // ... Делаем что надо
    // Clear the SPIx CRC Error flag
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
  }
  */
  // Enable general interrupts
  //enableInterrupts();   
}

/** [duke] 
  * @brief      Обработчик прерываний UART0
  * @param      None
  * @retval     None
  */
void USART1_IRQHandler (void) 
{
  uchar c;
  
  // Disable general interrupts
  disableInterrupts(); //====DEBUG HANG====
  
  //---------- Прерывание по завершению передачи байта UART0 (USART1) -----------
  if(USART_GetITStatus(USART1, USART_IT_TC) == SET)  
  { // Байт готов к передаче (Transmission complete interrupt)
    if(UARTx[0].SendPack)
    {
      if(UARTx[0].TxCnt) 
      { // Весь пакет ещё не передан
#ifdef __BYTESTUFFING__ // Обработка байтстаффинга на передачу
// При передаче байт 0x7e заменяется на последовательность 0x7d 0x5e
// а байт 0x7d заменяется на последовательность 0x7d 0x5d
        if(fl_ByteStuf) 			
        {	
          fl_ByteStuf = 0;			            // Сбрасываем флаг байтстаффинга
          USART1->DR = *UARTx[0].Ptr++ ^ 0x20;	// Накладываем на передаваемый байт маску XOR, 
                                                // Получаем из 0x7e -> 0x5e, из 0x7d -> 0x5d
                                                // Сдвигаем указатель [Ptr]
          UARTx[0].TxCnt--;                     // Уменьшаем счётчик передаваемых байт [TxCnt]
          goto complete;						// Конец передачи байта			
        } 
        else 
        { 
          if((*UARTx[0].Ptr == 0x7e) || (*UARTx[0].Ptr == 0x7d))  // если хотим передать байт 0x7e или 0x7d
          {
            fl_ByteStuf = 1;		// Выставляем флаг байстаффинга
            USART1->DR = 0x7d;		// и подменяем символ на 0x7d
                                    // счётчик байт не меняем, указатель не сдвигаем 
			goto complete;          // выход - берем опять этот же символ
          } 
        }
#endif  // Конец обработки байтстаффинга на передачу
        UARTx[0].TxCnt--;				// Декрементируем счетчик байт для выходного пакета
		USART1->DR = *UARTx[0].Ptr++;	// Записываем в регистр [DR] передаваемый байт
                                        // Сдвигаем указатель [Ptr]      
      }
      else
      { // Пакет передан 
        UARTx[0].SendPack = 0;
      }
    }
  } //---- передача байта завершена ----

  //---------- Прерывание по завершению приёма байта UART0 (USART1) -----------
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  { // Receive Data register not empty interrupt
	//c = USART1->DR;						// Читаем символ из регистра
    c = USART_ReceiveData(USART1);
	if(UARTx[0].RxComplete) { goto complete; }
	if(UARTx[0].ReceivePack) 
    { // Прием пакета не окончен
      UARTx[0].Timer = 20;     // 20 ms
#ifdef __BYTESTUFFING__  // Обработка байтстаффинга по приёму
// При приёме последовательность 0x7d 0x5e заменяется на байт 0x7e
// а последовательность 0x7d 0x5d заменяется на байт 0x7d
      if(fl_ByteStuf)			// Если установлен флаг байтстаффинга 
      {
        fl_ByteStuf = 0;	    // то сбрасываем его
		c = c ^ 0x20;		    // накладываем на принятый байт маску XOR
                                // и получаем из 0x5e -> 0x7e, из 0x5d -> 0x7d
      } 
      else 
      {
        if(c == 0x7d)		    // если получен байт 0x7d, то начинаем обработку байтстафиинга
		{ 
          fl_ByteStuf = 1;	    // устанавливаем флаг байтстаффинга
          goto complete;		// и выходим - этот байт больше не обрабатывается
								// следующий будет либо 0x5e, либо 0x5d
		}
      }
#endif  // Конец обработки байтстаффинга по приёму
      // Длина пакета = размер данных (содержится в байте 2) + 
      // + ADR_DST + ADR_SRC + SIZE + CMD + 2 байта CRC16
      if(UARTx[0].RxCnt == 2) { UARTx[0].RxSize = c + 6; }  
      if(UARTx[0].RxSize >= UART_BUF_SIZE) 
      { // Если размер пакета превышает размер буфера
        //UARTx[0].ReceivePack = 0;		// Заканчиваем приём
		UARTx[0].Timer = 0;
      }
      UARTx[0].InBuf[UARTx[0].RxCnt++] = c;     // Увеличиваем RxCnt
      if(UARTx[0].RxCnt >= UARTx[0].RxSize) 
      {
        //UARTx[0].ReceivePack = 0;		// Заканчиваем приём пакета
        //UARTx[0].RxComplete = 1;
        UARTx[0].Timer = 0;
      }
	} 
    else 
    { // Получен флаг начала пакета - начинаем принимать новый пакет
      if(c == FLAG_PACKET) 
      {
        UARTx[0].ReceivePack = 1;					
		UARTx[0].RxSize	= HEAD_PACKET;          
		UARTx[0].RxCnt = 0;
		UARTx[0].Timer = 20;       // 20 ms
      }
	}
    if (UARTx[0].Timer == 0) {
      UARTx[0].ReceivePack = 0;		// Заканчиваем приём пакета
      UARTx[0].RxComplete = 1;      // Пакет принят
      checkPacket_UARTx(0);
    }
  } //---- приём байта завершен ----

complete:   
  // Сбрасываем бит прерывания TC (бит RXNE сбрасывается после чтения Data Register)
  USART_ClearITPendingBit(USART1, USART_IT_TC);        
    
  // Enable general interrupts
  enableInterrupts();   //====DEBUG HANG====
}

/** [duke] 
  * @brief      Обработчик прерываний UART1
  * @param      None
  * @retval     None
  */
void USART2_IRQHandler (void) 
{
  uchar c;
  
  // Disable general interrupts
  disableInterrupts();  //====DEBUG HANG====
  
  //---------- Прерывание по завершению передачи байта -----------
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
#endif  // Конец обработки байтстаффинга на передачу
        UARTx[1].TxCnt--;				
		USART2->DR = *UARTx[1].Ptr++;	
      }
      else
      { // Пакет передан 
        UARTx[1].SendPack = 0;
        off(TX485);           // Выключили передатчик RS-485  
      }
    }
  } //---- передача байта завершена ----

  //---------- Прерывание по завершению приёма байта UART1 (USART2) -----------
  if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
  { 
    c = USART_ReceiveData(USART2);
	if(UARTx[1].RxComplete) { goto complete; }
	if(UARTx[1].ReceivePack) 
    { // Прием пакета не окончен
      UARTx[1].Timer = 20;     
#ifdef __BYTESTUFFING__  // Обработка байтстаффинга по приёму
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
#endif  // Конец обработки байтстаффинга по приёму
      if(UARTx[1].RxCnt == 2) { UARTx[1].RxSize = c + 6; }  
      if(UARTx[1].RxSize >= UART_BUF_SIZE) 
      { // Если размер пакета превышает размер буфера
		UARTx[1].Timer = 0;
      }
      UARTx[1].InBuf[UARTx[1].RxCnt++] = c;     // Увеличиваем RxCnt
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
      UARTx[1].ReceivePack = 0;		// Заканчиваем приём пакета
      UARTx[1].RxComplete = 1;      // Пакет принят
      checkPacket_UARTx(1);
    }
  } //---- приём байта завершен ----

complete:   
  // Сбрасываем бит прерывания TC (бит RXNE сбрасывается после чтения Data Register)
  USART_ClearITPendingBit(USART2, USART_IT_TC);        

  // Enable general interrupts
  enableInterrupts();   //====DEBUG HANG====
}

/** [duke] 
  * @brief      Обработчик прерываний I2C1
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
  /* Внешнее прерывание от радиомодуля MRF49XA (Line 8) */
  if(EXTI_GetITStatus(RF_EXTI_LINE) != RESET)
  {
    ENABLE_RF();      // CS active
    
    // Включаем светодиод
    on(LED_RF);
    GPIO_ResetBits(OUTPUTS_GPIO_PORT, LED_WORK);

    // ==== DEBUG ====
    //getRadioStatus(CS_YES);
    //if (RF.STSREG.TXRXFIFO)
    if (GPIO_ReadInputDataBit(RSDO) == 1)
    { // Режим передачи: Регистр передатчика готов к приёму следующего байта для передачи
      // Режим приёма: Есть принятые данные в FIFO, читаем байт из FIFO
      if (Radio.TransmitMode)
      {
//------------------ Режим передачи ---------------------------
        if(Radio.TxCnt < Radio.TxLen) 
        { // не все байты пакета ещё переданы
          RF.TXBREG.TXDB = Radio.TxBuf[Radio.TxCnt++];
          sendData_MRF49XA(RF.TXBREG.HalfWord, CS_YES);
        }
        else { // Все байты пакета переданы
          Radio.Transmit = 0;
          setRadioMode(RECEIVE);      // Устанавливаем режим приёма
        }  
      }
//------------------ Режим приема -----------------------------
      else          // Radio.TransmitMode == 0
      {
        Bt = readFIFO_MRF49XA();
        if (Radio.Receive) { // Пакет принимается
          if(Radio.RxCnt < Radio.RxLen) { // Есть ещё не вычитанные байты
            Radio.RxBuf[Radio.RxCnt++] = Bt;  // Записываем принятый байт в буфер
          }                                   // Инкрементируем счётчик байт
          else {  // Все байты пакета вычитаны
            Radio.Receive = 0;				// Завершаем приём пакета
            Radio.ReceiveComplete = 1;		// Пакет принят
            // Запрещаем FIFO
            sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
            // ==== DEBUG ====
            // Проверка принятого пакета   
            checkPacket_MRF49XA();
            NumsOfIPackets++;
            //================
          }
        }
        else { // Пакет ещё не начинал приниматься 
          if (Bt <= DATASIZE) { // Читаем первый байт пакета (DSize)
            Radio.DisChange = 1;
            Radio.Receive = 1;		// Устанавливаем флаг приёма пакета
            Radio.RxLen = (uint)Bt + sizeof(PackHeader_TypeDef) + 2; // Считаем количество байт в пакете
            Radio.RxCnt = 0;
            Radio.RxBuf[Radio.RxCnt++] = Bt; // Записываем в буфер и инкрементируем счётчик
          }
          else { // Неверное значение размера поля данных пакета
            Radio.DisChange = 0;
            sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);  // Запрет FIFO
            RF_SYNC_RESTART();        // Перезапускаем детектор синхропоследовательности
          }
        }
      }
    }
    else {    // Радиомодуль не готов ни к приёму, ни к передаче
      DISABLE_RF();
    }
    // Прерывание от MRF49XA закончено: байт принят или передан   
    getRadioStatus(CS_YES);

    // Выключаем светодиод
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
  * @brief  Черновик - прерывания от микросхемы MRF49XA
  */ 
/*
// Внешнее прерывание от радиомодуля MRF49XA (Line 8) 
  if(EXTI_GetITStatus(RF_EXTI_LINE) != RESET)
  {

	// Если нет сигнала прерывания на входе - выход
    if (!signal(IRQ, I)) goto exticomplete; // зачем? если мы сюда попали, значит сигнал был
	// Читаем регистр статуса радиомодуля
    getRadioStatus(1);

extitransmit:
//---------------------- В режиме передачи -------------------------------
	if (Radio.TransmitMode) 
    {
      // вначале пересылаем флаг пакета (он уже записан в регистр передачи)
      if(fl_StartPacket) {  
        fl_StartPacket = 0;
        sendData_MRF49XA(RF.TXBREG.HalfWord);
        goto extitransmit;
      } 
      if (RF.STSREG.TXRXFIFO)
      { // Регистр передатчика готов к приёму следующего байта для передачи
        if (Radio.TxCnt >= Radio.TxLen) {
          // Все байты пакета переданы
          // Устанавливаем режим приёма
          setRadioMode(RECEIVE);
          goto exticomplete;
		}

#ifdef BYTESTUFFING // Обработка байтстаффинга на передачу
// При передаче байт 0x7e заменяется на последовательность 0x7d 0x5e
// а байт 0x7d заменяется на последовательность 0x7d 0x5d
        if(fl_ByteStuf) 			
        {	
          fl_ByteStuf = 0;			         // Сбрасываем флаг байтстаффинга
          // Накладываем на передаваемый байт маску XOR                                      
          // Получаем из 0x7e -> 0x5e, из 0x7d -> 0x5d 
          Radio.TxBuf[Radio.TxCnt] ^= 0x20;	 
        } 
        else 
        { 
          if((Radio.TxBuf[Radio.TxCnt] == 0x7e) 
             || (Radio.TxBuf[Radio.TxCnt] == 0x7d))  // если хотим передать байт 0x7e или 0x7d
          {
            fl_ByteStuf = 1;		// Выставляем флаг байстаффинга
            RF.TXBREG.TXDB = 0x7d;	// и подменяем символ на 0x7d
                                    // счётчик байт не меняем 
			sendData_MRF49XA(RF.TXBREG.HalfWord);
            goto extitransmit;		// возврат - берем опять этот же символ
          } 
        }
#endif  // Конец обработки байтстаффинга на передачу

		RF.TXBREG.TXDB = Radio.TxBuf[Radio.TxCnt++];
		sendData_MRF49XA(RF.TXBREG.HalfWord);
      }
      goto extitransmit;
	}

//------------------------ В режиме приёма ---------------------------------
	else if (RF.STSREG.TXRXFIFO) 
    { // Есть принятые данные в FIFO, читаем байт из FIFO
      Bt = readData_MRF49XA();
      if (Radio.Receive) {
        if (Radio.RxCnt >= Radio.RxLen) {
          Radio.Receive = 0;				// Завершаем приём пакета
          Radio.ReceiveComplete = 1;		// Пакет принят
          // Перезапускаем детектор сихропоследовательности
          RF_SYNC_RESTART();
		}
		Radio.RxBuf[Radio.RxCnt++] = Bt;
      } 
      else {
        if (Bt <= DATASIZE) {
          Radio.Receive = 1;				// Устанавливаем флаг приёма пакета
          Radio.RxLen = (uint)Bt + sizeof(PackHeader_TypeDef) + 2;
          Radio.RxCnt = 0;
          Radio.RxBuf[Radio.RxCnt++] = Bt;
		} 
        else {
          // Перезапускаем детектор сихропоследовательности
          RF_SYNC_RESTART();
		}
      }
	}

exticomplete:   // прерывание завершено - пакет передан или принят    

    off (LED);
    // Clear the RF_IRO EXTI line pending bit 
    EXTI_ClearITPendingBit(RF_EXTI_LINE);
  }
*/

/******************* (C) COPYRIGHT 2013 SDK-SYSTEMS *****END OF FILE****/
