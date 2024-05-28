
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
// ���������
const char Znak[]={
	0x41,0xa0,0x42,0xa1,0xe0,0x45,0xa3,0xa4,		// c0 ��������
	0xa5,0xa6,0x4b,0xa7,0x4d,0x48,0x4f,0xa8,		// c8 ��������
	0x50,0x43,0x54,0xa9,0xaa,0x58,0xe1,0xab,		// d0 ��������
	0xac,0xe2,0xad,0xae,0x62,0xaf,0xb0,0xb1,		// d8 ��������
	0x61,0xb2,0xb3,0xb4,0xe3,0x65,0xb6,0xb7,		// e0 ��������
	0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0x6f,0xbe,		// e8 ��������
	0x70,0x63,0xbf,0x79,0xe4,0x78,0xe5,0xc0,		// f0 ��������
	0xc1,0xe6,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7			// f8 ��������
};
 
// ������ �������
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

// ������ USB 1
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

// ������ NORDIC
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

// ������ ��� ������
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

// ������ ��������� - �����������
const char _zntsk[] = {
	0x00, 0x00, 0x01, 0x02, 0x14, 0x08, 0x00, 0x00,
	0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x00, 0x00,
};

// ������ CARD
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
  * @brief  ����� ������ �������� �� �������.
  * @param  line: ����� ������ (1 ��� 2)
            pos: ������� ������� � ������ (�� 1 �� 16)
            buf: ��������� �� ������
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
  * @brief  ����� ����������� ������ �������� �� �������.
  * @param  line: ����� ������ (1 ��� 2)
            pos: ������� ������� � ������ (�� 1 �� 16)
            buf: ��������� �� ����������� ������
  * @retval None
  */

void displayCString(uchar line, uchar pos, const char *buf)
{
  clearString(line);    
  lcd_set_xy((pos - 1), (line - 1));
  lcd_out((char *) buf); 
}

/**
  * @brief  ������� ������ �������.
  * @param  line: ����� ������ (1 ��� 2)
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
  * @brief  ����������� ������������� LCD ������ �������.
  * @param  value: �������� ������������� (1...30)
  * @retval None
  */
void contrastLCD(uint value)
{
  volatile uint SwitchTime;
  
  // ������������ �������� value �� �������������
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
  * @brief  ��������� ��������� �������.
  * @param  time - ����� ��������� � �
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
  * @brief  ������������ ��������� �������� � ��.
  * @param  del: specifies the delay time length, in 1 ms.
  * @retval None
  */
void delay_1ms (uint del) 
{
  TimeForDelay = del;
  while(TimeForDelay != 0) _WDR(); 
}

/**
  * @brief  ������������ ��������� �������� ��� ���������� ������� 
  *         ������� LCD MTC-S16204
  * @param  pTicks: specifies the delay time in "no-operation".
  * @retval None
  */
void pause_MTC (uint pTicks)
{
  volatile uint i;
  
  for(i = pTicks; i > 0; i--) _NOP(); 
}


/*-----------------------------------------------------------------------------
�������� ��� �������
-----------------------------------------------------------------------------*/
void lcd_delay(void) 
{
  volatile ulong tmpvar;
  
  for (tmpvar = TIMEMTC; tmpvar != 0 ; tmpvar--) _NOP();
}

/*-----------------------------------------------------------------------------
������������� �������.
�� ��������� ������� ������������� ��������� �������: 4-� ������ �����, 
������� ������, ��� ����������� ������ ������� ������ ������������� �� �������, 
������� ����, ������ � ������� X0, Y0.
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
������� ����������� ������� � 4-������ �����.
���������� �������� � 4-� ������ ����� ������ ����� ���� ��� �� ��������� 
�� ����� DB7..DB4 �������� 0010 � ������� ����� E. 
��� ���� �� ��������� ����� ������� (DB3..DB0) ����� ���� ������ ��� ������, 
��� ���� ��������� � �������. ����� �������, ����� ��������� ������� 0010����, 
��� � ��� ����� ���� 1 ��� 0 (��������������). 
����������� ����� ����� ����� E, ������� �������� � 4-� ������ �����, 
�� ��� ��� ��� ���������� � ���� N � F ����������. 
������� ����� ������ �� ��� ���, � 4-� ������ ������ ��� ����� ��������. 
����� ������� ������ ��������� �������� ������� � 4-� ������ �����, 
��������� ���� ����� � ������������� �������� �������� 5 �� 8 �����:
-----------------------------------------------------------------------------*/
void lcd_set_4bit_mode(void) 
{
  //---- ������������� �� ���� ������� 0 � ������ DB5 � 1------------------
  // [Set 4-bit mode]
  LCD_PORT->BRR = (LCD_DB7 | LCD_DB6 | LCD_DB5 | LCD_DB4 | LCD_CD | LCD_EN);
  LCD_PORT->BSRR = LCD_DB5;
      
  //------------------ ������� ����� E --------------------------------
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
������� lcd_set_state ��� ���������� ������������ ������� � ���������� ������ �������. 
��� ������ �� ����� ������� � ���������� ������ � ���������� ��� ������. 
����� ���������� ����������� ���������/�������� LCD �������. 
�������� ����� ������ ������� �� ���������� �����������: 
lcd_set_state(LCD_DISABLE, CURSOR_DISABLE, NO_BLINK); - ������� ������ �� ����������, ���� ���� ������ � DDRAM ����. 
���� � �������� ������� ��������� ��������� LCD_DISABLE, �� �� ���� ������ ��� ������ �� �����, 
��� ��� �� ������ ������� ��� ����������� ������� �� �� ������.
lcd_set_state(LCD_ENABLE, CURSOR_DISABLE, NO_BLINK); - ������� �������, ������� ���, ������ �� ������
lcd_set_state(LCD_ENABLE, CURSOR_DISABLE, BLINK); - ������� �������, ������� ���, �� ������ ��� ���������� (������ ���������)
lcd_set_state(LCD_ENABLE, CURSOR_ENABLE, NO_BLINK); - ������� �������, ���� �� �������� ������
lcd_set_state(LCD_ENABLE, CURSOR_ENABLE, BLINK); - ������� �������, ������ ����, ������ �� ����������.
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
������� lcd_send ��������� ��� ���������: 
������ - ���� ������, ������ - COMMAND ��� DATA.  
������� ������ ��� �������� � ������� ����� ������ ��� �������. 
------------------------------------------------------------------------------*/
void lcd_send(uchar byte, dat_or_comm dc)  {

  //---------- �������� ��� ���� ---------------------------------------------
  LCD_PORT->BRR = (LCD_DB7 | LCD_DB6 | LCD_DB5 | LCD_DB4 | LCD_CD | LCD_EN);
        
  //---------- ���� ������� ������, ������ RS � "1" -------------------------
  if (dc) { // DATA
    LCD_PORT->BSRR = LCD_CD;   
    // ������� �����, ������ ���� ASCII ��� ������ A0 - ����� �������
    if(byte > 0xa0) {
      byte = russianFont(byte);
    }
  }
  
  //----- ��������� ���� � ������� ��������� � ���������� �� �� DB4...DB7 -----
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

  //----������� ����� E - ������� ������� �������� -------------------------
  LCD_PORT->BSRR = LCD_EN;
  lcd_delay();
  LCD_PORT->BRR = LCD_EN;
  lcd_delay();

  //---------- �������� ���� ������ ------------------------------------------
  LCD_PORT->BRR = (LCD_DB7 | LCD_DB6 | LCD_DB5 | LCD_DB4);

  //----- ��������� ���� � ������� ��������� � ���������� �� �� DB4...DB7 -----
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

  //----������� ����� E - ������� ������� �������� -------------------------
  LCD_PORT->BSRR = LCD_EN;
  lcd_delay();
  LCD_PORT->BRR = LCD_EN;
  
  //---- �������� ��� ������������ ------------------
  if (dc)
    pause_MTC(TIMEMTC);       // ����� ��� ������ ������
  else
    pause_MTC(TIMEFORCOMMAND);  // ����� ��� ���������� �������
}

/*-----------------------------------------------------------------------------
������� ������� �������.
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
������� ������ ������ ������.
��� ������ lcd_out ����� ������� ������ ������ � ������� ������� �������, 
������� ����� ���� ����������� ������� ������� lcd_set_xy. 
� ������� lcd_out ������� ���������� ��������� �� ������ 
(������ ������ ������������� ������������).
------------------------------------------------------------------------------*/
void lcd_out(char *txt) 
{
  while(*txt) {
    lcd_send(*txt, DATA);
    txt++;
  }
}

/*------------------------------------------------------------------------------
����������� ������� ��� ������� ������.
x � y - ���������� ������� �� ������.
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
������� ������������� ��� ������ ����������������� ������� � CGRAM ������ �������
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
		case 0xC0:		//�
			RusWord = 0x41;
			break;
		case 0xC1:		//�
			RusWord = 0xA0;
			break;
		case 0xC2:		//�
			RusWord = 0x42;
			break;
		case 0xC3:		//�
			RusWord = 0xA1;
			break;
		case 0xC4:		//�
			RusWord = 0xE0;
			break;
		case 0xC5:		//E
			RusWord = 0x45;
			break;
		case 0xA8:		//�
			RusWord = 0xA2;
			break;
		case 0xC6:		//�
			RusWord = 0xA3;
			break;
		case 0xC7:		//3
			RusWord = 0xA4;
			break;
		case 0xC8:		//�
			RusWord = 0xA5;
			break;
		case 0xC9:		//�
			RusWord = 0xA6;
			break;
		case 0xCA:		//�
			RusWord = 0x4B;
			break;
		case 0xCB:		//�
			RusWord = 0xA7;
			break;
		case 0xCC:		//�
			RusWord = 0x4D;
			break;
		case 0xCD:		//H
			RusWord = 0x48;
			break;
		case 0xCE:		//O
			RusWord = 0x4F;
			break;
		case 0xCF:		//�
			RusWord = 0xA8;
			break;
		case 0xD0:		//P
			RusWord = 0x50;
			break;
		case 0xD1:		//C
			RusWord = 0x43;
			break;
		case 0xD2:		//�
			RusWord = 0x54;
			break;
		case 0xD3:		//�
			RusWord = 0xA9;
			break;
		case 0xD4:		//�
			RusWord = 0xAA;
			break;
		case 0xD5:		//X
			RusWord = 0x58;
			break;
		case 0xD6:		//�
			RusWord = 0xE1;
			break;
		case 0xD7:		//�
			RusWord = 0xAB;
			break;
		case 0xD8:		//�
			RusWord = 0xAC;
			break;
		case 0xD9:		//�
			RusWord = 0xE2;
			break;
		case 0xDA:		//�
			RusWord = 0xAD;
			break;
		case 0xDB:		//�
			RusWord = 0xAE;
			break;
		case 0xDC:		//�
			RusWord = 0x62;
			break;
		case 0xDD:		//�
			RusWord = 0xAF;
			break;
		case 0xDE:		//�
			RusWord = 0xB0;
			break;
		case 0xDF:		//�
			RusWord = 0xB1;
			break;

		case 0xE0:		//�
			RusWord=0x61;
			break;
		case 0xE1:		//�
			RusWord=0xB2;
			break;
		case 0xE2:		//�
			RusWord=0xB3;
			break;
		case 0xE3:		//�
			RusWord=0xB4;
			break;
		case 0xE4:		//�
			RusWord=0xE3;
			break;
		case 0xE5:		//�
			RusWord=0x65;
			break;
		case 0xB8:		//�
			RusWord=0xA2;
			break;
		case 0xE6:		//�
			RusWord=0xB6;
			break;
		case 0xE7:		//�
			RusWord=0xB7;
			break;
		case 0xE8:		//�
			RusWord=0xB8;
			break;
		case 0xE9:		//�
			RusWord=0xB9;
			break;
		case 0xEA:		//�
			RusWord=0xBA;
			break;
		case 0xEB:		//�
			RusWord=0xBB;
			break;
		case 0xEC:		//�
			RusWord=0xBC;
			break;
		case 0xED:		//�
			RusWord=0xBD;
			break;
		case 0xEE:		//o
			RusWord=0x6F;
			break;
		case 0xEF:		//�
			RusWord=0xBE;
			break;
		case 0xF0:		//�
			RusWord=0x70;
			break;
		case 0xf1:		//c
			RusWord=0x63;
			break;
		case 0xf2:		//�
			RusWord=0xBF;
			break;
		case 0xf3:		//�
			RusWord=0x79;
			break;
		case 0xf4:		//�
			RusWord=0xE4;
			break;
		case 0xf5:		//x
			RusWord=0x78;
			break;
		case 0xf6:		//�
			RusWord=0xE5;
			break;
		case 0xf7:		//�
			RusWord=0xC0;
			break;
		case 0xf8:		//�
			RusWord=0xC1;
			break;
		case 0xf9:		//�
			RusWord=0xE6;
			break;
		case 0xfa:		//�
			RusWord=0xC2;
			break;
		case 0xfb:		//�
			RusWord=0xC3;
			break;
		case 0xfc:		//�
			RusWord=0xC4;
			break;
		case 0xfd:		//�
			RusWord=0xC5;
			break;
		case 0xfe:		//�
			RusWord=0xC6;
			break;
		case 0xff:		//�
			RusWord=0xC7;
			break;
	}
	return RusWord;
}

//------------------------------------------
//------------------------------------------------------------------------------
// ������� ������ (�� ����������)
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
// (�� ����������)
void printLabel(void) 
{
/*
  uchar i;

	PutStrP(0, 0, (char flash *) "      ��������  ");
	PutStrP(1, 0, (char flash *) "       ������   ");
	for(i = 0; i < 4; i++)
		DisplaySimbol(0, i, i, _znlab, i);
	for(i = 0; i < 4; i++)
		DisplaySimbol(1, i, i + 4, _znlab, i + 4);
	TimWork = 5000; while(TimWork) RES_WDR();
*/
}
