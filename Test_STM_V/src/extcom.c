 
/* Includes ------------------------------------------------------------------*/
#include "extcom.h"
#include "includes.h"
#include "STM32vldiscovery.h"

/* Private variables ---------------------------------------------------------*/
uint TriesSMS = 0;
vu8 LenOfInputData;
vu8 LenOfReceivedData;

// ==== DEBUG ====
u32 ii;
u16 TmpV1;
u16 TmpV2;

/** 
  * @brief      Проверка принятого пакета из UART0 и UART1
  * @param      uart_number - (0 или 1) номер COM-порта
  * @retval     None
  */
void checkPacket_UARTx (uchar uart_number)
{
  uint Crc, Pcrc;
  uchar ErrCode = NO_ERROR;
  PackUart_TypeDef *Pack = (PackUart_TypeDef *) &UARTx[uart_number].InBuf;

  if (UARTx[uart_number].Request) return;       // Пакет принят, но ещё не обработан - возврат
  if (!UARTx[uart_number].RxComplete) return;   // Приём пакета не закончен
  
  // Если обращаются не к нам - выход
  if (Pack->DstAdr != MY_ADDRESS) {
    UARTx[uart_number].RxComplete = 0; // Сбрасываем признак окончания приема пакета            
    return;
  }

  // Сохраняем адрес отправителя (для ответного пакета)
  UARTx[uart_number].RemoteAdr = Pack->SrcAdr;
  
  // Проверяем CRC
  Crc = MakeCRC16(&Pack->DstAdr, Pack->Size + HEAD_PACKET);
  Pcrc = *((uint *) &Pack->Data[Pack->Size]);
  if (Crc != Pcrc) {
    ErrCode = ERR_BAD_CRC16;
    sendPacket_UARTx(uart_number, Pack->SrcAdr, CMD_ERROR, &ErrCode, 1);
    UARTx[uart_number].RxComplete = 0; // Сбрасываем признак окончания приема пакета            
	return;
  }
  
  // Получен корректный пакет
  UARTx[uart_number].Request = 1;
  // Получен корректный ответ от GPS терминала
  if (uart_number == UART_GPS) { fl_AnsCmdGPS = 1; }
  // Получено подтверждение об отсылке СМС
  if ((Pack->Cmd == CMD_SEND_SMS) && (Pack->Size == 0)) { fl_AckSMS = 1; }
}
u8 swith_command; //Состояния опроса состояний и ошибок Лира
/** 
  * @brief      Опрос и проверка состояний, ошибок Лира
  * @param      permit - (0 или 1) запрешение / разрешение опроса  
  * @retval     None
  */
void checkError(uchar permit_ch)
{
  if (permit_ch)
  {
    if(TimerStateCheck == 0)
    {
      TimerStateCheck = 500;    //Уст таймер перед следущим опросом
      if (swith_command != 4)swith_command++;  //swith_command 
      else  swith_command = 0;
    
  switch (swith_command)
  {
  case 0:
  sendPacket_UARTx(RS485,SUPPLYUNIT,STATUS_SM,NULL, 0);  //Запрос состояния модуля выравнивания   
  break;
  case 1:
  sendPacket_UARTx(RS485,SUPPLYUNIT,ERROR_SM,NULL, 0);  //Запрос кода ошибки модуля выравнивания   
  break;
  case 2:
  sendPacket_UARTx(RS485,SUPPLYUNIT,STATUS_CM,NULL, 0);  //Запрос состояния модуля обрезки   
  break;
  case 3:
  sendPacket_UARTx(RS485,SUPPLYUNIT,ERROR_CM,NULL, 0);  //Запрос кода ошибки модуля обрезки  
  break;
  case 4:
  sendPacket_UARTx(RS485,SUPPLYUNIT,ERROR_Code,NULL, 0);  //Запрос кода ошибки модуля обрезки  
  break;
  }
  }
  }
} 
/** 
  * @brief      Парсер принятых пакетов из UART
  * @param      uart_number - номер используемого UARTа: 
  *             0 => UART0 (USART1) 
  *             1 => UART1 (USART2)
  * @retval     None
  */
void parserPacket_UARTx(uchar uart_number)
{
  //uint Crc, Pcrc;
  uchar ErrCode = NO_ERROR;

  PackUart_TypeDef *Pack = (PackUart_TypeDef *) &UARTx[uart_number].InBuf;
  /*
  DataTime_TypeDef *DT = (DataTime_TypeDef *) &Pack->Data;
  TGPSStatus *StatusGPS = (TGPSStatus *) &Pack->Data;
  DataTime_TypeDef *DateTime = (DataTime_TypeDef *) &StatusGPS[1];  // Byte #1
  TLocation *Location = (TLocation *) &Pack->Data;
  uint32_t *Event_ID = (uint32_t *) &Pack->Data;
  TSMSInt *SMSInt = (TSMSInt *) &Pack->Data;
  TReqSetDT *SetDT = (TReqSetDT *) &Pack->Data;
  TSendSMS *SendSMS = (TSendSMS *) &Pack->Data;
  TReqCalibr *CalibPar = (TReqCalibr *) &Pack->Data;
  TReqSetParamsPPD *SetParamsPPD = (TReqSetParamsPPD *) &Pack->Data;
  TReqTSA *ReqTSA = (TReqTSA *) &Pack->Data;
  TVoidReq *VR = (TVoidReq *) &Pack->Data;
  TReqSetChannelPPD *SetCh = (TReqSetChannelPPD *) &Pack->Data; 
  TReqGetInfoDT *ReqGIDT = (TReqGetInfoDT *) &Pack->Data;
  TReqGetEvn *ReqGE = (TReqGetEvn *) &Pack->Data;
  
  ulong NumTSA = ReqTSA->TSA_Number;
  uint NumCh = SetCh->Channel;
  
  DataTime_TypeDef TmpDT;
  TRtcPPD _RTC;
  TParamsPPD ParamsPPD;
  TCalibrSens CS;
  TTranz TSA;
  u32 DevID;
  TEvent Event;
  TWebData WD;
  */
//  uint8_t *CodeErrorlira = (uint8_t *) &Pack->Data;
  if (!UARTx[uart_number].Request) return;  // Корректный пакет не принят - возврат
  
/* Анализ данных пакета ------------------------------------------------------*/ 
  switch (Pack->Cmd) {
    case ERROR_Code:   //Отладка
       if (Pack->Size != 1) { ErrCode = ERR_BAD_SIZE; break; }
       ErrCode = Pack->Data[0];  
 //     ErrCode = *CodeErrorlira;
      STM32vldiscovery_LEDOn(LED3);    
    break;
    case STATUS_SM:   // 
      
    break;
    case ERROR_SM:   // 
    break;
    case STATUS_CM:   // 
    break;    
    case ERROR_CM:   // 
    break;
    
     // ==== DEBUG ====
    /*
    // Проверка байтстафиинга  
    case 0x7e:
      sendPacket_UARTx(uart_number, Pack->SrcAdr, 0x7e, (uchar *)&Pack->Data, 6);
      break;
    */

    default:
      // Неизвестная команда или запрос
      ErrCode = ERR_BAD_COMMAND;
      break;
  }
  /* Пакет обработан, сбрасываем флаги */
  UARTx[uart_number].RxComplete = 0;
  UARTx[uart_number].Request = 0;
  if (ErrCode != NO_ERROR) { // Если была ошибка в команде/запросе
    if (Pack->Cmd >= 0xa0) { // Если это были запросы - необходим ответ об ошибке
 //     sendError_UARTx (uart_number, Pack->SrcAdr, cmdERROR, ErrCode);
 //     delay_1ms(5); // Ждём, чтобы пакет отправился
    }
    displayError(ErrCode);
    printf_Consol("Error! CMD = 0x%02x: Err = 0x%02x\r\n", Pack->Cmd, ErrCode);
    TimerForAll = 500;
    //while(TimerForAll) { blink(OUTPUTS_GPIO_PORT, LED_ERROR, 30, SND_ON); }
  }
}






