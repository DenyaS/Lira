

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

/* Includes ----------------------------------------------------------------- */
#include "stm32F10x.h" 
#include "unsigned.h"

/* Define ------------------------------------------------------------------- */
#define LEDS           LEDBLUE | LEDGREEN

// Определение активных уровней выходов MCU
#define HIGH    1
#define LOW     0
// Датчики Лира модуль выравнивания
#define SEN_SM1             GPIO_ReadInputDataBit(GPIOC, Sen1SM)
#define SEN_SM2             GPIO_ReadInputDataBit(GPIOA, Sen2SM)
#define SEN_SM3             GPIO_ReadInputDataBit(GPIOA, Sen3SM)
//Датчик модуля обрезки
#define SEN_CM              GPIO_ReadInputDataBit(GPIOA, SenCM)  
//Home and Limit
#define SEN_Home            GPIO_ReadInputDataBit(GPIOA, Home) 
#define SEN_Limit           GPIO_ReadInputDataBit(GPIOC, Limit)
//Датчики пневмо цилиндров выдвижение захват палец
#define SEN_Man_Ahead        GPIO_ReadInputDataBit(GPIOA, Man_Ahead)
#define SEN_Man_Behind       GPIO_ReadInputDataBit(GPIOA, Man_Behind)
#define SEN_Zah_Open         GPIO_ReadInputDataBit(GPIOC, OpenZah)
#define SEN_Zah_Close        GPIO_ReadInputDataBit(GPIOC, CloseZah)
#define SEN_Fin_Ahead        GPIO_ReadInputDataBit(GPIOC, FingerAhed)    
#define SEN_Fin_Behind       GPIO_ReadInputDataBit(GPIOC, FingerBeh)
//Выходы Лира
#define _RELAY_CM               GPIOB, RELAY_CM,HIGH    //Двигатель обрезки  
#define _RELAY_MAN              GPIOB, Vid_Man,HIGH     //Пневмоклапан выдвижения манипулятора
#define _RELAY_ZAH              GPIOB, ZahvMan,HIGH     //Пневмоклапан захвата манипулятора   
#define _RELAY_FIN              GPIOB, FingerMan,HIGH     //Пневмоклапан пальца манипулятора 
    
//Датчики Димы 
#define SEN_HOME_STATE      GPIO_ReadInputDataBit(INPUTS_GPIO_PORT, SEN_HOME)
#define SEN_LIMIT_STATE     GPIO_ReadInputDataBit(INPUTS_GPIO_PORT, SEN_LIMIT)
#define SEN_AD1_STATE       GPIO_ReadInputDataBit(INPUTS_GPIO_PORT, SEN_AD1)
#define SEN_AD2_STATE       GPIO_ReadInputDataBit(INPUTS_GPIO_PORT, SEN_AD2)

#define BUT_UP_STATE        GPIO_ReadInputDataBit(JOYSTICK_GPIO_PORT, JOYSTICK_UP)
#define BUT_DN_STATE        GPIO_ReadInputDataBit(JOYSTICK_GPIO_PORT, JOYSTICK_DN)
#define BUT_LEFT_STATE      GPIO_ReadInputDataBit(JOYSTICK_GPIO_PORT, JOYSTICK_LEFT)
#define BUT_RIGHT_STATE     GPIO_ReadInputDataBit(JOYSTICK_GPIO_PORT, JOYSTICK_RIGHT)

#define BUT_START_STATE     GPIO_ReadInputDataBit(INPUTS_GPIO_PORT, BUT_START)
#define BUT_STOP_STATE      GPIO_ReadInputDataBit(INPUTS_GPIO_PORT, BUT_STOP)

#define SIG_ESTOP_STATE     GPIO_ReadInputDataBit(SIGNALS_GPIO_PORT, ESTOP_SIGNAL)
#define SIG_ENABLE_STATE    GPIO_ReadInputDataBit(SIGNALS_GPIO_PORT, ENABLE_SIGNAL)

#define _WORK               OUT_GPIO_PORT, WORK_DOUT, LOW   
#define _BUZZER             OUT_GPIO_PORT, BUZZER_DOUT, LOW   
#define _LITTER             OUT_GPIO_PORT, LITTER_DOUT, LOW   
#define _SEWAGE             OUT_GPIO_PORT, SEWAGE_DOUT, LOW   
#define _ERROR              OUT_GPIO_PORT, ERROR_DOUT, LOW   
#define _LEDSTART           OUT_GPIO_PORT, LEDSTART_DOUT, LOW   

/* Type definitions ----------------------------------------------------------*/
enum { DEBOUNCE_OFF = 0, DEBOUNCE_ON = !DEBOUNCE_OFF };

enum { 
  NO_CONTACT = 0, 
  CONTACT_FIXED,  
  CONTACT_AUTOREPEAT
};

enum {
  LEVEL_MAX = 8,     // 8 - максимальный уровень фильтрации сигнала
  LEVEL_8,		    // 9
  LEVEL_7,		    // 10    
  LEVEL_6,		    // 11
  LEVEL_5,		    // 12
  LEVEL_4,		    // 13
  LEVEL_3,		    // 14
  LEVEL_2,		    // 15
  LEVEL_1,		    // 16
  LEVEL_MIN		    // 17
};

enum { DONT_ZERO = 0, SET_TO_ZERO = !DONT_ZERO };

enum { PIN_OFF = 0, PIN_ON = !PIN_OFF };

typedef enum 
{
  IS_Idle,
  IS_Start,
  IS_SetTimerON,
  IS_StepON,
  IS_SetTimerOFF,
  IS_StepOFF,
  IS_Acceleration,
  IS_Braking,
  IS_Complete
} TInputSignalsState;

typedef struct _testsignals
{
  union
  {
      u8    Bits;
      struct {
        u8    Step:1;
        u8    Dir:1;
        u8    fl_Busy:1;
        u8    Reserve:5;
      };
  };
  u32   Number;
  u32   Timer;
  u16   Time;
  u16   Tclk;
  u16   Acc;
  u16   Sts;
  TInputSignalsState State;
} TestSignals_TypeDef;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void init_TimerTIM6(uint32_t tick_value, uint32_t period);
void init_TimerTIM7(uint32_t tick_value, uint32_t period);
void init_TimerTIM1(void);
void init_TimerTIM2(void);
void init_TimerTIM3(void);
void init_TimerTIM4(void);

void init_Inputs(void);
void init_Outputs(void);
void cpl(void);
void blink(uint16_t led, u16 period);
void delay_1ms(uint del); 
void printf_Consol(const char *pcFmt, ...);
u32 getTickCnt(void);
#endif /* __FUNCTIONS_H */


