

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MACROS_H
#define __MACROS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_gpio.h"

/* Defines -------------------------------------------------------------------*/
#define _NOP()  __NOP()

// ��������� ���
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

// ����������, ������� ��� �� ������� �������� ��� 
#define _get(port, bit, level)      ~(GPIO_ReadOutputDataBit(port, bit) ^ level)
#define fix(x)      _get(x)

// ��������� �������� ����
#define _write(port, val)       GPIO_Write(port, val)

// ��������� ��������� ��������� �����
#define _read(port)             GPIO_ReadOutputData(port)

// �������������� ��������� ������
#define _toggle(port, bit, level)      GPIO_Write(port, GPIO_ReadOutputData(port) ^ bit)
#define inv(x)      _toggle(x)

// ������� ��� �������� ��������� ������������� ��x���
#define _getApinstate(pin)       ((GPIOA->ODR & pin) != 0)
#define _getBpinstate(pin)       ((GPIOB->ODR & pin) != 0)
#define _getCpinstate(pin)       ((GPIOC->ODR & pin) != 0)
#define _getPinState(port, pin)  ((port->ODR & pin) != 0)

// This is used to define compiller specific things
#define memcpy_P memcpy

// ���������� �������� (0 ��� 1) �� ������ (���� ��� �����)
#define signal(x, type)         GPIO_Read##type##DataBit(x)
#define GPIO_ReadIDataBit       GPIO_ReadInputDataBit
#define GPIO_ReadODataBit       GPIO_ReadOutputDataBit

// ���������� �������� �����
#define abs(x) (x < 0 ? -x : x)

#endif /* __MACROS_H */


