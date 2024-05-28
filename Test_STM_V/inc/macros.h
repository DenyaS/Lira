
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MACROS_H
#define __MACROS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_gpio.h"
#include "main.h"


/* Defines -------------------------------------------------------------------*/
#define _set(port, bit)         GPIO_SetBits(port, bit)
#define on(x) _set(x)

#define _clr(port, bit)         GPIO_ResetBits(port, bit)
#define off(x) _clr(x)

#define _write(port, val)       GPIO_Write(port, val)

#define _read(port)             GPIO_ReadOutputData(port)

// Инвертирование состояния вывода
#define _toggle(port, bit)      GPIO_Write(port, GPIO_ReadOutputData(port) ^ bit)

// Макрос для проверки состояния определенного вывода
#define _getApinstate(pin)       ((GPIOA->ODR & pin) != 0)
#define _getBpinstate(pin)       ((GPIOB->ODR & pin) != 0)
#define _getCpinstate(pin)       ((GPIOC->ODR & pin) != 0)

// This is used to define compiller specific things
#define memcpy_P memcpy

// Определить значение (0 или 1) на выводе (вход или выход)
#define signal(x, type)         GPIO_Read##type##DataBit(x)
#define GPIO_ReadIDataBit       GPIO_ReadInputDataBit
#define GPIO_ReadODataBit       GPIO_ReadOutputDataBit

#endif /* __MACROS_H */

