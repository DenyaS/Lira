

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __EXTCOM_H
#define __EXTCOM_H

/* Includes ----------------------------------------------------------------- */
#include "unsigned.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/
#define HEAD_PACKET     4       // ����� ��������� ������ (ADDR_DST + ADDR_SRC + SIZE + CMD = 4)
#define WAITANSWER_RF   300     // ����� �������� ������ �� ��� ������ (��)
#define WAITANSWER_GPS  5000    // ����� �������� ������ �� GPS ��������� (��)
#define WAITACKSENDSMS  4000    // ����� �������� ������ �� �������� ������� SMS (��)
#define WAITCONNECT_RF  8000    // ����� �������� ���������� (�����������) � PPD (��)

// ������ ������ ��� COM, RS-485, USB2.0, CAN
/*  -       0         1        2      3    4...n  n+1, n+2   */
/* FLAG | ADR_DST | ADR_SRC | SIZE | CMD | DATA | CRC16      */

// �������� �����
#define FLAG_PACKET     0x7e                // ���� ������	            
#define ADDR_DST_IN(n)  UARTx[(n)].InBuf[0] // ����� ����������
#define ADDR_SRC_IN(n)  UARTx[(n)].InBuf[1] // ����� �����������
#define SIZE_IN(n)      UARTx[(n)].InBuf[2] // ����������� ���� ������ � ������
#define CMD_IN(n)       UARTx[(n)].InBuf[3] // ��� �������  
#define DATA_IN(n)      UARTx[(n)].InBuf[4] // ���� ������ ������ (<=250 ����)

// ��������� �����
// ���� ������ 0x7e ���������� �� ����� ��������� ����������
#define ADDR_DST_OUT(n) UARTx[(n)].OutBuf[0] // ����� ����������
#define ADDR_SRC_OUT(n) UARTx[(n)].OutBuf[1] // ����� �����������
#define SIZE_OUT(n)     UARTx[(n)].OutBuf[2] // ����������� ���� ������ � ������
#define CMD_OUT(n)      UARTx[(n)].OutBuf[3] // ��� �������   
#define DATA_OUT(n)     UARTx[(n)].OutBuf[4] // ������ � ������ ���������� � 5-�� �����


//���� ������ ������� ����
#define STATUS_SM       10      // 10 ������ ��������� ������ �������
#define ERROR_SM        11      // 11 ������ ������ ������ ������������
#define STATUS_CM       12      // 12 ������ ��������� ������ �������
#define ERROR_CM        13      // 13 ������ ������ ������ �������
#define ERROR_Code      17      // 17 ������ ������ ������

//���� ������� �� ��������� ����

//#define SUP_ALIGN_MOVE_LEFT 0x03      //��������� ������ ����� �� L ��, � �������������� N.

// ���� ������ ������� 
#define CMD_ERROR           0x00    // ALL: ����� � ����� ������
#define	cmdAUTORIZE1		0x01	// PDD: ����������� 1-� ����
#define	cmdAUTORIZE2		0x02	// PDD: ����������� 2-� ����
#define	cmdCHECK_SENSOR		0x03	// PDD: ��������� ��������� �������
#define	cmdPING_REQUEST		0x04	// PDD: ������ ������� � �������
#define	cmdPING_CONFIRM		0x04	// PDD: ����� �� ������� ������� �������
#define	cmdREAD_LTSA		0x05	// PDD: ��������� ��������� ����������
#define	cmdREAD_STSA		0x06	// PDD: ��������� ����������� ����������
#define	cmdSWITCH_SLEEP		0x07	// PDD: ����������/��������� ����� ����������������
#define	cmdGET_DATETIME		0x08	// PDD: ��������� ���� � �����
#define	cmdSET_DATETIME		0x09	// PDD: ���������� ���� � �����
#define	cmdCONFIG_PDD		0x0a	// PDD: �������� ��������� ������������
#define	cmdGETNUM			0x0b	// PDD: ��������� ����� �������
#define	cmdCALIBRATE		0x0c	// PDD: ���������� ��������
#define cmdREDUCE_PWR       0x0d    // PPD: ��������� ���������� � ����� ����������� �����������������
#define cmdGET_PARAMS       0x0e    // PPD: �������� ��������� ����������
#define cmdCLEAR_POINTERS   0x0f    // PPD: �������� ��������� ����������
/* 0x01 - 0x0f  PDD */
#define CMD_LOAD_FMW	    0x10	// ALL: ��������� ��������������
#define	cmdLOAD_FMW			0x10	// ALL: ��������� ��������������
#define CMD_GETSTATE_GPS	0x11	// GPS: ������ ������� GPS ���������
#define CMD_GET_LOCATION	0x12	// GPS: �������� ���������� ��������������
#define CMD_SEND_SMS	    0x13	// GPS: �������� SMS 
#define CMD_GET_DATETIME	0x14	// GPS: �������� ������� ���� � �����
/* 0x15 � 0x1f	GPS: ������ */

#define cmdCLOSE_SESS       0x20    // ��������� ������ c PPD
#define cmdERROR			0x22	// ������ ���������

#define cmdDISPL_SEAT       0x2a    // ���������� ��������� �������    
/* 0x2a � 0x2f	DRIVERUNIT: ������ */

#define cmdSYNCHRO_RF       0x30    // PPD: ����� �������������
#define cmdSET_CHANNEL      0x31    // PPD: ���������� ����������
#define cmdENABLE_SLEEP     0x32    // PPD: ������������ ����� ����������� �����������������
#define cmdDISABLE_SLEEP    0x33    // PPD: ��������� ����� ���������������� 
#define cmdRESET            0x34    // PPD: ������������ �������(��)
#define cmdDEBUG            0x3f    // ��� �������
/* 0x30 - 0x3f  PDD */

#define CMD_CHECK_BTSTF 	0x7e    // DEBUG - test bytestuffing

// ���� �������� ��� ��������� �����������
#define REQ_GET_LAST_EVN	0xa0	// �������� ��������� ������� �� ������� ���������
#define REQ_GET_SPEC_EVN	0xa1	// �������� ����������� �������
#define REQ_QTY_EVN	        0xa2	// ���������� ������������ �������
#define REQ_CLEAR_HIS	    0xa3	// �������� ������� �������
#define REQ_GET_DATETIME	0xa4	// �������� ������� ���� � �����
#define REQ_SET_DATETIME	0xa5	// ���������� ���� � �����
#define REQ_SET_SMSINT	    0xa6	// ���������� �������� ������� SMS
#define REQ_GET_STATE	    0xa7	// �������� ��������� �����������
#define REQ_EXT_IRQ	        0xa8	// ���������� �� �������� ����������
#define REQ_GET_SEATS_INFO  0xa9    // ��������� ������ � ��������� ������� �� ������� ������
#define REQ_AWAKE_PPD       0xaa    // ������� ������� �� ��������� "��������� ���" 
#define REQ_REINIT_RF       0xab    // �������������������� �����������
#define REQ_GET_INFO_DT     0xac    // �������� info � ������� �� �������� ���������� �������
#define REQ_GET_EVENT       0xad    // �������� ������ � ������� 
#define REQ_GET_WEB_DATA    0xae    // �������� ������ ��� ����������� �� Web-������� ����������� 

// ���� ������ ��� ������ �������
#define NO_ERROR	        0x00	// ��� ������
#define	errNoError			0x00	// 00 ��� ������
#define ERR_BAD_COMMAND	    0x01	// 01 ������ ���� ������� (CMD)
#define	errBadCommand		0x01	// 01 ������ ���� ������� (CMD)
#define ERR_DATA	        0x02	// 02 ������ � ����������� ������
#define	errBadData			0x02	// 02 ������ � ����������� ������
#define ERR_BAD_SIZE	    0x03	// 03 ������ ������� ��������� �������
#define	errBadSize			0x03	// 03 ������ ������� ��������� �������
#define ERR_BAD_CRC16	    0x04	// 04 ������ CRC
#define	errBadCrc			0x04	// 04 ������ CRC ������
#define ERR_NODATA	        0x05	// 05 ��� ������ ��� ��������
#define	errNoData			0x05	// 05 ��� ������ ��� ��������
#define ERR_BADPARAM	    0x06	// 06 �������� ��������
#define	errBadParam			0x06	// 06 �������� ��������
#define	errAccess			0x07	// 07 ��� ������� (������ �� �������)
#define ERR_BAD_SRCADR      0x08    // 08 �������� ����� �����������
#define ERR_NOTSENT_SMS     0x09    // 09 ���������� SMS ��� �� ����������
#define ERR_EXEC	        0x0a	// 10 ������ ���������� ������� (�������)
#define	errExec				0x0a	// 10 ������ ����������
#define ERR_SMS_NODATA      0x0b    // 11 � ������������� SMS ��� ������
#define ERR_NO_EVENTS       0x0c    // 12 ��� ������������ �������
#define ERR_BAD_DSTADR      0x0d    // 13 �������� ����� ����������
#define ERR_FROM_PPD        0x0e    // 14 ������ ����� � ������� �� ������� ����������� ���������
#define ERR_WAITING_ANSWER	0x0f    // 15 � �������� ������
#define	ERR_READ_DF	        0x10	// ������ ������ ������ �� DataFlash
#define ERR_IDLE            0x11       // ������ ���� - ��� ���������� ������
//���� ������ ����
#define  ERR_ROTATION   160          // 160 (a0) - ������ �������� ���������
#define  ERR_MOTORJAM   161          // 161 (a1) - ������� ����� ���������
#define  OK_COMPLETE    162          // 162 (a2) - ��� ������ - ���������
#define  ERR_NOSTEPS    163          // 163 (a3) - ��� ����� ��� �������� ��
/*
#define  ERR_MALLOC     164          // 164 (a4) - ������ ������������� ��������� ������
#define  ERR_SPI         165         // 165 (a5) - ������ ������ ������� �� ���������� SPI
#define  ERR_I2C_WRITE   166         // 166 (a6) - ������ ������ ������ �� ���������� I2C
#define  ERR_I2C_READ    167         // 167 (a7) - ������ ������ ������ �� ���������� I2C
#define  ERR_DF_WRITE    168         // 168 (a8) - ������ ������ �� ������� DataFlash
#define  ERR_DF_READ     169         // 169 (a9) - ������ ������ �� ������� DataFlash
#define  ERR_GET_DT      170         // 170 (aa) - ������ ������ ���� � ������� �� ����-���������
#define  ERR_SET_DT      171         // 171 (ab) - ������ ������ ���� � ������� � ����-���������
#define  ERR_INIT_RF     172         // 172 (ac) - ������ ������������� �����������
#define  ERR_RF_SEND     173         // 173 (ad) - ������ �������� ������ � �����������
#define  ERR_UART0_SB    174         // 174 (ae) - ������ �������� ����� ����� UART0
#define  ERR_UART1_SB    175         // 175 (af) - ������ �������� ����� ����� UART1
#define  ERR_ERRANSWER   178   // 178 (b2) - ������� ����� � �������
#define  ERR_WDT_RESET   181    // 181 (b5) - ��������� ����� �� WatchDog �������
#define  ERR_UNKNOWN,    182          // 182 (b6) - ���������� ����������� ������
*/
#define  ERR_HOME_ON_AT_INIT  183    // 183 (b7) - ������ Home � "1" ��� ������ ������������� ���
#define  ERR_NOT_INIT_AXIS    184    // 184 (b8) - ����������� �� ��� �� �������� �.�. ��� �� ����������������
#define  ERR_MOTOR_BUSY       185    // 185 (b9) - ����� ��� � ������ ��� ��������� ������ ���������
#define  ERR_AHEAD_MAN        186    // 186 (ba) - ����������� �� ���������� �� ����������� �����
#define  ERR_BEHIND_MAN       187    // 187 (bb) - ����������� �� ������� �� ����������� �����
#define  ERR_AHEAD_MAN_SEN    188    // 188 (bc) - ����������� ������ � ���������, ����� �� ��������   
#define  ERR_BEHIND_MAN_SEN   189    // 189 (bd) - ����������� ������ � ���������, ����� �� �������� 
#define  ERR_CLOSE_ZAH_SEN    190    // 190 (be) - ������ ������ �� �������, ����� �� ������
#define  ERR_OPEN_ZAH_SEN     191    // 191 (bf) - ������ ������ �� �������, ����� �� ������ 
#define  ERR_OPEN_ZAH         192    // 192 (c0) - ������ �� �������� �� ��������� �����  
#define  ERR_CLOSE_ZAH        193    // 193 (c1) - ������ �� �������� �� ��������� �����
#define  ERR_AHEAD_FIN        194    // 194 (c2) - ����� �� ���������� �� ����������� �����
#define  ERR_BEHIND_FIN       195    // 195 (c3) - ����� �� ������� �� ����������� �����
#define  ERR_AHEAD_FIN_SEN    196    // 196 (c4) - ����� ������ � ���������, ����� �� ��������   
#define  ERR_BEHIND_FIN_SEN   197     // 197 (c5) - ����� ������ � ���������, ����� �� ��������

// ��������� ��������� �������
#define DEBUGPC	            0x00	// ���������� ���������
#define ONBOARDCONTR	    0x01	// �������� ����������
#define GPSTERM             0x02	// GPS ��������
#define DRIVERUNIT	        0x03	// ������������ ����

#define DEBUGPC	            0x00	// ���������� ���������
#define CCU     0x01             //��� ����������� ����������� ������ 
#define PO      0x02              //����� ���������
#define SUPPLYUNIT     0x03     //������ ������ 
#define BENDINGUNIT     0x04    //������ �����	
                            	
// ����� � ���� UART
#define MY_ADDRESS          CCU 

// ��������� �������� ����������� ���������
#define UIN(number)         (u16)(BOARD + (PPD_1 + number - 1))         
#define NUM(uin)            (uchar)(uin - (BOARD + PPD_1 - 1))          // @ uin - 0xb36

/* Type definitions ----------------------------------------------------------*/
// ��������� ������ ��� MRF49XA
/*
 [0]    [1...2]   [3...4]    [5]     [6...n]     [n+1...n+2]
DSize   DstAddr   SrcAddr    Cmd	   Data		    Crc 			
1b		2b		  2b	     1b      0..DSize	    2b
*/
#pragma pack(1)
typedef struct _packheader
{
  uchar DSize;				// ������ ���� ������       0
  uint DstAddr;             // ����� �����������        1 2 
  uint SrcAddr;             // ����� ����������         3 4
  uchar Cmd;				// �������                  5
  uchar Data[0];			// ������ ������            6
  //uint Crc;				// ����������� �����
} PackHeader_TypeDef;
#pragma pack()

// ��������� ������ � �����������
/*
  [0]  [1...2]    [3]   [4...5]   [6...7]    [8]   [9...n] [n+1...n+2] 
Preamb	Sync |	DSize    DstAddr   SrcAddr   Cmd	Data	  Crc 			
 1b		 2b	 |	 1b		  2b		2b		 1b    0..DSize	   2b
*/
#pragma pack(1)
typedef struct _txpackheader
{
  uchar Preamble;	// ������ ���� ���������
  uchar Syncro1;
  uchar Syncro2;
  struct _packheader	Header;		// ��������� ������
} TxPackHeader_TypeDef;
#pragma pack()

// ��������� ������ UART
/*
Flag  |  Adr_Dst    Adr_Src     Size	Cmd	    Data	    Crc16
1b    |  1b		    1b		    1b	    1b		0�Size	    2b
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

// ��������� �������� ����������� ����������
typedef enum 
{
  PPD_1 = 0xA0,         // 0xa0 ������ ����������� ��������� �1
  PPD_2,                // 0xa1 ������ ����������� ��������� �2    
  PPD_3,                // 0xa2 ������ ����������� ��������� �3
  PPD_96 = 0xFF         // 0xff ������ ����������� ��������� �96
} AddrPPD_TypeDef;
       
// ���� ������ �������
typedef enum 
{
  ERR_WRITE_FLASH = 156,    // 156 (9c) - ������ ������ �� ���������� FLASH
  ERR_READ_FLASH,           // 157 (9d) - ������ ������ �� ���������� FLASH
  ERR_ADR_READ_MEMORY,      // 158 (9e) - �������� ����� ��� ������ ������ 
  ERR_ADR_WRITE_MEMORY,     // 159 (9f) - �������� ����� ��� ������ ������ 
  ERR_NOACK_GPS,            // 160 (a0) - ��� ������ �� GPS ���������
  ERR_NOACK_PPD,            // 161 (a1) - ��� ������ �� ������� ����������� ���������
  ERR_TRIES_SMS,            // 162 (a2) - ��������� ���������� ������� ������� SMS 
  ERR_RANGE_PPD,            // 163 (a3) - ����� ������� ��� ����������� ��������� �������
  ERR_MALLOC,               // 164 (a4) - ������ ������������� ��������� ������
  ERR_SPI,                  // 165 (a5) - ������ ������ ������� �� ���������� SPI
  ERR_I2C_WRITE,            // 166 (a6) - ������ ������ ������ �� ���������� I2C
  ERR_I2C_READ,             // 167 (a7) - ������ ������ ������ �� ���������� I2C
  ERR_DF_WRITE,             // 168 (a8) - ������ ������ �� ������� DataFlash
  ERR_DF_READ,              // 169 (a9) - ������ ������ �� ������� DataFlash
  ERR_GET_DT,               // 170 (aa) - ������ ������ ���� � ������� �� ����-���������
  ERR_SET_DT,               // 171 (ab) - ������ ������ ���� � ������� � ����-���������
  ERR_INIT_RF,              // 172 (ac) - ������ ������������� �����������
  ERR_RF_SEND,              // 173 (ad) - ������ �������� ������ � �����������
  ERR_UART0_SB,             // 174 (ae) - ������ �������� ����� ����� UART0
  ERR_UART1_SB,             // 175 (af) - ������ �������� ����� ����� UART0
  ERR_SEND_SMS,             // 176 (b0) - ������ ������� SMS
  ERR_NOCONNECT_PPD,        // 177 (b1) - ������ �� �������� (��� ����������)
  ERR_ERRANSWER,            // 178 (b2) - ������� ����� � �������
  ERR_NOEXIST_PPD,          // 179 (b3) - ������� ��� � ����
  ERR_FIND_EVENT,           // 180 (b4) - �� ������� ������ ������� � ������ DF
  ERR_WDT_RESET,            // 181 (b5) - ��������� ����� �� WatchDog �������
  ERR_UNKNOWN               // 182 (b6) - ���������� ����������� ������
} ErrCodes_TypeDef;

// ������� ��� �������
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

// �������� ���� � �������
#pragma pack(1)
typedef struct _datetime
{
  uchar		Day;	// ����� 1�.31
  uchar		Month;	// ����� 1�12
  uchar		Year;	// ��� 00�99
  uchar		Hour;	// ����    0�23
  uchar		Min;	// ������  0�59
  uchar		Sec;	// ������� 0�59
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
	uint  SelfAddr;			// ����������� �����
	uint  RemoteAddr;		// ����� �������
	uint  Sensitivity1;		// ��������������� ������� 1 [1 .. 1000]
	uint  Sensitivity2;		// ��������������� ������� 2 [1 .. 1000]
    uint  Threshold1;       // ������ ����� ������� 1 (����������)
    uint  Threshold2;       // ������ ����� ������� 2 (����������)
	uint  SensTime1;		// ����� �������� ��������� ������� 1 (sec) [0 ... 60]
	uint  SensTime2;		// ����� �������� ��������� ������� 2 (sec) [0 ... 60]
	uint  BatteryTresh;		// ��������� �������� ��� ��������� ������� �������
    uchar TransmitPower;    // ������� �������� �����������
	uint  Crc;
} TParamsPPD, *pTParamsPPD;
#pragma pack()

#pragma pack(1)
typedef struct _stat_pdd {
	union {
		uchar   FlagsWord;
		struct {
			uchar	Rtc:1;				// ������ ������� (1), ������ �� (0)
			uchar	Sensor:1;			// ������ ������� (1), ������ �� (0)
			uchar	DataFlash:1;		// ������ dataflash (1), dataflash �� (0)
			uchar	Battery:1;			// ���������� ������� ���� ������ (1), ���������� ������� � ����� (0)
			uchar	Passenger1:1;		// ������� �1 ������������ (1), ��� ��������� (0)
			uchar	Passenger2:1;		// ������� �2 ������������ (1), ��� ��������� (0)
            uchar   EnableSleep:1;      // ����������� ����� ����������������
			uchar	Res:1;

		};
	};
	union {
		ulong   DevId;				// ������������� ����������
		uchar	DevIdByte[4];		// ������������� ����������
	};
	TRtcPPD DateTime;		    // ������� ���� � �����	
	ulong	Qty_NonreadTSA;		// ���. ������������ ����������	
	ulong	Qty_Index;			// ������ 1-� ������������ ����������
    uint    C1;                 // ������� 1-�� ������� [fF]
    uint    C2;                 // ������� 2-�� ������� [fF]
} TStatusPDD, *pTStatusPDD;
#pragma pack()

// ����� � ������� 
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

// ��������� ���������� ������� ����������� ����������
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
    uint            C1;                 // ������� 1-�� ������� [fF]
    uint            C2;                 // ������� 2-�� ������� [fF]
} TTranz;
#pragma pack()

// ��������� ��� �������� ��������
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

// ��������� ������� GPS ���������
#pragma pack(1)
typedef struct _gpsstat
{
  //---------------- BYTE 0 ----------------------------------------- 
  union {
    uchar Byte_0;
    struct {
      uchar	Antenna:1;          // ������� GPS ���������� (1), ��������� (0)
      uchar	AntennaFailed:1;	// ������� ���������� (1), ������� �� (0)
      uchar	IntBatt:3;    		// ������� ������ ���������� �������
      uchar	LevelGSM:3;    	    // ������� GSM �������
    };
  };
  //---------------- BYTE 1 � 6 ----------------------------------------- 
  DataTime_TypeDef	DataTime;	// ������� ���� � ����� (��. �. 16.5.2)
  //---------------- BYTE 7 � 10 ----------------------------------------- 
  uint	Latitude_IntPart;	    // ����� ����� �������� ������
  uint	Latitude_FrPart;	    // ������� ����� �������� ������
  //---------------- BYTE 11 � 15 ----------------------------------------- 
  uchar	Longitude_IntPart1;	    // ����� ����� �������� ������� 1
  uint	Longitude_IntPart2;	    // ����� ����� �������� ������� 2
  uint	Longitude_FrPart	;	// ������� ����� �������� �������
  //---------------- BYTE 16 ----------------------------------------- 
  uchar	Speed	;			    // �������� �������� � ����� (������� ����)
  //---------------- BYTE 17 ----------------------------------------- 
  uchar	Qty_VisibleSats;		// ����� ������� ���������
  //---------------- BYTE 18 ----------------------------------------- 
  union {
    uchar Byte_18;
    struct {
      uchar	StartMoving:1;      // ���� ������ ��������
      uchar	EndMoving:1;		// ���� ��������� ��������
      uchar	SenGPIO_1:1;		// �������� ����������� ������� 1
      uchar	SenGPIO_2:1;		// �������� ����������� ������� 2
      uchar	SenGPIO_3:1;		// �������� ����������� ������� 3
      uchar	SenGPIO_4:1;		// �������� ����������� ������� 4
      uchar	SenGPIO_5:1;		// �������� ����������� ������� 5
      uchar	SenGPIO_6:1;		// �������� ����������� ������� 6
    };
  };
  //---------------- BYTE 19 ----------------------------------------- 
  union {
    uchar Byte_19;
    struct {
      uchar	ExtBatt:1;          // ������� ���. ����-�� ���������� (1),
                                //  �� ���������� (0)
      uchar	DataExists:1;		// � ������ ���� ������ ��� �������� (1),
                                // ��� ������ (0)
      uchar NotSentSMS:1;       // ���������� SMS �� ���������� �� ������ (1)
                                // SMS ���������� �� ������ (0)
      uchar	ReserveBits:5;		// ������
    };
  };
  //---------------- BYTE 20 � 22 ----------------------------------------- 
  uchar	SenAnalog_1;		    // �������� ����������� ������� 1
  uchar	SenAnalog_2;		    // �������� ����������� ������� 2
  uchar	SenAnalog_3;		    // �������� ����������� ������� 3
  //---------------- BYTE 23 ----------------------------------------- 
  uchar	ReserveByte;		    // ������
} TGPSStatus, *pTGPSStatus;
#pragma pack()

// ��������� �������������� �� GPS ��������� "M2M"
#pragma pack(1)
typedef struct _location
{ 
  //---------------- BYTE 0 - 3 ----------------------------------------- 
  uint	Latitude_IntPart;		// ����� ����� �������� ������
  uint	Latitude_FrPart;		// ������� ����� �������� ������
  //---------------- BYTE 4 � 8 ----------------------------------------- 
  uchar	Longitude_IntPart1;	    // ����� ����� �������� ������� 1
  uint	Longitude_IntPart2;	    // ����� ����� �������� ������� 2
  uint	Longitude_FrPart;  	    // ������� ����� �������� �������
} TLocation, *pTLocation;
#pragma pack()

// ��������� ��������� ��� GoogleMaps
#pragma pack(1)
typedef struct _googlegps
{ 
  //---------------- BYTE 0 - 3 ----------------------------------------- 
  uchar	Lat_Degree;		// ������� ������
  uchar Lat_Min;        // ������ ������
  uchar Lat_Sec;        // ������� ������
  uchar	Lat_FrSec;		// ������� ����� ������ ������
  //---------------- BYTE 4 � 8 ----------------------------------------- 
  uchar	Lon_Degree1;	// ������� ������� ����� 1
  uchar	Lon_Degree2;	// ������� ������� ����� 2
  uchar Lon_Min;        // ������ �������
  uchar Lon_Sec;        // ������� �������
  uchar	Lon_FrSec;		// ������� ����� ������ �������
} TGoogleGPS, *pTGoogleGPS;
#pragma pack()

/** ��������� ��� ����������� �� �������� �������� (DriverMonitor) 
    ���������� � ��������� �������� � � �������/��������� �������� */

#pragma pack(1)
typedef struct _involvedppd {
	uchar		Total;				// ����� ���������� �������� � �������
} TInvolvedPPD, *pTInvolvedPPD;
#pragma pack()

#pragma pack(1)
typedef struct _seat {
	uchar Number;				    // ���������� ����� PPD
    union {
        uchar Params;
	    struct {
		    uchar	Acked:1;			// ��������� ����������� PPD: 1 - ��������, 0 - �� ��������
		    uchar	RightSeat:1;		// ��������� ������� �������: 1 - ������, 0 - ��������
		    uchar	LeftSeat:1;			// ��������� ������ �������: 1 - ������, 0 - ��������
		    uchar	PowerLow:1;			// ��������� �������: 1 - ������� ���������, 0 - � �����
            uchar   EnableSleep:1;      // 1 - � ������� ����������� ����� ����������������
		    uchar	Reserve:3;
	    };
    };
} TSeat, *pTSeat;
#pragma pack()

/** � ���� ������ ������ ���������� �������:
    TCountPPD (1 Byte) + TSeat[TCountPPD::Total]    */
#pragma pack(1)
typedef struct _showseats {
  uchar Total;              // ����� ���������� �������� � �������
  TSeat Seats[QTYPPD];  
} TShowSeats, *pTShowSeats;
#pragma pack()

// ���������� � �������� � ��������� �� �������
#pragma pack(1)
typedef struct _infoseats {
  DataTime_TypeDef  DT;     // ���� � ����� ����� ����������
  uint		        Ch;		// ����� �������� �����������
  TShowSeats        Show;   // ���������� �� �������� � ��������
} TInfoSeats, *pTInfoSeats;
#pragma pack()

// ��������� �������
#pragma pack(1)
typedef struct _event
{
  ulong		        ID;				// �����. ����� �������	
  DataTime_TypeDef	DataTime;       // ���� � ����� �������
  TGoogleGPS        Coordinates;	// ����������
  uchar 	        OccupSeats;	    // ���-�� ������� ����
  uchar		        FreeSeats;	    // ���-�� ��������� ����
  uchar		        FailedPPD;	    // ���-�� ����������� ��������
  TInfoSeats        IS;             // ���������� �� ���� ��������
  union {
    uchar Flags;
	struct {
      uchar	AutoSave:1;     // 1 - ������������� ��������� �������, 0 - �� ���������� �����
      uchar StartRoute:1;   // 1 - ������� ������ ��������
      uchar EndRoute:1;     // 1 - ������� ����� ��������      
      uchar	Reserve:5;
	};
  }; 
  double	        Mileage;		// ������ � ������
  uchar 	        Speed;		    // �������� �������� � �����
  uchar             Reserve_1;
  uchar             Reserve_2;
  uchar             Reserve_3;
  uint		        Crc;			// CRC ������ �������
} TEvent, *pTEvent;
#pragma pack()

// ��������� ������ ��������� �������
#pragma pack(1)
typedef struct _systemerrors
{
  uchar ErrorsExist:1;       // ���� ������(�) (1), ��� ������ (0)
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

// ��������� ��������� ��������� �����������
#pragma pack(4)
typedef struct _systemstatus
{
  uchar		Version;			// ������ ��
  DataTime_TypeDef	DataTime;   // ���������  ���� � �����
  TGoogleGPS    Coordinates;	// ���������� ��������������
  uchar 	OccupSeats;		    // ���-�� ������� ����
  uchar		FreeSeats;		    // ���-�� ��������� ����
  uchar		GoodPPD;		    // ���-�� ��������� ��������
  uchar		FailedPPD;		    // ���-�� ����������� ��������
  ulong		NonReadEvents;	    // ���-�� ������������ �������
  TSystemErrors	Errors; 		// ������ ��� ����������� ����� �������
  uchar     KSES[8];            // ���� ������ //!!! ����� �� ����� !!!
  uint		WorkRadioCh;		// ����� ����������� �����������
  uint		TimeWaitConnectRF;	// ����� �������� ���������� � �������� //!!! ����� �� ����� !!!
  uchar     Speed;              // �������� �������� � �����
  uchar		Reserve_1;			// ������
  uchar		Reserve_2;			// ������
  uchar		Reserve_3;			// ������
  ulong		Reserve_4567;		// ������
  ulong		Reserve_8901;		// ������
} TSystemStatus, *pTSystemStatus;
#pragma pack()

// ��������� ������ ��������� SMS:
#pragma pack(1)
typedef struct _sendsms
{
  uchar	Len;			// ����� ���������	
  char	ShortMes[140];	// ���������
  char	CelNumber[32];	// ����� �������� (������: +380509229934).
                        // ���� CelNumber ����� 0, �� ����� 
                        // ���������� �� ������
} TSendSMS, *pTSendSMS;
#pragma pack()

// ��������� ��������� ��������� �������� SMS:
#pragma pack(1)
typedef struct _smsint
{
  char	CelNumber[32];  // ����� �������� (��������: "+380509229934\n").
                        // ���� CelNumber ����� 0, �� ������ 
                        // ���������� �� ������
  uint	SMS_Interval;	// �������� � �������
                        // ���� SMS_Interval ����� 0, �� ������ 
                        // �� ����������
} TSMSInt, *pTSMSInt;
#pragma pack()

// ��������� ������ ���������� PPD:
#pragma pack(1)
typedef struct _numbertsa
{
  uint		UIN;			// ���������� ����� �������
  ulong		TSA_Number;		// ����� ����������
} TNumTSA, *pTNumTSA;
#pragma pack()

// ��������� ������� �� ��������� ���������� PPD:
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
  TVoidReq	VReq;			// ����� ������� �� �����
  ulong		TSA_Number;		// ����� ����������
} TReqTSA, *pTReqTSA;
#pragma pack()

// ��������� ������� �� ��������� ���� � ������� � PPD:
#pragma pack(1)
typedef struct _reqsetdt
{
  TVoidReq	          VReq;	// ����� ������� �� �����
  DataTime_TypeDef    DT;  	// ��������������� ���� � �����
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

// ��������� ������ ��� ������������ �������:
#pragma pack(1)
typedef struct _configsen
{
  uchar	Ch1_Setup;		    // SETUP REGISTER ������ 1
  uchar	Ch2_Setup;		    // SETUP REGISTER ������ 2
  uchar	Configuration;		// CONFIGURATION REGISTER
  uchar	PowerDownTimer;	    // POWER-DOWN TIMER REGISTER
  uchar	Ch1_Capdac;		    // CAPDAC REGISTER ������ 1
  uchar	Ch2_Capdac;		    // CAPDAC REGISTER ������ 2
} TConfigSen, *pTConfigSen;
#pragma pack()

// ��������� ������� �� ��������� ���������� ������� � PPD:
#pragma pack(1)
typedef struct _reqcfgsen
{
  TVoidReq	    VReq;		// ����� ������� �� �����
  TConfigSen    CfgParams; 	// ��������������� ��������� ������������
} TReqCfgSen, *pTReqCfgSen;
#pragma pack()

// ��������� ������ ������� ����������� ����������
#pragma pack(1)
typedef struct _numppd
{
  uint	UIN;			// ���������� ����� �������
} TNumPPD, *pTNumPPD;
#pragma pack()

// ��������� ��������� ��������������� �������� ������� ����������� ����������
#pragma pack(1)
typedef struct _checksensor
{
  uchar	StatusRegister;	// �������� STATUS REGISTER 
  ulong	SerialNumber;	// �������� SERIAL NUMBER REGISTER
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

// ������ info �� ������ ���������� �������
#pragma pack(1)
typedef struct _reqgetinfodt
{
  ulong             Board;      // ����� �������� (��������, 2711)    
  DataTime_TypeDef  DTStart;    // ���� � ����� ���������� �������     
  DataTime_TypeDef  DTEnd;      // ���� � ����� ��������� ������� 
} TReqGetInfoDT, *pTReqGetInfoDT;
#pragma pack()

// ����� �� ������ ���������� �� ������ ���������� �������
#pragma pack(1)
typedef struct _answerinfodt
{
  ulong             Board;          // ����� �������� (��������, 2711)    
  DataTime_TypeDef DT_StartEvn;     // ���� � ����� ������� ������� � �������� ���������� �������
  DataTime_TypeDef DT_EndEvn;       // ���� � ����� ���������� ������� � �������� ���������� �������
  ulong         ID_StartEvn;        // �����. ����� ���������� ������� � �������� ���������� �������
  ulong         ID_EndEvn;          // �����. ����� ��������� ������� � �������� ���������� �������
  ulong         QtyStops;           // ���������� ��������� �� ������ ���������� �������
  double        Time;               // ����� � ���� �� ������ ���������� ������� (s)
  double        Distance;           // ���������� �� ������ ���������� ������� (m)
  double        Passengers;         // ������� ���������� ������������ ����������
  double        TotalPasTime;       // ����� ��������� ������� �� ������ ���������� ������� [h*s] (�������*������) 
  double        TotalPasDistance;   // ����� ���������� �� ���� ������� �������� [h*m] (�������*����)
} TAnswerInfoDT, *pTAnswerDT;
#pragma pack()

// ������ �� ��������� �������������� "���� ���������� � �������"
#pragma pack(1)
typedef struct _reqgetevn
{
	ulong   Board;
	ulong   NumEvn;
} TReqGetEvn;
#pragma pack()

// ��������� ������ ��� ����������� �� WEB-�������
#pragma pack(1)
typedef struct _webdata
{
  uint		        ID;			// �����. ����� �������	
  DataTime_TypeDef	DataTime;   // ���� � ����� �������
  TGoogleGPS    Coordinates;	// ���������� ��������������
  uchar 	OccupSeats;		    // ���-�� ������� ����
  uchar		FreeSeats;		    // ���-�� ��������� ����
  uchar		FailedPPD;		    // ���-�� ����������� ��������
  uint		WorkRadioCh;		// ����� ����������� �����������
  uchar     Speed;              // �������� �������� � �����
  union {
    uchar Flags;
	struct {
      uchar	AutoSave:1;     // 1 - ������������� ��������� �������, 0 - �� ���������� �����
      uchar StartRoute:1;   // 1 - ������� ������ ��������
      uchar EndRoute:1;     // 1 - ������� ����� ��������      
      uchar	Reserve:5;
	};
  };
  uchar		Reserve_1;			// ������
  uchar		Reserve_2;			// ������
  uint		Reserve_34; 		// ������
  uint		Reserve_56;	    	// ������
} TWebData, *pTWebData;
#pragma pack()

// ��������� ���������� �������� � �������:
#pragma pack(1)
typedef struct _countppd
{
  uchar		Total;		// ����� ���������� �������� � �������
  uchar		Alive;		// ���������� "�����" ��������
  uchar     Dead;       // ���������� "������" ��������
  union 
  {
    ulong WhereIs_1;
	struct 
    {
      uchar D_1:1;          // 1 - ������ "�����", 0 - ������ "������"
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

// ��������� ������� ���������� ��������� ������ � ��������
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
void checkError(uchar permit_ch);       //�������� ��������� � ������ ����

#endif /* __EXTCOM_H */

