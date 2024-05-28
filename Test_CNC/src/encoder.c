
 
/* Includes ------------------------------------------------------------------*/
#include "encoder.h"
#include "includes.h"
#include "stm32f10x_it.h"

/* Private Defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Функция инициализации входов для подключения енкодеров
  * @param  None
  * @retval None
  */
void init_Encoders (void)
{
    /* Enable the GPIOs clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);
  
  /* Прерывания - это альтернативная функция порта 
  поэтому надо необходимо включить тактирование AFIO */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  
 
  
 //----------Инициализация порта A--------------------
  
    GPIO_InitTypeDef GpioA;
                      
  /* Fill each [gpio] with its default value */
  GPIO_StructInit(&GpioA);            

  /* Конфигурируем порт A */
  GpioA.GPIO_Pin = ENCY_A | ENCY_B | ENCA_A | ENCA_B | ENCY_A | ENCY_B | ENCA_A | ENCA_B;     
  GpioA.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GpioA.GPIO_Speed = GPIO_Speed_2MHz/*GPIO_Speed_50MHz*/;
  GPIO_Init(GPIOA, &GpioA);
  
  //----------Инициализация порта B--------------------
  
    GPIO_InitTypeDef GpioB;

  /* Fill each [gpio] with its default value */
  GPIO_StructInit(&GpioB);            

  /* Конфигурируем порт B */
  GpioB.GPIO_Pin = ENCX_A | ENCX_B;      
  GpioB.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GpioB.GPIO_Speed = GPIO_Speed_2MHz/*GPIO_Speed_50MHz*/;
  GPIO_Init(GPIOB, &GpioB);
  
      //----------Инициализация порта C--------------------
  
  GPIO_InitTypeDef GpioC;
                          
  /* Fill each [gpio] with its default value */
  GPIO_StructInit(&GpioC);            

  /* Конфигурируем порт С */
  GpioC.GPIO_Pin = ENCZ_A | ENCZ_B | ENCZ_A | ENCZ_B | ENCX_Z | ENCY_Z | ENCZ_Z | ENCA_Z;      
  GpioC.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GpioC.GPIO_Speed = GPIO_Speed_2MHz/*GPIO_Speed_50MHz*/;
  GPIO_Init(GPIOC, &GpioC);
  
  // Map TIM3 to GPIOC
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); // PC.06 TIM3_CH1, PC.07 TIM3_CH2
  // Remap TIM4 не нужно делать Remap т.к. он и так на PB6 и PB7
  //  Remap TIM2 не нужно делать Remap т.к. он и так на PA0 и PA1 
  // Remap TIM1 не нужно делать Remap т.к. он и так на PA8 и PA9 
  /* Deinitialize the EXTI peripheral registers to their default reset values */
  EXTI_DeInit();
}

/** 
  * @brief      Запретить прерывания и дать значение тиков енкодера
  * @param      num - номер енкодера (0...3)
  *             name - название фазы енкодера (0 = A, 1 = B)
  *             zero - обнулить значение (1), не обнулять (0)
  * @retval     Значение тиков 
  */
u32 getTickEnc (u8 num, u8 name, u8 zero) 
{
  u32 Value = 0;

  disableInterrupts();  // Disable general interrupts
  if (name == _A) {
    if (zero == SET_TO_ZERO) { Value = Enc[num].TickA = 0; }
    else if (zero == DONT_ZERO) { Value = Enc[num].TickA; }
  }
  else if (name == _B) {
    if (zero == SET_TO_ZERO) { Value = Enc[num].TickB = 0; }
    else if (zero == DONT_ZERO)  { Value = Enc[num].TickB; }
  }
  enableInterrupts();   // Enable general interrupts
  return Value;
}



