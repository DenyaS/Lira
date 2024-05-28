
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "unsigned.h"

/* Global defines ------------------------------------------------------------*/
#define __RF_MASTER_MODE__	   // Режим работы радиомодуля [MASTER/SLAVE]
#define __BYTESTUFFING__       // Включен байтстаффинг
#define __AT45DB161D__         // Используется микросхема AT45DB161D (внешняя DataFlash)
#define __SHOWHAIL__           // Отображать на дисплее приветствие при запуске 
#define __RFINT_ENABLED__      // (Define Radio interrupts) Прерывания от УКВ-модуля включены 
#define __RADIONET__           // (Define Radio Network) Создать радиосеть с датчиками пассажиров
//#define __GPS_EMULATOR__       // (Define emulator of GPS Terminal) Работа в режиме эмуляции ответов и запросов GPS терминала 

#define __DEBUGPORT			0       // № СOM-порта для вывода отладки (0 или 1)
                                    // Если значение >1, то отладка не работает      
#define __MAXQTYEVENTS      1000    // Количество событий, хранящихся в DataFlash
#define __NUMBEROFTRIES_SMS 3       // Количество попыток отправить SMS
#define __TIMERSYNCHRORF    100     // (ms) Период отсылки пакетов синхронизации радиоканала
#define __TIMEPPDSLEEP      6000    // (ms) Время нахождения датчика в режиме пониженного энергопотребления
#define __SLEEPCHANNEL      11      // Канал для "летаргического" сна
#define __TIMEOUTSHOWSEATS  1200    // (s) Период отсылки данных о состоянии сидений на DM

/* Private defines ------------------------------------------------------------*/
#define BOUNCECOUNT     3     // Антидребезг кнопок
#define UART_PC         0     // UART0 - COM-порт для подключения управляющего компьютера  
#define UART_GPS        1     // UART1 - COM-порт для подключения GPS терминала  
#define UART_DM         0     // COM-порт для DriverMonitor  


// Параметры по умолчанию
#define BOARD           2711    // Номер автобуса (BH 2711 BI)
#define LCDCONTRAST     4       // Чем больше значение, тем больше контрастность
#define CELNUMBER       "+380509229934\n"//"+380633759524\n"
#define SMSINT          15      // (сек) 0 - Сообщения автоматически не отправляются
#define QTYPPD          2       // Количество датчиков присутствия пассажира в системе  
#define BATTRESH        280     // ?
#define ENCRYPT_RF      1       // Шифровать пакеты УКВ
#define TIMEFORSEATS    120     // (s) Время, после которого при нажатии на кнопку открытия двери анализируется состояние сидений
// ...
//==========================================================================
// ВНИМАНИЕ! Если изменились параметры по умолчанию, то необходимо изменить:
#define MINUEND         2011
#define SUBTRAHEND      1999
//==========================================================================
#define DIFFERENCE      (MINUEND - SUBTRAHEND)

/* Type definitions --------------------------------------------------------*/
typedef enum 
{
    MENU_Start,
    MENU_Settings,
    MENU_Test
} MenuState_TypeDef;

typedef enum 
{
    Display_Idle,
    Display_Hint,
    Display_Waiting,
    Display_Feeding,
    Display_Rolling,
    Display_Holding_MM,
    Display_Holding_RM,
    Display_Menu,
    Display_Rewind,
    Display_Working
} DisplayState_TypeDef;

// Неисправности устройств
typedef struct _devfault
{
  union 
  {
	uchar	State;
	struct 
    {
      uchar	Rtc:1;
      uchar	Sensor:1;
      uchar	DataFlash:1;
      uchar	Tranceiver:1;
      uchar	Battery:1;
      uchar	Res:3;
    };
  };
} DevFault_TypeDef;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Function prototypes ----------------------------------------------------------*/
void handler_Menu(void);
void convertDataGPS(void);
void sendStatusSMS(void);
void showInfo(uchar sec, uchar bkl);
void handler_Keyboard(void);
    
#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
