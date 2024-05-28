
/* Includes ------------------------------------------------------------------*/
#include "radio.h"
#include "includes.h"
#include "stm32f10x_spi.h"

/* Private defines -----------------------------------------------------------*/

/* Private arrays ------------------------------------------------------------*/
const Bdata_TypeDef BandData[] = 
{
	{4302400L, 4397575L, 43000L, 100L, 25},
	{8604800L, 8795150L, 43000L, 200L, 50},
	{9007200L, 9292725L, 30000L, 300L, 75},
};

const uint BandWidth[] = {670, 1340, 2000, 2700, 3400, 4000};

// �������� ��������� �� ���������
const RfRegisters_TypeDef RFdef = 
{
/*STSREG*/		0x0000,
/*GENCREG*/		//0x8058,				// 433 MHz; Cload=12.5pF; TX registers & FIFO are enabled
                0x8018,				// 433 MHz; Cload=12.5pF; TX registers & FIFO are disabled
/*AFCCREG*/		//0xc4d7,				// Auto AFC (default)
                0xc4f7,
/*TXCREG*/		0x9800,				// (default)
/*TXBREG*/		0xb800,				// (default)
/*CFSREG*/		0xa060,				// Fo=430.240 MHz
/*RXCREG*/		0x9481,				// BW=200 kHz, LNA=0dbm, DRSSI=-97dBm, pin16=DIO, fast VDI
/*BBFCREG*/		0xc22c,				// Digital LPF (default)
/*RXFIFOREG*/	0xb000,				// (default)
/*FIFORSTREG*/	0xca81,				// Sync. latch cleared, limit=8bits, disable sensitive reset
/*SYNBREG*/		0xced4,				// (default)
/*DRSREG*/		0xc623,				// 9579 Baud (default)
/*PMCREG*/		//0x8209,				// Everything off, uC clk disabled
                //0x8208,				// Everything off, uC clk enabled
                0x820a,
/*WTSREG*/		0xe196,				// (default)
/*DCSREG*/		0xc80e,				// (default)
/*BCSREG*/		0xc000,				// (default)
/*PLLCREG*/		0xcc77				// (default)
};

/* Private variables ---------------------------------------------------------*/
//--------------------------------------------------------------------
// MRF49XA SPI commands (by MicroChip):
//--------------------------------------------------------------------
const long    GENCREG 		= 0x8038;		// Cload=12.5pF; TX registers & FIFO are disabled
const long    PMCREG 		= 0x8200;		// Everything off, uC clk enabled
const long    RXCREG 		= 0x94A1;		// BW=135kHz, DRSSI=-97dBm, pin8=VDI, fast VDI
const long    TXBREG 		= 0xB800;
const long    FIFORSTREG	= 0xCA81;		// Sync. latch cleared, limit=8bits, disable sensitive reset
const long    BBFCREG 		= 0xC22C;		// Digital LPF (default)
const long    AFCCREG		= 0xC4D7;		// Auto AFC (default)
const long    CFSREG 		= 0xA7D0;		// Fo=915.000MHz (default)
const long    TXCREG		= 0x9830;		// df=60kHz, Pmax, normal modulation polarity 
const long    DRSREG 		= 0xC623;		// 9579Baud (default)

/* Private functions prototypes ----------------------------------------------*/

/** 
  * @brief      ��������� RADIO
  * @param      None
  * @retval     None
  */
void init_MRF49XA(void) 
{
  printf_Consol("Init MRF49XA...\n\r");

  /* ���������� �������� ������ */
  // �������� ��������� ��������� �����
  memset((void *) &Radio, 0, sizeof (Radio_TypeDef));
  // �������� �������� ���������� ���������� �� ���������
  memcpy_P((void *) &RF, &RFdef, sizeof (RfRegisters_TypeDef));

  /* �������� �������� ������� ������� */
  RF.BCSREG.LBDVB = SET_BAT_TRESH(SysParams.BatTresh/*270*/);
  RF.PMCREG.LBDEN = 1;
  RF.PMCREG.SYNEN = 0;
  
  /* ����������� �������� �������� ����������� */
  RF.TXCREG.OTXPWR = POWER_7_5;// POWER_0;     // max

  /* Send init cmd */
  sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
  RF_SYNC_RESTART();
  
  RF.GENCREG.LCS = 8;
  sendCommand_MRF49XA(RF.GENCREG.HalfWord);
  sendCommand_MRF49XA(RF.AFCCREG.HalfWord);
  sendCommand_MRF49XA(RF.CFSREG.HalfWord);
  sendCommand_MRF49XA(RF.PMCREG.HalfWord);
  sendCommand_MRF49XA(RF.RXCREG.HalfWord);
  sendCommand_MRF49XA(RF.TXCREG.HalfWord);  // ������� ��� ��������� �������� �����������
  sendCommand_MRF49XA(RF.BBFCREG.HalfWord);
  sendCommand_MRF49XA(RF.BCSREG.HalfWord);        // ?
  
  sendCommand_MRF49XA(RF.PLLCREG.HalfWord);
  
  /* ������������� ����������� */
  setRadioData(RANGE_433, BWIDTH_400, DEV_150, 9600/*115200*/);

  /* ����������� �������, �������� ���������� (PMCREG | 0x0020) */
  sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_TXCEN);	
  delay_1ms(4);
  
  /* ��������� ����������, �������� ������� */
  sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_RXCEN);	
  
  /* ������������� ����� ����� */
  setRadioMode(RECEIVE);
  
  /* ��������� ������� ������� ����� ������������� */
  ENABLE_RF();
  delay_1ms(1);
  getRadioStatus(CS_NO);
  DISABLE_RF();   
  if (RF.STSREG.HalfWord == 0x00) {
	displayError(ERR_INIT_RF);
    while (1) { blink(LEDS_GPIO_PORT, LEDBLUE | LEDBLUE, 40, SND_ON); }
  }
  
  /* ��������� ������ */
  setRadioChannel(1);
  SysStat.WorkRadioCh = Radio.Channel;
 
  printf_Consol("Init completed!\n\r");
  printf_Consol("Status: ....... 0x%04x\n\r", RF.STSREG.HalfWord);
  printf_Consol("PMCREG: ....... 0x%04x\n\r", RF.PMCREG.HalfWord);
  printf_Consol("BCSREG: ....... 0x%04x\n\r", RF.BCSREG.HalfWord);
  printf_Consol("Channels: ..... %u\r\n\r", Radio.NumChannel);
  
  // ��������� ���������� �� ����������
#ifdef __RFINT_ENABLED__ 
  enableRFInterrupts();
#else
  disableRFInterrupts();
#endif
  
  /* ����������� ��������� ��������� ������ ��������� ���������� */
  Radio.TaskState = IDLE_STATE;

  /* ������������� ����������� ����� � ��������� */
  Radio.SelfAddr = (BOARD + MY_ADDRESS);
  
  /* ������������� ����� ���������� - 1-� ���������� ����� ������� */
  Radio.RemoteAddr = UIN(1);
  
  sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
  RF_SYNC_RESTART();
}

/** 
  * @brief      �������� ������� � �����������
  * @param      cmd - ��� ������� (2 �����)
  * @retval     None
  */
void sendCommand_MRF49XA(uint cmd) 
{
  DISABLE_RF(); 
  ENABLE_RF();
  send2SPI((cmd & 0xFF00) >> 8);
  send2SPI((cmd & 0x00FF));
  DISABLE_RF();
}

/** 
  * @brief      ��������� �������� �� ���������
  * @param      None
  * @retval     None
  */
void resetRadio_MRF49XA (void)
{
  sendCommand_MRF49XA(FIFORSTREG | 0x0002);
}

/** 
  * @brief      �������� ������ � �����������
  * @param      data - ������ ��� �������� (2 �����)
  *             cs - CS_NO (0) �� ��������� ������ CS MRF49XA
  *                  CS_YES (1) ��������� 
  * @retval     None
  */
void sendData_MRF49XA (uint data, uchar cs) 
{
  if (cs == CS_YES) { ENABLE_RF(); }
  send2SPI((data & 0xFF00) >> 8);
  send2SPI((data & 0x00FF));
  if (cs == CS_YES) { DISABLE_RF(); }
}

/** 
  * @brief      ������ ������ �� �����������
  * @param      None
  * @retval     1 ���� ����������� ������
  */
uchar readData_MRF49XA (void) 
{
  uchar Bt;
 
  ENABLE_RF();
  send2SPI(RF.RXFIFOREG.Adr);
  Bt = send2SPI(0x00);
  DISABLE_RF();
 
  return Bt;
}

/** 
  * @brief      ������ ������ �� FIFO �����������
  * @param      None
  * @retval     1 ���� ����������� ������
  */
uchar readFIFO_MRF49XA (void) 
{
  uchar Bt;
 
  off(RFSEL);
  Bt = send2SPI(0x00);
  on(RFSEL);
 
  return Bt;
}


/** 
  * @brief      ��������� ������� ������� �����������
  * @param      cs - CS_NO (0) �� ��������� ������ CS MRF49XA
  *                  CS_YES (1) ��������� 
  * @retval     ������ ����������� (2 �����)
  */
uint getRadioStatus (uchar cs) 
{
  if (cs == CS_YES) { ENABLE_RF(); }
  RF.STSREG.HByte = send2SPI(0x00);
  RF.STSREG.LByte = send2SPI(0x00);
  if (cs == CS_YES) { DISABLE_RF(); }
  return RF.STSREG.HalfWord;
}

/** 
  * @brief      ��������� ����������� �� ���������� �����������������
  * @param      None
  * @retval     None
  */
void reducePower_MRF49XA(void) 
{
  // Power down the radio chip sendCommand_Radio(0x8201);
  //sendCommand_MRF49XA(0x8201);
  sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_CLKODIS);
}

/** 
  * @brief      ��������� ��������� ������
  * @param      range - �������� ��������� ������
  * @retval     None
  */
void setRadioRange(uchar range) 
{
  Radio.Range = range;
  RF.GENCREG.FBS = range;
  sendCommand_MRF49XA(RF.GENCREG.HalfWord);
}

/** 
  * @brief      ��������� ������ ������
  * @param      bandwidth - �������� ������ ������
  * @retval     None
  */
void setRadioBandWidth(uchar bandwidth) 
{
  if (bandwidth == 0) return;
  Radio.BandWidth = bandwidth;
  Radio.NumChannel = (BandData[Radio.Range - 1].MaxFreq 
                      - BandData[Radio.Range - 1].MinFreq) 
                      / BandWidth[bandwidth - 1];
  
  /**------------------- ��������! ������ � ��������! ------------------------------
  ��� �������� ����������� [SetRadioData(RANGE_433, BWIDTH_400, DEV_150, 9600L)]
  ���������� ������� ������� 17 [0-16] */
  Radio.NumChannel = 17;
  /*-------------------------------------------------------------------------------*/
    
  RF.RXCREG.RXBW = bandwidth;
  RF.RXCREG.RXLNA = LNA_GAIN_0;
  RF.RXCREG.DRSSIT = RSSI_103;
  sendCommand_MRF49XA(RF.GENCREG.HalfWord);
}

/** 
  * @brief      ��������� �������� �����������
  * @param      dev - �������� ��������
  * @retval     None
  */
void setRadioDeviation(uchar dev) 
{
  Radio.Deviation = dev;
  RF.TXCREG.MODBW = dev;
  sendCommand_MRF49XA(RF.TXCREG.HalfWord);
}

/** 
  * @brief      ��������� �������� �������� �����������
  * @param      pow - �������� �������� �������� �����������
  * @retval     None
  */
void setRadioPower(uchar pow) 
{
  RF.TXCREG.OTXPWR = pow;
  sendCommand_MRF49XA(RF.TXCREG.HalfWord);
}

/** 
  * @brief      ��������� �������� �������� ������ � �����������
  * @param      baudrate - �������� �������� �������� ������
  * @retval     None
  */
void setRadioBaud(ulong baudrate) 
{
  RF.DRSREG.DRPE = 0;
  RF.DRSREG.DRPV = ((10000000L / (baudrate * 29 
                    * (1 + RF.DRSREG.DRPE * 7))) - 1) & 0x007f;
  sendCommand_MRF49XA(RF.DRSREG.HalfWord);
}

/** 
  * @brief      ��������� ������
  * @param      channel - ����� ������
  * @retval     None
  */
void setRadioChannel(uint channel) 
{
  ulong Freq, Fval;

  //if (channel >= Radio.NumChannel) return;
  if (channel >= Radio.NumChannel) { channel = 0; }
  Radio.Channel = channel;
  Freq = BandData[Radio.Range - 1].MinFreq + BandWidth[Radio.BandWidth - 1] * channel;
  Fval = ((Freq / (BandData[Radio.Range - 1].FA1) - BandData[Radio.Range - 1].FA0)) * 4;
  RF.CFSREG.FREQB = Fval & 0x0fff;
  sendCommand_MRF49XA(RF.CFSREG.HalfWord);
}

/** 
  * @brief      ������������ �� ��������� �����
  * @param      None
  * @retval     None
  */
void setRadioNextChannel(void) 
{
  // ==== DEBUG SLEEP ====
  if (fl_NoChangeChannel) return;
    
  uint chan = Radio.Channel + HOP_CHANNEL;
  if (Radio.DisChange) return;
  if (chan >= Radio.NumChannel) { chan %= Radio.NumChannel; }
  //printf_Consol("Channel: %u\r\n", chan);
  setRadioChannel(chan);
}

/** 
  * @brief      ��������� �����������
  * @param      range - �������� ��������� ������
  *             bandwidth - �������� ������ ������
  *             dev - �������� ��������
  *             baudrate - �������� �������� �������� ������ � ����������� 
  * @retval     None
  */
void setRadioData(uchar range, uchar bandwidth, uchar dev, ulong baudrate) 
{
  /* ��������� ��������� ������ */
  setRadioRange(range);
  /* ��������� ������ ������ */
  setRadioBandWidth(bandwidth);
  /* ��������� �������� ����������� */
  setRadioDeviation(dev);
  /* ��������� �������� �������� ������ � ����������� */
  setRadioBaud(baudrate);
  // ��������� ������ 0
  //setRadioChannel(0);
}

/** 
  * @brief      ��������� ������ ������ ����������
  * @param      mode - ����� ������ ����������
  *                     RECEIVE - ����
  *                     TRANSMIT - ��������
  * @retval     None
  */
void setRadioMode(uchar mode) 
{
  switch (mode) 
  {
    case RECEIVE:
      /* ������������� ����� ����� */
      Radio.TransmitMode = 0;
      /* ��������� ���������� � ������� */
      sendCommand_MRF49XA(RF.PMCREG.HalfWord); 
      /* ��������� FIFO */
      sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
      /* ��������� FIFO */
      sendCommand_MRF49XA(RF.GENCREG.HalfWord | B_FIFOEN);  // (GENCREG | 0x0040)
      /* �������� ������� */
      sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_RXCEN); // (PMCREG | 0x0080)
      /* ������������� �������� ����������������������� */
      //sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
      RF_SYNC_RESTART();
     /* FIFO Select: Selects the FIFO and the first bit appears on
      the next clock when reading the RXFIFOREG. This pin must be �high� when
      the TX register is enabled. In order to achieve minimum
      current consumption, keep this pin �high� in Sleep mode. */
      on(RFSEL);
      
      //off(LED_RF);
      //printf_Consol("RX MODE\r\n");
      break;
    
    case TRANSMIT:
      /* ������������� ����� �������� */
      Radio.TransmitMode = 1;   
      /* ��������� ������� � ���������� */
      sendCommand_MRF49XA(RF.PMCREG.HalfWord);
      /* ��������� FIFO */
      sendCommand_MRF49XA(RF.GENCREG.HalfWord);
      /* ��������� ������ �������� ����������� 1-� ���� ��������� ��� ��� */
      sendCommand_MRF49XA(RF.GENCREG.HalfWord | B_TXDEN);
      /* �������� ���������� - Enable Tx */
      sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_TXCEN);
      //------- ������ - �������� �� ����������� -----------/
      
      //on(LED_RF);
      //printf_Consol("TX MODE\r\n");
      break;
  }
}

/** 
  * @brief      ������ ��������� ����������
  * @param      None
  * @retval     None
  */
void taskTransiver (void) 
{
#ifdef __RF_MASTER_MODE__   // MASTER  
  switch (Radio.TaskState) 
  {
    case IDLE_STATE:
      // ������ �� ������, ���...
      break;
  
    case SCAN_STATE:
      // ==== DEBUG ====
      /*
      if (++TryNum > 30) {
          TryNum = 0;
          Replay = 0;
          //Radio.RemoteAddr++;
          //Radio.RemoteAddr &= 1;
          Radio.TaskState = IDLE_STATE;
          break;
      }
      */
      
      /* �������� ����� 1-�� ����� ����������� */
      Radio.Rnd.Rnd1 = rnd();
      sendPacket_MRF49XA(cmdAUTORIZE1, &Radio.Rnd.Rnd1, 4, NOENCRIPT);
      /* ������������� ������ �� ����� �������� ������ �� ����� */
      Radio.ScanTimer = WAIT_ACK_TIME;
      Radio.TaskState = WAIT_STATE;
      break;
      
      case WAIT_STATE:
        /* ��� ������ �� ��������� ������ */
        if (Radio.ScanTimer) break;
		if (Radio.Timer == 0) 
        { // ������ �� ���������, ����������� �����
          setRadioNextChannel();
		}
		Radio.TaskState = SCAN_STATE;
        break;
		
      case AUTORIZE_STATE:
		if (Radio.ScanTimer) break;
        Radio.SessState = SESS_CLOSE;
		if (Radio.Timer == 0) {
          setRadioNextChannel();
		}
		Radio.TaskState = SCAN_STATE;
		break;
		
      case SESSION_STATE:
        /* ��������� ������ */
		if (Radio.ScanTimer) break;
		printf_Consol("Session closed!!!\n\r");
        Radio.SessState = SESS_CLOSE;
		/* ����� ������ �������, ��������� � ��������� ������������ */
		// ==== DEBUG ====
        fl_SendSynchroRF = 0;       // ��������� ������������
        /* 
        if (Radio.Timer == 0) {
          setRadioNextChannel();
		}
        Radio.TaskState = SCAN_STATE;
        */
		Radio.TaskState = IDLE_STATE;
        break;
		
      default:
		Radio.TaskState = IDLE_STATE;
		break;
	}
#else       // SLAVE MODE
	switch (Radio.TaskState) 
    {
      case IDLE_STATE:
		// ������ �� ������, ���������� ������� ����� ��� �����
		break;
        
      case SCAN_STATE:
        if (Radio.Timer == 0) 
        { // ����������� �����
          if (!fl_SendSynchroRF) { // ==== DEBUG ====
            setRadioNextChannel();
          }                             
		}
		/* ������������� ������ �� ����� �������� ������ */
		Radio.ScanTimer = SCAN_TIME;
		Radio.TaskState = WAIT_STATE;
        break;
	
      case WAIT_STATE:
        /* ��� ������ �� ��������� ������ */
		if (Radio.ScanTimer) break;
		/* ������ �� ���������, ��������� ����� */
		Radio.TaskState = SCAN_STATE;
		break;
		
      case AUTORIZE_STATE:
        if (Radio.ScanTimer) break;
		Radio.SessState = SESS_CLOSE;
		/* ������ �� ���������, ��������� ����� */
		Radio.TaskState = SCAN_STATE;
		break;
		
      case SESSION_STATE:
        /* ��������� ������ */
        if (Radio.ScanTimer) break;
        Radio.SessState = SESS_CLOSE;
		Radio.TaskState = SCAN_STATE;
        break;
		
      default:
		Radio.TaskState = IDLE_STATE;
		break;
	}
#endif /* __RF_MASTER_MODE__ */

    /* ��������� ���� ������� �� ����������� */
    fl_packRF = 1;
	/* �������� ��������� ������ */
	//checkPacket_MRF49XA();

	/* ������ �������� ������� */
	parserPacket_MRF49XA();
    //================
}

/** 
  * @brief      ��������� ����� � ����������
  * @param      cmd - ��� �������
  *             *adr - ��������� �� ���� ������ ������ [DATA]
  *             size - ������ ���� ������ � ������
  *             enc - �� ��������� ������ (0)
  *                   ��� ��������� ������ (1)  
  * @retval     SUCCESS (1) - ����� �������
  *             ERROR (0) - ������ ��������
  */
ErrorStatus sendPacket_MRF49XA (uchar cmd, void *adr, uint size, uchar enc) 
{
  uint Crc;
  vu16 Tmp;
  TxPackHeader_TypeDef	*Pack = (TxPackHeader_TypeDef *) &Radio.TxBuf;
  ErrorStatus RetVal = SUCCESS;

  // ������ �� ��������� ������� ������
  if(Radio.Transmit) { return ERROR; } 
  
  if (size > DATASIZE) { return ERROR; }

  /* ���������� ��������� ������ */
  Pack->Preamble = PREAMBLE_2;		// ������ ���� ���������, ������ ��� ������ ���� � �������� ��������
  //Pack->Syncro = SYNCHROSEQUENCE;	// ������ ������������������
  Pack->Syncro1 = 0x2D;				// ������ ������������������ ByteL
  Pack->Syncro2 = 0xD4;				// ������ ������������������ ByteH
  Pack->Header.DSize = size;
  Pack->Header.SrcAddr = Radio.SelfAddr;
  Pack->Header.DstAddr = Radio.RemoteAddr;
  Pack->Header.Cmd = cmd;

  /* ��������� ������ � ����� */
  if (size != 0) { 
    memcpy(Pack->Header.Data, adr, size); 
	// ������� ������
    if (enc == ENCRIPT) {
      encryptData(Pack->Header.Data, size);
    }
  }
  
  /* ������� CRC (�� DSize �� ���������� ����� Data) */
  Crc = MakeCRC_RF((uchar *)&Pack->Header, Pack->Header.DSize + sizeof(PackHeader_TypeDef));

  /* ��������� CRC � ����� - ����� ����� Data */
  *((uint *) (Pack->Header.Data + Pack->Header.DSize)) = Crc;

  /* ��������� ������ ���� ��������� */
  *((uchar *) (Pack->Header.Data + Pack->Header.DSize + 2)) = 0xaa;

  /* ������������ � ������ ������������� ������ */
  Radio.TxCnt = 0;      // ���������� ���������� ����
  Radio.TxLen = size + sizeof(TxPackHeader_TypeDef) + 2 + 1;	// 2 [CRC] + 1 - ���� ��� ���������� �������� ���������� ����� ������
  //printf_Consol("Radio.TxLen: .. %u\r\n", Radio.TxLen);

  /* ������������� ����� �������� */
  Radio.Transmit = 1;   // ����� ���������
  setRadioMode(TRANSMIT);
  
  /* ...� �������� �������� */
  ENABLE_RF();    // chip select low - start the transmission
  delay_1ms(1);
  
  /* ���������� ����������� - 1-� ���� ��������� ��� � ��������
  ...�������� �� ����������� ��� Polling */
  
#ifndef __RFINT_ENABLED__   // ���� ���������� �� MRF49XA �� ��������
/*----------------- POLLING -----------------*/
  TimerRF = 100;
  while (Radio.TxCnt < Radio.TxLen)
  {
    getRadioStatus(CS_YES);  // ������ �������� ��������� �����������
    if (RF.STSREG.TXRXFIFO) 
    { // ������� ����������� ����� � ����� ���������� ����� ��� ��������
      RF.TXBREG.TXDB = Radio.TxBuf[Radio.TxCnt++];
      sendData_MRF49XA(RF.TXBREG.HalfWord);
      // Reload Timer
      TimerRF = 5;
    }
    if (TimerRF == 0) {
      RetVal = ERROR; 
      break;
    }
  }
  // ��� ����� ������ ��������
  setRadioMode(RECEIVE);
/*-------------- END OF POLLING ------------*/
#else  
  // ��� ��������� �������� ������
  delay_1ms(50);
  // ����� ��������� ���������� ��������� ��� �� ����� ��������
  if (Radio.TxCnt != Radio.TxLen) { RetVal = ERROR; }
#endif
  
  DISABLE_RF();     // chip select high - end of transmission
  return RetVal;
}

/** 
  * @brief      �������� ����� �� �����������
  * @param      
  * @retval     RF_NODATA (0) - ������  �� ��������
  *             RF_DATA_RECEIVED (1) - ������� ���� ������, ������� ������������
  *             RF_PACKET_RECEIVED (2) - ������� �����
  * @notes      This function should be called periodically until it returns
  *             a valid data packet.
  *             This function should be called periodically , at least once 
  *             every 800us for 9600bps.
  *             For higher speed, the function should be called more rapidly.
  *             When a complete packet has been received, 
  *             the function returns a PACKET_RECEIVED.
  *             In order to "re-arm" the receiver to receive a new packet call the 
  *             RF_SYNC_RESTART().
  *
  *             Use a call similar to :
  *             if (receivePacket_MRF49XA(dataPacket, &length) == RF_PACKET_RECEIVED)
  *             { 
  *                 ... do something with the received packet...
  *             }
  *             //  re-arm for next packet
  *             RF_SYNC_RESTART();
  */
RFReceiveStatus receivePacket_MRF49XA (uchar *data, uchar *length)
{
  static uchar PacketLen = 0;   // �� ���������� ��� ������ �� �������
  static uchar NrBytes = 0;     
  uchar Bl;
  RFReceiveStatus RetVal;

  // Disable interrupts from Radio
  disableRFInterrupts();

  setRadioMode(RECEIVE);  
  ENABLE_RF();                // chip select low
  delay_1ms(1);
  
  getRadioStatus(CS_YES);  // ������ �������� �������� �������
  if (RF.STSREG.TXRXFIFO) 
  { // whether the RX FIFO has reached the preprogrammed limit
    DISABLE_RF();    // SPI CS signal non-active	
	off(RFSEL);      // FIFO Select signal
	if(PacketLen)
	{ // is this the first byte? go on if not
      data[(NrBytes++)] = readData_MRF49XA();   
      if(NrBytes >= PacketLen)
      { // end of packet
        sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);    // reset FIFO	
		on(RFSEL);
		// whole packet received
		*length = NrBytes;				
		NrBytes = 0;
        PacketLen = 0;
		RetVal = RF_PACKET_RECEIVED;	
      }
	}
	else 
	{ // the first byte received
      Bl = readData_MRF49XA();          // number of bytes in payload is the first byte
      if((Bl > 0) && (Bl < DATASIZE))
      { // check if correct number of bytes in payload
        PacketLen = Bl;
		NrBytes = 0; 
      }
      else 
      {	// bad packet length	
		// turn off tx and rx; reset FIFO; disable FIFO, Tx_latch
		// enable the FIFO; turn on receiver; FIFO syncron latch re-enable
		setRadioMode(RECEIVE);
        RetVal = RF_NODATA;
      }
	}
	on(RFSEL);
	RetVal = RF_DATA_RECEIVED;
  }
  else {
    RetVal = RF_NODATA;  
  } 
  
  DISABLE_RF();

  // Enable interrupts from Radio
  enableRFInterrupts();
  
  return RetVal;
}

/** 
  * @brief      ��������� ������������ ����� � ����������
  * @param      *data - ��������� �� ������ ��� ��������
  *             len - ����� ������
  * @retval     SUCCESS - ������ ������� ����������
  *             ERROR - ������ �������� ������
  */
ErrorStatus sendRandPacket_MRF49XA(uchar *data, uchar len)
{
  int i;
  
  //---- turn off receiver , enable Tx register
  sendCommand_MRF49XA(PMCREG);			    // turn off the transmitter and receiver
  sendCommand_MRF49XA(GENCREG | 0x0080);	// Enable the Tx register
  
  // Disable interrupts from Radio
  disableRFInterrupts();
  
  //---- Packet transmission
  // Reset value of the Tx regs are [AA AA], we can start transmission
  //---- Enable Tx
  sendCommand_MRF49XA(PMCREG | 0x0020);		// turn on tx

  ENABLE_RF();  						// chip select low

  TimerRF = 10;
  while (GPIO_ReadInputDataBit(RSDO) == 0) {
    if (TimerRF == 0) { return ERROR; } 
  }
  sendData_MRF49XA(TXBREG | 0xAA, CS_NO);	// preamble
 
  //ENABLE_RF();
  TimerRF = 10;
  while (GPIO_ReadInputDataBit(RSDO) == 0) {
    if (TimerRF == 0) { return ERROR; } 
  }
  sendData_MRF49XA(TXBREG | 0x2D, CS_NO);	// sync pattern 1st byte
  
  //ENABLE_RF();
  TimerRF = 10;
  while (GPIO_ReadInputDataBit(RSDO) == 0) {
    if (TimerRF == 0) { return ERROR; } 
  }
  sendData_MRF49XA(TXBREG | 0xD4, CS_NO);	// sync pattern 2nd byte
  
  //ENABLE_RF();
  TimerRF = 10;
  while (GPIO_ReadInputDataBit(RSDO) == 0) {
    if (TimerRF == 0) { return ERROR; } 
  }
  sendData_MRF49XA(TXBREG | len, CS_NO);	

#ifdef SPECTRUM_TEST
  for (i = 0; 1;) {				// endless loop
#else	
  for (i = 0; i < len; i++) {	// send payload
#endif
  				// wait for SDO, when buffer data <=8 bits
#ifdef SPECTRUM_TEST
  //ENABLE_RF();
  TimerRF = 10;
  while (GPIO_ReadInputDataBit(RSDO) == 0) {
    if (TimerRF == 0) { return ERROR; } 
  }
  sendData_MRF49XA(0xAA, CS_NO); 		// spectrum test
#else			
  //ENABLE_RF();
  TimerRF = 10;
  while (GPIO_ReadInputDataBit(RSDO) == 0) {
    if (TimerRF == 0) { return ERROR; } 
  }
  sendData_MRF49XA(TXBREG | data[i], CS_NO); // write a byte to tx register
#endif
  }

  //ENABLE_RF();
  TimerRF = 10;
  while (GPIO_ReadInputDataBit(RSDO) == 0) {
    if (TimerRF == 0) { return ERROR; } 
  }
  sendData_MRF49XA(TXBREG | 0x00, CS_NO);	// write a dummy byte since the previous byte is still in buffer 		

  // wait for the last byte transmission end
  //ENABLE_RF();
  TimerRF = 10;
  while (GPIO_ReadInputDataBit(RSDO) == 0) {
    if (TimerRF == 0) { return ERROR; } 
  }
  
  DISABLE_RF();			// chip select high, end transmission

  //---- Turn off Tx disable the Tx register
  sendCommand_MRF49XA(PMCREG | 0x0080);		// turn off Tx, turn on the receiver
  sendCommand_MRF49XA(GENCREG | 0x0040);	// disable the Tx register, Enable the FIFO

  // Enable interrupts from Radio
  enableRFInterrupts();
  
  return SUCCESS;
}

/** 
  * @brief      ��������� ����� � ����� ������ � ����������
  * @param      cmd - ��� �������, ������� ������� ������
  *             err - ��� ������
  * @retval     SUCCESS (1) - ����� �������
  *             ERROR (0) - ������ ��������
  * @Note       ����� �� ���������
  */
ErrorStatus sendError_MRF49XA(uchar cmd, uchar err)
{
	TError Error;
	
    Error.Cmd = cmd;
	Error.Code = err;
    return sendPacket_MRF49XA(cmdERROR, &Error, sizeof(TError), NOENCRIPT); 
}

/** 
  * @brief      �������� �������� ������
  * @param      cmd - ��� �������
  * @retval     0 - OK
  *             1 - Error
  */
uchar isNoOpenSess (uchar cmd) 
{
	if (Radio.SessState == SESS_OPEN) return 0;
	sendError_MRF49XA(cmd, errAccess);
	return 1;
}

/** 
  * @brief      ������� ������ ������������� � ����������  
  * @param      channel - ����� ������ ��� ������� ������� �������������  
  *             sw - 1 ��������� ������������ �������
  *                  0 ��������� ������������ �������
  * @retval     None
  */
void synchroRF (u16 channel, u8 sw)
{
  u16 TmpAddr = Radio.RemoteAddr;
  u16 TmpChannel = Radio.Channel;
#ifndef __RADIONET__
  return;
#endif /* __RADIONET__ */  
  if (channel != TmpChannel) 
  { setRadioChannel(channel); }
  if (!sw)
  { Radio.Timer = SCAN_WAIT_TIME; }    
  
  if (TimerSynchroRF == 0) {
    TimerSynchroRF = __TIMERSYNCHRORF;
    Radio.RemoteAddr = BROADCAST_ADDR;
    //printf_Consol("W:%u R:%u :: ", SysStat.WorkRadioCh, TmpChannel);
    sendPacket_MRF49XA(cmdSYNCHRO_RF, NULL, 0, NOENCRIPT);
  }
  
  /* ���������� �� ���� */
  Radio.RemoteAddr = TmpAddr;
  if (channel != TmpChannel) 
  { setRadioChannel(TmpChannel); }
}

