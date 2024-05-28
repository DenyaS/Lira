
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MEM_H
#define __MEM_H

/* Defines ------------------------------------------------------------------*/

//---------- 1. Внутренняя FLASH память контроллера ---------------------------
#define ADR_START_FLASH         ((uint32_t) 0x08000000)
#define ADR_END_FLASH           ((uint32_t) 0x0801FFFF)
#define FLASHMEMORY_SIZE        (ADR_END_FLASH - ADR_START_FLASH + 1)  // 131072 bytes = 128 kByte

/* Define the STM32F10x FLASH Page Size */
#define FLASH_PAGE_SIZE         1024           // размер страницы 1 kByte

/* < Permanent Data Bytes base address */
//#define ADR_DEFAULTVALS_BASE    ((uint32_t)0x0801F000)      // Стартовый адрес для хранения данных по умолчанию
// храним данные в последней странице
#define ADR_DEFAULTVALS_BASE    (ADR_END_FLASH - FLASH_PAGE_SIZE + 1)  //0x801FC00

// Объём памяти FLASH, доступной для хранения данных
#define FLASH_FREESIZE          ADR_END_DATAFLASH - ADR_DEFAULTVALS_BASE    // 0x100 = 4 kByte

// Распределение адресов в памяти FLASH
#define DEFAULTVALS_DATA        ((SystemParams_TypeDef *) ADR_DEFAULTVALS_BASE)

#define ADR_USERPARAMS_BASE     (ADR_DEFAULTVALS_BASE + sizeof(SystemParams_TypeDef) + 4) /* < USER Vals Data Bytes base address */  // orig. (uint32_t)0x08080000
#define USERPARAMS_DATA         ((SystemParams_TypeDef *) ADR_USERPARAMS_BASE)
    
#define ADR_VERSION             ADR_USERPARAMS_BASE
#define ADR_BITVARS             (ADR_VERSION + 1)
#define ADR_BOARDNUMBER         (ADR_VERSION + 4)
#define ADR_INDEXREAD           (ADR_BOARDNUMBER + 4)
#define ADR_INDEXWRITE          (ADR_INDEXREAD + 4)
#define ADR_CELNUMBER           (ADR_INDEXWRITE + 4)
#define ADR_SMSINT              (ADR_CELNUMBER + 32)
#define ADR_QTYPPD              (ADR_SMSINT + 4)
#define ADR_BATTRESH            (ADR_QTYPPD + 4)
#define ADR_TIMESEATS           (ADR_BATTRESH + 4)
#define ADR_DVAL_10             (ADR_TIMESEATS + 4)
#define ADR_DVAL_11             (ADR_DVAL_10 + 4)
#define ADR_LCD_CONTRAST        (ADR_DVAL_11 + 4)
#define ADR_DVAL_13             (ADR_LCD_CONTRAST + 4)
#define ADR_DVAL_14             (ADR_DVAL_13 + 4)
#define ADR_DVAL_15             (ADR_DVAL_14 + 4)
#define ADR_DVAL_16             (ADR_DVAL_15 + 4)
#define ADR_DVAL_17             (ADR_DVAL_16 + 4)
#define ADR_DVAL_18             (ADR_DVAL_17 + 4)
#define ADR_DVAL_19             (ADR_DVAL_18 + 4)
#define ADR_DVAL_20             (ADR_DVAL_19 + 4)
#define ADR_DVAL_21             (ADR_DVAL_20 + 4)
#define ADR_DVAL_22             (ADR_DVAL_21 + 4)
#define ADR_DVAL_23             (ADR_DVAL_22 + 4)
#define ADR_DVAL_24             (ADR_DVAL_23 + 4)
#define ADR_DVAL_25             (ADR_DVAL_24 + 4)
#define ADR_DVAL_26             (ADR_DVAL_25 + 4)
#define ADR_DVAL_27             (ADR_DVAL_26 + 4)
#define ADR_DVAL_28             (ADR_DVAL_27 + 4)
#define ADR_DVAL_29             (ADR_DVAL_28 + 4)
#define ADR_DVAL_30             (ADR_DVAL_29 + 4)
#define ADR_MINUEND             (ADR_DVAL_30 + 4)
#define ADR_SUBTRAHEND          (ADR_MINUEND + 4)

//---------- 2. Внeшняя External Flash  AT45DB161D 2 MByte ---------------------------
#define ADR_START_EXTFLASH      ((uint32_t) 0x40400000)
#define ADR_END_EXTFLASH        ((uint32_t) 0x40600000)

#define EXTFLASH_SIZE           (ADR_END_EXTFLASH - ADR_START_EXTFLASH)  // 2097152 Byte = 2 MByte

// Байты адреса, определяющие тип памяти
#define STARTPOINT_FLASH        (uchar)((ADR_START_FLASH >> 16) & 0xFF)
#define ENDPOINT_FLASH          (uchar)((ADR_END_FLASH >> 16) & 0xFF)

#define STARTPOINT_EXTFLASH     (uchar)((ADR_START_EXTFLASH >> 16) & 0xFF)
#define ENDPOINT_EXTFLASH       (uchar)((ADR_END_EXTFLASH >> 16) & 0xFF)

#endif /* __MEM_H */


