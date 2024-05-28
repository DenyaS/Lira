
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __EXTCOM_H
#define __EXTCOM_H

/* Includes ----------------------------------------------------------------- */
#include "unsigned.h"

/* Private defines -----------------------------------------------------------*/
#define HEAD_PACKET     4       // Длина заголовка пакета (ADDR_DST + ADDR_SRC + SIZE + CMD = 4)

// Формат пакета для COM, RS-485, USB2.0, CAN
/*  -       0         1        2      3    4...n  n+1, n+2   */
/* FLAG | ADR_DST | ADR_SRC | SIZE | CMD | DATA | CRC16      */

// Входящий пакет
#define FLAG_PACKET     0x7e                // Флаг пакета	            
#define ADDR_DST_IN(n)  UARTx[(n)].InBuf[0] // Адрес получателя
#define ADDR_SRC_IN(n)  UARTx[(n)].InBuf[1] // Адрес отправителя
#define SIZE_IN(n)      UARTx[(n)].InBuf[2] // Размерность поля данных в байтах
#define CMD_IN(n)       UARTx[(n)].InBuf[3] // Код команды  
#define DATA_IN(n)      UARTx[(n)].InBuf[4] // Поле данных пакета (<=250 байт)

// Исходящий пакет
// Флаг пакета 0x7e отсекается на этапе обработки прерываний
#define ADDR_DST_OUT(n) UARTx[(n)].OutBuf[0] // Адрес получателя
#define ADDR_SRC_OUT(n) UARTx[(n)].OutBuf[1] // Адрес отправителя
#define SIZE_OUT(n)     UARTx[(n)].OutBuf[2] // Размерность поля данных в байтах
#define CMD_OUT(n)      UARTx[(n)].OutBuf[3] // Код команды   
#define DATA_OUT(n)     UARTx[(n)].OutBuf[4] // Данные в пакете содержатся с 5-го байта

// Коды команд системы 
#define CMD_ERROR           0x00            // ALL: Ответ с кодом ошибки
#define CMD_TEST            0x01

// Коды ошибок для обмена данными
#define NO_ERROR	        0x00	// Нет ошибок
#define ERR_BAD_COMMAND	    0x01	// 01 Ошибка кода команды (CMD)
#define ERR_DATA	        0x02	// 02 Ошибка в загружаемых данных
#define ERR_BAD_SIZE	    0x03	// 03 Ошибка размера аргумента команды
#define ERR_BAD_CRC16	    0x04	// 04 Ошибка CRC
#define ERR_NODATA	        0x05	// 05 Нет данных для передачи
#define ERR_BADPARAM	    0x06	// 06 Неверный параметр
#define	ERR_ACCESS			0x07	// 07 Нет доступа (сессия не открыта)
#define ERR_BAD_SRCADR      0x08    // 08 Неверный адрес отправителя
#define ERR_NOTSENT_SMS     0x09    // 09 Предыдущее SMS ещё не отправлено
#define ERR_EXEC	        0x0a	// 10 Ошибка выполнения команды (запроса)
                            //0x0b
                            //0x0c
#define ERR_BAD_DSTADR      0x0d    // 13 Неверный адрес получателя
                            //0x0e
                            //0x0f
#define	ERR_READ_DF	        0x10	// Ошибка чтения данных из DataFlash
#define ERR_IDLE            0x11    // Пустой вход - нет выполнения задачи

// Адресация устройств системы
#define DEBUGPC	            0x00	// Отладочный компьютер
#define CCU     0x01             //ЦУМ Центральный управляющий модуль 
#define PO      0x02              //Пульт оператора
#define SUPPLYUNIT     0x03     //Модуль подачи 
#define BENDINGUNIT     0x04    //Модуль гибки
#define MY_ADDRESS          SUPPLYUNIT   

//Коды команд Лира
typedef enum 
{
REQ_STATUS_SM = 10,     // 10 Запрос состояния модуля выравнивания
REQ_ERROR_SM,           // 11 Запрос ошибки модуля выравнивания
REQ_STATUS_CM,          // 12 Запрос состояния модуля обрезки
REQ_ERROR_CM,           // 13 Запрос ошибки модуля обрезки
REQ_STATUS_Man,         // 14 Запрос состояния воздушного цилиндра выдвижение манипулятора
REQ_STATUS_Zah,         // 15 Запрос состояния воздушного цилиндра захват манипулятора
REQ_STATUS_Fin,         // 16 Запрос состояния воздушного цилиндра выдвижение пальца
REQ_ERROR_Code,          // 17 Запрос ошибок 
SUP_STATE = 100,        // 100 Передать состояние модуля выравнивания
SUP_ERROR,              // 101 Передать код ошибки модуля выравнивания
SUP_ALIGN_MOVE,         // 102 Протянуть полосу  на L мм, с интенсивностью N.
CUT_STATE,              // 103 Передать состояние модуля обрезки
CUT_ERROR,              // 104 Передать код ошибки модуля обрезки
CUT_REINIT,             // 105 Выполнить инициализацию 
CUT_RUN,                // 106 Выполнить обрезку
INIT_AXIS,              // 107 Инициализация осей
INIT_AXIS_STATE,        // 108 Состояние автомата инициализации
INIT_AXIS_ERROR,        // 109 Ошибки иинициализации осей
GO_TO_X,                 // Занять позицию X
GO_TO_Y,                 // Занять позицию Y
GO_TO_Z,                 // Занять позицию Z
MAN_AHED,               //Выдвинуть манипулятор
MAN_DI                  //Задвинуть манипулятор
}Code_TypeDef;
 
//Запросы модулю Лира

/* Type definitions ----------------------------------------------------------*/
// Заголовок пакета для MRF49XA
/*
 [0]    [1...2]   [3...4]    [5]     [6...n]     [n+1...n+2]
DSize   DstAddr   SrcAddr    Cmd	   Data		    Crc 			
1b		2b		  2b	     1b      0..DSize	    2b
*/
#pragma pack(1)
typedef struct _supmotor
{
  unsigned short int          length;         //Длинна протяжки
  unsigned char           speed;          //Скорость мотора
} SupMotor_TypeDef;  
#pragma pack()
#pragma pack(1)
typedef struct _packheader
{
  uchar DSize;				// Размер поля данных       0
  uint DstAddr;             // Адрес отправителя        1 2 
  uint SrcAddr;             // Адрес получателя         3 4
  uchar Cmd;				// Команда                  5
  uchar Data[0];			// Начало данных            6
  //uint Crc;				// Контрольная сумма
} PackHeader_TypeDef;
#pragma pack()

// Структура пакета в радиоканале
/*
  [0]  [1...2]    [3]   [4...5]   [6...7]    [8]   [9...n] [n+1...n+2] 
Preamb	Sync |	DSize    DstAddr   SrcAddr   Cmd	Data	  Crc 			
 1b		 2b	 |	 1b		  2b		2b		 1b    0..DSize	   2b
*/
#pragma pack(1)
typedef struct _txpackheader
{
  uchar Preamble;	// Второй байт преамбулы
  uchar Syncro1;
  uchar Syncro2;
  struct _packheader	Header;		// Заголовок пакета
} TxPackHeader_TypeDef;
#pragma pack()

// Структура пакета UART
/*
Flag  |  Adr_Dst    Adr_Src     Size	Cmd	    Data	    Crc16
1b    |  1b		    1b		    1b	    1b		0…Size	    2b
*/
#pragma pack(1)
typedef struct _packuart
{
  uchar DstAdr;     // 0
  uchar SrcAdr;     // 1
  uchar Size;       // 2
  uchar Cmd;        // 3
  uchar Data[0];    // 4
  //uint Crc16;
} PackUart_TypeDef;
#pragma pack(1)
#define UART_PACKDATA(uart_number)   UARTx[uart_number].InBuf[4]

// Коды ошибок системы
typedef enum 
{
  ERR_WRITE_FLASH = 156,    // 156 (9c) - Ошибка записи во внутреннюю FLASH
  ERR_READ_FLASH,           // 157 (9d) - Ошибка чтения из внутренней FLASH
  ERR_ADR_READ_MEMORY,      // 158 (9e) - Неверный адрес для чтения данных 
  ERR_ADR_WRITE_MEMORY,     // 159 (9f) - Неверный адрес для записи данных
  ERR_ROTATION,             // 160 (a0) - Ошибка вращения двигателя
  ERR_MOTORJAM,             // 161 (a1) - Шаговый мотор заклинило
  OK_COMPLETE,              // 162 (a2) - Нет ошибки - завершено
  ERR_NOSTEPS,              // 163 (a3) - Нет шагов для вращения ШД
  ERR_MALLOC,               // 164 (a4) - Ошибка динамического выделения памяти
  ERR_SPI,                  // 165 (a5) - Ошибка обмена данными по интерфейсу SPI
  ERR_I2C_WRITE,            // 166 (a6) - Ошибка записи данных по интерфейсу I2C
  ERR_I2C_READ,             // 167 (a7) - Ошибка чтения данных по интерфейсу I2C
  ERR_DF_WRITE,             // 168 (a8) - Ошибка записи во внешнюю DataFlash
  ERR_DF_READ,              // 169 (a9) - Ошибка чтения из внешней DataFlash
  ERR_GET_DT,               // 170 (aa) - Ошибка чтения даты и времени из часы-календаря
  ERR_SET_DT,               // 171 (ab) - Ошибка записи даты и времени в часы-календарь
  ERR_INIT_RF,              // 172 (ac) - Ошибка инициализации радиомодуля
  ERR_RF_SEND,              // 173 (ad) - Ошибка передачи данных в радиомодуль
  ERR_UART0_SB,             // 174 (ae) - Ошибка передачи байта через UART0
  ERR_UART1_SB,             // 175 (af) - Ошибка передачи байта через UART1
  ERR_ERRANSWER = 178,      // 178 (b2) - Получен ответ с ошибкой
  ERR_WDT_RESET = 181,      // 181 (b5) - Произошёл сброс по WatchDog таймеру
  ERR_UNKNOWN,              // 182 (b6) - Обнаружена неизвестная ошибка
  ERR_HOME_ON_AT_INIT,      // 183 (b7) - Датчик Home в "1" при начале инициализации оси
  ERR_NOT_INIT_AXIS,        // 184 (b8) - Перемещение по оси не возможно т.к. ось не инициализирована
  ERR_MOTOR_BUSY,           // 185 (b9) - Мотор оси в работе при установке нового положения
  ERR_AHEAD_MAN,            // 186 (ba) - Манипулятор не выдвинулся за определённое время
  ERR_BEHIND_MAN,           // 187 (bb) - Манипулятор не убрался за определённое время
  ERR_AHEAD_MAN_SEN,        // 188 (bc) - Манипулятор отошёл в состоянии, когда он выдвинут   
  ERR_BEHIND_MAN_SEN,       // 189 (bd) - Манипулятор отошёл в состоянии, когда он задвинут 
  ERR_CLOSE_ZAH_SEN,        // 190 (be) - Захват отошёл от датчика, когда он закрыт
  ERR_OPEN_ZAH_SEN,         // 191 (bf) - Захват отошёл от датчика, когда он открыт 
  ERR_OPEN_ZAH,             // 192 (c0) - Захват не открылся за отведённое время  
  ERR_CLOSE_ZAH,            // 193 (c1) - Захват не закрылся за отведённое время
  ERR_AHEAD_FIN,            // 194 (c2) - Палец не выдвинулся за определённое время
  ERR_BEHIND_FIN,           // 195 (c3) - Палец не убрался за определённое время
  ERR_AHEAD_FIN_SEN,        // 196 (c4) - Палец отошёл в состоянии, когда он выдвинут   
  ERR_BEHIND_FIN_SEN,       // 197 (c5) - Палец отошёл в состоянии, когда он задвинут 
  ERR_ROTATION_A,           // 198 (c6) - Ошибка вращения двигателя оси А
  ERR_ROTATION_X,           // 199 (c7) - Ошибка вращения двигателя оси X  
  ERR_ROTATION_Y,           // 200 (c8) - Ошибка вращения двигателя оси Y  
  ERR_ROTATION_Z            // 201 (c9) - Ошибка вращения двигателя оси Z  
} ErrCodes_TypeDef;

// Стуктура даты и времени
#pragma pack(1)
typedef struct _datetime
{
  uchar		Day;	// число 1….31
  uchar		Month;	// месяц 1…12
  uchar		Year;	// год 00…99
  uchar		Hour;	// часы    0…23
  uchar		Min;	// минуты  0…59
  uchar		Sec;	// секунды 0…59
} DataTime_TypeDef;             // 6 Byte
#pragma pack()

#pragma pack(1)
typedef struct _rtcppd {
	uchar	Sec;
	uchar	Min;
	uchar	Hour;
	uchar	Day;
	uchar	Week;
	uchar	Month;
	uchar	Year;
} TRtcPPD, *pTRtcPPD;
#pragma pack()

// Пакет с ошибкой 
#pragma pack(1)
typedef struct _err
{
	uchar   Cmd;
	uchar   Code;
} TError;
#pragma pack()

#pragma pack(1)
typedef struct _aut1
{
  ulong     DevId;
  uint      Version;
  ulong     Rnd2;
} AutRF_TypeDef;
#pragma pack()

// Структура для передачи прошивки
#pragma pack(1)
typedef struct _rom
{
	struct {
		uint		Segment:15;
		uint		LastSegment:1;
	};
	uchar			Data[0];
} TRom;
#pragma pack()

/* Exported functions ------------------------------------------------------- */
void checkPacket_UARTx(uchar uart_number);
void parserPacket_UARTx(uchar uart_number);
void checkError(uchar permit_ch);

#endif /* __EXTCOM_H */

