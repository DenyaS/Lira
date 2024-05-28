

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_usart.h"
#include "includes.h"

void TimingDelay_Decrement(void);
void stepMotorsTimers_Decrement(void);
void encodersTimers_Decrement(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//Лира
u8 Sen1SM_state;                //Модуль подачи 
u8 Sen1SM_old_state = 0;
u8 Sen2SM_state;
u8 Sen2SM_old_state = 0;
u8 Sen3SM_state;
u8 Sen3SM_old_state = 0;
u8 SenCM_state;                 //Модуль обрезки
u8 SenCM_old_state = 0;    
u8 SenHome_state;                 //Home
u8 SenHome_old_state = 0; 
u8 SenLimit_state;                 //Limit
u8 SenLimit_old_state = 0; 
u8 SenManAhead_state;                 //Man Ahead
u8 SenManAhead_old_state = 0; 
u8 SenManBehind_state;                 //Man Behind
u8 SenManBehind_old_state = 0; 
u8 SenZahOpen_state;                 //ZahOpen
u8 SenZahOpen_old_state = 0; 
u8 SenZahClose_state;                 //Zah Close
u8 SenZahClose_old_state = 0; 
u8 SenFinAhead_state;                 //FinAhead
u8 SenFinAhead_old_state = 0; 
u8 SenFinBehind_state;                 //FinBehind
u8 SenFinBehind_old_state = 0; 

u8 _state;                 //IndexA
u8 _old_state = 0; 
// Димины
//u8 SenHome_state;
//u8 SenHome_old_state = 0;
//u8 SenLimit_state;
//u8 SenLimit_old_state = 0;
u8 SenAd1_state;
u8 SenAd1_old_state = 0;
u8 SenAd2_state;
u8 SenAd2_old_state = 0;

u8 SigEStop_state;
u8 SigEStop_old_state = 0;
u8 SigEnable_state;
u8 SigEnable_old_state = 0;

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

/** @brief      Обработчик прерывания базового таймера TIM6
  * @param      None
  * @retval     None
  * @note       Обрабатываются входы 
  */
void TIM6_DAC_IRQHandler(void)
{
  /* Димины рудементы
  static u16 ButStart_state = 0;     // Current debounce status
  static u16 ButStop_state = 0;
  static u16 ButUp_state = 0;
  static u16 ButDn_state = 0;
  static u16 ButLeft_state = 0;
  static u16 ButRight_state = 0;
  */
  
  // Disable general interrupts
  //disableInterrupts();  

  // Так как этот обработчик вызывается и для ЦАП, нужно проверять,
  // произошло ли прерывание по переполнению счётчика таймера TIM6.
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
    // Очищаем бит обрабатываемого прерывания
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    
    
    
    
    //======================Обработка датчиков Лира============================
    //Обработка датчика Sen1SM - датчик на входе модуля подачи
    Sen1SM_state = !SEN_SM1;
       if ((Sen1SM_state == 1) && (Sen1SM_old_state == 0)) {
      // Сработал датчик "Sen1SM"
      fl_SEN_SM1 = 1;
    }
    else if ((Sen1SM_state == 0) && (Sen1SM_old_state == 1)) {
      // Погас датчик "Sen1SM"
      fl_SEN_SM1 = 0;
    }
    Sen1SM_old_state = Sen1SM_state;
    //Обработка датчика Sen2SM - датчик на выходе после энкодера
    Sen2SM_state = !SEN_SM2;
       if ((Sen2SM_state == 1) && (Sen2SM_old_state == 0)) {
      // Сработал датчик "Sen2SM"
      fl_SEN_SM2 = 1;
    }
    else if ((Sen2SM_state == 0) && (Sen2SM_old_state == 1)) {
      // Погас датчик "Sen2SM"
      fl_SEN_SM2 = 0;
    }
    Sen2SM_old_state = Sen2SM_state;
    //Обработка датчика Sen3SM - датчик на выходе модуля обрезки
    Sen3SM_state = !SEN_SM3;
       if ((Sen3SM_state == 1) && (Sen3SM_old_state == 0)) {
      // Сработал датчик "Sen3SM"
      fl_SEN_SM3 = 1;
    }
    else if ((Sen3SM_state == 0) && (Sen3SM_old_state == 1)) {
      // Погас датчик "Sen3SM"
      fl_SEN_SM3 = 0;
    }
    Sen3SM_old_state = Sen3SM_state; 
//Обработка датчика SenCM - датчик модуля обрезки
    SenCM_state = !SEN_CM;
       if ((SenCM_state == 1) && (SenCM_old_state == 0)) {
      // Сработал датчик "SenCM"
      fl_SEN_CM = 1;
    }
    else if ((SenCM_state == 0) && (SenCM_old_state == 1)) {
      // Погас датчик "SenCM"
      fl_SEN_CM = 0;
    }
    SenCM_old_state = SenCM_state; 
//Обработка датчика Home 
    SenHome_state = !SEN_Home;
       if ((SenHome_state == 1) && (SenHome_old_state == 0)) {
      // Сработал датчик "Home"
      fl_Home = 1;
      MotorX.State = MRS_Complete;      //Стоп мотор X
      MotorY.State = MRS_Complete;      //Стоп мотор Y
      MotorA.State = MRS_Complete;      //Стоп мотор A
    }
    else if ((SenHome_state == 0) && (SenHome_old_state == 1)) {
      // Погас датчик "Home"
      fl_Home = 0;
    }
    SenHome_old_state = SenHome_state;     
//Обработка датчика Limit 
    SenLimit_state = !SEN_Limit;
       if ((SenLimit_state == 1) && (SenLimit_old_state == 0)) {
      // Сработал датчик "Limit"
      fl_Limit = 1;
      MotorX.State = MRS_Complete;      //Стоп мотор X
      MotorY.State = MRS_Complete;      //Стоп мотор Y
      MotorA.State = MRS_Complete;      //Стоп мотор A
    }
    else if ((SenLimit_state == 0) && (SenLimit_old_state == 1)) {
      // Погас датчик "Limit"
      fl_Limit = 0;
    }
    SenLimit_old_state = SenLimit_state;
    
    //Обработка датчика Man_Ahead 
    SenManAhead_state = !SEN_Man_Ahead;
       if ((SenManAhead_state == 1) && (SenManAhead_old_state == 0)) {
      // Сработал датчик "ManAhead"
      fl_Man_Ahead = 1;
    }
    else if ((SenManAhead_state == 0) && (SenManAhead_old_state == 1)) {
      // Погас датчик "ManAhead"
      fl_Man_Ahead = 0;
    }
    SenManAhead_old_state = SenManAhead_state;
    
      //Обработка датчика Man_Behind 
    SenManBehind_state = !SEN_Man_Behind;
       if ((SenManBehind_state == 1) && (SenManBehind_old_state == 0)) {
      // Сработал датчик "Man_Behind"
      fl_Man_Behind = 1;
    }
    else if ((SenManBehind_state == 0) && (SenManBehind_old_state == 1)) {
      // Погас датчик "Man_Behind"
      fl_Man_Behind = 0;
    }
    SenManBehind_old_state = SenManBehind_state;
    
      //Обработка датчика Zah_Open 
    SenZahOpen_state = !SEN_Zah_Open;
       if ((SenZahOpen_state == 1) && (SenZahOpen_old_state == 0)) {
      // Сработал датчик "Zah_Open"
      fl_Zah_Open = 1;
    }
    else if ((SenZahOpen_state == 0) && (SenZahOpen_old_state == 1)) {
      // Погас датчик "Zah_Open"
      fl_Zah_Open = 0;
    }
    SenZahOpen_old_state = SenZahOpen_state; 
 
       //Обработка датчика Zah_Close 
    SenZahClose_state = !SEN_Zah_Close;
       if ((SenZahClose_state == 1) && (SenZahClose_old_state == 0)) {
      // Сработал датчик "Zah_Close"
      fl_Zah_Close = 1;
    }
    else if ((SenZahClose_state == 0) && (SenZahClose_old_state == 1)) {
      // Погас датчик "Zah_Close"
      fl_Zah_Close = 0;
    }
    SenZahClose_old_state = SenZahClose_state; 
 
       //Обработка датчика Fin_Ahead 
    SenFinAhead_state = !SEN_Fin_Ahead;
       if ((SenFinAhead_state == 1) && (SenFinAhead_old_state == 0)) {
      // Сработал датчик "Sen Fin Ahead"
      fl_Fin_Ahead = 1;
    }
    else if ((SenFinAhead_state == 0) && (SenFinAhead_old_state == 1)) {
      // Погас датчик "Sen Fin Ahead"
      fl_Fin_Ahead = 0;
    }
    SenFinAhead_old_state = SenFinAhead_state;
     
       //Обработка датчика Fin_Behind
    SenFinBehind_state = !SEN_Fin_Behind;
       if ((SenFinBehind_state == 1) && (SenFinBehind_old_state == 0)) {
      // Сработал датчик "Sen Fin_Behind"
      fl_Fin_Behind = 1;
    }
    else if ((SenFinBehind_state == 0) && (SenFinBehind_old_state == 1)) {
      // Погас датчик "Sen Fin_Behind"
      fl_Fin_Behind = 0;
    }
    SenFinBehind_old_state = SenFinBehind_state;
    
        
    
    
      
    
    /*
    //====================== Обработка датчиков ===============================
    // Обработка датчика SenHome 
    SenHome_state = !SEN_HOME_STATE;
    if ((SenHome_state == 1) && (SenHome_old_state == 0)) {
      // Сработал датчик "HOME"
      fl_SenHome = 1;
    }
    else if ((SenHome_state == 0) && (SenHome_old_state == 1)) {
      // Погас датчик "HOME"
      fl_SenHome = 0;
    }
    SenHome_old_state = SenHome_state;

    // Обработка датчика SenLimit 
//    SenLimit_state = !SEN_LIMIT_STATE;
    if ((SenLimit_state == 1) && (SenLimit_old_state == 0)) {
      // Сработал датчик "LIMIT"
      fl_SenLimit = 1;
    }
    else if ((SenLimit_state == 0) && (SenLimit_old_state == 1)) {
      // Погас датчик "LIMIT"
      fl_SenLimit = 0;
    }
    SenLimit_old_state = SenLimit_state;
    
    // Обработка датчика SenAd1 (торможение)
    SenAd1_state = !SEN_AD1_STATE;
    if ((SenAd1_state == 1) && (SenAd1_old_state == 0)) {
      // Сработал датчик "AD1"
      fl_SenAd1 = 1;
    }
    SenAd1_old_state = SenAd1_state;
      
    // Обработка датчика SenAd2 
    SenAd2_state = !SEN_AD2_STATE;
    if ((SenAd2_state == 1) && (SenAd2_old_state == 0)) {
      // Сработал датчик "AD2"
      fl_SenAd2 = 1;
    }
    SenAd2_old_state = SenAd2_state;
    //=========================================================================

    //====================== Обработка кнопок ===============================
    // Обработка кнопки START 
    // Антидребезг
    ButStart_state = (ButStart_state << 1) | BUT_START_STATE | 0x8000; // 0xe000
    if (ButStart_state == 0xfff0) { // 0xf000
      // Нажата и держится нажатой кнопка "START"
      fl_ButStart = 1;
    }
    else if (ButStart_state == 0xffff) {
      // Кнопка "START" не нажата
      fl_ButStart = 0;
    }
    
    // Обработка кнопки STOP 
    ButStop_state = (ButStop_state << 1) | BUT_STOP_STATE | 0xe000; 
    if (ButStop_state == 0xf000) { fl_ButStop = 1; }
    
    // Обработка джойстика UP 
    ButUp_state = (ButUp_state << 1) | BUT_UP_STATE | 0xe000; 
    if (ButUp_state == 0xf000) { fl_ButUp = 1; }

    // Обработка джойстика DOWN 
    ButDn_state = (ButDn_state << 1) | BUT_DN_STATE | 0xe000; 
    if (ButDn_state == 0xf000) { fl_ButDn = 1; }
    
    // Обработка джойстика LEFT 
    ButLeft_state = (ButLeft_state << 1) | BUT_LEFT_STATE | 0xe000; 
    if (ButLeft_state == 0xffff) { fl_ButLeft = 0; }        // кнопка отпущена
    else if (ButLeft_state == 0xf000) { fl_ButLeft = 1; }   // кнопка нажата
    
    // Обработка джойстика RIGHT 
    ButRight_state = (ButRight_state << 1) | BUT_RIGHT_STATE | 0xe000; 
    if (ButRight_state == 0xffff) { fl_ButRight = 0; } 
    else if (ButRight_state == 0xf000) { fl_ButRight = 1; }

    // Обработка сигнала E-STOP 
    SigEStop_state = SIG_ESTOP_STATE;
    if ((SigEStop_state == 1) && (SigEStop_old_state == 0)) {
      // Поступил сигнал "Emergency STOP"
      fl_SigEStop = 1;
    }
    SigEStop_old_state = SigEStop_state;
    
    // Обработка сигнала ENABLE 
//    SigEnable_state = SIG_ENABLE_STATE;
    if ((SigEnable_state == 1) && (SigEnable_old_state == 0)) {
      // Поступил сигнал "ENABLE"
      fl_SigEnable = 1;
    }
    SigEnable_old_state = SigEnable_state;
    */
    //=========================================================================
    stepMotorsTimers_Decrement();
  }
    
  // Enable general interrupts
  //enableInterrupts();   
}

/** @brief      Обработчик прерывания базового таймера TIM7
  * @param      None
  * @retval     None
  * @note       Обрабатываются выходы для управления драйверами шаговых двигателей
  *             2) Название для функции см. в файле "startup_stm32f10x_md_vl.s"
  */
void TIM7_IRQHandler(void)
{
  // Disable general interrupts
  //disableInterrupts();  
  
  // Так как этот обработчик вызывается и для ЦАП, нужно проверять,
  // произошло ли прерывание по переполнению счётчика таймера TIM6.
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
  {
    // Очищаем бит обрабатываемого прерывания
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    
    // Уменьшаем значение таймеров енкодеров
    encodersTimers_Decrement();

    // Уменьшаем значение таймеров шаговых двигателей
    stepMotorsTimers_Decrement();
  }

  // Enable general interrupts
  //enableInterrupts();   
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  * @note   Line0 - interrupt from phase A of EncoderX
  */
void EXTI0_IRQHandler (void)
{
  // Disable general interrupts
  //disableInterrupts();

  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    // Clear the RF_IRO EXTI line pending bit 
    EXTI_ClearITPendingBit(EXTI_Line0);
  }

  // Enable general interrupts
  //enableInterrupts();
}

/**
  * @brief  This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  * @note   Line1 - interrupt from phase B of EncoderX
  */
void EXTI1_IRQHandler (void)
{
  // Disable general interrupts
  //disableInterrupts();

  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    // Clear the RF_IRO EXTI line pending bit 
    EXTI_ClearITPendingBit(EXTI_Line1);
  }

  // Enable general interrupts
  //enableInterrupts();
}

/**
  * @brief  This function handles External lines 5-8 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler (void)
{

  // Disable general interrupts
  //disableInterrupts();

  /* Внешнее прерывание  */
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
    // Clear the RF_IRO EXTI line pending bit 
    EXTI_ClearITPendingBit(EXTI_Line6);
  }

  if(EXTI_GetITStatus(EXTI_Line7) != RESET)
  {
    // Clear the RF_IRO EXTI line pending bit 
    EXTI_ClearITPendingBit(EXTI_Line7);
  }
  
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


