
/* Includes ------------------------------------------------------------------*/
#include "extflash.h"
#include "includes.h"
#include "stm32f10x_spi.h"

/* Private defines -----------------------------------------------------------*/
#define	ENABLE_DF()     off(DFCS)      // Enable DataFlash
#define	DISABLE_DF()	on(DFCS)       // Disable DataFlash

/* Private macro -------------------------------------------------------------*/

/* Private structures --------------------------------------------------------*/
struct {
  uint	RPageDF;				// Номер страницы датафлеш в буфере для чтения
  uint	WPageDF;				// Номер страницы датафлеш в буфере для записи
} DFLASH;

/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

/** 
  * @brief      Инициализация DataFlash
  * @param      None
  * @retval     None
  */
void init_AT45 (void) 
{
  DFLASH.RPageDF = 0xffff;
  DFLASH.WPageDF = 0xffff;

  // Отключаем DataFlash
  DISABLE_DF();

  // Dummy Write
  send2SPI(0x00);

  // Формируем сброс на AT45DB161D
  on(FRST);
  delay_1ms(10);
  off(FRST);
  delay_1ms(1);
  
  // First call for init PageBits
  readStatus_AT45();
}

/** 
  * @brief      Прочитать статус DataFlash
  * @param      None
  * @retval     Полученный от DataFlash байт статуса
  */
uchar readStatus_AT45 (void) 
{
  uchar Bt;
  
  DISABLE_DF(); ENABLE_DF();    // Make sure to toggle CS signal
  Bt = send2SPI(STATUSREG);     // Передаем StatusRegister read opcode 
  Bt = send2SPI(0x00);          // Dummy write to get result 
  DISABLE_DF();
  return Bt;
}

/** 
  * @brief      Переинициализация DataFlash
  * @param      None
  * @retval     None
  */
void reinit_AT45(void) 
{
  DISABLE_DF();
  on(FRST);
  delay_1ms(10);
  off(FRST);			
  delay_1ms(20);
  init_AT45();
}

/** 
  * @brief      Стирание блока без ожидания готовности
  * @param      block = 0...1023
  * @retval     0
  */
void eraseBlock_AT45 (uint block) 
{
  DISABLE_DF(); ENABLE_DF();
  send2SPI(ERASE_BLOCK);
  send2SPI(block >> 3);
  send2SPI(block << 5);
  send2SPI(0);
  DISABLE_DF();
}

/** 
  * @brief      Ждать готовность DataFlash
  * @param      None
  * @retval     SUCCESS (1) - AT45DB161D is ready
  *             ERROR (0) - AT45DB161D is not ready
  */
ErrorStatus waitReady_AT45 (void) 
{
  uchar State, Reply;
  uint TO;
	
  Reply = 2;
  TO = getTickCnt();
  while (1) {
    State = readStatus_AT45();
	// Проверка чтения именно Status Register
	if ((State & 0x3c) == 0x2c) {
      if (State & 0x80) break;
	}
	if ((getTickCnt() - TO) > DFMaxTOReady) {
      if (Reply) {
		Reply--;
		reinit_AT45();
      } 
      else {
		return ERROR;
      }
	}
  }
  return SUCCESS;
}

/** 
  * @brief      Сравнение страницы из памяти DataFlash с содержимым её буфера
  * @param      buf - номер буфера (0 или 1)
  *             page - номер страницы
  * @retval     SUCCESS - данные буфера и страницы совпали
  *             ERRROR - DataFlash не готова
  */
ErrorStatus compare_AT45 (uchar buf, uint page) 
{
  DISABLE_DF(); ENABLE_DF();
  if (buf == 0) { send2SPI (BUF1_COMPARE); }
  else { send2SPI (BUF2_COMPARE); }
  send2SPI(page >> 6);
  send2SPI(page << 2);
  send2SPI(0x00);
  DISABLE_DF();
  if (waitReady_AT45() == ERROR) { return ERROR; }
  if ((readStatus_AT45() & 0x40) != 0) { return ERROR; }
  return SUCCESS;
}

/** 
  * @brief      Прочитать в буфер AT45 страницу page размером 528 байт
  * @param      buf - номер буфера (0 или 1)
  *             page - номер страницы
  * @retval     SUCCESS - данные успешно прочитаны в буфер
  *             ERROR - AT45 не готова
  */
ErrorStatus readPage_AT45 (uchar buf, uint page) 
{
  if (waitReady_AT45() == ERROR) { return ERROR; }
  DISABLE_DF(); ENABLE_DF();
  if (buf) { send2SPI(BUF2_TRANSFER); }
  else { send2SPI(BUF1_TRANSFER); }
  send2SPI(page >> 6);
  send2SPI(page << 2);
  send2SPI(0x00);
  DISABLE_DF();
  if (waitReady_AT45() == ERROR) { return ERROR; }
  return SUCCESS;
}

/** 
  * @brief      Записать из буфера AT45 страницу page размером 528 байт
  * @param      buf - номер буфера (0 или 1)
  *             page - номер страницы
  * @retval     SUCCESS - данные успешно записаны из буфера в страницу
  *             ERROR - AT45 не готова
  */
ErrorStatus writePage_AT45 (uchar buf, uint page) 
{
  if (waitReady_AT45() == ERROR) { return ERROR; }
  DISABLE_DF(); ENABLE_DF();
  if (buf) { send2SPI(BUF2_ToFlashWE); }
  else { send2SPI(BUF1_ToFlashWE); }
  send2SPI(page >> 6);
  send2SPI(page << 2);
  send2SPI(0x00);
  DISABLE_DF();
  if (waitReady_AT45() == ERROR) { return ERROR; }
  return SUCCESS;
}

/** 
  * @brief      Прочитать (скопировать) в память [*adr] данные длиной [len] из буфера AT45 
  * @param      buf - номер буфера AT45 (0 или 1)
  *             *adr - указатель на данные для копирования из буфера AT45
  *             offset - 
  *             len - размер данных для копирования
  * @retval     SUCCESS - данные успешно скопированы
  *             ERROR - AT45 не готова
  */
ErrorStatus readBuf_AT45 (uchar buf, uchar *adr, uint offset, uint len) 
{
  if (waitReady_AT45() == ERROR) return ERROR;
  DISABLE_DF(); ENABLE_DF();
  if (buf) { send2SPI(BUF2_READ); }
  else { send2SPI(BUF1_READ); }
  send2SPI(0x00);
  send2SPI(offset >> 8);
  send2SPI(offset & 0xff);
  send2SPI(0x00);
  do {
    *adr++ = send2SPI(0x00);
  } while (--len);
  DISABLE_DF();
  if (waitReady_AT45() == ERROR) { return ERROR; }
  return SUCCESS;
}

/** 
  * @brief      Записать (скопировать) в буфер AT45 данные [*adr] длиной [len]  
  * @param      buf - номер буфера AT45 (0 или 1)
  *             *adr - указатель на данные для копирования в буфер AT45
  *             offset - 
  *             len - размер данных для копирования
  * @retval     SUCCESS - данные успешно скопированы
  *             ERROR - AT45 не готова
  */
ErrorStatus writeBuf_AT45 (uchar buf, uchar *adr, uint offset, uint len) 
{
  if (waitReady_AT45() == ERROR) { return ERROR; }
  DISABLE_DF(); ENABLE_DF();
  if (buf) 
    send2SPI(BUF2_WRITE);
  else 
    send2SPI(BUF1_WRITE);
  send2SPI(0x00);
  send2SPI(offset >> 8);
  send2SPI(offset & 0xff);
  do {
    send2SPI(*adr++);
  } while (--len);
  DISABLE_DF();
  if (waitReady_AT45() == ERROR) { return ERROR; }
  return SUCCESS;
}

/** 
  * @brief      Сбросить КЭШ DataFlash AT45DB161D 
  * @param      None
  * @retval     SUCCESS - успешно
  *             ERROR - ошибка
  */
ErrorStatus flushCache_AT45 (void) 
{
  if (writePage_AT45(1, DFLASH.WPageDF) == ERROR) { return ERROR; }
  DFLASH.RPageDF = 0xFFFF;
  DFLASH.WPageDF = 0xFFFF;
  return SUCCESS;
}

/** 
  * @brief      Прочитать данные размером [size] в буфер [*buf] по адресу [adr] 
  * @param      adr - адрес для чтения данных
  *             *dst - указатель на буфер для копирования данных
  *             len - размер данных для чтения
  * @retval     SUCCESS - данные успешно прочитаны
  *             ERROR - ошибка
  */
ErrorStatus readData_AT45 (u8 *dst, u32 adr, u16 len)
{ 
    u16 IntPageAdr;    //Internal page address
    u16 IntByteAdrBuff;
    u16 Size, Allsize, Npage, i;
          
    IntPageAdr = (adr / DFPageSize);
    IntByteAdrBuff  = (adr % DFPageSize);
    Allsize = IntByteAdrBuff + len;

    if(Allsize > DFPageSize)  // необходимые данные расположены в нескольких страницах
    {
      Npage = Allsize / DFPageSize;
      for(i = 0; i < (Npage + 1); i++ )
      {
        //Read from Page to Buffer1 DF
        while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
        if (readPage_AT45(1, IntPageAdr + i) == ERROR) return ERROR;        

        Size = DFPageSize - IntByteAdrBuff;
        if(Size > (Allsize - DFPageSize * i)) Size = (Allsize - DFPageSize * i);

        //Read from Buffer1 DF
        while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
        if (readBuf_AT45(1, dst, IntByteAdrBuff, Size) == ERROR) return ERROR;
        IntByteAdrBuff = 0;
        dst += Size;
      }
    }
    else                        //необходимые данные расположены в пределах одной страницы
    {
      //Read from Page to Buffer1 DF
      while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
      if (readPage_AT45(1, IntPageAdr) == ERROR) return ERROR;

      //Read from Buffer1 DF
      while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
      if (readBuf_AT45(1, dst, IntByteAdrBuff, len) == ERROR) return ERROR;
    }
    while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
    return SUCCESS;
}

/** 
  * @brief      Записать данные размером [size] из буфера [*buf] по адресу adr  
  * @param      adr - адрес памяти для записи данных
  *             *src - указатель на буфер для чтения данных
  *             len - размер данных 
  * @retval     SUCCESS - данные успешно записаны
  *             ERROR - ошибка
  */
ErrorStatus writeData_AT45 (u8 *src, u32 adr, u16 len)
{ 
    u16 IntPageAdr;    //Internal page address
    u16 IntByteAdrBuff;
    u16 Size, Allsize, Npage, i;
          
    IntPageAdr = (adr / DFPageSize);
    IntByteAdrBuff = (adr % DFPageSize);
    Allsize = IntByteAdrBuff + len;

    if(Allsize > DFPageSize)  //данные необходимо расположить в нескольких страницах
    {
        Npage = Allsize / DFPageSize;
        for(i = 0; i < (Npage + 1); i++ )
        {          
            //Read from Page to Buffer2 DF
            while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
            if (readPage_AT45(2, IntPageAdr + i) == ERROR) return ERROR;
    
            Size = DFPageSize - IntByteAdrBuff;
            if(Size > (Allsize - DFPageSize * i)) Size = (Allsize - DFPageSize * i);

            //Write to Buffer2 DF
            while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
            if (writeBuf_AT45(2, src, IntByteAdrBuff, Size) == ERROR) return ERROR;
            IntByteAdrBuff = 0;
            src += Size;
    
            //Write from Buffer2 to Page DF
            while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
            if (writePage_AT45(2, IntPageAdr + i) == ERROR) return ERROR;
        }
    }
    else                        //данные необходимо расположить в пределах одной страницы
    {     
        //Read from Page to Buffer2 DF
        while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
        if (readPage_AT45(2, IntPageAdr) == ERROR) return ERROR;
  
        //Write to Buffer2 DF
        while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
        if (writeBuf_AT45(2, src, IntByteAdrBuff, len) == ERROR) return ERROR;
  
        //Write from Buffer2 to Page DF
        while(!(readStatus_AT45() & 0x80));		//monitor the status register, wait until busy-flag is high
        if (writePage_AT45(2, IntPageAdr) == ERROR) return ERROR;
    }
    return SUCCESS;
}

