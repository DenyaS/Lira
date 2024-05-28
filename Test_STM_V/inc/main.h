
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "unsigned.h"

/* Global defines ------------------------------------------------------------*/
#define __RF_MASTER_MODE__	   // ����� ������ ����������� [MASTER/SLAVE]
#define __BYTESTUFFING__       // ������� ������������
#define __AT45DB161D__         // ������������ ���������� AT45DB161D (������� DataFlash)
#define __SHOWHAIL__           // ���������� �� ������� ����������� ��� ������� 
#define __RFINT_ENABLED__      // (Define Radio interrupts) ���������� �� ���-������ �������� 
#define __RADIONET__           // (Define Radio Network) ������� ��������� � ��������� ����������
//#define __GPS_EMULATOR__       // (Define emulator of GPS Terminal) ������ � ������ �������� ������� � �������� GPS ��������� 

#define __DEBUGPORT			0       // � �OM-����� ��� ������ ������� (0 ��� 1)
                                    // ���� �������� >1, �� ������� �� ��������      
#define __MAXQTYEVENTS      1000    // ���������� �������, ���������� � DataFlash
#define __NUMBEROFTRIES_SMS 3       // ���������� ������� ��������� SMS
#define __TIMERSYNCHRORF    100     // (ms) ������ ������� ������� ������������� �����������
#define __TIMEPPDSLEEP      6000    // (ms) ����� ���������� ������� � ������ ����������� �����������������
#define __SLEEPCHANNEL      11      // ����� ��� "��������������" ���
#define __TIMEOUTSHOWSEATS  1200    // (s) ������ ������� ������ � ��������� ������� �� DM

/* Private defines ------------------------------------------------------------*/
#define BOUNCECOUNT     3     // ����������� ������
#define UART_PC         0     // UART0 - COM-���� ��� ����������� ������������ ����������  
#define UART_GPS        1     // UART1 - COM-���� ��� ����������� GPS ���������  
#define UART_DM         0     // COM-���� ��� DriverMonitor  


// ��������� �� ���������
#define BOARD           2711    // ����� �������� (BH 2711 BI)
#define LCDCONTRAST     4       // ��� ������ ��������, ��� ������ �������������
#define CELNUMBER       "+380509229934\n"//"+380633759524\n"
#define SMSINT          15      // (���) 0 - ��������� ������������� �� ������������
#define QTYPPD          2       // ���������� �������� ����������� ��������� � �������  
#define BATTRESH        280     // ?
#define ENCRYPT_RF      1       // ��������� ������ ���
#define TIMEFORSEATS    120     // (s) �����, ����� �������� ��� ������� �� ������ �������� ����� ������������� ��������� �������
// ...
//==========================================================================
// ��������! ���� ���������� ��������� �� ���������, �� ���������� ��������:
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

// ������������� ���������
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
