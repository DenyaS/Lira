
 
/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include "functions.h"
#include "includes.h"
#include "stm32f10x_it.h"
#include "stm32f10x_usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

/** 
  * @brief      Инициализация базового таймера TIM6
  * @param      tick_value - время в мкс инкрементирования счётчика таймера [* 1 us]
  *             period - период срабатывания таймера
  * @retval     None
  * @note       Используется для опроса енкодеров
  */
void init_TimerTIM6(uint32_t tick_value, uint32_t period)
{
  // Затактируем таймер
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  // Вычисляем делитель
  uint32_t Divisor = (SystemCoreClock / 1000000) * tick_value;   // срабатывание каждые [1 us * tick_value]

  // Инициализируем базовый таймер TIM6: делитель , период.
  // Другие параметры структуры TIM_TimeBaseInitTypeDef не имеют смысла для базовых таймеров.
  TIM_TimeBaseInitTypeDef Base_timer;
  TIM_TimeBaseStructInit(&Base_timer);
  
  Base_timer.TIM_Prescaler = Divisor - 1; // Делитель учитывается как TIM_Prescaler + 1, поэтому отнимаем 1
  Base_timer.TIM_Period = period;
  TIM_TimeBaseInit(TIM6, &Base_timer);

  // Разрешаем прерывание по обновлению (в данном случае - по переполнению) счётчика таймера TIM6
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  
  // Включаем таймер
  TIM_Cmd(TIM6, ENABLE); 
 
  // Разрешаем обработку прерывания по переполнению счётчика таймера TIM6
  NVIC_EnableIRQ(TIM6_DAC_IRQn); 
}

/** 
  * @brief      Инициализация базового таймера TIM7
  * @param      tick_value - время в мкс инкрементирования счётчика таймера [* 1 us]
  *             period - период срабатывания таймера
  * @retval     None
  * @note       Используется для управления драйверами шаговых двигателей
  */
void init_TimerTIM7(uint32_t tick_value, uint32_t period)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  
  uint32_t Divisor = ((SystemCoreClock / 1000000)) * tick_value;   // срабатывание каждые [1 us * tick_value]
  
  TIM_TimeBaseInitTypeDef Base_timer;
  TIM_TimeBaseStructInit(&Base_timer);
  
  Base_timer.TIM_Prescaler = Divisor - 1; 
  Base_timer.TIM_Period = period;
  TIM_TimeBaseInit(TIM7, &Base_timer);

  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  
  TIM_Cmd(TIM7, ENABLE); 
 
  NVIC_EnableIRQ(TIM7_IRQn); 
}

/** 
  * @brief      Инициализация таймера TIM3 для енкодера Z
  * @param      None
  * @retval     None
  * @note       Используется для управления декодирования сигналов квадратурного
  *             енкодера 3 (Z)
  *             TIM3->CNT contains current position
  */
void init_TimerTIM3 (void)
{
    TIM_TimeBaseInitTypeDef Timer;
 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    // clock for TIM3
    
    Timer.TIM_Prescaler = 0;
    Timer.TIM_Period = 65535;               // Maximal
    Timer.TIM_ClockDivision = TIM_CKD_DIV1;
    Timer.TIM_CounterMode = TIM_CounterMode_Up;
 
    TIM_TimeBaseInit(TIM3, &Timer);
 
    
    // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
     
    TIM_Cmd(TIM3, ENABLE);  
}
/** 
  * @brief      Инициализация таймера TIM4 для енкодера X
  * @param      None
  * @retval     None
  * @note       Используется для управления декодирования сигналов квадратурного
  *             енкодера  (Z)
  *             TIM4->CNT contains current position
  */
void init_TimerTIM4 (void)
{
    TIM_TimeBaseInitTypeDef Timer;
 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);    // clock for TIM4
    
    Timer.TIM_Prescaler = 0;
    Timer.TIM_Period = 65535;               // Maximal
    Timer.TIM_ClockDivision = TIM_CKD_DIV1;
    Timer.TIM_CounterMode = TIM_CounterMode_Up;
 
    TIM_TimeBaseInit(TIM4, &Timer);
 
    
    // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
     
    TIM_Cmd(TIM4, ENABLE);  
}
/** 
  * @brief      Инициализация таймера TIM2 для енкодера Y
  * @param      None
  * @retval     None
  * @note       Используется для управления декодирования сигналов квадратурного
  *             енкодера  (Y)
  *             TIM2->CNT contains current position
  */
void init_TimerTIM2 (void)
{
    TIM_TimeBaseInitTypeDef Timer;
 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    // clock for TIM2
    
    Timer.TIM_Prescaler = 0;
    Timer.TIM_Period = 65535;               // Maximal
    Timer.TIM_ClockDivision = TIM_CKD_DIV1;
    Timer.TIM_CounterMode = TIM_CounterMode_Up;
 
    TIM_TimeBaseInit(TIM2, &Timer);
 
    
    // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
     
    TIM_Cmd(TIM2, ENABLE);  
}
/** 
  * @brief      Инициализация таймера TIM1 для енкодера A
  * @param      None
  * @retval     None
  * @note       Используется для управления декодирования сигналов квадратурного
  *             енкодера  (A)
  *             TIM1->CNT contains current position
  */
void init_TimerTIM1 (void)
{
    TIM_TimeBaseInitTypeDef Timer;
 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);    // clock for TIM1 
    
    Timer.TIM_Prescaler = 0;
    Timer.TIM_Period = 65535;               // Maximal
    Timer.TIM_ClockDivision = TIM_CKD_DIV1;
    Timer.TIM_CounterMode = TIM_CounterMode_Up;
 
    TIM_TimeBaseInit(TIM1, &Timer);
 
    
    // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    TIM_ITConfig(TIM1,TIM_IT_Update , ENABLE); 
     
    TIM_Cmd(TIM1, ENABLE);  
}

/** 
  * @brief      Инициализация входов (кнопки, датчики)
  * @param      None
  * @retval     None
  */
void init_Inputs (void)
{
  GPIO_InitTypeDef Gpio;

  /* Enable the GPIOs clocks - Port C */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Fill each GPIO with its default value */
  GPIO_StructInit(&Gpio);            

  /* Конфигурируем порт C на вход с подтяжкой на "+" */
  Gpio.GPIO_Pin = Sen1SM | Limit | CloseZah | OpenZah | FingerAhed | FingerBeh;  
  Gpio.GPIO_Mode = GPIO_Mode_IPU;
  Gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &Gpio);
  
  /* Config Port A */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_StructInit(&Gpio);            
  Gpio.GPIO_Pin = Sen2SM | Sen3SM | SenCM | Home | Man_Ahead | Man_Behind;  
  Gpio.GPIO_Mode = GPIO_Mode_IPU;
  Gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &Gpio);
}

/**
  * @brief  Configures the general purpose outputs 
  * @param  None
  * @retval None
  */
void init_Outputs (void)
{
  GPIO_InitTypeDef Gpio;

  // Port B
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  Gpio.GPIO_Pin = RELAY_CM | Vid_Man;
  Gpio.GPIO_Mode = GPIO_Mode_Out_OD;    // открытый коллектор
  Gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &Gpio);

  // Все выключаем
  off(_RELAY_CM);
}

/** 
  * @brief      Тестовый индикатор главного цикла программы
  * @param      None
  * @retval     None
  */
void cpl (void)
{
  BitAction BitVal;
  
  if (TimerCPL == 0) {
    // Мигание светодиодом LEDBLUE каждую секунду
    BitVal = !GPIO_ReadOutputDataBit(LEDS_GPIO_PORT, LEDBLUE) ? Bit_SET : Bit_RESET;
    GPIO_WriteBit(LEDS_GPIO_PORT, LEDBLUE, BitVal);
    TimerCPL = 500;      // = 500 ms = 0.5 s
  }
}

/** 
  * @brief      Моргание зелёного светодиода с заданным периодом
  * @param      led - ножка порта, подключенного к нужному светодиоду
  *             period - период подмигивания в мс
  * @retval     None
  * @note       Если period = 0, то светодиод не мигает
  */
void blink (uint16_t led, u16 period)
{
  BitAction BitVal;
  
  if (period == 0) { return; }
  
  if (TimerLED == 0) {
    // Мигание светодиодом led (LEDBLUE или LEDGREEN) 
    BitVal = !GPIO_ReadOutputDataBit(LEDS_GPIO_PORT, led) ? Bit_SET : Bit_RESET;
    GPIO_WriteBit(LEDS_GPIO_PORT, led, BitVal);
    TimerLED = period;      
  }
}

/**  
  * @brief      Writes output to the consol stream, under control of the string pointed to by
  *             format that specifies how subsequent arguments are converted for output. If 
  *             there are insufficient arguments for the format, the behavior is undefined.
  *             If the format is exhausted while arguments remain, the excess arguments are
  *             ignored.
  * @param      pcFmt - Format string. It can contain all the format specifies.
  *             ...   - Are the passed parameters (pointers to the objects to receive the
  *                      converted input).
  * @retval     None
  */
void printf_Consol(const char *pcFmt, ...)
{
#if __DEBUGPORT > 1
  return;
#endif

#if __DEBUGPORT == 0
  if ((UARTx[0].ReceivePack) || (UARTx[0].SendPack)) return; // Пакет в процессе приема или передачи
#elif __DEBUGPORT == 1 
  if ((UARTx[1].ReceivePack) || (UARTx[1].SendPack)) return; 
#endif
  
  va_list ap;
  char pbString[256];
  
  va_start(ap, pcFmt);
  vsprintf(pbString, pcFmt, ap);

#if __DEBUGPORT == 0
  sendStr_UART0(pbString);
#elif __DEBUGPORT == 1 
  sendStr_UART1(pbString);
#endif

  va_end(ap);
}

/** 
  * @brief      Просмотр дампа  
  * @param      sym - количество симвволов байт (bb), отображаемых в одной строке
  *             *adr - указатель на данные для просмотра
  *             len - размер дампа 
  * @retval     None
  */
void viewDump_Consol (uchar sym, uchar *adr, uint len) 
{
	uchar i = 0;
	uchar *Ptr = adr;

	if(!len) return;
	do {
      //_WDR();													
      if((++i % (sym + 1)) == 0)
      {	
        i = 1;	
        printf_Consol((const char *) "\r\n");	
      }
      printf_Consol((const char *) "%02x ", *Ptr++);
	} while(--len);
	printf_Consol((const char *) "\r\n");
}

/**
  * @brief  Формирование временной задержки в мс.
  * @param  del: specifies the delay time length, in 1 ms.
  * @retval None
  */
void delay_1ms (uint del) 
{
  TimeForDelay = del;
  while(TimeForDelay != 0) _WDR(); 
}

/** 
  * @brief      Запретить прерывания и дать значение системных тиков
  * @param      None
  * @retval     Значение системных тиков TickCnt
  */
u32 getTickCnt (void) 
{
  u32 Value;

  disableInterrupts();  // Disable general interrupts
  Value = TickCnt;
  enableInterrupts();   // Enable general interrupts
  return Value;
}


