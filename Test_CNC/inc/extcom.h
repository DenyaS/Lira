
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __EXTCOM_H
#define __EXTCOM_H

/* Includes ----------------------------------------------------------------- */
#include "unsigned.h"

/* Private defines -----------------------------------------------------------*/
#define HEAD_PACKET     4       // ����� ��������� ������ (ADDR_DST + ADDR_SRC + SIZE + CMD = 4)

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

// ���� ������ ������� 
#define CMD_ERROR           0x00            // ALL: ����� � ����� ������
#define CMD_TEST            0x01

// ���� ������ ��� ������ �������
#define NO_ERROR	        0x00	// ��� ������
#define ERR_BAD_COMMAND	    0x01	// 01 ������ ���� ������� (CMD)
#define ERR_DATA	        0x02	// 02 ������ � ����������� ������
#define ERR_BAD_SIZE	    0x03	// 03 ������ ������� ��������� �������
#define ERR_BAD_CRC16	    0x04	// 04 ������ CRC
#define ERR_NODATA	        0x05	// 05 ��� ������ ��� ��������
#define ERR_BADPARAM	    0x06	// 06 �������� ��������
#define	ERR_ACCESS			0x07	// 07 ��� ������� (������ �� �������)
#define ERR_BAD_SRCADR      0x08    // 08 �������� ����� �����������
#define ERR_NOTSENT_SMS     0x09    // 09 ���������� SMS ��� �� ����������
#define ERR_EXEC	        0x0a	// 10 ������ ���������� ������� (�������)
                            //0x0b
                            //0x0c
#define ERR_BAD_DSTADR      0x0d    // 13 �������� ����� ����������
                            //0x0e
                            //0x0f
#define	ERR_READ_DF	        0x10	// ������ ������ ������ �� DataFlash
#define ERR_IDLE            0x11    // ������ ���� - ��� ���������� ������

// ��������� ��������� �������
#define DEBUGPC	            0x00	// ���������� ���������
#define CCU     0x01             //��� ����������� ����������� ������ 
#define PO      0x02              //����� ���������
#define SUPPLYUNIT     0x03     //������ ������ 
#define BENDINGUNIT     0x04    //������ �����
#define MY_ADDRESS          SUPPLYUNIT   

//���� ������ ����
typedef enum 
{
REQ_STATUS_SM = 10,     // 10 ������ ��������� ������ ������������
REQ_ERROR_SM,           // 11 ������ ������ ������ ������������
REQ_STATUS_CM,          // 12 ������ ��������� ������ �������
REQ_ERROR_CM,           // 13 ������ ������ ������ �������
REQ_STATUS_Man,         // 14 ������ ��������� ���������� �������� ���������� ������������
REQ_STATUS_Zah,         // 15 ������ ��������� ���������� �������� ������ ������������
REQ_STATUS_Fin,         // 16 ������ ��������� ���������� �������� ���������� ������
REQ_ERROR_Code,          // 17 ������ ������ 
SUP_STATE = 100,        // 100 �������� ��������� ������ ������������
SUP_ERROR,              // 101 �������� ��� ������ ������ ������������
SUP_ALIGN_MOVE,         // 102 ��������� ������  �� L ��, � �������������� N.
CUT_STATE,              // 103 �������� ��������� ������ �������
CUT_ERROR,              // 104 �������� ��� ������ ������ �������
CUT_REINIT,             // 105 ��������� ������������� 
CUT_RUN,                // 106 ��������� �������
INIT_AXIS,              // 107 ������������� ����
INIT_AXIS_STATE,        // 108 ��������� �������� �������������
INIT_AXIS_ERROR,        // 109 ������ �������������� ����
GO_TO_X,                 // ������ ������� X
GO_TO_Y,                 // ������ ������� Y
GO_TO_Z,                 // ������ ������� Z
MAN_AHED,               //��������� �����������
MAN_DI                  //��������� �����������
}Code_TypeDef;
 
//������� ������ ����

/* Type definitions ----------------------------------------------------------*/
// ��������� ������ ��� MRF49XA
/*
 [0]    [1...2]   [3...4]    [5]     [6...n]     [n+1...n+2]
DSize   DstAddr   SrcAddr    Cmd	   Data		    Crc 			
1b		2b		  2b	     1b      0..DSize	    2b
*/
#pragma pack(1)
typedef struct _supmotor
{
  unsigned short int          length;         //������ ��������
  unsigned char           speed;          //�������� ������
} SupMotor_TypeDef;  
#pragma pack()
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

// ���� ������ �������
typedef enum 
{
  ERR_WRITE_FLASH = 156,    // 156 (9c) - ������ ������ �� ���������� FLASH
  ERR_READ_FLASH,           // 157 (9d) - ������ ������ �� ���������� FLASH
  ERR_ADR_READ_MEMORY,      // 158 (9e) - �������� ����� ��� ������ ������ 
  ERR_ADR_WRITE_MEMORY,     // 159 (9f) - �������� ����� ��� ������ ������
  ERR_ROTATION,             // 160 (a0) - ������ �������� ���������
  ERR_MOTORJAM,             // 161 (a1) - ������� ����� ���������
  OK_COMPLETE,              // 162 (a2) - ��� ������ - ���������
  ERR_NOSTEPS,              // 163 (a3) - ��� ����� ��� �������� ��
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
  ERR_UART1_SB,             // 175 (af) - ������ �������� ����� ����� UART1
  ERR_ERRANSWER = 178,      // 178 (b2) - ������� ����� � �������
  ERR_WDT_RESET = 181,      // 181 (b5) - ��������� ����� �� WatchDog �������
  ERR_UNKNOWN,              // 182 (b6) - ���������� ����������� ������
  ERR_HOME_ON_AT_INIT,      // 183 (b7) - ������ Home � "1" ��� ������ ������������� ���
  ERR_NOT_INIT_AXIS,        // 184 (b8) - ����������� �� ��� �� �������� �.�. ��� �� ����������������
  ERR_MOTOR_BUSY,           // 185 (b9) - ����� ��� � ������ ��� ��������� ������ ���������
  ERR_AHEAD_MAN,            // 186 (ba) - ����������� �� ���������� �� ����������� �����
  ERR_BEHIND_MAN,           // 187 (bb) - ����������� �� ������� �� ����������� �����
  ERR_AHEAD_MAN_SEN,        // 188 (bc) - ����������� ������ � ���������, ����� �� ��������   
  ERR_BEHIND_MAN_SEN,       // 189 (bd) - ����������� ������ � ���������, ����� �� �������� 
  ERR_CLOSE_ZAH_SEN,        // 190 (be) - ������ ������ �� �������, ����� �� ������
  ERR_OPEN_ZAH_SEN,         // 191 (bf) - ������ ������ �� �������, ����� �� ������ 
  ERR_OPEN_ZAH,             // 192 (c0) - ������ �� �������� �� ��������� �����  
  ERR_CLOSE_ZAH,            // 193 (c1) - ������ �� �������� �� ��������� �����
  ERR_AHEAD_FIN,            // 194 (c2) - ����� �� ���������� �� ����������� �����
  ERR_BEHIND_FIN,           // 195 (c3) - ����� �� ������� �� ����������� �����
  ERR_AHEAD_FIN_SEN,        // 196 (c4) - ����� ������ � ���������, ����� �� ��������   
  ERR_BEHIND_FIN_SEN,       // 197 (c5) - ����� ������ � ���������, ����� �� �������� 
  ERR_ROTATION_A,           // 198 (c6) - ������ �������� ��������� ��� �
  ERR_ROTATION_X,           // 199 (c7) - ������ �������� ��������� ��� X  
  ERR_ROTATION_Y,           // 200 (c8) - ������ �������� ��������� ��� Y  
  ERR_ROTATION_Z            // 201 (c9) - ������ �������� ��������� ��� Z  
} ErrCodes_TypeDef;

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

/* Exported functions ------------------------------------------------------- */
void checkPacket_UARTx(uchar uart_number);
void parserPacket_UARTx(uchar uart_number);
void checkError(uchar permit_ch);

#endif /* __EXTCOM_H */

