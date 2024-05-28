
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

// Значения регистров по умолчанию
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
  * @brief      Настройка RADIO
  * @param      None
  * @retval     None
  */
void init_MRF49XA(void) 
{
  printf_Consol("Init MRF49XA...\n\r");

  /* Подготовка структур данных */
  // Обнуляем структуру состояния радио
  memset((void *) &Radio, 0, sizeof (Radio_TypeDef));
  // Заполням регистры микросхемы значениями по умолчанию
  memcpy_P((void *) &RF, &RFdef, sizeof (RfRegisters_TypeDef));

  /* Включаем контроль батареи питания */
  RF.BCSREG.LBDVB = SET_BAT_TRESH(SysParams.BatTresh/*270*/);
  RF.PMCREG.LBDEN = 1;
  RF.PMCREG.SYNEN = 0;
  
  /* Декларируем выходную мощность передатчика */
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
  sendCommand_MRF49XA(RF.TXCREG.HalfWord);  // Команда для установки мощности передатчика
  sendCommand_MRF49XA(RF.BBFCREG.HalfWord);
  sendCommand_MRF49XA(RF.BCSREG.HalfWord);        // ?
  
  sendCommand_MRF49XA(RF.PLLCREG.HalfWord);
  
  /* Устанавливаем радиоданные */
  setRadioData(RANGE_433, BWIDTH_400, DEV_150, 9600/*115200*/);

  /* Настраиваем антенну, включаем передатчик (PMCREG | 0x0020) */
  sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_TXCEN);	
  delay_1ms(4);
  
  /* Выключаем передатчик, включаем приёмник */
  sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_RXCEN);	
  
  /* Устанавливаем режим приёма */
  setRadioMode(RECEIVE);
  
  /* Проверяем регистр статуса после инициализации */
  ENABLE_RF();
  delay_1ms(1);
  getRadioStatus(CS_NO);
  DISABLE_RF();   
  if (RF.STSREG.HalfWord == 0x00) {
	displayError(ERR_INIT_RF);
    while (1) { blink(LEDS_GPIO_PORT, LEDBLUE | LEDBLUE, 40, SND_ON); }
  }
  
  /* Установка канала */
  setRadioChannel(1);
  SysStat.WorkRadioCh = Radio.Channel;
 
  printf_Consol("Init completed!\n\r");
  printf_Consol("Status: ....... 0x%04x\n\r", RF.STSREG.HalfWord);
  printf_Consol("PMCREG: ....... 0x%04x\n\r", RF.PMCREG.HalfWord);
  printf_Consol("BCSREG: ....... 0x%04x\n\r", RF.BCSREG.HalfWord);
  printf_Consol("Channels: ..... %u\r\n\r", Radio.NumChannel);
  
  // Разрешаем прерывания от трансивера
#ifdef __RFINT_ENABLED__ 
  enableRFInterrupts();
#else
  disableRFInterrupts();
#endif
  
  /* Устанавивам начальное состояние задачи обработки трансивера */
  Radio.TaskState = IDLE_STATE;

  /* Устанавливаем собственный адрес в радиосети */
  Radio.SelfAddr = (BOARD + MY_ADDRESS);
  
  /* Устанавливаем адрес получателя - 1-й порядковый номер датчика */
  Radio.RemoteAddr = UIN(1);
  
  sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
  RF_SYNC_RESTART();
}

/** 
  * @brief      Передача команды в радиомодуль
  * @param      cmd - код команды (2 байта)
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
  * @brief      Установка значений по умолчанию
  * @param      None
  * @retval     None
  */
void resetRadio_MRF49XA (void)
{
  sendCommand_MRF49XA(FIFORSTREG | 0x0002);
}

/** 
  * @brief      Передача данных в радиомодуль
  * @param      data - данные для передачи (2 байта)
  *             cs - CS_NO (0) не управлять входом CS MRF49XA
  *                  CS_YES (1) управлять 
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
  * @brief      Чтение данных из радиомодуля
  * @param      None
  * @retval     1 байт прочитанных данных
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
  * @brief      Чтение данных из FIFO радиомодуля
  * @param      None
  * @retval     1 байт прочитанных данных
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
  * @brief      Прочитать регистр статуса радиомодуля
  * @param      cs - CS_NO (0) не управлять входом CS MRF49XA
  *                  CS_YES (1) управлять 
  * @retval     Статус радиомодуля (2 байта)
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
  * @brief      Перевести радиомодуль на пониженное энергопотребление
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
  * @brief      Установка диапазона частот
  * @param      range - значение диапазона частот
  * @retval     None
  */
void setRadioRange(uchar range) 
{
  Radio.Range = range;
  RF.GENCREG.FBS = range;
  sendCommand_MRF49XA(RF.GENCREG.HalfWord);
}

/** 
  * @brief      Установка ширины канала
  * @param      bandwidth - значение ширины канала
  * @retval     None
  */
void setRadioBandWidth(uchar bandwidth) 
{
  if (bandwidth == 0) return;
  Radio.BandWidth = bandwidth;
  Radio.NumChannel = (BandData[Radio.Range - 1].MaxFreq 
                      - BandData[Radio.Range - 1].MinFreq) 
                      / BandWidth[bandwidth - 1];
  
  /**------------------- ВНИМАНИЕ! Ошибка в расчётах! ------------------------------
  При заданных радиоданных [SetRadioData(RANGE_433, BWIDTH_400, DEV_150, 9600L)]
  количество рабочих каналов 17 [0-16] */
  Radio.NumChannel = 17;
  /*-------------------------------------------------------------------------------*/
    
  RF.RXCREG.RXBW = bandwidth;
  RF.RXCREG.RXLNA = LNA_GAIN_0;
  RF.RXCREG.DRSSIT = RSSI_103;
  sendCommand_MRF49XA(RF.GENCREG.HalfWord);
}

/** 
  * @brief      Установка девиации передатчика
  * @param      dev - значение девиации
  * @retval     None
  */
void setRadioDeviation(uchar dev) 
{
  Radio.Deviation = dev;
  RF.TXCREG.MODBW = dev;
  sendCommand_MRF49XA(RF.TXCREG.HalfWord);
}

/** 
  * @brief      Установка выходной мощности передатчика
  * @param      pow - значение выходной мощности передатчика
  * @retval     None
  */
void setRadioPower(uchar pow) 
{
  RF.TXCREG.OTXPWR = pow;
  sendCommand_MRF49XA(RF.TXCREG.HalfWord);
}

/** 
  * @brief      Установка скорости передачи данных в радиоканале
  * @param      baudrate - значение скорости передачи данных
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
  * @brief      Установка канала
  * @param      channel - номер канала
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
  * @brief      Переключение на следующий канал
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
  * @brief      Установка радиоданных
  * @param      range - значение диапазона частот
  *             bandwidth - значение ширины канала
  *             dev - значение девиации
  *             baudrate - значение скорости передачи данных в радиоканале 
  * @retval     None
  */
void setRadioData(uchar range, uchar bandwidth, uchar dev, ulong baudrate) 
{
  /* Установка диапазона частот */
  setRadioRange(range);
  /* Установка ширины канала */
  setRadioBandWidth(bandwidth);
  /* Установка девиации передатчика */
  setRadioDeviation(dev);
  /* Установка скорости передачи данных в радиоканале */
  setRadioBaud(baudrate);
  // Установка канала 0
  //setRadioChannel(0);
}

/** 
  * @brief      Установка режима работы трансивера
  * @param      mode - режим работы трансивера
  *                     RECEIVE - приём
  *                     TRANSMIT - передача
  * @retval     None
  */
void setRadioMode(uchar mode) 
{
  switch (mode) 
  {
    case RECEIVE:
      /* Устанавливаем режим приёма */
      Radio.TransmitMode = 0;
      /* Выключаем передатчик и приёмник */
      sendCommand_MRF49XA(RF.PMCREG.HalfWord); 
      /* Сбрасывам FIFO */
      sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
      /* Разрешаем FIFO */
      sendCommand_MRF49XA(RF.GENCREG.HalfWord | B_FIFOEN);  // (GENCREG | 0x0040)
      /* Включаем приёмник */
      sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_RXCEN); // (PMCREG | 0x0080)
      /* Перезапускаем детектор сихропоследовательности */
      //sendCommand_MRF49XA(RF.FIFORSTREG.HalfWord);
      RF_SYNC_RESTART();
     /* FIFO Select: Selects the FIFO and the first bit appears on
      the next clock when reading the RXFIFOREG. This pin must be “high” when
      the TX register is enabled. In order to achieve minimum
      current consumption, keep this pin “high” in Sleep mode. */
      on(RFSEL);
      
      //off(LED_RF);
      //printf_Consol("RX MODE\r\n");
      break;
    
    case TRANSMIT:
      /* Устанавливаем режим передачи */
      Radio.TransmitMode = 1;   
      /* Выключили приёмник и передатчик */
      sendCommand_MRF49XA(RF.PMCREG.HalfWord);
      /* Запрещаем FIFO */
      sendCommand_MRF49XA(RF.GENCREG.HalfWord);
      /* Разрешили работу регистра передатчика 1-й байт преамбулы уже там */
      sendCommand_MRF49XA(RF.GENCREG.HalfWord | B_TXDEN);
      /* Включили передатчик - Enable Tx */
      sendCommand_MRF49XA(RF.PMCREG.HalfWord | B_TXCEN);
      //------- Дальше - передача по прерываниям -----------/
      
      //on(LED_RF);
      //printf_Consol("TX MODE\r\n");
      break;
  }
}

/** 
  * @brief      Задача обработки трансивера
  * @param      None
  * @retval     None
  */
void taskTransiver (void) 
{
#ifdef __RF_MASTER_MODE__   // MASTER  
  switch (Radio.TaskState) 
  {
    case IDLE_STATE:
      // Ничего не делаем, ждём...
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
      
      /* Посылаем пакет 1-го этапа авторизации */
      Radio.Rnd.Rnd1 = rnd();
      sendPacket_MRF49XA(cmdAUTORIZE1, &Radio.Rnd.Rnd1, 4, NOENCRIPT);
      /* Устанавливаем таймер на время ожидания ответа на вызов */
      Radio.ScanTimer = WAIT_ACK_TIME;
      Radio.TaskState = WAIT_STATE;
      break;
      
      case WAIT_STATE:
        /* Ждём ответа на выбранном канале */
        if (Radio.ScanTimer) break;
		if (Radio.Timer == 0) 
        { // Вызова не дождались, переключаем канал
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
        /* Состояние сессии */
		if (Radio.ScanTimer) break;
		printf_Consol("Session closed!!!\n\r");
        Radio.SessState = SESS_CLOSE;
		/* Время сессии истекло, переходим в состояние сканирования */
		// ==== DEBUG ====
        fl_SendSynchroRF = 0;       // Запрещаем синхропакеты
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
		// Ничего не делаем, используем текущий канал для приёма
		break;
        
      case SCAN_STATE:
        if (Radio.Timer == 0) 
        { // Переключаем канал
          if (!fl_SendSynchroRF) { // ==== DEBUG ====
            setRadioNextChannel();
          }                             
		}
		/* Устанавливаем таймер на время ожидания вызова */
		Radio.ScanTimer = SCAN_TIME;
		Radio.TaskState = WAIT_STATE;
        break;
	
      case WAIT_STATE:
        /* Ждём вызова на выбранном канале */
		if (Radio.ScanTimer) break;
		/* Вызова не дождались, следующий канал */
		Radio.TaskState = SCAN_STATE;
		break;
		
      case AUTORIZE_STATE:
        if (Radio.ScanTimer) break;
		Radio.SessState = SESS_CLOSE;
		/* Вызова не дождались, следующий канал */
		Radio.TaskState = SCAN_STATE;
		break;
		
      case SESSION_STATE:
        /* Состояние сессии */
        if (Radio.ScanTimer) break;
        Radio.SessState = SESS_CLOSE;
		Radio.TaskState = SCAN_STATE;
        break;
		
      default:
		Radio.TaskState = IDLE_STATE;
		break;
	}
#endif /* __RF_MASTER_MODE__ */

    /* Разрешаем приём пакетов из радиоканала */
    fl_packRF = 1;
	/* Проверка принятого пакета */
	//checkPacket_MRF49XA();

	/* Парсер принятых пакетов */
	parserPacket_MRF49XA();
    //================
}

/** 
  * @brief      Отправить пакет в радиоканал
  * @param      cmd - код команды
  *             *adr - указатель на поле данных пакета [DATA]
  *             size - размер поля данных в байтах
  *             enc - не шифровать данные (0)
  *                   или шифровать данные (1)  
  * @retval     SUCCESS (1) - пакет передан
  *             ERROR (0) - ошибка передачи
  */
ErrorStatus sendPacket_MRF49XA (uchar cmd, void *adr, uint size, uchar enc) 
{
  uint Crc;
  vu16 Tmp;
  TxPackHeader_TypeDef	*Pack = (TxPackHeader_TypeDef *) &Radio.TxBuf;
  ErrorStatus RetVal = SUCCESS;

  // Защита от повторной посылки пакета
  if(Radio.Transmit) { return ERROR; } 
  
  if (size > DATASIZE) { return ERROR; }

  /* Подготовка заголовка пакета */
  Pack->Preamble = PREAMBLE_2;		// Второй байт преамбулы, первый уже должен быть в регистре передачи
  //Pack->Syncro = SYNCHROSEQUENCE;	// Синхро последовательность
  Pack->Syncro1 = 0x2D;				// Синхро последовательность ByteL
  Pack->Syncro2 = 0xD4;				// Синхро последовательность ByteH
  Pack->Header.DSize = size;
  Pack->Header.SrcAddr = Radio.SelfAddr;
  Pack->Header.DstAddr = Radio.RemoteAddr;
  Pack->Header.Cmd = cmd;

  /* Вставляем данные в пакет */
  if (size != 0) { 
    memcpy(Pack->Header.Data, adr, size); 
	// Шифруем данные
    if (enc == ENCRIPT) {
      encryptData(Pack->Header.Data, size);
    }
  }
  
  /* Считаем CRC (от DSize до последнего байта Data) */
  Crc = MakeCRC_RF((uchar *)&Pack->Header, Pack->Header.DSize + sizeof(PackHeader_TypeDef));

  /* Вставлаем CRC в пакет - сразу после Data */
  *((uint *) (Pack->Header.Data + Pack->Header.DSize)) = Crc;

  /* Вставлаем второй байт преамбулы */
  *((uchar *) (Pack->Header.Data + Pack->Header.DSize + 2)) = 0xaa;

  /* Определяемся с длиной передаваемого пакета */
  Radio.TxCnt = 0;      // Количество переданных байт
  Radio.TxLen = size + sizeof(TxPackHeader_TypeDef) + 2 + 1;	// 2 [CRC] + 1 - байт для корректной передачи последнего байта пакета
  //printf_Consol("Radio.TxLen: .. %u\r\n", Radio.TxLen);

  /* Устанавливаем режим передачи */
  Radio.Transmit = 1;   // Пакет передаётся
  setRadioMode(TRANSMIT);
  
  /* ...и начинаем передачу */
  ENABLE_RF();    // chip select low - start the transmission
  delay_1ms(1);
  
  /* прерывание срабатывает - 1-й байт преамбулы уже в регистре
  ...работаем по прерываниям или Polling */
  
#ifndef __RFINT_ENABLED__   // Если прерывания от MRF49XA не включены
/*----------------- POLLING -----------------*/
  TimerRF = 100;
  while (Radio.TxCnt < Radio.TxLen)
  {
    getRadioStatus(CS_YES);  // Читаем значения регистров радиомодуля
    if (RF.STSREG.TXRXFIFO) 
    { // Регистр передатчика готов к приёму следующего байта для передачи
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
  // Все байты пакета переданы
  setRadioMode(RECEIVE);
/*-------------- END OF POLLING ------------*/
#else  
  // Ждём окончания передачи пакета
  delay_1ms(50);
  // После обработки прерываний проверяем все ли байты переданы
  if (Radio.TxCnt != Radio.TxLen) { RetVal = ERROR; }
#endif
  
  DISABLE_RF();     // chip select high - end of transmission
  return RetVal;
}

/** 
  * @brief      Получить пакет из радиоканала
  * @param      
  * @retval     RF_NODATA (0) - данные  не получены
  *             RF_DATA_RECEIVED (1) - получен байт данных, процесс продолжается
  *             RF_PACKET_RECEIVED (2) - получен пакет
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
  static uchar PacketLen = 0;   // не изменяются при выходе из функции
  static uchar NrBytes = 0;     
  uchar Bl;
  RFReceiveStatus RetVal;

  // Disable interrupts from Radio
  disableRFInterrupts();

  setRadioMode(RECEIVE);  
  ENABLE_RF();                // chip select low
  delay_1ms(1);
  
  getRadioStatus(CS_YES);  // Читаем значения регистра статуса
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
  * @brief      Отправить произвольный пакет в радиоканал
  * @param      *data - указатель на данные для отправки
  *             len - длина пакета
  * @retval     SUCCESS - данные успешно отправлены
  *             ERROR - ошибка отправки данных
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
  * @brief      Отправить пакет с кодом ошибки в радиоканал
  * @param      cmd - код команды, которая вызвала ошибку
  *             err - код ошибки
  * @retval     SUCCESS (1) - пакет передан
  *             ERROR (0) - ошибка передачи
  * @Note       Пакет не шифруется
  */
ErrorStatus sendError_MRF49XA(uchar cmd, uchar err)
{
	TError Error;
	
    Error.Cmd = cmd;
	Error.Code = err;
    return sendPacket_MRF49XA(cmdERROR, &Error, sizeof(TError), NOENCRIPT); 
}

/** 
  * @brief      Проверка открытой сессии
  * @param      cmd - код команды
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
  * @brief      Посылка пакета синхронизации в радиоканал  
  * @param      channel - номер канала для посылки пакетов синхронизации  
  *             sw - 1 разрешить переключение каналов
  *                  0 запретить переключение каналов
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
  
  /* Возвращаем всё взад */
  Radio.RemoteAddr = TmpAddr;
  if (channel != TmpChannel) 
  { setRadioChannel(TmpChannel); }
}

