
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __EXTFLASH_H
#define __EXTFLASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32F10x.h" 
#include "unsigned.h"

/* Defines ------------------------------------------------------------------ */
#define DFCS       DBOARD_GPIO_PORT_1, EE_CS  // Вывод CS микросхемы памяти
#define FRST       DBOARD_GPIO_PORT_2, EE_RST // Reset signal for AT45DB161D

#define DFMaxTOReady	600		// = 60 ms - максимальное время ожидания готовности DataFlash

#ifdef __AT45DB161D__
  #define DFNumBlock    512				// число блоков в DataFlash
  #define DFBlockSize   4096            // Размер блока (Byte)

  #define DFNumPage		4096			// Число страниц по DFPageSize
  #define DFPageSize	512				// Размер страницы (Byte)
#else
  #define DFNumBlock    264				
  #define DFBlockSize   2048            

  #define DFNumPage		2048			
  #define DFPageSize	264				
#endif

#define DFSpareSize		16				// Размер дополнительной области

#define DF_SIZE         (DFNumBlock * DFBlockSize) // Размер DataFlash 2MByte

// DataFlash opcodes
#define STATUSREG		0xd7    // Status Register
#define ERASE_BLOCK     0x50    // Block erase
#define BUF1_COMPARE	0x60	// Main memory page to buffer 1 compare
#define BUF2_COMPARE   	0x61	// Main memory page to buffer 2 compare
#define BUF1_TRANSFER 	0x53	// Main memory page to buffer 1 transfer
#define BUF2_TRANSFER	0x55	// Main memory page to buffer 2 transfer
#define BUF1_READ		0xd4	// Buffer 1 read
#define BUF2_READ		0xd6	// Buffer 2 read
#define BUF1_ToFlashWE	0x83	// Buffer 1 to main memory page program with built-in erase
#define BUF2_ToFlashWE	0x86	// Buffer 2 to main memory page program with built-in erase
#define BUF1_WRITE		0x84	// Buffer 1 write
#define BUF2_WRITE		0x87	// Buffer 2 write

/* Exported functions ------------------------------------------------------- */
void init_AT45(void); 
uchar readStatus_AT45(void); 
void reinit_AT45(void);
void eraseBlock_AT45(uint block); 
ErrorStatus waitReady_AT45(void); 
ErrorStatus compare_AT45(uchar buf, uint page);
ErrorStatus readPage_AT45(uchar buf, uint page); 
ErrorStatus writePage_AT45(uchar buf, uint page);
ErrorStatus readBuf_AT45(uchar buf, uchar *adr, uint offset, uint len); 
ErrorStatus writeBuf_AT45(uchar buf, uchar *adr, uint offset, uint len); 
ErrorStatus flushCache_AT45(void); 
ErrorStatus readData_AT45(u8 *dst, u32 adr, u16 len);
ErrorStatus writeData_AT45(u8 *src, u32 adr, u16 len);

#endif /* __EXTFLASH_H */

