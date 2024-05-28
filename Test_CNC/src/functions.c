
 
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
  * @brief      ������������� �������� ������� TIM6
  * @param      tick_value - ����� � ��� ����������������� �������� ������� [* 1 us]
  *             period - ������ ������������ �������
  * @retval     None
  * @note       ������������ ��� ������ ���������
  */
void init_TimerTIM6(uint32_t tick_value, uint32_t period)
{
  // ����������� ������
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  // ��������� ��������
  uint32_t Divisor = (SystemCoreClock / 1000000) * tick_value;   // ������������ ������ [1 us * tick_value]

  // �������������� ������� ������ TIM6: �������� , ������.
  // ������ ��������� ��������� TIM_TimeBaseInitTypeDef �� ����� ������ ��� ������� ��������.
  TIM_TimeBaseInitTypeDef Base_timer;
  TIM_TimeBaseStructInit(&Base_timer);
  
  Base_timer.TIM_Prescaler = Divisor - 1; // �������� ����������� ��� TIM_Prescaler + 1, ������� �������� 1
  Base_timer.TIM_Period = period;
  TIM_TimeBaseInit(TIM6, &Base_timer);

  // ��������� ���������� �� ���������� (� ������ ������ - �� ������������) �������� ������� TIM6
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  
  // �������� ������
  TIM_Cmd(TIM6, ENABLE); 
 
  // ��������� ��������� ���������� �� ������������ �������� ������� TIM6
  NVIC_EnableIRQ(TIM6_DAC_IRQn); 
}

/** 
  * @brief      ������������� �������� ������� TIM7
  * @param      tick_value - ����� � ��� ����������������� �������� ������� [* 1 us]
  *             period - ������ ������������ �������
  * @retval     None
  * @note       ������������ ��� ���������� ���������� ������� ����������
  */
void init_TimerTIM7(uint32_t tick_value, uint32_t period)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  
  uint32_t Divisor = ((SystemCoreClock / 1000000)) * tick_value;   // ������������ ������ [1 us * tick_value]
  
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
  * @brief      ������������� ������� TIM3 ��� �������� Z
  * @param      None
  * @retval     None
  * @note       ������������ ��� ���������� ������������� �������� �������������
  *             �������� 3 (Z)
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
  * @brief      ������������� ������� TIM4 ��� �������� X
  * @param      None
  * @retval     None
  * @note       ������������ ��� ���������� ������������� �������� �������������
  *             ��������  (Z)
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
  * @brief      ������������� ������� TIM2 ��� �������� Y
  * @param      None
  * @retval     None
  * @note       ������������ ��� ���������� ������������� �������� �������������
  *             ��������  (Y)
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
  * @brief      ������������� ������� TIM1 ��� �������� A
  * @param      None
  * @retval     None
  * @note       ������������ ��� ���������� ������������� �������� �������������
  *             ��������  (A)
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
  * @brief      ������������� ������ (������, �������)
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

  /* ������������� ���� C �� ���� � ��������� �� "+" */
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
  Gpio.GPIO_Mode = GPIO_Mode_Out_OD;    // �������� ���������
  Gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &Gpio);

  // ��� ���������
  off(_RELAY_CM);
}

/** 
  * @brief      �������� ��������� �������� ����� ���������
  * @param      None
  * @retval     None
  */
void cpl (void)
{
  BitAction BitVal;
  
  if (TimerCPL == 0) {
    // ������� ����������� LEDBLUE ������ �������
    BitVal = !GPIO_ReadOutputDataBit(LEDS_GPIO_PORT, LEDBLUE) ? Bit_SET : Bit_RESET;
    GPIO_WriteBit(LEDS_GPIO_PORT, LEDBLUE, BitVal);
    TimerCPL = 500;      // = 500 ms = 0.5 s
  }
}

/** 
  * @brief      �������� ������� ���������� � �������� ��������
  * @param      led - ����� �����, ������������� � ������� ����������
  *             period - ������ ������������ � ��
  * @retval     None
  * @note       ���� period = 0, �� ��������� �� ������
  */
void blink (uint16_t led, u16 period)
{
  BitAction BitVal;
  
  if (period == 0) { return; }
  
  if (TimerLED == 0) {
    // ������� ����������� led (LEDBLUE ��� LEDGREEN) 
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
  if ((UARTx[0].ReceivePack) || (UARTx[0].SendPack)) return; // ����� � �������� ������ ��� ��������
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
  * @brief      �������� �����  
  * @param      sym - ���������� ��������� ���� (bb), ������������ � ����� ������
  *             *adr - ��������� �� ������ ��� ���������
  *             len - ������ ����� 
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
  * @brief  ������������ ��������� �������� � ��.
  * @param  del: specifies the delay time length, in 1 ms.
  * @retval None
  */
void delay_1ms (uint del) 
{
  TimeForDelay = del;
  while(TimeForDelay != 0) _WDR(); 
}

/** 
  * @brief      ��������� ���������� � ���� �������� ��������� �����
  * @param      None
  * @retval     �������� ��������� ����� TickCnt
  */
u32 getTickCnt (void) 
{
  u32 Value;

  disableInterrupts();  // Disable general interrupts
  Value = TickCnt;
  enableInterrupts();   // Enable general interrupts
  return Value;
}


