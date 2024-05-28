

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MACROS_H
#define __MACROS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_gpio.h"

/* Defines -------------------------------------------------------------------*/
#define _NOP()  __NOP()

// Выставить бит
#define _set(port, bit, level)  \
do {                            \
  if (level)                    \
    GPIO_SetBits(port, bit);    \
  else                          \
    GPIO_ResetBits(port, bit);  \
} while(0)
#define on(x) _set(x)

#define _clr(port, bit, level)  \
do {                            \
  if (level)                    \
    GPIO_ResetBits(port, bit);  \
  else                          \
    GPIO_SetBits(port, bit);    \
} while(0)
#define off(x) _clr(x)

// Определить, активен или не активен выходной бит 
#define _get(port, bit, level)      ~(GPIO_ReadOutputDataBit(port, bit) ^ level)
#define fix(x)      _get(x)

// Выставить выходной порт
#define _write(port, val)       GPIO_Write(port, val)

// Прочитать состояние выходного порта
#define _read(port)             GPIO_ReadOutputData(port)

// Инвертирование состояния вывода
#define _toggle(port, bit, level)      GPIO_Write(port, GPIO_ReadOutputData(port) ^ bit)
#define inv(x)      _toggle(x)

// Макросы для проверки состояния определенного выxода
#define _getApinstate(pin)       ((GPIOA->ODR & pin) != 0)
#define _getBpinstate(pin)       ((GPIOB->ODR & pin) != 0)
#define _getCpinstate(pin)       ((GPIOC->ODR & pin) != 0)
#define _getPinState(port, pin)  ((port->ODR & pin) != 0)

// This is used to define compiller specific things
#define memcpy_P memcpy

// Определить значение (0 или 1) на выводе (вход или выход)
#define signal(x, type)         GPIO_Read##type##DataBit(x)
#define GPIO_ReadIDataBit       GPIO_ReadInputDataBit
#define GPIO_ReadODataBit       GPIO_ReadOutputDataBit

// Абсолютная величина числа
#define abs(x) (x < 0 ? -x : x)

#endif /* __MACROS_H */


