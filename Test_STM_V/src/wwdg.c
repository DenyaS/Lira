/**
  ******************************************************************************
  * @file    wwdg.c
  * @author  Dmitri Kobylyaev aka [duke]
  * @version V 1.0.1
  * @date    APR-2014
  * @brief   Functions for Window WatchDog timer
  ******************************************************************************
  * COPYRIGHT 2014 SDK-SYSTEMS ODESSA
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include <stm32f10x_wwdg.h>
#include "wwdg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

/** 
  * @brief      Инициализация WatchDog таймера
  * @param      ms - время срабатывания тамера в милисекундах
  * @retval     None
  */
void init_WWDG (uint ms)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Set WWDG interrupt vector */
  NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable WWDG clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
  
  /* WWDG clock counter = (PCLK1 (24MHz)/4096)/8 = 732 Hz (~1366 µs)  */
  WWDG_SetPrescaler(WWDG_Prescaler_8);

  /* Set Window value to 65 */
  WWDG_SetWindowValue(0x80);

  /* Enable WWDG and set counter value to 127, WWDG timeout = ~1366 µs * 64 = 87.42 ms */
  WWDG_Enable(0x7f);

  /* Clear EWI flag */
  WWDG_ClearFlag();

  /* Enable EW interrupt */
  WWDG_EnableIT();
}

/** 
  * @brief      Сброс WatchDog таймера
  * @param      None
  * @retval     None
  */
void update_WWDG (void)
{
  /* Update WWDG counter */
  WWDG_SetCounter(0x7F);    
  /* Clear EWI flag */
  WWDG_ClearFlag();
}

/******************* (C) COPYRIGHT 2014 SDK-SYSTEMS *****END OF FILE****/
