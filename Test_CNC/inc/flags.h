

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLAGS_H
#define __FLAGS_H

#include "unsigned.h"

typedef struct _tflags 
{
	uint  _bit0:1;
	uint  _bit1:1;
	uint  _bit2:1;
	uint  _bit3:1;
	uint  _bit4:1;
	uint  _bit5:1;
	uint  _bit6:1;
	uint  _bit7:1;

	uint  _bit8:1;
	uint  _bit9:1;
	uint  _bit10:1;
	uint  _bit11:1;
	uint  _bit12:1;
	uint  _bit13:1;
	uint  _bit14:1;
	uint  _bit15:1;

	uint  _bit16:1;
	uint  _bit17:1;
	uint  _bit18:1;
	uint  _bit19:1;
	uint  _bit20:1;
	uint  _bit21:1;
	uint  _bit22:1;
	uint  _bit23:1;

	uint  _bit24:1;
	uint  _bit25:1;
	uint  _bit26:1;
	uint  _bit27:1;
	uint  _bit28:1;
	uint  _bit29:1;
	uint  _bit30:1;
	uint  _bit31:1;
} _TFLAGS;

typedef union 
{
	_TFLAGS		Fl;
	ulong		Flw;
} _FLAGS;

// Flags
#define fl_Air_Cyl_Man       Flags.Fl._bit0    // Команды цилиндру выдвижения манипулятора
#define fl_Air_Cyl_Zah       Flags.Fl._bit1   // Флаг 
#define fl_Air_Cyl_Fin       Flags.Fl._bit2   // Флаг 
#define fl_SenAd2        Flags.Fl._bit3   // Флаг 
#define fl_ButUp    	 Flags.Fl._bit4   // Флаг  
#define fl_ButDn  		 Flags.Fl._bit5   // Флаг 
#define fl_ButLeft	     Flags.Fl._bit6   // Флаг 
#define fl_ButRight      Flags.Fl._bit7   // Флаг 
#define fl_ButStart      Flags.Fl._bit8   // Флаг       
#define fl_ButStop       Flags.Fl._bit9   // Флаг      
#define fl_SigEStop      Flags.Fl._bit10  // Флаг
#define fl_SigEnable     Flags.Fl._bit11  // Флаг 
#define fl_HoldJoystick  Flags.Fl._bit12  //  
#define fl_HoldStart     Flags.Fl._bit13  // Флаг 
#define fl_SEN_SM1       Flags.Fl._bit14  // Датчик на входе модуля выравн
#define fl_SEN_SM2       Flags.Fl._bit15  // Датчик после мерного ролика
#define fl_SEN_SM3       Flags.Fl._bit16  // Датчик после выхода модуля обрезки
#define fl_ERR_ROT_Z	 Flags.Fl._bit17  // Ошибка затягивания полосы
#define fl_SEN_CM        Flags.Fl._bit18  //  Датчик модуля обрезки
#define fl_Home          Flags.Fl._bit19  //   Home
#define fl_Limit         Flags.Fl._bit20  //  Limit
#define fl_Man_Ahead      Flags.Fl._bit21  // Man Ahead 
#define fl_Man_Behind     Flags.Fl._bit22  // Man Behind
#define fl_Zah_Open         Flags.Fl._bit23  // Zah Open
#define fl_Zah_Close         Flags.Fl._bit24  // Zah Close
#define fl_Fin_Ahead      Flags.Fl._bit25  //  Fin_Ahead
#define fl_Fin_Behind    Flags.Fl._bit26  //  Fin_Behind  
#define fl_Zah_open      Flags.Fl._bit27  // Zah_open
#define fl_Zah_close     Flags.Fl._bit28  // Zah_close
#define fl_30             Flags.Fl._bit29  // 
#define fl_31	         Flags.Fl._bit30  // 
#define fl_32  		 Flags.Fl._bit31   

#endif /* __FUNCTIONS_H */



