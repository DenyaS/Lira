

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __EXTCOM_H
#define __EXTCOM_H

/* Includes ----------------------------------------------------------------- */
#include "unsigned.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/
#define HEAD_PACKET     4       // Длина заголовка пакета (ADDR_DST + ADDR_SRC + SIZE + CMD = 4)
#define WAITANSWER_RF   300     // Время ожидания ответа от УКВ модуля (мс)
#define WAITANSWER_GPS  5000    // Время ожидания ответа от GPS терминала (мс)
#define WAITACKSENDSMS  4000    // Время ожидания ответа об успешной отсылке SMS (мс)
#define WAITCONNECT_RF  8000    // Время ожидания соединения (авторизации) с PPD (мс)

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


//Коды команд системы ЛИРА
#define STATUS_SM       10      // 10 Запрос состояния модуля обрезки
#define ERROR_SM        11      // 11 Запрос ошибки модуля выравнивания
#define STATUS_CM       12      // 12 Запрос состояния модуля обрезки
#define ERROR_CM        13      // 13 Запрос ошибки модуля обрезки
#define ERROR_Code      17      // 17 Запрос ошибок модуля

//Коды ответов от переферии Лира

//#define SUP_ALIGN_MOVE_LEFT 0x03      //Протянуть полосу влево на L мм, с интенсивностью N.

// Коды команд системы 
#define CMD_ERROR           0x00    // ALL: Ответ с кодом ошибки
#define	cmdAUTORIZE1		0x01	// PDD: авторизация 1-й этап
#define	cmdAUTORIZE2		0x02	// PDD: авторизация 2-й этап
#define	cmdCHECK_SENSOR		0x03	// PDD: проверить состояние сенсора
#define	cmdPING_REQUEST		0x04	// PDD: запрос наличия и статуса
#define	cmdPING_CONFIRM		0x04	// PDD: ответ на команду запроса статуса
#define	cmdREAD_LTSA		0x05	// PDD: прочитать последнюю транзакцию
#define	cmdREAD_STSA		0x06	// PDD: прочитать определённую транзакцию
#define	cmdSWITCH_SLEEP		0x07	// PDD: установить/выключить режим энергосбережения
#define	cmdGET_DATETIME		0x08	// PDD: прочитать дату и время
#define	cmdSET_DATETIME		0x09	// PDD: установить дату и время
#define	cmdCONFIG_PDD		0x0a	// PDD: записать параметры конфигурации
#define	cmdGETNUM			0x0b	// PDD: запросить номер датчика
#define	cmdCALIBRATE		0x0c	// PDD: калибровка сенсоров
#define cmdREDUCE_PWR       0x0d    // PPD: перевести передатчик в режим пониженного энергопотребления
#define cmdGET_PARAMS       0x0e    // PPD: получить параметры устройства
#define cmdCLEAR_POINTERS   0x0f    // PPD: сбросить указатель транзакций
/* 0x01 - 0x0f  PDD */
#define CMD_LOAD_FMW	    0x10	// ALL: Загрузить микропрограмму
#define	cmdLOAD_FMW			0x10	// ALL: загрузить микропрограмму
#define CMD_GETSTATE_GPS	0x11	// GPS: Запрос статуса GPS терминала
#define CMD_GET_LOCATION	0x12	// GPS: Получить координаты местоположения
#define CMD_SEND_SMS	    0x13	// GPS: отослать SMS 
#define CMD_GET_DATETIME	0x14	// GPS: передать текущие дату и время
/* 0x15 – 0x1f	GPS: Резерв */

#define cmdCLOSE_SESS       0x20    // Завершить сессию c PPD
#define cmdERROR			0x22	// Ошибка протокола

#define cmdDISPL_SEAT       0x2a    // Отобразить состояние сидений    
/* 0x2a – 0x2f	DRIVERUNIT: Резерв */

#define cmdSYNCHRO_RF       0x30    // PPD: пакет синхронизации
#define cmdSET_CHANNEL      0x31    // PPD: установить радиоканал
#define cmdENABLE_SLEEP     0x32    // PPD: активировать режим пониженного энергопотребления
#define cmdDISABLE_SLEEP    0x33    // PPD: отключить режим энергосбережения 
#define cmdRESET            0x34    // PPD: перезагрузка датчика(ов)
#define cmdDEBUG            0x3f    // для отладки
/* 0x30 - 0x3f  PDD */

#define CMD_CHECK_BTSTF 	0x7e    // DEBUG - test bytestuffing

// Коды запросов для бортового контроллера
#define REQ_GET_LAST_EVN	0xa0	// Передать последнее событие со сдвигом указателя
#define REQ_GET_SPEC_EVN	0xa1	// Передать определённое событие
#define REQ_QTY_EVN	        0xa2	// Количество невычитанных событий
#define REQ_CLEAR_HIS	    0xa3	// Очистить историю событий
#define REQ_GET_DATETIME	0xa4	// Получить текущие дату и время
#define REQ_SET_DATETIME	0xa5	// Установить дату и время
#define REQ_SET_SMSINT	    0xa6	// Установить интервал отсылки SMS
#define REQ_GET_STATE	    0xa7	// Получить состояние контроллера
#define REQ_EXT_IRQ	        0xa8	// Прерывание от внешнего устройства
#define REQ_GET_SEATS_INFO  0xa9    // Запросить данные о занятости сидений на рабочем канале
#define REQ_AWAKE_PPD       0xaa    // Вывести датчики из состояния "глубокого сна" 
#define REQ_REINIT_RF       0xab    // Переинициализировать радиомодуль
#define REQ_GET_INFO_DT     0xac    // Передать info о системе за заданный промежуток времени
#define REQ_GET_EVENT       0xad    // Передать данные о событии 
#define REQ_GET_WEB_DATA    0xae    // Передать данные для отображения на Web-сервере мониторинга 

// Коды ошибок для обмена данными
#define NO_ERROR	        0x00	// Нет ошибок
#define	errNoError			0x00	// 00 Нет ошибок
#define ERR_BAD_COMMAND	    0x01	// 01 Ошибка кода команды (CMD)
#define	errBadCommand		0x01	// 01 Ошибка кода команды (CMD)
#define ERR_DATA	        0x02	// 02 Ошибка в загружаемых данных
#define	errBadData			0x02	// 02 Ошибка в загружаемых данных
#define ERR_BAD_SIZE	    0x03	// 03 Ошибка размера аргумента команды
#define	errBadSize			0x03	// 03 Ошибка размера аргумента команды
#define ERR_BAD_CRC16	    0x04	// 04 Ошибка CRC
#define	errBadCrc			0x04	// 04 Ошибка CRC пакета
#define ERR_NODATA	        0x05	// 05 Нет данных для передачи
#define	errNoData			0x05	// 05 Нет данных для передачи
#define ERR_BADPARAM	    0x06	// 06 Неверный параметр
#define	errBadParam			0x06	// 06 Неверный параметр
#define	errAccess			0x07	// 07 Нет доступа (сессия не открыта)
#define ERR_BAD_SRCADR      0x08    // 08 Неверный адрес отправителя
#define ERR_NOTSENT_SMS     0x09    // 09 Предыдущее SMS ещё не отправлено
#define ERR_EXEC	        0x0a	// 10 Ошибка выполнения команды (запроса)
#define	errExec				0x0a	// 10 Ошибка выполнения
#define ERR_SMS_NODATA      0x0b    // 11 В запрашиваемом SMS нет данных
#define ERR_NO_EVENTS       0x0c    // 12 Нет невычитанных событий
#define ERR_BAD_DSTADR      0x0d    // 13 Неверный адрес получателя
#define ERR_FROM_PPD        0x0e    // 14 Пришел ответ с ошибкой от датчика присутствия пассажира
#define ERR_WAITING_ANSWER	0x0f    // 15 В ожидании ответа
#define	ERR_READ_DF	        0x10	// Ошибка чтения данных из DataFlash
#define ERR_IDLE            0x11       // Пустой вход - нет выполнения задачи
//Коды ошибок Лира
#define  ERR_ROTATION   160          // 160 (a0) - Ошибка вращения двигателя
#define  ERR_MOTORJAM   161          // 161 (a1) - Шаговый мотор заклинило
#define  OK_COMPLETE    162          // 162 (a2) - Нет ошибки - завершено
#define  ERR_NOSTEPS    163          // 163 (a3) - Нет шагов для вращения ШД
/*
#define  ERR_MALLOC     164          // 164 (a4) - Ошибка динамического выделения памяти
#define  ERR_SPI         165         // 165 (a5) - Ошибка обмена данными по интерфейсу SPI
#define  ERR_I2C_WRITE   166         // 166 (a6) - Ошибка записи данных по интерфейсу I2C
#define  ERR_I2C_READ    167         // 167 (a7) - Ошибка чтения данных по интерфейсу I2C
#define  ERR_DF_WRITE    168         // 168 (a8) - Ошибка записи во внешнюю DataFlash
#define  ERR_DF_READ     169         // 169 (a9) - Ошибка чтения из внешней DataFlash
#define  ERR_GET_DT      170         // 170 (aa) - Ошибка чтения даты и времени из часы-календаря
#define  ERR_SET_DT      171         // 171 (ab) - Ошибка записи даты и времени в часы-календарь
#define  ERR_INIT_RF     172         // 172 (ac) - Ошибка инициализации радиомодуля
#define  ERR_RF_SEND     173         // 173 (ad) - Ошибка передачи данных в радиомодуль
#define  ERR_UART0_SB    174         // 174 (ae) - Ошибка передачи байта через UART0
#define  ERR_UART1_SB    175         // 175 (af) - Ошибка передачи байта через UART1
#define  ERR_ERRANSWER   178   // 178 (b2) - Получен ответ с ошибкой
#define  ERR_WDT_RESET   181    // 181 (b5) - Произошёл сброс по WatchDog таймеру
#define  ERR_UNKNOWN,    182          // 182 (b6) - Обнаружена неизвестная ошибка
*/
#define  ERR_HOME_ON_AT_INIT  183    // 183 (b7) - Датчик Home в "1" при начале инициализации оси
#define  ERR_NOT_INIT_AXIS    184    // 184 (b8) - Перемещение по оси не возможно т.к. ось не инициализирована
#define  ERR_MOTOR_BUSY       185    // 185 (b9) - Мотор оси в работе при установке нового положения
#define  ERR_AHEAD_MAN        186    // 186 (ba) - Манипулятор не выдвинулся за определённое время
#define  ERR_BEHIND_MAN       187    // 187 (bb) - Манипулятор не убрался за определённое время
#define  ERR_AHEAD_MAN_SEN    188    // 188 (bc) - Манипулятор отошёл в состоянии, когда он выдвинут   
#define  ERR_BEHIND_MAN_SEN   189    // 189 (bd) - Манипулятор отошёл в состоянии, когда он задвинут 
#define  ERR_CLOSE_ZAH_SEN    190    // 190 (be) - Захват отошёл от датчика, когда он закрыт
#define  ERR_OPEN_ZAH_SEN     191    // 191 (bf) - Захват отошёл от датчика, когда он открыт 
#define  ERR_OPEN_ZAH         192    // 192 (c0) - Захват не открылся за отведённое время  
#define  ERR_CLOSE_ZAH        193    // 193 (c1) - Захват не закрылся за отведённое время
#define  ERR_AHEAD_FIN        194    // 194 (c2) - Палец не выдвинулся за определённое время
#define  ERR_BEHIND_FIN       195    // 195 (c3) - Палец не убрался за определённое время
#define  ERR_AHEAD_FIN_SEN    196    // 196 (c4) - Палец отошёл в состоянии, когда он выдвинут   
#define  ERR_BEHIND_FIN_SEN   197     // 197 (c5) - Палец отошёл в состоянии, когда он задвинут

// Адресация устройств системы
#define DEBUGPC	            0x00	// Отладочный компьютер
#define ONBOARDCONTR	    0x01	// Бортовой контроллер
#define GPSTERM             0x02	// GPS терминал
#define DRIVERUNIT	        0x03	// Водительский блок

#define DEBUGPC	            0x00	// Отладочный компьютер
#define CCU     0x01             //ЦУМ Центральный управляющий модуль 
#define PO      0x02              //Пульт оператора
#define SUPPLYUNIT     0x03     //Модуль подачи 
#define BENDINGUNIT     0x04    //Модуль гибки	
                            	
// Адрес в сети UART
#define MY_ADDRESS          CCU 

// Адресация датчиков присутствия пассажира
#define UIN(number)         (u16)(BOARD + (PPD_1 + number - 1))         
#define NUM(uin)            (uchar)(uin - (BOARD + PPD_1 - 1))          // @ uin - 0xb36

/* Type definitions ----------------------------------------------------------*/
// Заголовок пакета для MRF49XA
/*
 [0]    [1...2]   [3...4]    [5]     [6...n]     [n+1...n+2]
DSize   DstAddr   SrcAddr    Cmd	   Data		    Crc 			
1b		2b		  2b	     1b      0..DSize	    2b
*/
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

// Нумерация датчиков присутствия пассажиров
typedef enum 
{
  PPD_1 = 0xA0,         // 0xa0 Датчик присутствия пассажира №1
  PPD_2,                // 0xa1 Датчик присутствия пассажира №2    
  PPD_3,                // 0xa2 Датчик присутствия пассажира №3
  PPD_96 = 0xFF         // 0xff Датчик присутствия пассажира №96
} AddrPPD_TypeDef;
       
// Коды ошибок системы
typedef enum 
{
  ERR_WRITE_FLASH = 156,    // 156 (9c) - Ошибка записи во внутреннюю FLASH
  ERR_READ_FLASH,           // 157 (9d) - Ошибка чтения из внутренней FLASH
  ERR_ADR_READ_MEMORY,      // 158 (9e) - Неверный адрес для чтения данных 
  ERR_ADR_WRITE_MEMORY,     // 159 (9f) - Неверный адрес для записи данных 
  ERR_NOACK_GPS,            // 160 (a0) - Нет ответа от GPS терминала
  ERR_NOACK_PPD,            // 161 (a1) - Нет ответа от датчика присутствия пассажира
  ERR_TRIES_SMS,            // 162 (a2) - Исчерпано количество попыток отсылки SMS 
  ERR_RANGE_PPD,            // 163 (a3) - Номер датчика вне допустимого диапазона номеров
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
  ERR_UART1_SB,             // 175 (af) - Ошибка передачи байта через UART0
  ERR_SEND_SMS,             // 176 (b0) - Ошибка отсылки SMS
  ERR_NOCONNECT_PPD,        // 177 (b1) - Датчик не отвечает (нет соединения)
  ERR_ERRANSWER,            // 178 (b2) - Получен ответ с ошибкой
  ERR_NOEXIST_PPD,          // 179 (b3) - Датчика нет в сети
  ERR_FIND_EVENT,           // 180 (b4) - Не найдено нужное Событие в памяти DF
  ERR_WDT_RESET,            // 181 (b5) - Произошёл сброс по WatchDog таймеру
  ERR_UNKNOWN               // 182 (b6) - Обнаружена неизвестная ошибка
} ErrCodes_TypeDef;

// Команды для отладки
typedef enum 
{
  REQ_PPD_TRSM_ON = 0xe0,   // 0xe0
  REQ_PPD_TRSM_OFF,         // 0xe1
  REQ_PPD_GET_PARAMS,       // 0xe2
  REQ_PPD_SET_PARAMS,       // 0xe3    
  REQ_PPD_PING_REQUEST,     // 0xe4
  REQ_PPD_READ_LTSA,        // 0xe5
  REQ_PPD_READ_STSA,        // 0xe6
  REQ_PPD_REDUCE_PWR,       // 0xe7
  REQ_PPD_SET_DATATIME,     // 0xe8
  REQ_PPD_CONFIG_SEN,       // 0xe9
  REQ_PPD_GETUIN,           // 0xea
  REQ_PPD_GET_DT,           // 0xeb
  REQ_PPD_CALIBRATE,        // 0xec
  REQ_PPD_CLEAR_PTRS,       // 0xed
  REQ_PPD_SWITCH_SLEEP,     // 0xee
  REQ_PPD_SET_CHANNEL,      // 0xef
  REQ_COUNT_PPD,            // 0xf0
  REQ_RESYNCH_PPD,          // 0xf1
  REQ_PPD_ENABLE_SLEEP,     // 0xf2
  REQ_PPD_DISABLE_SLEEP,    // 0xf3
  REQ_PPD_RESET,            // 0xf4
  REQ_STOP_SYNCH_PPD,       // 0xf5
  REQ_NEXT_CH,              // 0xf6
  REQ_PREV_CH,              // 0xf7
  REQ_GPS_GETSTATE,         // 0xf8
  REQ_GPS_GET_LOCATION,     // 0xf9
  REQ_GPS_SEND_SMS,         // 0xfa
  REQ_GPS_GET_DATETIME      // 0xfb
} DebugCmd_TypeDef;

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

#pragma pack(1)
typedef struct _param {
	uint  SelfAddr;			// Собственный адрес
	uint  RemoteAddr;		// Адрес клиента
	uint  Sensitivity1;		// Чуствительность сенсора 1 [1 .. 1000]
	uint  Sensitivity2;		// Чуствительность сенсора 2 [1 .. 1000]
    uint  Threshold1;       // Нижний порог сенсора 1 (гистерезис)
    uint  Threshold2;       // Нижний порог сенсора 2 (гистерезис)
	uint  SensTime1;		// Время задержки состояния сенсора 1 (sec) [0 ... 60]
	uint  SensTime2;		// Время задержки состояния сенсора 2 (sec) [0 ... 60]
	uint  BatteryTresh;		// Пороговое значение для детектора разряда батареи
    uchar TransmitPower;    // Уровень мощности передатчика
	uint  Crc;
} TParamsPPD, *pTParamsPPD;
#pragma pack()

#pragma pack(1)
typedef struct _stat_pdd {
	union {
		uchar   FlagsWord;
		struct {
			uchar	Rtc:1;				// Ошибка таймера (1), таймер ОК (0)
			uchar	Sensor:1;			// Ошибка сенсора (1), сенсор ОК (0)
			uchar	DataFlash:1;		// Ошибка dataflash (1), dataflash ОК (0)
			uchar	Battery:1;			// Напряжение батареи ниже порога (1), Напряжение батареи в норме (0)
			uchar	Passenger1:1;		// сиденье №1 присутствует (1), нет пассажира (0)
			uchar	Passenger2:1;		// сиденье №2 присутствует (1), нет пассажира (0)
            uchar   EnableSleep:1;      // активирован режим энергосбережения
			uchar	Res:1;

		};
	};
	union {
		ulong   DevId;				// Идентификатор устройства
		uchar	DevIdByte[4];		// Идентификатор устройства
	};
	TRtcPPD DateTime;		    // Текущие Дата и Время	
	ulong	Qty_NonreadTSA;		// Кол. невычитанных транзакций	
	ulong	Qty_Index;			// Индекс 1-й невычитанной транзакции
    uint    C1;                 // Емкость 1-го сенсора [fF]
    uint    C2;                 // Емкость 2-го сенсора [fF]
} TStatusPDD, *pTStatusPDD;
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

#pragma pack(1)
typedef struct _calibr
{
	union {
        uchar   Sensors;
		struct {
			uchar   Sensor1:1;
			uchar   Sensor2:1;
			uchar	Reserv:6;
		};
	};
} TCalibrSens;
#pragma pack()

// Структура транзакции датчика присутствия пассажиров
#pragma pack(1)
typedef struct _tranz 
{
	ulong			Id;
	struct {
		uchar		Passenger1:1;
		uchar		Passenger2:1;
		uchar		Reserv:6;
	};
	DataTime_TypeDef Dt;
    uint            C1;                 // Емкость 1-го сенсора [fF]
    uint            C2;                 // Емкость 2-го сенсора [fF]
} TTranz;
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

// Структура статуса GPS терминала
#pragma pack(1)
typedef struct _gpsstat
{
  //---------------- BYTE 0 ----------------------------------------- 
  union {
    uchar Byte_0;
    struct {
      uchar	Antenna:1;          // Антенна GPS подключена (1), отключена (0)
      uchar	AntennaFailed:1;	// Антенна закорочена (1), антенна ОК (0)
      uchar	IntBatt:3;    		// Уровень заряда внутренней батареи
      uchar	LevelGSM:3;    	    // Уровень GSM сигнала
    };
  };
  //---------------- BYTE 1 – 6 ----------------------------------------- 
  DataTime_TypeDef	DataTime;	// Текущие Дата и Время (см. п. 16.5.2)
  //---------------- BYTE 7 – 10 ----------------------------------------- 
  uint	Latitude_IntPart;	    // Целая часть градусов широты
  uint	Latitude_FrPart;	    // Дробная часть градусов широты
  //---------------- BYTE 11 – 15 ----------------------------------------- 
  uchar	Longitude_IntPart1;	    // Целая часть градусов долготы 1
  uint	Longitude_IntPart2;	    // Целая часть градусов долготы 2
  uint	Longitude_FrPart	;	// Дробная часть градусов долготы
  //---------------- BYTE 16 ----------------------------------------- 
  uchar	Speed	;			    // Скорость движения в узлах (морская миля)
  //---------------- BYTE 17 ----------------------------------------- 
  uchar	Qty_VisibleSats;		// Число видимых спутников
  //---------------- BYTE 18 ----------------------------------------- 
  union {
    uchar Byte_18;
    struct {
      uchar	StartMoving:1;      // Флаг начала движения
      uchar	EndMoving:1;		// Флаг окончания движения
      uchar	SenGPIO_1:1;		// Значение дискретного датчика 1
      uchar	SenGPIO_2:1;		// Значение дискретного датчика 2
      uchar	SenGPIO_3:1;		// Значение дискретного датчика 3
      uchar	SenGPIO_4:1;		// Значение дискретного датчика 4
      uchar	SenGPIO_5:1;		// Значение дискретного датчика 5
      uchar	SenGPIO_6:1;		// Значение дискретного датчика 6
    };
  };
  //---------------- BYTE 19 ----------------------------------------- 
  union {
    uchar Byte_19;
    struct {
      uchar	ExtBatt:1;          // Внешнее зар. устр-во подключено (1),
                                //  не подключено (0)
      uchar	DataExists:1;		// В памяти есть данные для отгрузки (1),
                                // нет данных (0)
      uchar NotSentSMS:1;       // предыдущее SMS не отправлено на сервер (1)
                                // SMS отправлено на сервер (0)
      uchar	ReserveBits:5;		// Резерв
    };
  };
  //---------------- BYTE 20 – 22 ----------------------------------------- 
  uchar	SenAnalog_1;		    // Значение аналогового датчика 1
  uchar	SenAnalog_2;		    // Значение аналогового датчика 2
  uchar	SenAnalog_3;		    // Значение аналогового датчика 3
  //---------------- BYTE 23 ----------------------------------------- 
  uchar	ReserveByte;		    // Резерв
} TGPSStatus, *pTGPSStatus;
#pragma pack()

// Структура местоположения от GPS терминала "M2M"
#pragma pack(1)
typedef struct _location
{ 
  //---------------- BYTE 0 - 3 ----------------------------------------- 
  uint	Latitude_IntPart;		// Целая часть градусов широты
  uint	Latitude_FrPart;		// Дробная часть градусов широты
  //---------------- BYTE 4 – 8 ----------------------------------------- 
  uchar	Longitude_IntPart1;	    // Целая часть градусов долготы 1
  uint	Longitude_IntPart2;	    // Целая часть градусов долготы 2
  uint	Longitude_FrPart;  	    // Дробная часть градусов долготы
} TLocation, *pTLocation;
#pragma pack()

// Структура координат для GoogleMaps
#pragma pack(1)
typedef struct _googlegps
{ 
  //---------------- BYTE 0 - 3 ----------------------------------------- 
  uchar	Lat_Degree;		// Градусы широты
  uchar Lat_Min;        // Минуты широты
  uchar Lat_Sec;        // Секунды широты
  uchar	Lat_FrSec;		// Дробная часть секунд широты
  //---------------- BYTE 4 – 8 ----------------------------------------- 
  uchar	Lon_Degree1;	// Градусы долготы часть 1
  uchar	Lon_Degree2;	// Градусы долготы часть 2
  uchar Lon_Min;        // Минуты долготы
  uchar Lon_Sec;        // Секунды долготы
  uchar	Lon_FrSec;		// Дробная часть секунд долготы
} TGoogleGPS, *pTGoogleGPS;
#pragma pack()

/** Структуры для отображения на мониторе водителя (DriverMonitor) 
    информации о состоянии датчиков и о занятых/свободных сидениях */

#pragma pack(1)
typedef struct _involvedppd {
	uchar		Total;				// Общее количество датчиков в системе
} TInvolvedPPD, *pTInvolvedPPD;
#pragma pack()

#pragma pack(1)
typedef struct _seat {
	uchar Number;				    // Порядковый номер PPD
    union {
        uchar Params;
	    struct {
		    uchar	Acked:1;			// Состояние контроллера PPD: 1 - отвечает, 0 - не отвечает
		    uchar	RightSeat:1;		// Состояние правого сидения: 1 - занято, 0 - свободно
		    uchar	LeftSeat:1;			// Состояние левого сидения: 1 - занято, 0 - свободно
		    uchar	PowerLow:1;			// Состояние батареи: 1 - батарея разряжена, 0 - в норме
            uchar   EnableSleep:1;      // 1 - в датчике активирован режим энергосбережения
		    uchar	Reserve:3;
	    };
    };
} TSeat, *pTSeat;
#pragma pack()

/** В поле данных пакета контроллер передаёт:
    TCountPPD (1 Byte) + TSeat[TCountPPD::Total]    */
#pragma pack(1)
typedef struct _showseats {
  uchar Total;              // Общее количество датчиков в системе
  TSeat Seats[QTYPPD];  
} TShowSeats, *pTShowSeats;
#pragma pack()

// Информация о сидениях с привязкой ко времени
#pragma pack(1)
typedef struct _infoseats {
  DataTime_TypeDef  DT;     // Дата и время сбора информации
  uint		        Ch;		// Номер рабочего радиоканала
  TShowSeats        Show;   // Информация по датчикам и сидениям
} TInfoSeats, *pTInfoSeats;
#pragma pack()

// Структура события
#pragma pack(1)
typedef struct _event
{
  ulong		        ID;				// Идент. номер события	
  DataTime_TypeDef	DataTime;       // Дата и время события
  TGoogleGPS        Coordinates;	// Координаты
  uchar 	        OccupSeats;	    // Кол-во занятых мест
  uchar		        FreeSeats;	    // Кол-во свободных мест
  uchar		        FailedPPD;	    // Кол-во неисправных датчиков
  TInfoSeats        IS;             // Информация по всем сидениям
  union {
    uchar Flags;
	struct {
      uchar	AutoSave:1;     // 1 - автоматически созданное событие, 0 - по открыванию двери
      uchar StartRoute:1;   // 1 - признак начала маршрута
      uchar EndRoute:1;     // 1 - признак конца маршрута      
      uchar	Reserve:5;
	};
  }; 
  double	        Mileage;		// Пробег в метрах
  uchar 	        Speed;		    // Скорость движения в узлах
  uchar             Reserve_1;
  uchar             Reserve_2;
  uchar             Reserve_3;
  uint		        Crc;			// CRC данных события
} TEvent, *pTEvent;
#pragma pack()

// Структура ошибок состояния системы
#pragma pack(1)
typedef struct _systemerrors
{
  uchar ErrorsExist:1;       // Есть ошибка(и) (1), нет ошибок (0)
  uchar ErrSetDT:1;     
  uchar ErrGetDT:1;     
  uchar ErrGetEvent:1;     
  uchar Err_4:1;     
  uchar Err_5:1;     
  uchar Err_6:1;     
  uchar Err_7:1;     

  uchar Err_8:1;     
  uchar Err_9:1;     
  uchar Err_10:1;     
  uchar Err_11:1;     
  uchar Err_12:1;     
  uchar Err_13:1;     
  uchar Err_14:1;     
  uchar Err_15:1;     

  uchar Err_16:1;     
  uchar Err_17:1;     
  uchar Err_18:1;     
  uchar Err_19:1;     
  uchar Err_20:1;     
  uchar Err_21:1;     
  uchar Err_22:1;     
  uchar Err_23:1;     
 
  uchar Err_24:1;     
  uchar Err_25:1;     
  uchar Err_26:1;     
  uchar Err_27:1;     
  uchar Err_28:1;     
  uchar Err_29:1;     
  uchar Err_30:1;     
  uchar Err_31:1;     
} TSystemErrors, *pTSystemErrors;
#pragma pack()

// Структура состояния бортового контроллера
#pragma pack(4)
typedef struct _systemstatus
{
  uchar		Version;			// Версия ПО
  DataTime_TypeDef	DataTime;   // системные  дата и время
  TGoogleGPS    Coordinates;	// Координаты местоположения
  uchar 	OccupSeats;		    // Кол-во занятых мест
  uchar		FreeSeats;		    // Кол-во свободных мест
  uchar		GoodPPD;		    // Кол-во исправных датчиков
  uchar		FailedPPD;		    // Кол-во неисправных датчиков
  ulong		NonReadEvents;	    // Кол-во невычитанных Событий
  TSystemErrors	Errors; 		// Ошибки при выполненияя задач системы
  uchar     KSES[8];            // Ключ сессии //!!! Здесь не нужен !!!
  uint		WorkRadioCh;		// Номер работающего радиоканала
  uint		TimeWaitConnectRF;	// Время ожидания соединения с датчиком //!!! Здесь не нужен !!!
  uchar     Speed;              // Скорость движения в узлах
  uchar		Reserve_1;			// Резерв
  uchar		Reserve_2;			// Резерв
  uchar		Reserve_3;			// Резерв
  ulong		Reserve_4567;		// Резерв
  ulong		Reserve_8901;		// Резерв
} TSystemStatus, *pTSystemStatus;
#pragma pack()

// Структура данных сообщения SMS:
#pragma pack(1)
typedef struct _sendsms
{
  uchar	Len;			// Длина сообщения	
  char	ShortMes[140];	// Сообщение
  char	CelNumber[32];	// Номер телефона (пример: +380509229934).
                        // Если CelNumber равен 0, то пакет 
                        // отсылается на сервер
} TSendSMS, *pTSendSMS;
#pragma pack()

// Структура установки интервала передачи SMS:
#pragma pack(1)
typedef struct _smsint
{
  char	CelNumber[32];  // Номер телефона (например: "+380509229934\n").
                        // Если CelNumber равен 0, то пакеты 
                        // отсылаются на сервер
  uint	SMS_Interval;	// Интервал в минутах
                        // Если SMS_Interval равен 0, то пакеты 
                        // не отсылаются
} TSMSInt, *pTSMSInt;
#pragma pack()

// Структура номера транзакции PPD:
#pragma pack(1)
typedef struct _numbertsa
{
  uint		UIN;			// Уникальный номер датчика
  ulong		TSA_Number;		// Номер транзакции
} TNumTSA, *pTNumTSA;
#pragma pack()

// Структура запроса на получение транзакции PPD:
#pragma pack(1)
typedef struct _voidreg
{
    uchar       NumPPD;
    uchar       CheckNet;
} TVoidReq, *pTVoidReq;
#pragma pack()

#pragma pack(1)
typedef struct _reqtsa
{
  TVoidReq	VReq;			// Номер датчика на борту
  ulong		TSA_Number;		// Номер транзакции
} TReqTSA, *pTReqTSA;
#pragma pack()

// Структура запроса на установку даты и времени в PPD:
#pragma pack(1)
typedef struct _reqsetdt
{
  TVoidReq	          VReq;	// Номер датчика на борту
  DataTime_TypeDef    DT;  	// Устанавливаемые дата и время
} TReqSetDT, *pTReqSetDT;
#pragma pack()

#pragma pack(1)
typedef struct _reqcalibr
{
  TVoidReq	VReq;
  uchar		Sensor1:1;
  uchar		Sensor2:1;
  uchar		Reserv:6;
} TReqCalibr, *pTReqCalibr;
#pragma pack()

// Структура данных для конфигурации датчика:
#pragma pack(1)
typedef struct _configsen
{
  uchar	Ch1_Setup;		    // SETUP REGISTER канала 1
  uchar	Ch2_Setup;		    // SETUP REGISTER канала 2
  uchar	Configuration;		// CONFIGURATION REGISTER
  uchar	PowerDownTimer;	    // POWER-DOWN TIMER REGISTER
  uchar	Ch1_Capdac;		    // CAPDAC REGISTER канала 1
  uchar	Ch2_Capdac;		    // CAPDAC REGISTER канала 2
} TConfigSen, *pTConfigSen;
#pragma pack()

// Структура запроса на установку параметров сенсора в PPD:
#pragma pack(1)
typedef struct _reqcfgsen
{
  TVoidReq	    VReq;		// Номер датчика на борту
  TConfigSen    CfgParams; 	// Устанавливаемые параметры конфигурации
} TReqCfgSen, *pTReqCfgSen;
#pragma pack()

// Структура номера датчика присутствия пассажиров
#pragma pack(1)
typedef struct _numppd
{
  uint	UIN;			// Уникальный номер датчика
} TNumPPD, *pTNumPPD;
#pragma pack()

// Структура состояния чувствительного элемента датчика присутствия пассажиров
#pragma pack(1)
typedef struct _checksensor
{
  uchar	StatusRegister;	// значение STATUS REGISTER 
  ulong	SerialNumber;	// значение SERIAL NUMBER REGISTER
} TCheckSensor, *pTCheckSensor;
#pragma pack()

#pragma pack(1)
typedef struct _reqsetparamsppd
{
  TVoidReq	    VReq;
  TParamsPPD    Params;
} TReqSetParamsPPD, *pTReqSetParamsPPD;
#pragma pack()

#pragma pack(1)
typedef struct _reqsetchannelppd
{
  TVoidReq  VReq;
  uint      Channel;
} TReqSetChannelPPD, *pTReqSetChannelPPD;
#pragma pack()

// Запрос info за данный промежуток времени
#pragma pack(1)
typedef struct _reqgetinfodt
{
  ulong             Board;      // Номер автобуса (например, 2711)    
  DataTime_TypeDef  DTStart;    // Дата и время начального события     
  DataTime_TypeDef  DTEnd;      // Дата и время конечного события 
} TReqGetInfoDT, *pTReqGetInfoDT;
#pragma pack()

// Ответ на запрос информации за данный промежуток времени
#pragma pack(1)
typedef struct _answerinfodt
{
  ulong             Board;          // Номер автобуса (например, 2711)    
  DataTime_TypeDef DT_StartEvn;     // Дата и время первого события в заданном промежутке времени
  DataTime_TypeDef DT_EndEvn;       // Дата и время последнего события в заданном промежутке времени
  ulong         ID_StartEvn;        // Идент. номер начального события в заданном промежутке времени
  ulong         ID_EndEvn;          // Идент. номер конечного события в заданном промежутке времени
  ulong         QtyStops;           // Количество остановок за данный промежуток времени
  double        Time;               // Время в пути за данный промежуток времени (s)
  double        Distance;           // Расстояние за данный промежуток времени (m)
  double        Passengers;         // Среднее количество перевезенных пассажиров
  double        TotalPasTime;       // Время занятости сидений за данный промежуток времени [h*s] (человек*секунд) 
  double        TotalPasDistance;   // Общее расстояние по всем занятым сидениям [h*m] (человек*метр)
} TAnswerInfoDT, *pTAnswerDT;
#pragma pack()

// Запрос от программы администратора "Дать информацию о событии"
#pragma pack(1)
typedef struct _reqgetevn
{
	ulong   Board;
	ulong   NumEvn;
} TReqGetEvn;
#pragma pack()

// Структура данных для отображения на WEB-сервере
#pragma pack(1)
typedef struct _webdata
{
  uint		        ID;			// Идент. номер события	
  DataTime_TypeDef	DataTime;   // Дата и время события
  TGoogleGPS    Coordinates;	// Координаты местоположения
  uchar 	OccupSeats;		    // Кол-во занятых мест
  uchar		FreeSeats;		    // Кол-во свободных мест
  uchar		FailedPPD;		    // Кол-во неисправных датчиков
  uint		WorkRadioCh;		// Номер работающего радиоканала
  uchar     Speed;              // Скорость движения в узлах
  union {
    uchar Flags;
	struct {
      uchar	AutoSave:1;     // 1 - автоматически созданное событие, 0 - по открыванию двери
      uchar StartRoute:1;   // 1 - признак начала маршрута
      uchar EndRoute:1;     // 1 - признак конца маршрута      
      uchar	Reserve:5;
	};
  };
  uchar		Reserve_1;			// Резерв
  uchar		Reserve_2;			// Резерв
  uint		Reserve_34; 		// Резерв
  uint		Reserve_56;	    	// Резерв
} TWebData, *pTWebData;
#pragma pack()

// Структура количества датчиков в системе:
#pragma pack(1)
typedef struct _countppd
{
  uchar		Total;		// Общее количество датчиков в системе
  uchar		Alive;		// Количество "живых" датчиков
  uchar     Dead;       // Количество "дохлых" датчиков
  union 
  {
    ulong WhereIs_1;
	struct 
    {
      uchar D_1:1;          // 1 - датчик "живой", 0 - датчик "дохлый"
      uchar	D_2:1;
      uchar	D_3:1;
      uchar	D_4:1;
      uchar	D_5:1;
      uchar	D_6:1;
      uchar	D_7:1;
      uchar	D_8:1;
      uchar	D_9:1;
      uchar	D_10:1;
      uchar	D_11:1;
      uchar	D_12:1;
      uchar	D_13:1;
      uchar	D_14:1;
      uchar	D_15:1;
      uchar	D_16:1;
      uchar	D_17:1;
      uchar	D_18:1;
      uchar	D_19:1;
      uchar	D_20:1;
      uchar	D_21:1;
      uchar	D_22:1;
      uchar	D_23:1;
      uchar	D_24:1;
      uchar	D_25:1;
      uchar	D_26:1;
      uchar	D_27:1;
      uchar	D_28:1;
      uchar	D_29:1;
      uchar	D_30:1;
      uchar	D_31:1;
      uchar	D_32:1;
	};
  };
} TCountPPD, *pTCountPPD;
#pragma pack()
/* --- Draft ---
  union 
  {
    ulong WhereIs_2;
	struct 
    {
      uchar D_33:1;          
      uchar	D_34:1;
      uchar	D_35:1;
      uchar	D_36:1;
      uchar	D_37:1;
      uchar	D_38:1;
      uchar	D_39:1;
      uchar	D_40:1;
      uchar	D_41:1;
      uchar	D_42:1;
      uchar	D_43:1;
      uchar	D_44:1;
      uchar	D_45:1;
      uchar	D_46:1;
      uchar	D_47:1;
      uchar	D_48:1;
      uchar	D_49:1;
      uchar	D_50:1;
      uchar	D_51:1;
      uchar	D_52:1;
      uchar	D_53:1;
      uchar	D_54:1;
      uchar	D_55:1;
      uchar	D_56:1;
      uchar	D_57:1;
      uchar	D_58:1;
      uchar	D_59:1;
      uchar	D_60:1;
      uchar	D_61:1;
      uchar	D_62:1;
      uchar	D_63:1;
      uchar	D_64:1;
	};
  };
*/

// Состояния функций управления передачей команд и запросов
typedef enum 
{
  SMS_Idle,
  SMS_Prepare,
  SMS_Send,
  SMS_WaitAck,
  SMS_Complete
} SendSmsState_TypeDef;

typedef enum 
{
  GC_Idle,
  GC_PrepareData,
  GC_StartConnect,
  GC_WaitConnect,
  GC_SendCmd,
  GC_WaitAns,
  GC_GetData,
  GC_ReducePwr,
  GC_Exit,
  GC_Complete
} GiveCmdState_TypeDef;

/* Exported functions ------------------------------------------------------- */
void checkPacket_MRF49XA(void);     
void parserPacket_MRF49XA(void);
void checkPacket_UARTx(uchar uart_number);
void parserPacket_UARTx(uchar uart_number);
uint handler_SendSMS(uchar uart_number, uchar adr_dst, uchar cmd, uchar *data);
uint sendSMS(uchar uart_number, uchar adr_dst, uchar cmd, uchar *data, SendSmsState_TypeDef *state);
uint handler_giveCmdPPD(uchar num, uchar cmd, void *idata, void *rdata, uchar enc, uchar checknet);
uint giveCmdPPD(uchar cmd, void *idata, void *rdata, GiveCmdState_TypeDef *state, uchar enc);
uint handler_giveCmdGPS(uchar cmd, uchar *idata, uchar *rdata);
uint giveCmdGPS(uchar cmd, void *idata, void *rdata, GiveCmdState_TypeDef *state);
void GPS_parserPacket_UARTx(uchar uart_number);
void checkError(uchar permit_ch);       //Проверка состояний и ошибок Лира

#endif /* __EXTCOM_H */

