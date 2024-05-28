
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __RTC_H
#define __RTC_H

/* Includes ----------------------------------------------------------------- */
#include "unsigned.h"
#include "stm32f10x_it.h"

/* Define ------------------------------------------------------------------- */
#define	PCF_I2CADR	0xa2	// Адрес таймера на шине I2C 10100010

#define	PCFREG_CONST1	        0x00	// Регистр ControlStatus 1
#define	PCFREG_CONST2	        0x01	// Регистр ControlStatus 2
#define PCFREG_SECONDS          0x02
#define PCFREG_MINUTES          0x03
#define PCFREG_HOURS            0x04
#define PCFREG_DAYS             0x05
#define PCFREG_WEEKDAYS         0x06
#define PCFREG_CENTURY_MONTHS   0x07
#define PCFREG_YEARS            0x08
#define PCFREG_MINUTE_ALARM     0x09
#define PCFREG_HOUR_ALARM       0x0a
#define PCFREG_DAY_ALARM        0x0b
#define PCFREG_WEEKDAY_ALARM    0x0c
#define	PCFREG_CLKOUT	        0x0d	// Регистр частоты генерации на выводе CLKOUT
#define	PCFREG_TIMCON	        0x0e	// Регистр Timer control
#define	PCFREG_COUNTD	        0x0f	// Регистр Timer countdown

/* Values for the Control/Status2 register configuration */
#define TIE_ON          0x01
#define AIE_ON          0x02
#define TP_ON           0x10

/* Values for the ClkOut Register configuration */
#define CLKOUT_OFF      0x00
#define CLKOUT_1        0x83
#define CLKOUT_32       0x82
#define CLKOUT_1024     0x81
#define CLKOUT_32768    0x80

/* Values for the Timer Register configuration */
#define TIMER_OFF       0x00
#define TIMER_CLK_1_60  0x83
#define TIMER_CLK_1     0x82
#define TIMER_CLK_64    0x81
#define TIMER_CLK_4096  0x80


/* Type definitions ----------------------------------------------------------*/
typedef struct _rtc
{
  uchar	Sec;
  uchar	Min;
  uchar	Hour;
  uchar	Week;
  uchar	Day;
  uchar	Month;
  uchar	Year;
  uchar	Badtime;
} RTCParams_TypeDef;

/* Exported functions ------------------------------------------------------- */
ErrorStatus init_PCF8563T(void); 
void startClock_PCF8563T(void);
void stopClock_PCF8563T(void);
void writeByte_PCF8563T(uchar reg, uchar bt);
uchar readByte_PCF8563T(uchar reg);
ErrorStatus getTime_PCF8563T(uchar *hour, uchar *min, uchar *sec);
ErrorStatus setTime_PCF8563T(uchar hour, uchar min, uchar sec);
ErrorStatus getDate_PCF8563T(uchar *day, uchar *month, ushort *year);
ErrorStatus setDate_PCF8563T(uchar day, uchar month, ushort year);
void offAlarm_PCF8563T(void);
void onAlarm_PCF8563T(void);
void setAlarm_PCF8563T(uchar day, uchar hour, uchar min);
void getAlarm_PCF8563T(uchar *day, uchar *hour, uchar *min);
void setTimer_PCF8563T(uchar val);

#endif /* __RTC_H */

