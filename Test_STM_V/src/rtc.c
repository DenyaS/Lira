 
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include "includes.h"
#include "stm32f10x_it.h"

/** 
  * @brief      Инициализация микросхемы таймера PCF8563T
  * @param      None
  * @retval     None
  */
ErrorStatus init_PCF8563T (void) 
{
  printf_Consol((const char *) "Init RTC PCF8563T...\n\r");
  
  // Загрузить регистр Control/Status 2
  writeByte_PCF8563T(PCFREG_CONST2, 0x00);

  // Установить частоту генерации на выводе CLKOUT
  writeByte_PCF8563T(PCFREG_CLKOUT, CLKOUT_OFF);    // 0x00     // Not in use
  
  // Загрузить регистр Timer control
  writeByte_PCF8563T(PCFREG_TIMCON, TIMER_OFF);     // 0x00

  // Загрузить Timer countdown значение
  writeByte_PCF8563T(PCFREG_COUNTD, 0x00);

  // Загрузить регистр Control/Status 1
  startClock_PCF8563T();

  if (!ErrorI2C) {
    printf_Consol((const char *) "OK!\n\r\r");
    return SUCCESS;
  } 
  else {
	DevFault.Rtc = 1;
	printf_Consol((const char *) "Error!\n\r\r");
    return ERROR;
  }
}

/** 
  * @brief      Запуск часов в PCF8563T
  * @param      None`
  * @retval     None
  */
void startClock_PCF8563T (void)
{
  writeByte_PCF8563T(PCFREG_CONST1, 0);
}

/** 
  * @brief      Остановить часы в PCF8563T
  * @param      None
  * @retval     None
  */
void stopClock_PCF8563T (void)
{
    writeByte_PCF8563T(PCFREG_CONST1, 0x20);
}

/** 
  * @brief      Записать байт во внешний таймер PCF8563T по шине I2C
  * @param      reg - адрес регистра, в который записывается байт
  *             bt - байт для передачи
  * @retval     None
  */
void writeByte_PCF8563T (uchar reg, uchar bt)
{
  uchar Buf[2];
  
  // Prepare Data for transmit
  Buf[0] = reg;
  Buf[1] = bt;
  
  while (1) {
    if (sendData_I2C(PCF_I2CADR, Buf, 2) == ERROR)
    { 
      displayError(ERR_I2C_WRITE);
      //while (1) { blink(LEDS_GPIO_PORT, LEDGREEN | LEDBLUE, 30, SND_ON); }
      delay_1ms(500);
    }
    else break;
  }
}

/** 
  * @brief      Считать байт из регистра внешнего таймера PCF8563T по шине I2C
  * @param      adr - адрес регистра, из которого считывается байт
  * @retval     Прочитанный байт (unsigned char)
  */
uchar readByte_PCF8563T (uchar reg)
{
  uchar Pack[1];

  Pack[0] = reg;
  while(1) {
    if (sendData_I2C(PCF_I2CADR, Pack, 1) == ERROR)  
    { 
      displayError(ERR_I2C_WRITE);
      //while (1) { blink(LEDS_GPIO_PORT, LEDGREEN | LEDBLUE, 30, SND_ON); }
      delay_1ms(500);
    }
    else break;
  }
  
  while(1) {
    if (receiveData_I2C(PCF_I2CADR, Pack, 1) == ERROR)
    { 
      displayError(ERR_I2C_READ);
      //while (1) { blink(LEDS_GPIO_PORT, LEDBLUE | LEDBLUE, 30, SND_ON); }
      delay_1ms(500);
    }
    else break;
  }

  return Pack[0];
}

/** 
  * @brief      Считать время из внешнего таймера PCF8563T
  * @param      *hour - указатель на буфер для записи Часов
  *             *min - указатель на буфер для записи Минут
  *             *sec - указатель на буфер для записи Секунд
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus getTime_PCF8563T (uchar *hour, uchar *min, uchar *sec)
{
  uchar Bt;
  
  // Get Seconds
  Bt = readByte_PCF8563T(PCFREG_SECONDS);
  *sec = bcd2bin(Bt & 0x7f);

  // Get Minutes
  *min = bcd2bin(readByte_PCF8563T(PCFREG_MINUTES) & 0x7f);
  
  // Get Hours
  *hour = bcd2bin(readByte_PCF8563T(PCFREG_HOURS) & 0x3f);

  /*if (Bt & 0x80) { return ERROR; }
  else*/ { return SUCCESS; }
}

/** 
  * @brief      Установить время внешнего таймера PCF8563T
  * @param      hour - Часы (0...59)
  *             Min - Минуты (0...59)
  *             Sec - Cекунды (0...59)
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus setTime_PCF8563T (uchar hour, uchar min, uchar sec)
{
  if (hour > 59) { return ERROR; }
  if (min > 59) { return ERROR; }
  if (sec > 59) { return ERROR; }
  
  stopClock_PCF8563T();
  writeByte_PCF8563T(PCFREG_SECONDS, bin2bcd(sec & 0x7f));
  writeByte_PCF8563T(PCFREG_MINUTES, bin2bcd(min & 0x7f));
  writeByte_PCF8563T(PCFREG_HOURS, bin2bcd(hour & 0x3f));
  startClock_PCF8563T();
  
  return SUCCESS;
}

/** 
  * @brief      Считать дату из внешнего таймера PCF8563T
  * @param      *day - указатель на буфер для записи Дня
  *             *month - указатель на буфер для записи Месяца
  *             *year - указатель на буфер для записи Года
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus getDate_PCF8563T (uchar *day, uchar *month, ushort *year)
{
  uchar CentMonth;
  uint Year;
  
  *day = bcd2bin(readByte_PCF8563T(PCFREG_DAYS) & 0x3f);
  CentMonth = readByte_PCF8563T(PCFREG_CENTURY_MONTHS);
  *month = bcd2bin(CentMonth & 0x1f);
  if (CentMonth & 0x80) { Year = 2000; }
  *year = bcd2bin(readByte_PCF8563T(PCFREG_YEARS) & 0xff) + Year;
  
  return SUCCESS;
}

/** 
  * @brief      Установить дату во внешний таймер PCF8563T
  * @param      day - День (1...31)
  *             month - Месяц (1...12)
  *             year - Год (1999...2052)
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus setDate_PCF8563T (uchar day, uchar month, ushort year)
{
  uchar CentMonth;
  
  if ((day < 1) & (day > 31)) { return ERROR; }
  if ((month < 1) && (month > 12)) { return ERROR; }
  if ((year < 1977) || (year > 2052)) { return ERROR; }
  
  CentMonth = bin2bcd(month & 0x1f);
  if(year >= 2000) { CentMonth |= 0x80; }

  stopClock_PCF8563T();
  writeByte_PCF8563T(PCFREG_DAYS, bin2bcd(day & 0x3f));
  writeByte_PCF8563T(PCFREG_CENTURY_MONTHS, CentMonth);
  writeByte_PCF8563T(PCFREG_YEARS, bin2bcd(year % 100));
  startClock_PCF8563T();
  
  return SUCCESS;
}

/** 
  * @brief      Отключить "будильник" внешнего таймера PCF8563T
  * @param      None
  * @retval     None
  */
void offAlarm_PCF8563T (void)
{
  writeByte_PCF8563T(PCFREG_MINUTE_ALARM, readByte_PCF8563T(PCFREG_MINUTE_ALARM) | 0x80);
  writeByte_PCF8563T(PCFREG_HOUR_ALARM, readByte_PCF8563T(PCFREG_HOUR_ALARM) | 0x80);
  writeByte_PCF8563T(PCFREG_DAY_ALARM, readByte_PCF8563T(PCFREG_DAY_ALARM) | 0x80);
  writeByte_PCF8563T(PCFREG_WEEKDAY_ALARM, 0x80);
}

/** 
  * @brief      Включить "будильник" внешнего таймера PCF8563T
  * @param      None
  * @retval     None
  */
void onAlarm_PCF8563T (void)
{
  uchar Day;
  
  writeByte_PCF8563T(PCFREG_MINUTE_ALARM, readByte_PCF8563T(PCFREG_MINUTE_ALARM) | 0x7f);
  writeByte_PCF8563T(PCFREG_HOUR_ALARM, readByte_PCF8563T(PCFREG_HOUR_ALARM) | 0x3f);
  Day = readByte_PCF8563T(PCFREG_DAY_ALARM) & 0x3f;
  if (Day == 0) { Day |= 0x80; }
  writeByte_PCF8563T(PCFREG_DAY_ALARM, Day);
}

/** 
  * @brief      Установить "будильник"
  * @param      day - День
  *             hour - Часы
  *             min - Минуты
  * @retval     None
  */
void setAlarm_PCF8563T (uchar day, uchar hour, uchar min)
{
  writeByte_PCF8563T(PCFREG_MINUTE_ALARM, bin2bcd(min) | 0x80);
  writeByte_PCF8563T(PCFREG_HOUR_ALARM, bin2bcd(hour) | 0x80);
  writeByte_PCF8563T(PCFREG_DAY_ALARM, bin2bcd(day) | 0x80);
  writeByte_PCF8563T(PCFREG_WEEKDAY_ALARM, 0x80);
}

/** 
  * @brief      Прочитать время "будильника" из микросхемы PCF8563T
  * @param      *day - указатель на буфер День
  *             *hour - указатель на буфер Часы
  *             *min - указатель на буфер Минуты
  * @retval     None
  */
void getAlarm_PCF8563T (uchar *day, uchar *hour, uchar *min)
{
  *min = bcd2bin(readByte_PCF8563T(PCFREG_MINUTE_ALARM) & 0x7f);
  *hour = bcd2bin(readByte_PCF8563T(PCFREG_HOUR_ALARM) & 0x3f);
  *day = bcd2bin(readByte_PCF8563T(PCFREG_DAY_ALARM) & 0x3f);
}

/** 
  * @brief      Установить таймер микросхемы PCF8563T
  * @param      val - значение таймера (сек)
  * @retval     None
  */
void setTimer_PCF8563T (uchar val)
{
  writeByte_PCF8563T(PCFREG_COUNTD, val);
}

