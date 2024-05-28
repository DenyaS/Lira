
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_H
#define __ENCODER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32F10x.h" 
#include "unsigned.h"

/* Defines -------------------------------------------------------------------*/
#define NO_SPIN     0x00
#define CW_SPIN     0x01 
#define CCW_SPIN    0xff

#define EncX        Enc[0]
#define EncY        Enc[1]
#define EncZ        Enc[2]
#define EncA        Enc[3]

#define _X_         0
#define _Y_         1
#define _Z_         2
#define _A_         3

#define _A       0
#define _B       1

/* Type defenitions ----------------------------------------------------------*/
typedef struct _encoder 
{
  union
  {
    u8 Signals;
    struct {
      u8 A:1;           // Фаза А
      u8 B:1;           // Фаза В
      u8 A_Prev:1;
      u8 B_Prev:1;  
      u8 Reserve:6;
    } Signal;
  };
  union
  {
    u8 Flags;
    struct {
        u8 ExtiA:1;           
        u8 ExtiB:1;
        u8 FixA:1;
        u8 FixB:1;
        u8 Reserve:4;
    } Flag;
  };
  u8            Buffer;     // Буфер енкодера
  u8            State;      // Хранит последовательность состояний энкодера
  u16           Cnt;
  u32           A_Falling;
  u32           A_Rising;
  u32           B_Falling;
  u32           B_Rising;
  u32           A_Falling_B_LOW;
  u32           A_Rising_B_LOW;
  u32           A_Falling_B_HIGH;
  u32           A_Rising_B_HIGH;
  int           Delta_A;
  u32           B_Falling_A_LOW;
  u32           B_Rising_A_LOW;
  u32           B_Falling_A_HIGH;
  u32           B_Rising_A_HIGH;
  int           Delta_B;
  u32           Both_Int;
  u32           ImpCW;      // Счётчик импульсов по часовой стрелке
  u32           ImpCCW;     // Счётчик импульсов против часовой стрелки
  u16           StA;        // Holds present state of phase A
  u16           StB;        // Holds present state of phase B
  u32           Timer;      // Таймер
  u32           TickA;      // Тиккер для замера времени
  u32           TickB;      // Тиккер для замера времени
  u32           PrevTick;   // Фиксирует предыдущий замер времени
  u32           Period;     // Показывает время между двумя импульсами
  GPIO_TypeDef* PortA;      // Порт, к которому подключена фаза А 
  GPIO_TypeDef* PortB;      // Порт, к которому подключена фаза B 
  uint16_t      PinA;       // Номер входа фазы А        
  uint16_t      PinB;       // Номер входа фазы B 
} Encoder_TypeDef;

/* Exported functions ------------------------------------------------------- */
void init_Encoders(void);
u32 getTickEnc(u8 num, u8 name, u8 zero); 

#endif /* __ENCODER_H */


