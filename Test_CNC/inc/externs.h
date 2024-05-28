
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXTERNS_H
#define __EXTERNS_H

#include "flags.h"
#include "functions.h"
#include "stepper.h"
#include "uart.h"

extern volatile uint32_t TickCnt;
extern volatile uint32_t TimerCPL;
extern volatile uint32_t TimerLED;
extern volatile uint32_t TimeForDelay;
extern volatile uint32_t STimer1;
extern volatile uint32_t STimer2;
extern volatile _FLAGS Flags;         
extern UART_TypeDef UARTx[2];  

extern StepMotor_TypeDef Motor[4];
extern Encoder_TypeDef Enc[4];
//Переменные модуля подачи Лира
extern volatile uint8_t ErrCodeAll;            //Все ошибки модуля
extern volatile uint8_t StateSM;                  //Состояние модуля подачи
extern volatile uint8_t MessegSM;               //Сообщения модулю подачи
extern volatile uint32_t LengthPol;             //Длинна полосы на которую надо отрезать
extern volatile uint8_t speedSM;                 //Скорость затяжки
extern volatile uint8_t CodeErrorSM;            //Сообщение ошибок модуля подачи
extern volatile uint32_t Timer_inputSM;           //Таймер ожидание полосы по входу в сек
extern volatile uint32_t Timer_Sup_roller;        //Таймер ожидания на затягивание под ролик
//Переменные модуля обрезки Лира
extern volatile uint8_t StateCM;                 //Состояние модуля обрезки
extern volatile uint8_t MessegCM;               //Сообщения модулю обрезки
extern volatile uint8_t CodeErrorCM;            //Сообщение ошибок модуля обрезки
extern volatile uint32_t Timer_RunCM;           //Таймер максимального времени обрезки полосы в сек
//Автомат инициализации осей
extern volatile uint8_t State_Init_Axis;        //Состояние инициализации осей
extern volatile uint8_t Code_Error_init_axis;   //Коды ошибок
extern volatile uint8_t Timer_Init_axis;        //Таймер перед реверсом
//Состояния автоматов пневмо цилиндров
extern volatile uint8_t state_Man;          //Состояние автомата пневмоцилиндра выдвижения манипулятора
extern volatile uint8_t state_Zah;          //Состояние автомата пневмоцилиндра захвата манипулятора
extern volatile uint8_t state_Fin;          //Состояние автомата пневмоцилиндра пальца манипулятора
//Таймера пневмо цилиндров
extern volatile uint8_t Timer_Man_Ahead;        //Таймер ограничения времени выдвижения манипулятора
extern volatile uint8_t Timer_Man_Behind;       //Таймер ограничения времени задвижения манипулятора
extern volatile uint8_t Timer_Zah_Open;         //Таймер ограничения времени открытия захвата
extern volatile uint8_t Timer_Zah_Close;        //Таймер ограничения времени закрытия захвата
extern volatile uint8_t Timer_Fin_Ahead;        //Таймер ограничения времени выдвижения пальца манипулятора
extern volatile uint8_t Timer_Fin_Behind;       //Таймер ограничения времени задвижения пальца манипулятора
//Удалить
extern volatile uint8_t MessegX;               //Сообщения автомату оси X
extern volatile uint8_t CodeErrorX;            //Сообщение ошибок автомата оси X
extern volatile uint32_t LengthX;              //Длинна на которую надо переместиться X
extern volatile uint8_t speedX;                //Скорость перемещения оси X
#endif /* __EXTERNS_H */


