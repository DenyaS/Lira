
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
  uint	RPageDF;				// ����� �������� �������� � ������ ��� ������
  uint	WPageDF;				// ����� �������� �������� � ������ ��� ������
} DFLASH;

/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

/** 
  * @brief      ������������� DataFlash
  * @param      None
  * @retval     None
  */
void init_AT45 (void) 
{
  DFLASH.RPageDF = 0xffff;
  DFLASH.WPageDF = 0xffff;

  // ��������� DataFlash
  DISABLE_DF();

  // Dummy Write
  send2SPI(0x00);

  // ��������� ����� �� AT45DB161D
  on(FRST);
  delay_1ms(10);
  off(FRST);
  delay_1ms(1);
  
  // First call for init PageBits
  readStatus_AT45();
}

/** 
  * @brief      ��������� ������ DataFlash
  * @param      None
  * @retval     ���������� �� DataFlash ���� �������
  */
uchar readStatus_AT45 (void) 
{
  uchar Bt;
  
  DISABLE_DF(); ENABLE_DF();    // Make sure to toggle CS signal
  Bt = send2SPI(STATUSREG);     // �������� StatusRegister read opcode 
  Bt = send2SPI(0x00);          // Dummy write to get result 
  DISABLE_DF();
  return Bt;
}

/** 
  * @brief      ����������������� DataFlash
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
  * @brief      �������� ����� ��� �������� ����������
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
  * @brief      ����� ���������� DataFlash
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
	// �������� ������ ������ Status Register
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
  * @brief      ��������� �������� �� ������ DataFlash � ���������� � ������
  * @param      buf - ����� ������ (0 ��� 1)
  *             page - ����� ��������
  * @retval     SUCCESS - ������ ������ � �������� �������
  *             ERRROR - DataFlash �� ������
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
  * @brief      ��������� � ����� AT45 �������� page �������� 528 ����
  * @param      buf - ����� ������ (0 ��� 1)
  *             page - ����� ��������
  * @retval     SUCCESS - ������ ������� ��������� � �����
  *             ERROR - AT45 �� ������
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
  * @brief      �������� �� ������ AT45 �������� page �������� 528 ����
  * @param      buf - ����� ������ (0 ��� 1)
  *             page - ����� ��������
  * @retval     SUCCESS - ������ ������� �������� �� ������ � ��������
  *             ERROR - AT45 �� ������
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
  * @brief      ��������� (�����������) � ������ [*adr] ������ ������ [len] �� ������ AT45 
  * @param      buf - ����� ������ AT45 (0 ��� 1)
  *             *adr - ��������� �� ������ ��� ����������� �� ������ AT45
  *             offset - 
  *             len - ������ ������ ��� �����������
  * @retval     SUCCESS - ������ ������� �����������
  *             ERROR - AT45 �� ������
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
  * @brief      �������� (�����������) � ����� AT45 ������ [*adr] ������ [len]  
  * @param      buf - ����� ������ AT45 (0 ��� 1)
  *             *adr - ��������� �� ������ ��� ����������� � ����� AT45
  *             offset - 
  *             len - ������ ������ ��� �����������
  * @retval     SUCCESS - ������ ������� �����������
  *             ERROR - AT45 �� ������
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
  * @brief      �������� ��� DataFlash AT45DB161D 
  * @param      None
  * @retval     SUCCESS - �������
  *             ERROR - ������
  */
ErrorStatus flushCache_AT45 (void) 
{
  if (writePage_AT45(1, DFLASH.WPageDF) == ERROR) { return ERROR; }
  DFLASH.RPageDF = 0xFFFF;
  DFLASH.WPageDF = 0xFFFF;
  return SUCCESS;
}

/** 
  * @brief      ��������� ������ �������� [size] � ����� [*buf] �� ������ [adr] 
  * @param      adr - ����� ��� ������ ������
  *             *dst - ��������� �� ����� ��� ����������� ������
  *             len - ������ ������ ��� ������
  * @retval     SUCCESS - ������ ������� ���������
  *             ERROR - ������
  */
ErrorStatus readData_AT45 (u8 *dst, u32 adr, u16 len)
{ 
    u16 IntPageAdr;    //Internal page address
    u16 IntByteAdrBuff;
    u16 Size, Allsize, Npage, i;
          
    IntPageAdr = (adr / DFPageSize);
    IntByteAdrBuff  = (adr % DFPageSize);
    Allsize = IntByteAdrBuff + len;

    if(Allsize > DFPageSize)  // ����������� ������ ����������� � ���������� ���������
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
    else                        //����������� ������ ����������� � �������� ����� ��������
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
  * @brief      �������� ������ �������� [size] �� ������ [*buf] �� ������ adr  
  * @param      adr - ����� ������ ��� ������ ������
  *             *src - ��������� �� ����� ��� ������ ������
  *             len - ������ ������ 
  * @retval     SUCCESS - ������ ������� ��������
  *             ERROR - ������
  */
ErrorStatus writeData_AT45 (u8 *src, u32 adr, u16 len)
{ 
    u16 IntPageAdr;    //Internal page address
    u16 IntByteAdrBuff;
    u16 Size, Allsize, Npage, i;
          
    IntPageAdr = (adr / DFPageSize);
    IntByteAdrBuff = (adr % DFPageSize);
    Allsize = IntByteAdrBuff + len;

    if(Allsize > DFPageSize)  //������ ���������� ����������� � ���������� ���������
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
    else                        //������ ���������� ����������� � �������� ����� ��������
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

