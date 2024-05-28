
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "lcd.h"
#include "macros.h"
#include "functions.h"
#include "externs.h"
#include "iwdg.h"

/* Defines ------------------------------------------------------------------*/

/* Private variables --------------------------------------------------------*/
volatile uchar tmp;

/* Extern variables --------------------------------------------------------*/
/* Constant arrays ---------------------------------------------------------*/
// Кириллица
const char Znak[]={
	0x41,0xa0,0x42,0xa1,0xe0,0x45,0xa3,0xa4,		// c0 АБВГДЕЖЗ
	0xa5,0xa6,0x4b,0xa7,0x4d,0x48,0x4f,0xa8,		// c8 ИЙКЛМНОП
	0x50,0x43,0x54,0xa9,0xaa,0x58,0xe1,0xab,		// d0 РСТУФХЦЧ
	0xac,0xe2,0xad,0xae,0x62,0xaf,0xb0,0xb1,		// d8 ШЩЪЫЬЭЮЯ
	0x61,0xb2,0xb3,0xb4,0xe3,0x65,0xb6,0xb7,		// e0 абвгдежз
	0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0x6f,0xbe,		// e8 ийклмноп
	0x70,0x63,0xbf,0x79,0xe4,0x78,0xe5,0xc0,		// f0 рстуфхцч
	0xc1,0xe6,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7			// f8 шщъыьэюя
};
 
// Значёк батареи
const char _znbat[] = {
	0x0e, 0x1f, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1f,
	0x0e, 0x1f, 0x11, 0x11, 0x11, 0x11, 0x1f, 0x1f,
	0x0e, 0x1f, 0x11, 0x11, 0x11, 0x1f, 0x1f, 0x1f,
	0x0e, 0x1f, 0x11, 0x11, 0x1f, 0x1f, 0x1f, 0x1f,
	0x0e, 0x1f, 0x11, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
	0x0e, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
	0x03, 0x05, 0x0b, 0x15, 0x11, 0x12, 0x14, 0x18,
	0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x00
};

// Значёк USB 1
const char _znusb[] = {
	0x03, 0x03, 0x03, 0x00, 0x00, 0x18, 0x18, 0x18,
	0x03, 0x07, 0x0b, 0x00, 0x00, 0x18, 0x18, 0x18,
	0x03, 0x07, 0x0b, 0x08, 0x00, 0x18, 0x18, 0x18,
	0x03, 0x07, 0x0b, 0x08, 0x08, 0x18, 0x18, 0x18,
	0x03, 0x03, 0x03, 0x00, 0x00, 0x18, 0x18, 0x18,
	0x03, 0x03, 0x03, 0x00, 0x00, 0x1a, 0x1c, 0x18,
	0x03, 0x03, 0x03, 0x00, 0x02, 0x1a, 0x1c, 0x18,
	0x03, 0x03, 0x03, 0x02, 0x02, 0x1a, 0x1c, 0x18,
};

// Значёк NORDIC
const char _znnrd[] = {
	0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x18, 0x18, 0x08, 0x00, 0x00,
	0x00, 0x04, 0x0c, 0x1c, 0x1c, 0x0c, 0x04, 0x00,
	0x02, 0x06, 0x0e, 0x1e, 0x1e, 0x0e, 0x06, 0x02,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x03, 0x03, 0x02, 0x00, 0x00,
	0x00, 0x04, 0x06, 0x07, 0x07, 0x06, 0x04, 0x00,
	0x08, 0x0c, 0x0e, 0x0f, 0x0f, 0x0e, 0x0c, 0x08,
};

// Значёк дня недели
const char _znday[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1c, 0x14, 0x15, 0x15, 0x17, 0x05, 0x05, 0x00,
	0x18, 0x14, 0x18, 0x17, 0x1a, 0x02, 0x02, 0x00,
	0x0c, 0x10, 0x16, 0x15, 0x0e, 0x04, 0x04, 0x00,
	0x14, 0x14, 0x0c, 0x07, 0x02, 0x02, 0x02, 0x00,
	0x1c, 0x14, 0x14, 0x17, 0x02, 0x02, 0x02, 0x00,
	0x0c, 0x10, 0x17, 0x14, 0x0e, 0x05, 0x06, 0x00,
	0x18, 0x14, 0x18, 0x17, 0x18, 0x08, 0x07, 0x00,
};

// Значёк выполнено - невыполнено
const char _zntsk[] = {
	0x00, 0x00, 0x01, 0x02, 0x14, 0x08, 0x00, 0x00,
	0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x00, 0x00,
};

// Значёк CARD
const char _zncrd[] = {
	0x07, 0x0a, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x1f,
	0x07, 0x0a, 0x1c, 0x00, 0x00, 0x00, 0x04, 0x1f,
	0x07, 0x0a, 0x1c, 0x00, 0x00, 0x0e, 0x04, 0x1f,
	0x07, 0x0a, 0x1c, 0x00, 0x1f, 0x0e, 0x04, 0x1f,
	0x07, 0x0a, 0x1c, 0x1f, 0x0e, 0x04, 0x00, 0x1f,
	0x07, 0x0a, 0x1c, 0x0e, 0x04, 0x00, 0x00, 0x1f,
	0x07, 0x0a, 0x1c, 0x04, 0x00, 0x00, 0x00, 0x1f,
	0x07, 0x0a, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x1f,
	0x07, 0x0a, 0x1c, 0x04, 0x00, 0x00, 0x00, 0x1f,
	0x07, 0x0a, 0x1c, 0x04, 0x0e, 0x00, 0x00, 0x1f,
	0x07, 0x0a, 0x1c, 0x04, 0x0e, 0x1f, 0x00, 0x1f,
	0x07, 0x0a, 0x1c, 0x00, 0x04, 0x0e, 0x1f, 0x1f,
	0x07, 0x0a, 0x1c, 0x00, 0x00, 0x04, 0x0e, 0x1f,
	0x07, 0x0a, 0x1c, 0x00, 0x00, 0x00, 0x04, 0x1f,
};

const char _znlab[] = {
	0x1f, 0x1f, 0x1f, 0x1f, 0x1c, 0x1c, 0x19, 0x1b,
	0x1f, 0x1f, 0x1f, 0x1f, 0x04, 0x13, 0x17, 0x0e,
	0x1f, 0x1f, 0x1f, 0x1f, 0x1c, 0x0e, 0x1c, 0x1d,
	0x1f, 0x1f, 0x1f, 0x1f, 0x1c, 0x1d, 0x1b, 0x07,
	0x17, 0x06, 0x0e, 0x06, 0x1f, 0x1f, 0x1f, 0x1f,
	0x1d, 0x1d, 0x1b, 0x19, 0x1f, 0x1f, 0x1f, 0x1f,
	0x19, 0x13, 0x17, 0x07, 0x1f, 0x1f, 0x1f, 0x1f,
	0x0f, 0x17, 0x17, 0x19, 0x1f, 0x1f, 0x1f, 0x1f,
};
//------------------------------------------------------------------------------

/**
  * @brief  Вывод строки символов на дисплей.
  * @param  line: номер строки (1 или 2)
            pos: позиция символа в строке (от 1 до 16)
            buf: указатель на строку
  * @retval None
  */
void displayString(uchar line, uchar pos, char* buf)
{
  /*
  clearString(line);    
  lcd_set_xy((pos - 1), (line - 1));
  lcd_out(buf); 
  */
  displayCString(line, pos, (const char *) buf);
}

/**
  * @brief  Вывод константной строки символов на дисплей.
  * @param  line: номер строки (1 или 2)
            pos: позиция символа в строке (от 1 до 16)
            buf: указатель на константную строку
  * @retval None
  */

void displayCString(uchar line, uchar pos, const char *buf)
{
  clearString(line);    
  lcd_set_xy((pos - 1), (line - 1));
  lcd_out((char *) buf); 
}

/**
  * @brief  Очистка строки дисплея.
  * @param  line: номер строки (1 или 2)
  * @retval None
  */
void clearString (uchar line)
{
  volatile uchar i = 16;
  
  lcd_set_xy(0, (line - 1));
  while(i) {
    lcd_send(0x20, DATA);
    i--;
  }
}

/**
  * @brief  Регулировка контрастности LCD панели дисплея.
  * @param  value: значение контрастности (1...30)
  * @retval None
  */
void contrastLCD(uint value)
{
  volatile uint SwitchTime;
  
  // Корректируем значения value по необходимости
  if (value < LCDCONTRASTMINVALUE) value = LCDCONTRASTMINVALUE;
  if (value > LCDCONTRASTMAXVALUE) value = LCDCONTRASTMAXVALUE;
  
  //SwitchTime = 27000 / (value ^ 3);
  
  switch (value)
  {
    case 1: SwitchTime = 10000; break;
    case 2: SwitchTime = 4000; break;
    case 3: SwitchTime = 3000; break;
    case 4: SwitchTime = 2000; break;
    case 5: SwitchTime = 1500; break;
    case 6: SwitchTime = 1200; break;
    case 7: SwitchTime = 900; break;
    case 8: SwitchTime = 750; break;
    case 9: SwitchTime = 700; break;
    case 10: SwitchTime = 600; break;
    default: SwitchTime = 3000; break;
  }

  //LCD_PORT->BRR = LCD_V0;
  on(V0);
  pause_MTC(SwitchTime);
  //LCD_PORT->BSRR = LCD_V0;
  off(V0);
  pause_MTC(SwitchTime);
}

/**
  * @brief  Включение подсветки дисплея.
  * @param  time - время подсветки в с
  * @retval None
  */
void onBacklight(ulong time)
{
	on(BKL); 
    STimerBacklight = time;
    // ...
}

//------------------------------------------------------------------------------

/**
  * @brief  Формирование временной задержки в мс.
  * @param  del: specifies the delay time length, in 1 ms.
  * @retval None
  */
void delay_1ms (uint del) 
{
  TimeForDelay = del;
  while(TimeForDelay != 0) _WDR(); 
}

/**
  * @brief  Сформировать временную задержку для выполнения команды 
  *         модулем LCD MTC-S16204
  * @param  pTicks: specifies the delay time in "no-operation".
  * @retval None
  */
void pause_MTC (uint pTicks)
{
  volatile uint i;
  
  for(i = pTicks; i > 0; i--) _NOP(); 
}


/*-----------------------------------------------------------------------------
Задержка для дисплея
-----------------------------------------------------------------------------*/
void lcd_delay(void) 
{
  volatile ulong tmpvar;
  
  for (tmpvar = TIMEMTC; tmpvar != 0 ; tmpvar--) _NOP();
}

/*-----------------------------------------------------------------------------
Инициализация дисплея.
По умолчанию дисплей настраивается следующим образом: 4-х битный режим, 
включен курсор, при поступлении данных счетчик адреса увеличивается на единицу, 
дисплей чист, курсор в позиции X0, Y0.
-----------------------------------------------------------------------------*/
void lcd_init(void) 
{
  delay_1ms(100);
  lcd_set_4bit_mode();
  lcd_set_2line_display();
  lcd_set_state(LCD_ENABLE, CURSOR_ENABLE, NO_BLINK);
  lcd_clear();
  lcd_set_entrymode();
  delay_1ms(100);
}

/*-----------------------------------------------------------------------------
Перевод контроллера дисплея в 4-битный режим.
Контроллер перейдет в 4-х битный режим только после того как мы установим 
на ногах DB7..DB4 значение 0010 и дрыгнем ногой E. 
При этом на остальных ногах дисплея (DB3..DB0) может быть вообще что угодно, 
они ведь болтаются в воздухе. Таким образом, будет выполнена команда 0010ХХХХ, 
где Х это может быть 1 или 0 (непредсказуемо). 
Естественно после дрыга ногой E, дисплей перейдет в 4-х битный режим, 
но вот что там записалось в биты N и F неизвестно. 
Поэтому нужно задать их еще раз, в 4-х битном режиме это стало возможно. 
Таким образом полная процедура перевода дисплея в 4-х битный режим, 
включения двух строк и использования символов размером 5 на 8 точек:
-----------------------------------------------------------------------------*/
void lcd_set_4bit_mode(void) 
{
  //---- Устанавливаем на всех выводах 0 и ставим DB5 в 1------------------
  // [Set 4-bit mode]
  LCD_PORT->BRR = (LCD_DB7 | LCD_DB6 | LCD_DB5 | LCD_DB4 | LCD_CD | LCD_EN);
  LCD_PORT->BSRR = LCD_DB5;
      
  //------------------ Дрыгнем ногой E --------------------------------
  LCD_PORT->BSRR = LCD_EN;
  lcd_delay();
  LCD_PORT->BRR = LCD_EN;
  lcd_delay();
}

/*-----------------------------------------------------------------------------
[Function Set]: 4-bit bus mode, 2-line display, dots 5x8
------------------------------------------------------------------------------*/
void lcd_set_2line_display (void)
{
  lcd_send(0x28, COMMAND);
  lcd_send(0x28, COMMAND);
}

/*-----------------------------------------------------------------------------
Функция lcd_set_state для управления отображением курсора и состоянием самого дисплея. 
При помощи неё можно прятать и показывать курсор и заставлять его мигать. 
Также существует возможность выключать/включать LCD дисплей. 
Возможен вызов данной функции со следующими параметрами: 
lcd_set_state(LCD_DISABLE, CURSOR_DISABLE, NO_BLINK); - дисплей ничего не показывает, даже если данные в DDRAM есть. 
Если в качестве первого параметра выступает LCD_DISABLE, то по идее вторые два вообще не важны, 
так как ни какого курсора при выключенном дисплее мы не увидим.
lcd_set_state(LCD_ENABLE, CURSOR_DISABLE, NO_BLINK); - дисплей включен, курсора нет, ничего не мигает
lcd_set_state(LCD_ENABLE, CURSOR_DISABLE, BLINK); - дисплей включен, курсора нет, но мигает все знакоместо (черным квадратом)
lcd_set_state(LCD_ENABLE, CURSOR_ENABLE, NO_BLINK); - дисплей включен, есть не мигающий курсор
lcd_set_state(LCD_ENABLE, CURSOR_ENABLE, BLINK); - дисплей включен, курсор есть, мигает всё знакоместо.
-----------------------------------------------------------------------------*/
void lcd_set_state(lcd_state state, cursor_state cur_state, cursor_mode cur_mode)
{
  if (state == LCD_DISABLE)  {
    lcd_send(0x08, COMMAND);
  } 
  else {
    if (cur_state == CURSOR_DISABLE) 
    {
      if (cur_mode == NO_BLINK)  {
        lcd_send(0x0c, COMMAND);
      } 
      else {
        lcd_send(0x0d, COMMAND);
      }
    } 
    else {
      if (cur_mode == NO_BLINK)  
      {
	lcd_send(0x0e, COMMAND);
      } 
      else {
	lcd_send(0x0f, COMMAND);
      }
    }
  }
}

/*------------------------------------------------------------------------------
Функция lcd_send принимает два параметра: 
Первый - байт данных, второй - COMMAND или DATA.  
Функция служит для передачи в дисплей байта данных или команды. 
------------------------------------------------------------------------------*/
void lcd_send(uchar byte, dat_or_comm dc)  {

  //---------- Обнуляем все ноги ---------------------------------------------
  LCD_PORT->BRR = (LCD_DB7 | LCD_DB6 | LCD_DB5 | LCD_DB4 | LCD_CD | LCD_EN);
        
  //---------- Если передаём данные, ставим RS в "1" -------------------------
  if (dc) { // DATA
    LCD_PORT->BSRR = LCD_CD;   
    // Русский шрифт, только если ASCII код больше A0 - точно русский
    if(byte > 0xa0) {
      byte = russianFont(byte);
    }
  }
  
  //----- Проверяем биты в старшем полубайте и выставляем их на DB4...DB7 -----
  if (byte & 0x10) {
    LCD_PORT->BSRR = LCD_DB4;
  }
  if (byte & 0x20) {
    LCD_PORT->BSRR = LCD_DB5;
  }
  if (byte & 0x40) {
    LCD_PORT->BSRR = LCD_DB6;
  }
  if (byte & 0x80) {
    LCD_PORT->BSRR = LCD_DB7;
  }

  //----Дрыгнем ногой E - передаём старший полубайт -------------------------
  LCD_PORT->BSRR = LCD_EN;
  lcd_delay();
  LCD_PORT->BRR = LCD_EN;
  lcd_delay();

  //---------- Обнуляем шину данных ------------------------------------------
  LCD_PORT->BRR = (LCD_DB7 | LCD_DB6 | LCD_DB5 | LCD_DB4);

  //----- Проверяем биты в младшем полубайте и выставляем их на DB4...DB7 -----
  if (byte & 0x01) {
    LCD_PORT->BSRR = LCD_DB4;
  }
  if (byte & 0x02) {
    LCD_PORT->BSRR = LCD_DB5;
  }
  if (byte & 0x04) {
    LCD_PORT->BSRR = LCD_DB6;
  }
  if (byte & 0x08) {
    LCD_PORT->BSRR = LCD_DB7;
  }

  //----Дрыгнем ногой E - передаём младший полубайт -------------------------
  LCD_PORT->BSRR = LCD_EN;
  lcd_delay();
  LCD_PORT->BRR = LCD_EN;
  
  //---- Задержка для стабилизации ------------------
  if (dc)
    pause_MTC(TIMEMTC);       // Пауза для вывода данных
  else
    pause_MTC(TIMEFORCOMMAND);  // Пауза для выполнения команды
}

/*-----------------------------------------------------------------------------
Функция очистки дисплея.
------------------------------------------------------------------------------*/
void lcd_clear(void) 
{
  // [Clear Display]
  lcd_send(0x01, COMMAND);
}

/*-----------------------------------------------------------------------------
[Entry Mode Set: cursor moves to right
------------------------------------------------------------------------------*/
void lcd_set_entrymode (void)
{
  lcd_send(0x06, COMMAND);
}


/*-----------------------------------------------------------------------------
Функция вывода строки текста.
При помощи lcd_out можно вывести строку текста в текущую позицию курсора, 
которая может быть установлена вызовом функции lcd_set_xy. 
В функцию lcd_out следует передавать указатель на строку 
(строка должна заканчиваться нульсимволом).
------------------------------------------------------------------------------*/
void lcd_out(char *txt) 
{
  while(*txt) {
    lcd_send(*txt, DATA);
    txt++;
  }
}

/*------------------------------------------------------------------------------
Перемещение курсора без очистки экрана.
x и y - координаты курсора на экране.
------------------------------------------------------------------------------*/
void lcd_set_xy(uchar x, uchar y)  
{
  if (y == 0) {
    lcd_send(((1<<7) | (x)), COMMAND);
  } 
  else {
    lcd_send(((3<<6) | (x)), COMMAND);
  }
}

void lcd_contrast (uint value)
{
}  

/*------------------------------------------------------------------------------
Функция предназначена для записи пользовательского символа в CGRAM память дисплея
------------------------------------------------------------------------------*/
void lcd_set_user_char(uchar char_num, uchar *char_data) 
{
  uchar i;
  
  lcd_send(((1<<6) | (char_num * 8)), COMMAND);
  for (i = 0; i <= 7; i++) {
    lcd_send(char_data[i], DATA);
  }
  lcd_send((1<<7), COMMAND);
}


/*-----------------------------------------------------------------------------
------------------------------------------------------------------------------*/
int russianFont(uchar RusWord)
{
	switch (RusWord){
		case 0xC0:		//А
			RusWord = 0x41;
			break;
		case 0xC1:		//Б
			RusWord = 0xA0;
			break;
		case 0xC2:		//В
			RusWord = 0x42;
			break;
		case 0xC3:		//Г
			RusWord = 0xA1;
			break;
		case 0xC4:		//Д
			RusWord = 0xE0;
			break;
		case 0xC5:		//E
			RusWord = 0x45;
			break;
		case 0xA8:		//Ё
			RusWord = 0xA2;
			break;
		case 0xC6:		//Ж
			RusWord = 0xA3;
			break;
		case 0xC7:		//3
			RusWord = 0xA4;
			break;
		case 0xC8:		//И
			RusWord = 0xA5;
			break;
		case 0xC9:		//Й
			RusWord = 0xA6;
			break;
		case 0xCA:		//К
			RusWord = 0x4B;
			break;
		case 0xCB:		//Л
			RusWord = 0xA7;
			break;
		case 0xCC:		//М
			RusWord = 0x4D;
			break;
		case 0xCD:		//H
			RusWord = 0x48;
			break;
		case 0xCE:		//O
			RusWord = 0x4F;
			break;
		case 0xCF:		//П
			RusWord = 0xA8;
			break;
		case 0xD0:		//P
			RusWord = 0x50;
			break;
		case 0xD1:		//C
			RusWord = 0x43;
			break;
		case 0xD2:		//Т
			RusWord = 0x54;
			break;
		case 0xD3:		//У
			RusWord = 0xA9;
			break;
		case 0xD4:		//Ф
			RusWord = 0xAA;
			break;
		case 0xD5:		//X
			RusWord = 0x58;
			break;
		case 0xD6:		//Ц
			RusWord = 0xE1;
			break;
		case 0xD7:		//Ч
			RusWord = 0xAB;
			break;
		case 0xD8:		//Ш
			RusWord = 0xAC;
			break;
		case 0xD9:		//Щ
			RusWord = 0xE2;
			break;
		case 0xDA:		//Ъ
			RusWord = 0xAD;
			break;
		case 0xDB:		//Ы
			RusWord = 0xAE;
			break;
		case 0xDC:		//Ь
			RusWord = 0x62;
			break;
		case 0xDD:		//Э
			RusWord = 0xAF;
			break;
		case 0xDE:		//Ю
			RusWord = 0xB0;
			break;
		case 0xDF:		//Я
			RusWord = 0xB1;
			break;

		case 0xE0:		//а
			RusWord=0x61;
			break;
		case 0xE1:		//б
			RusWord=0xB2;
			break;
		case 0xE2:		//в
			RusWord=0xB3;
			break;
		case 0xE3:		//г
			RusWord=0xB4;
			break;
		case 0xE4:		//д
			RusWord=0xE3;
			break;
		case 0xE5:		//е
			RusWord=0x65;
			break;
		case 0xB8:		//ё
			RusWord=0xA2;
			break;
		case 0xE6:		//ж
			RusWord=0xB6;
			break;
		case 0xE7:		//з
			RusWord=0xB7;
			break;
		case 0xE8:		//и
			RusWord=0xB8;
			break;
		case 0xE9:		//й
			RusWord=0xB9;
			break;
		case 0xEA:		//к
			RusWord=0xBA;
			break;
		case 0xEB:		//л
			RusWord=0xBB;
			break;
		case 0xEC:		//м
			RusWord=0xBC;
			break;
		case 0xED:		//н
			RusWord=0xBD;
			break;
		case 0xEE:		//o
			RusWord=0x6F;
			break;
		case 0xEF:		//п
			RusWord=0xBE;
			break;
		case 0xF0:		//р
			RusWord=0x70;
			break;
		case 0xf1:		//c
			RusWord=0x63;
			break;
		case 0xf2:		//т
			RusWord=0xBF;
			break;
		case 0xf3:		//у
			RusWord=0x79;
			break;
		case 0xf4:		//Ф
			RusWord=0xE4;
			break;
		case 0xf5:		//x
			RusWord=0x78;
			break;
		case 0xf6:		//ц
			RusWord=0xE5;
			break;
		case 0xf7:		//ч
			RusWord=0xC0;
			break;
		case 0xf8:		//ш
			RusWord=0xC1;
			break;
		case 0xf9:		//щ
			RusWord=0xE6;
			break;
		case 0xfa:		//ъ
			RusWord=0xC2;
			break;
		case 0xfb:		//ы
			RusWord=0xC3;
			break;
		case 0xfc:		//ь
			RusWord=0xC4;
			break;
		case 0xfd:		//э
			RusWord=0xC5;
			break;
		case 0xfe:		//ю
			RusWord=0xC6;
			break;
		case 0xff:		//я
			RusWord=0xC7;
			break;
	}
	return RusWord;
}

//------------------------------------------
//------------------------------------------------------------------------------
// Бегущая строка (не доделанная)
void runCString(uchar line, uchar dir, const char *adr)
{
/*
  uchar n;

	memset(TEMP, 0x20, 16);
	for(n = 0; n < 16; n++) {
		if(dir)	MemCpyP(&TEMP[15 - n], adr, n + 1);
		else	MemCpyP(TEMP, &adr[15 - n], n + 1);
		PutStr(line, 0, TEMP);
		TimWork = 10; while(TimWork) RES_WDR();
	}
*/
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// (не доделанная)
void printLabel(void) 
{
/*
  uchar i;

	PutStrP(0, 0, (char flash *) "      ТЕЛЕКАРТ  ");
	PutStrP(1, 0, (char flash *) "       ПРИБОР   ");
	for(i = 0; i < 4; i++)
		DisplaySimbol(0, i, i, _znlab, i);
	for(i = 0; i < 4; i++)
		DisplaySimbol(1, i, i + 4, _znlab, i + 4);
	TimWork = 5000; while(TimWork) RES_WDR();
*/
}
