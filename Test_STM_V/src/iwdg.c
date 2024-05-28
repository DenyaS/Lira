 
/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "iwdg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

/*----------------------------------------------------------------------------
 STM32 Independent watchdog setup.
 initializes the IWDG register
 *----------------------------------------------------------------------------*/
#if __IWDG_SETUP
/*__inline static*/ void setup_IWDG (void) 
{
//  RCC->CSR |= (1<<0);                                           // LSI enable, necessary for IWDG
//  while ((RCC->CSR & (1<<1)) == 0);                             // wait till LSI is ready

  IWDG->KR  = 0x5555;       // enable write to PR, RLR
  IWDG->PR  = __IWDG_PR;    // Init prescaler
  IWDG->RLR = __IWDG_RLR;   // Init RLR
  IWDG->KR  = 0xAAAA;       // Reload the watchdog
  IWDG->KR  = 0xCCCC;       // Start the watchdog
} 
#endif /* __IWDG_SETUP */
