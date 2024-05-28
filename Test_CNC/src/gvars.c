
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

// Counters
volatile uint32_t TickCnt = 0;

// Таймеры 1 ms
volatile uint32_t TimerCPL = 0;
volatile uint32_t TimerLED = 0;
volatile uint32_t TimeForDelay = 0;

// Таймеры 1 s
volatile uint32_t STimer1 = 0;       
volatile uint32_t STimer2 = 0;       

// Flags
volatile _FLAGS Flags;   

// Interfaces 
UART_TypeDef UARTx[2];  

// Моторы
StepMotor_TypeDef Motor[4];

// Енкодеры
Encoder_TypeDef Enc[4];

//Переменные модуля подачи Лира
volatile uint8_t ErrCodeAll;            //Все ошибки модуля
volatile uint8_t StateSM;             //Состояние модуля подачи
volatile uint8_t MessegSM;            //Сообщения модулю подачи
volatile uint32_t LengthPol;             //Длинна полосы на которую надо отр
volatile uint8_t speedSM;                 //Скорость затяжки
volatile uint8_t CodeErrorSM;            //Сообщение ошибок модуля подачи
volatile uint32_t Timer_inputSM;           //Таймер ожидание полосы по входу в сек
volatile uint32_t Timer_Sup_roller;        //Таймер ожидания на затягивание под ролик
//Переменные модуля обрезки Лира
volatile uint8_t StateCM;               //Состояние модуля обрезки
volatile uint8_t MessegCM;               //Сообщения модулю обрезки
volatile uint8_t CodeErrorCM;            //Сообщение ошибок модуля обрезки
volatile uint32_t Timer_RunCM;           //Таймер максимального времени обрезки полосы в сек
//Автомат инициализации осей
volatile uint8_t State_Init_Axis;                //Состояние инициализации осей
volatile uint8_t Code_Error_init_axis;                    //Коды ошибок
//Инициализация осей
volatile uint8_t State_Init_Axis;                 //Состояние автомата инициализации оси 
volatile uint8_t Timer_Init_axis;        //Таймер перед реверсом
//Состояния автоматов пневмо цилиндров
volatile uint8_t state_Man = 0;          //Состояние автомата пневмоцилиндра выдвижения манипулятора
volatile uint8_t state_Zah = 0;          //Состояние автомата пневмоцилиндра захвата манипулятора
volatile uint8_t state_Fin = 0;          //Состояние автомата пневмоцилиндра пальца манипулятора
//Таймера пневмо цилиндров
volatile uint8_t Timer_Man_Ahead;        //Таймер ограничения времени выдвижения манипулятора
volatile uint8_t Timer_Man_Behind;       //Таймер ограничения времени задвижения манипулятора
volatile uint8_t Timer_Zah_Open;         //Таймер ограничения времени открытия захвата
volatile uint8_t Timer_Zah_Close;        //Таймер ограничения времени закрытия захвата
volatile uint8_t Timer_Fin_Ahead;        //Таймер ограничения времени выдвижения пальца манипулятора
volatile uint8_t Timer_Fin_Behind;       //Таймер ограничения времени задвижения пальца манипулятора
//Удалить
volatile uint8_t MessegX;               //Сообщения автомату оси X
volatile uint8_t CodeErrorX;            //Сообщение ошибок автомата оси X
volatile uint32_t LengthX;              //Длинна на которую надо переместиться X
volatile uint8_t speedX;                //Скорость перемещения оси X


