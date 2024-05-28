

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __UART_H
#define __UART_H

/* Includes ----------------------------------------------------------------- */
#include "stm32F10x.h" 
#include "unsigned.h"

/* Define ------------------------------------------------------------------- */
#define TX485           GPIOC, DIR, HIGH   // Включение передатчика для RS-485
#define UART_BUF_SIZE   250                         // Размер буфера СОМ портов

/* Type definitions ----------------------------------------------------------*/
typedef struct _uart 
{
  struct 
  {
    uchar	RxComplete:1;
	uchar	SendPack:1;         // Необходим для защиты от повторной посылки пакета
	uchar	ReceivePack:1;
    uchar   Request:1;			// Получен корректный пакет
    uchar	Free:4;
  };
  uchar	RxCnt;
  uchar	RxSize;
  uchar	TxCnt;                      // кол-во байт в передаваемом пакете
  uchar	*Ptr;                       // указатель на передаваемый байт
  uchar SelfAdr;                    // Собственный адрес
  uchar RemoteAdr;                  // Адрес клиента
  uint	Timer;
  uchar	InBuf[UART_BUF_SIZE];
  uchar	OutBuf[UART_BUF_SIZE];
} UART_TypeDef;

enum { UART0 = 0, UART1, UART2, UART3 };
enum { WAIT = 0, NOWAIT = !WAIT };

/* Exported functions ------------------------------------------------------- */
void init_UART0(void);
void init_UART1(void);
void sendByte_UART0(uint8_t data);
void sendByte_UART1(uint8_t data);
void sendStr_UART0(char *string);
void sendStr_UART1(char *string);
uint8_t readByte_UART0(void); 
uint8_t readByte_UART1(void); 
void sendPacket_UARTx(uchar uart_number, uchar adr_dst, uchar cmd, uchar *ptr, uint len);
void sendError_UARTx(uchar uart_number, uchar adr_dst, uchar cmd, uchar err);

#endif /* __UART_H */


