
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

/* Includes ------------------------------------------------------------------*/
#include "unsigned.h"
#include "pin_defs.h"

/* Defines -------------------------------------------------------------------*/
#define TIMEMTC         40           // Константа временной задержки для LCD MTC-S16204 (min = 20)
                                     // Частота тактирования МК 24 Мгц 
#define TIMEFORCOMMAND  190
#define LCDCONTRASTMINVALUE     1
#define LCDCONTRASTMAXVALUE     10

/* Define LCD pins -----------------------------------------------------------*/
#define V0      MTC_GPIO_PORT_V0, MTC_V0  // Выход управления контрастностью
#define RS      MTC_GPIO_PORT, MTC_RS  // Выход RS (Data Register Select)
#define E       MTC_GPIO_PORT, MTC_E   // Выход EI (Enable Signal)
#define BKL     OUTPUTS_GPIO_PORT, MTC_BKL // Подсветка дисплея 

// Шина данных LCD
#define DB0     0
#define DB1     0
#define DB2     0
#define DB3     0
#define DB4     MTC_DB4
#define DB5     MTC_DB5
#define DB6     MTC_DB6
#define DB7     MTC_DB7

//#define DBPORT  ((uint8_t) ((DB0 | DB1 | DB2 | DB3 | DB4 | DB5 | DB6 | DB7) >> 4))
#define MTC_DBPORT  (DB0 | DB1 | DB2 | DB3 | DB4 | DB5 | DB6 | DB7)

/* Other defines -------------------------------------------------------------*/
#define _NOP()  __NOP()

/* Stolen defines -------------------------------------------------------------*/
/////should be defined by user /////
#define LCD_PORT        MTC_GPIO_PORT
#define LCD_CD          MTC_RS
#define LCD_EN          MTC_E
#define LCD_DB4         MTC_DB4
#define LCD_DB5         MTC_DB5
#define LCD_DB6         MTC_DB6
#define LCD_DB7         MTC_DB7
#define LCD_V0          MTC_V0
#define LCD_BKL         MTC_BKL

/* Stolen Type definitions ---------------------------------------------------*/    
typedef enum 
{
  COMMAND = 0,
  DATA = !COMMAND
} dat_or_comm;

typedef enum 
{
  LCD_DISABLE = 0,
  LCD_ENABLE = !LCD_DISABLE
} lcd_state;

typedef enum 
{
  CURSOR_DISABLE = 0,
  CURSOR_ENABLE = !CURSOR_DISABLE
} cursor_state;

typedef enum 
{
  NO_BLINK = 0,
  BLINK = !NO_BLINK
} cursor_mode;
    
/* Function prototypes -------------------------------------------------------*/
// Пользовательские функции:
void displayString(uchar line, uchar pos, char* buf);
void displayCString(uchar l, uchar p, const char *buf);
void clearString (uchar line);
void runCString(uchar line, uchar dir, const char *adr);    
void printLabel(void);
void contrastLCD(uint value);
void onBacklight(ulong time);

// Базовые функции:
void delay_1ms(uint del);
void pause_MTC(uint pTicks);       // pause_MTC(1) ~ 0,5 us
void lcd_delay(void); 
void lcd_init(void); 
void lcd_set_4bit_mode(void);
void lcd_set_2line_display (void);
void lcd_set_state(lcd_state state, cursor_state cur_state, cursor_mode cur_mode);
void lcd_send(uchar byte, dat_or_comm dc);
void lcd_clear(void); 
void lcd_set_entrymode (void);
void lcd_out(char *txt);
void lcd_set_xy(uchar x, uchar y);  
void lcd_set_user_char(uchar char_num, uchar *char_data); 
void lcd_contrast (uint value);
int russianFont(uchar RusWord);

//------------------------------------------

#endif /* __LCD_H */


