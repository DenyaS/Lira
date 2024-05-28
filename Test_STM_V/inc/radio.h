
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __RADIO_H
#define __RADIO_H

/* Includes ------------------------------------------------------------------*/
#include "unsigned.h"
#include "macros.h"

/* Defines ------------------------------------------------------------------*/
#define RCS     DBOARD_GPIO_PORT_2, RF_CS       // Вывод CS микросхемы MRF49XA
#define RRST    DBOARD_GPIO_PORT_2, RF_RST      // Reset signal for MRF49XA
#define RFSEL   DBOARD_GPIO_PORT_1, RF_FSEL     // Frequency Shift Keying signal    
#define RIRQ    DBOARD_GPIO_PORT_1, RF_IRO      // Сигнал прерывания от MRF49XA
#define RSDO    SPI_GPIO_PORT, MISO             // Входной сигнал от MRF49XA

#define LED_RF  LEDS_GPIO_PORT, LEDGREEN        // Индикация передачи УКВ - зеленый светодиод

#define NOENCRIPT			0					// Не шифровать данные в пакете
#define ENCRIPT				1					// Шифровать данные в пакете


#define BROADCAST_ADDR		(BOARD + 0xD8F0)    // Широковещательный адрес
#define GROUPCAST_ADDR		(BOARD + 0xD8EF)	// Групповой адрес
#define DATASIZE			68					// Максимальный размер данных в пакете
#define BUFSIZE				DATASIZE + 1 + 2 + 10 + 2	// Размер буфера для пакета

#define PREAMBLE_2          0xAA        // Второй байт преамбулы
#define SYNCHROSEQUENCE     0xD42D      // 1st Byte = 0x2d, 2nd Byte = 0xd4

// Статус сессии связи
#define SESS_CLOSE				0
#define SESS_AUTORIZE			1
#define SESS_OPEN				2

#define F_OFFSV		0x0010
#define F_AFCCT		0x0020
#define F_CLKRL		0x0040
#define F_DQDO		0x0080
#define F_ATRSSI	0x0100
#define F_FIFOEM	0x0200
#define F_LBTD		0x0400
#define F_LCEXINT	0x0800
#define F_WUTINT	0x1000
#define F_TXOWRXOF	0x2000
#define F_POR		0x4000
#define F_TXRXFIFO	0x8000

#define B_FIFOEN	(1 << 6)
#define B_TXDEN		(1 << 7)    // 0x80

#define B_FOFEN		(1 << 0)
#define B_FOREN		(1 << 1)
#define B_HAM		(1 << 2)
#define B_MFCS		(1 << 3)

#define B_MODPLY	(1 << 8)    // 0x0100

#define B_DRSTM		(1 << 0)
#define B_FSCF		(1 << 1)
#define B_FFCS		(1 << 2)
#define B_SYCLEC	(1 << 3)

#define B_CLKODIS	(1 << 0)    // 0x00
#define B_WUTEN		(1 << 1)    // 0x02
#define B_LBDEN		(1 << 2)    // 0x04
#define B_OSCEN		(1 << 3)    // 0x08
#define B_SYNEN		(1 << 4)    // 0x10
#define B_TXCEN		(1 << 5)    // 0x20
#define B_BBCEN		(1 << 6)    // 0x40
#define B_RXCEN		(1 << 7)    // 0x80

// Режимы работы трансивера
#define RECEIVE		0
#define TRANSMIT	1

// Список состояний задачи обработки трансивера
#define IDLE_STATE			0
#define SCAN_STATE			1
#define WAIT_STATE			2
#define AUTORIZE_STATE		3
#define SESSION_STATE		4

// Режим работы радиомодуля [MASTER/SLAVE]
#ifdef __RF_MASTER_MODE__  // --- MASTER ---
	#define WAIT_ACK_TIME	60//100 // ==== DEBUG ====						// (100 mS) Время ожидания ответа на вызов
    #define HOP_CHANNEL		13						// шаг изменения номера канала (! обязательно нечетное)
#else                      // --- SLAVE --- 
	#define SCAN_TIME		50						// (50 mS) Время ожидания вызова на выбранном канале
	#define HOP_CHANNEL		9						// шаг изменения номера канала (! обязательно нечетное)
#endif /* __RF_MASTER_MODE__ */

#define AUTORIZE_TIME		600						// (600 mS) Время ожидания пакета в режиме авторизации
#define SESSION_TIME		1000					// (1000 mS) Время ожидания пакета в режиме сессии
#define SCAN_WAIT_TIME		2000					// (2000 mS) Время запрета переключения канала

/* Macroses ----------------------------------------------------------------- */
#define SET_BAT_TRESH1(x,y)	(((x) * 100 + (y)) - 225) / 10
#define SET_BAT_TRESH(x)	SET_BAT_TRESH1(x / 100, x % 100)
#define	ENABLE_RF()			off(RCS)
#define	DISABLE_RF()		on(RCS)

// Reset Radio - перезапуск детектора сихропоследовательности (FIFORSTREG | 0x0002);
#define RF_SYNC_RESTART()	{											          \
                              sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord | B_FSCF); \
							} 

// After completing the receive of packet
#define CLEAR_COMPL()		{											\
							Radio.ReceiveComplete = 0;					\
							Radio.DisChange = 0;						\
							}

/* Extern variables ----------------------------------------------------------*/
/* Numerated symbols ---------------------------------------------------------*/
enum 
{
  RANGE_RES,	// 0
  RANGE_433,	// 1
  RANGE_868,	// 2
  RANGE_915		// 3
};

enum 
{
  DEV_15,		// 0
  DEV_30,		// 1
  DEV_45,		// 2
  DEV_60,		// 3
  DEV_75,		// 4
  DEV_90,		// 5
  DEV_105,		// 6
  DEV_120,		// 7
  DEV_135,		// 8
  DEV_150,		// 9
  DEV_165,		// 10
  DEV_180,		// 11
  DEV_195,		// 12
  DEV_210,		// 13
  DEV_225,		// 14
  DEV_240		// 15
};

enum 
{
  BWIDTH_RES1,	// 0
  BWIDTH_67,	// 1
  BWIDTH_134,	// 2
  BWIDTH_200,	// 3
  BWIDTH_270,	// 4
  BWIDTH_340,	// 5
  BWIDTH_400,	// 6
  BWIDTH_RES2	// 7
};

enum 
{
  LNA_GAIN_0,	// 0
  LNA_GAIN_6,	// 1
  LNA_GAIN_14,	// 2
  LNA_GAIN_20,	// 3
};

enum 
{
  RSSI_103,		// 0
  RSSI_97,		// 1
  RSSI_91,		// 2
  RSSI_85,		// 3
  RSSI_79,		// 4
  RSSI_73		// 5
};

enum 
{
  POWER_0,		// 0 (max)
  POWER_2_5,	// 1
  POWER_5_0,	// 2
  POWER_7_5,	// 3
  POWER_10_5,	// 4
  POWER_12_5,	// 5
  POWER_15_0,	// 6
  POWER_17_5	// 7 (min)
};

enum { CS_NO = 0, CS_YES = !CS_NO };

/* Type definitions ----------------------------------------------------------*/
typedef enum
{
  RF_NODATA = 0,
  RF_DATA_RECEIVED,
  RF_PACKET_RECEIVED
} RFReceiveStatus;

typedef struct _stsreg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uchar LByte;
      uchar HByte;
	};
	struct 
    {
      uint OFFSB:4;
      uint OFFSV:1;
      uint AFCCT:1;
      uint CLKRL:1;
      uint DQDO:1;
      uint ATRSSI:1;
      uint FIFOEM:1;
      uint LBTD:1;
      uint LCEXINT:1;
      uint WUTINT:1;
      uint TXOWRXOF:1;
      uint POR:1;
      uint TXRXFIFO:1;
	};
  };
} StsReg_TypeDef;

typedef struct _gencreg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	LCS:4;
      uint	FBS:2;
      uint	FIFOEN:1;
      uint	TXDEN:1;
      uint	Adr:8;
	};
  };
} GencReg_TypeDef;

typedef struct _afccreg
{
  union 
  {
    uint HalfWord;
    struct 
    {
      uint	FOFEN:1;
      uint	FOREN:1;
      uint	HAM:1;
      uint	MFCS:1;
      uint	ARFO:2;
      uint	AUTOMS:2;
      uint	Adr:8;
	};
  };
} AfccReg_TypeDef;

typedef struct _txcreg 
{
  union 
  {
    uint HalfWord;
    struct 
    {
      uint	OTXPWR:3;
      uint	RES:1;
      uint	MODBW:4;
      uint	MODPLY:1;
      uint	Adr:7;
	};
  };
} TxcReg_TypeDef;

typedef struct _txbreg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	TXDB:8;
      uint	Adr:8;
	};
  };
} TxbReg_TypeDef;

typedef struct _cfsreg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	FREQB:12;
      uint	Adr:4;
	};
  };
} CfsReg_TypeDef;

typedef struct _rxcreg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	DRSSIT:3;
      uint	RXLNA:2;
      uint	RXBW:3;
      uint	DIORT:2;
      uint	FINTDIO:1;
      uint	Adr:5;
	};
  };
} RxcReg_TypeDef;

typedef struct _bbfcreg
{
  union 
  {
    uint HalfWord;
    struct 
    {
      uint	DQTI:3;
      uint	RES1:1;
      uint	FTYPE:1;
      uint	RES2:1;
      uint	MCRLC:1;
      uint	ACRLC:1;
      uint	Adr:8;
	};
  };
} BbfcReg_TypeDef;

typedef struct _rxfiforeg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	RXDB:8;
      uint	Adr:8;
	};
  };
} RxFifoReg_TypeDef;

typedef struct _fiforstreg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	DRSTM:1;
      uint	FSCF:1;
      uint	FFCS:1;
      uint	SYCLEC:1;
      uint	FFBC:4;
      uint	Adr:8;
	};
  };
} FifoRstReg_TypeDef;

typedef struct _synbreg 
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	SYNCB:8;
      uint	Adr:8;
	};
  };
} SynbReg_TypeDef;

typedef struct _drsreg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	DRPV:7;
      uint	DRPE:7;
      uint	Adr:8;
	};
  };
} DrsReg_TypeDef;

typedef struct _pmcreg 
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	CLKODIS:1;
      uint	WUTEN:1;
      uint	LBDEN:1;
      uint	OSCEN:1;
      uint	SYNEN:1;
      uint	TXCEN:1;
      uint	BBCEN:1;
      uint	RXCEN:1;
      uint	Adr:8;
	};
  };
} PmcReg_TypeDef;

typedef struct _wtsreg
{
  union 
  {
    uint HalfWord;
	struct 
    {
      uint	WTMV:8;
      uint	WTEV:5;
      uint	Adr:3;
	};
  };
} WtsReg_TypeDef;

typedef struct _dcsreg
{
  union 
  {
    uint HalfWord;
    struct 
    {
      uint	DCMEN:1;
      uint	DCMV:7;
      uint	Adr:8;
	};
  };
} DcsReg_TypeDef;

typedef struct _bcsreg 
{
	union 
    {
      uint HalfWord;
      struct 
      {
        uint LBDVB:4;
        uint RES:1;
        uint COFSB:3;
        uint Adr:8;
      };
  };
} BcsReg_TypeDef;

typedef struct _pllcreg
{
  union 
  {
    uint HalfWord;
    struct 
    {
      uint	PLLBWB:1;
      uint	RES1:1;
      uint	PLLDD:1;
      uint	PDDS:1;
      uint	ALWONE:1;
      uint	CBTC:2;
      uint	RES2:1;
      uint	Adr:8;
	};
  };
} PllcReg_TypeDef;

typedef struct _rfregisters
{
  struct _stsreg        STSREG;			// Status read register
  struct _gencreg       GENCREG;		// General configuration register
  struct _afccreg       AFCCREG;		// Automatic frequency control register
  struct _txcreg        TXCREG;			// Transmit configuration register
  struct _txbreg        TXBREG;			// Transmit byte register
  struct _cfsreg        CFSREG;			// Center frequency configuration register
  struct _rxcreg        RXCREG;			// Receive control register
  struct _bbfcreg       BBFCREG;		// Baseband filter configuration register
  struct _rxfiforeg     RXFIFOREG;		// Receive FIFO read register
  struct _fiforstreg    FIFORSTREG;		// FIFO and reset mode configuration register
  struct _synbreg	    SYNBREG;		// Syncronous byte configuration register
  struct _drsreg		DRSREG;			// Data rate value set register
  struct _pmcreg		PMCREG;			// Power management configuration register
  struct _wtsreg		WTSREG;			// Wake-up timer value set register
  struct _dcsreg		DCSREG;			// Duty cycle value set register
  struct _bcsreg		BCSREG;			// Battery threshold detectand clock output value set register
  struct _pllcreg	    PLLCREG;		// PLL configuration register
} RfRegisters_TypeDef;

typedef struct _rnd
{
  union {
	uchar   Buf[8];
	struct {
		ulong	Rnd1;
		ulong	Rnd2;
	};
  };
} Rnd_TypeDef;

typedef struct _frag
{
  uint  WaitSegment;
  ulong Index;
} Frag_TypeDef;

typedef struct _radio
{
	uchar		TransmitMode:1;			// Режим передачи
	uchar		Receive:1;				// Пакет принимается
	uchar		ReceiveComplete:1;		// Пакет принят
	uchar		Transmit:1;				// Пакет передаётся
	uchar		Request:1;				// Получен корректный пакет запроса
	uchar		Broadcast:1;			// Получен широковещательный пакет
	uchar		Groupcast:1;			// Получен групповой пакет
	uchar		DisChange:1;			// Запрет смены канала

	uchar		SessState:2;			// Состояние сессии: 0-1й этап авт., 0-2й этап авт., 2-открыта
	uchar		Reserv2:6;				//

	uchar		Range;					// Выбранный диапазон частот
	uchar		BandWidth;				// Выбранная ширина канала
	uchar		Deviation;				// Выбранная девиация передатчика
	uchar		Power;					// Выбранный диапазон частот
	uint		NumChannel;				// Количество каналов для заданного диапазона
	uint		Channel;				// Номер текущего канала
	uchar		TaskState;				// Состояние задачи обработки трансивера
	uint		ScanTimer;				// Таймер для сканирования каналов и сессии
    uint	    Timer;                  // Дополнительный таймер
    uint		SelfAddr;				// Собственный адрес
    uint		RemoteAddr;				// Адрес клиента
	union {
		Rnd_TypeDef Rnd;
		Frag_TypeDef Frag;
	};
	uchar		RxLen;
	uchar		RxCnt;
	uchar		TxLen;
	uchar		TxCnt;
	uchar		RxBuf[BUFSIZE];
    uchar		TxBuf[BUFSIZE];
} Radio_TypeDef;

typedef struct _bdata
{
  ulong		MinFreq;			// Минимальная центральная частота в сотнях Гц
  ulong		MaxFreq;			// Максимальная центральная частота в сотнях Гц
  ulong		FA0;				//
  ulong		FA1;				//
  uint		TuneRes;			// Минимальный шаг настройки в сотнях Гц
} Bdata_TypeDef;

typedef struct _banddata
{
	struct _bdata Band[3];
} BandData_TypeDef;

/* Exported functions ------------------------------------------------------- */
void init_MRF49XA(void);
void resetRadio_MRF49XA(void);
void sendCommand_MRF49XA(uint cmd);   // Передача команды в радиомодуль
void resetRadio_MRF49XA(void);
//void sendData_MRF49XA(uint data);     // Передача данных в радиомодуль
void sendData_MRF49XA (uint data, uchar cs);
uchar readData_MRF49XA(void);
uchar readFIFO_MRF49XA(void); 
uint getRadioStatus(uchar cs);      // Прочитать регистр статуса радиомодуля
void reducePower_MRF49XA(void);
void setRadioMode(uchar mode);      // Установка режима работы трансивера
void taskTransiver(void);           // Задача обработки трансивера
ErrorStatus sendPacket_MRF49XA(uchar cmd, void *adr, uint size, uchar enc); 
RFReceiveStatus receivePacket_MRF49XA(uchar *data, uchar *length);
void setRadioRange(uchar range);                    // Установка диапазона частот
void setRadioBandWidth(uchar bandwidth);            // Установка ширины канала
void setRadioDeviation(uchar dev);                  // Установка девиации передатчика                  
void setRadioPower(uchar pow);  // Установка выходной мощности передатчика
void setRadioBaud(ulong baudrate);  // Установка скорости передачи данных в радиоканале
void setRadioChannel(uint channel); // Установка канала
void setRadioNextChannel(void);     // Установка следующего номера канала
void setRadioData(uchar range, uchar bandwidth, uchar dev, ulong baudrate); // Установка радиоданных
ErrorStatus sendRandPacket_MRF49XA(uchar *data, uchar len);
ErrorStatus sendError_MRF49XA(uchar cmd, uchar err);
// !!! ПЕРЕДЕЛАТЬ на ErrorStatus !!!
uchar isNoOpenSess (uchar cmd);   
void synchroRF(u16 channel, u8 sw);

                                
#endif /* __RADIO_H */

