/**
  ******************************************************************************
  * @file    extcom.c
  * @author  Dmitri Kobylyaev aka [duke]
  * @version V 1.0.1
  * @date    OCT-2013
  * @brief   Functions for external communications of On-Board Controller
  ******************************************************************************
  * COPYRIGHT 2013 SDK-SYSTEMS ODESSA
  ******************************************************************************
  */
 
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
  * @brief      �������� ��������� ������ �� �����������
  * @param      None
  * @retval     None
  */
void checkPacket_MRF49XA(void) 
{
  uint Crc, Pcrc;
  PackHeader_TypeDef *Pack = (PackHeader_TypeDef *) &Radio.RxBuf;
  uchar ErrCode;
  
  _WDR();
  
  if (Radio.Request) return;            // ����� ������, �� ��� �� ��������� - �������
  if (!Radio.ReceiveComplete) return;   // �� �������� ���� ������ - �������
  if (!fl_packRF) return;

  // ��������� ����� �����������
  //Radio.RemoteAddr = Pack->SrcAddr;

  // ��������� CRC
  Crc = MakeCRC_RF((uchar *)Pack, Pack->DSize + sizeof(PackHeader_TypeDef));
  Pcrc = *((uint *)(Pack->Data + Pack->DSize));
  if (Crc != Pcrc) {
    // �������� ������� � ����� ������
    ErrCode = errBadCrc;
#ifndef __RF_MASTER_MODE__
    sendError_MRF49XA(Pack->Cmd, errBadCrc);
#endif    
	CLEAR_COMPL();
	RF_SYNC_RESTART();
    printf_Consol("Error packet MRF49XA 0x%02x\r\n", ErrCode); 
	return;
  }
  // �����, �����, �������. �� ����� ��� ������ � ������� ������� SCAN_WAIT_TIME
  Radio.Timer = SCAN_WAIT_TIME;
  // ��������� ����� �����������
      if ((Pack->SrcAddr < UIN(1)) || (Pack->SrcAddr > UIN(SysParams.QtyPPD))) 
      {
        ErrCode = ERR_BAD_SRCADR; 
#ifndef __RF_MASTER_MODE__
        // �������� ������� � ����� ������
        sendError_MRF49XA(Pack->Cmd, errExec);
#endif /* __RF_MASTER_MODE__ */
		CLEAR_COMPL();
		RF_SYNC_RESTART();
        printf_Consol("Error packet MRF49XA 0x%02x\n", ErrCode); 		
		return;
      }

      // ��������� ����� ���������� (��� �����)
	if (Pack->DstAddr != Radio.SelfAddr) {
      if (Pack->DstAddr == BROADCAST_ADDR) Radio.Broadcast = 1;
      else if (Pack->DstAddr == GROUPCAST_ADDR) Radio.Groupcast = 1; //????
      else {
        ErrCode = ERR_BAD_DSTADR;
#ifndef __RF_MASTER_MODE__
        // �������� ������� � ����� ������
        sendError_MRF49XA(Pack->Cmd, errExec);
#endif /* __RF_MASTER_MODE__ */
		CLEAR_COMPL();
		RF_SYNC_RESTART();
        printf_Consol("Error packet MRF49XA 0x%02x\n", ErrCode); 		
        return;
      }
	}
	
    // ������ ���������� �����
	Radio.Request = 1;
    fl_AnsCmdPPD = 1;
}

/** 
  * @brief      ������ �������� ������� �� ���-������ MRF49XA
  * @param      None
  * @retval     None
  */
void parserPacket_MRF49XA (void) 
{
#ifdef __RF_MASTER_MODE__ 
  union {
	Rnd_TypeDef     Rnd;
	AutRF_TypeDef   *Aut;
  };
  char String[16];
  uchar ErrCode = NO_ERROR;
  PackHeader_TypeDef *Pack = (PackHeader_TypeDef *) &Radio.RxBuf;
  TError *Err = (TError *) Pack->Data;
  
// ==== DEBUG ====
  //if (!Radio.ReceiveComplete) return;   // �� �������� ���� ������ - �������
  
  if (!Radio.Request) return;           // �� ������ ���������� ����� - �������

  // �������� ���������� ����� ������� ��� ������ � �������
  sprintf(String, "UIN: 0x%02x\n", Pack->SrcAddr);
  
  // ������������� ������ �� ������������ �������� � ������
  Radio.ScanTimer = SESSION_TIME;
  //printf_Consol("�������� ����� �� ������� 0x%02x\r\n", Pack->Cmd);
  
  // ������������ ������ ��� ����� �� �������
  switch (Pack->Cmd) {
    case cmdAUTORIZE1:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (Pack->DSize != sizeof(AutRF_TypeDef)) {
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE;
		break;
      }
      printf_Consol("AUTORIZE1: SUCCESS!\r");
      Aut = (AutRF_TypeDef *) &Pack->Data;
      Radio.Rnd.Rnd2 = Aut->Rnd2;
      // ��������� ���� ������ �� ��������� RND � ������ RND �� ������ ����� ����������
      memcpy(SysStat.KSES, Radio.Rnd.Buf, 8);
      encryptData(SysStat.KSES, 8);
      // ������� (RND1 | RND2) ������������������ ������ KSES � ����������
      memcpy(Rnd.Buf, Radio.Rnd.Buf, 8);
      encryptData(Rnd.Buf, 8);
      Radio.SessState = SESS_AUTORIZE;
      // ������������� ������ �� ������������ �������� � �����������
      Radio.ScanTimer = AUTORIZE_TIME;
      Radio.TaskState = AUTORIZE_STATE;
      // Send cmdAUTORIZE2
      sendPacket_MRF49XA(cmdAUTORIZE2, Rnd.Buf, 8, NOENCRIPT);
      break;

    case cmdAUTORIZE2:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (Pack->DSize != 8) {
        Radio.SessState = SESS_CLOSE;
		Radio.TaskState = SCAN_STATE;
        ErrCode = ERR_BAD_SIZE;
        break;
      }
      if (Radio.SessState != SESS_AUTORIZE) {
        Radio.SessState = SESS_CLOSE;
		Radio.TaskState = SCAN_STATE;
        break;
      }
      // ���������  �������� coded(RND2 | RND1) ������������������ ������ KSES � ����������
      decryptData(Pack->Data, 8);
      Rnd.Rnd1 = Radio.Rnd.Rnd2;
      Rnd.Rnd2 = Radio.Rnd.Rnd1;
      if (memcmp(Pack->Data, Rnd.Buf, 8) != 0) {
        // �� �������
        Radio.ScanTimer = WAIT_ACK_TIME;
        Radio.TaskState = WAIT_STATE;
        Radio.SessState = SESS_CLOSE;
        printf_Consol("AUTORIZE2: FAILED! �� ������� �����!\r");
        break;
      }
      printf_Consol("AUTORIZE2: SUCCESS!\r\n");
      // �������� ������ PING
      sendPacket_MRF49XA(cmdPING_REQUEST, NULL, 0, NOENCRIPT); 
      Radio.SessState = SESS_OPEN;
      Radio.ScanTimer = SESSION_TIME;
      Radio.TaskState = SESSION_STATE;
      break;

    case cmdPING_CONFIRM:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != sizeof(TStatusPDD)) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      decryptData(Pack->Data, Pack->DSize);
      /* ���������� ������ � ��������� ������� ������� */
      memcpy(&StatusPPD, &Pack->Data, sizeof(TStatusPDD));
      printf_Consol("Get answer to cmdPING_REQUEST!\r\n");
      fl_PPDPingConfirm = 1;
      break;
    
    case cmdREAD_LTSA:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != sizeof(TTranz)) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Got Last Transaction: \r\n");
      break;
      
    case cmdREAD_STSA:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != sizeof(TTranz)) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Got Special Transaction: \r\n");
      break;
      
    case cmdGET_DATETIME:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != sizeof(TRtcPPD)) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Got DateTime: \r\n");
      break;
      
    case cmdSET_DATETIME:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Set DateTime - OK! \r\n");
      break;

    case cmdCONFIG_PDD:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Set Params PPD - OK! \r\n");
      break;
      
    case cmdGETNUM:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 4) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Got DeviceID: 0x%x\r\n", *(u32 *)Pack->Data);
      break;
      
    case cmdCALIBRATE:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Calibrate Sensors - OK! \r\n");
      break;
        
    case cmdREDUCE_PWR:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Reduce Power - OK!\r\n");
      break;
      
    case cmdCLEAR_POINTERS:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Clear Pointers - OK!\r\n");
      break;
      
    case cmdGET_PARAMS:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != sizeof(TParamsPPD)) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Got PPD Params:\r\n");
      break;

    case cmdSWITCH_SLEEP:       // ����������� ������ � ����� ��� ��� ��������� ����� ���
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Change Sleep Mode - OK! \r\n");
      break;

    case cmdSET_CHANNEL:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Set Channel - OK!\r\n");
      break;

    case cmdENABLE_SLEEP:       // ����������� ������ � ����� ��� 
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Enable Sleep Mode - OK! \r\n");
      break;
      
    case cmdDISABLE_SLEEP:       // ��������� ����� ��� 
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      fl_AckAwake = 1;
      printf_Consol("Disable Sleep Mode - OK! \r\n");
      break;
      
    case cmdRESET:       // ������������� ������ 
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Reset PPD - OK! \r\n");
      break;

    case cmdCLOSE_SESS:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != 0) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      printf_Consol("Get answer to cmdCLOSE_SESS!\r\n");
      Radio.SessState = SESS_CLOSE;
      Radio.TaskState = SCAN_STATE;
      break;
      
    case cmdDEBUG:
      printf_Consol("DEBUG received byte: 0x%x\r\n", *(u8 *)Pack->Data);
      break;

    case cmdERROR:
      if (Radio.Broadcast || Radio.Groupcast) break;
      if (isNoOpenSess(Pack->Cmd)) break;
      if (Pack->DSize != sizeof(TError)) { 
        Radio.ScanTimer = WAIT_ACK_TIME;
		Radio.TaskState = WAIT_STATE;
        ErrCode = ERR_BAD_SIZE; 
        break; 
      }
      ErrCode = ERR_FROM_PPD;
      break;

    default: // ����������� �������
      Radio.ScanTimer = WAIT_ACK_TIME;
      Radio.TaskState = WAIT_STATE;
      ErrCode = ERR_BAD_COMMAND;
      fl_AnsCmdPPD = 0; // ����� �� ������� �� �������!
      break;
  }
  // ����� ���������, ���������� �����
  Radio.Request = 0;
  CLEAR_COMPL();
  RF_SYNC_RESTART();
  if (ErrCode != NO_ERROR) {
    /* ���� ���� ������ � ������ ��� ������ ����� � ����� ������ */
    Radio.TaskState = IDLE_STATE;
    displayError(ErrCode);
    if(ErrCode == ERR_FROM_PPD) {
      fl_ErrPPD = 1;
      printf_Consol((const char *)"Error CMD_RF: Cmd = 0x%02x, Err = 0x%02x! %s\r\n", 
                    (u8)Err->Cmd, (u8)Err->Code, String);
    }
    // ==== DEBUG ====
    NumsOfPackets++;
  }
#else /* __RF_MASTER_MODE__ */
#endif /* __RF_MASTER_MODE__ */
}

/** 
  * @brief      �������� ��������� ������ �� UART0 � UART1
  * @param      uart_number - (0 ��� 1) ����� COM-�����
  * @retval     None
  */
void checkPacket_UARTx (uchar uart_number)
{
  uint Crc, Pcrc;
  uchar ErrCode = NO_ERROR;
  PackUart_TypeDef *Pack = (PackUart_TypeDef *) &UARTx[uart_number].InBuf;

  if (UARTx[uart_number].Request) return;       // ����� ������, �� ��� �� ��������� - �������
  if (!UARTx[uart_number].RxComplete) return;   // ���� ������ �� ��������
  
  // ���� ���������� �� � ��� - �����
  if (Pack->DstAdr != MY_ADDRESS) {
    UARTx[uart_number].RxComplete = 0; // ���������� ������� ��������� ������ ������            
    return;
  }

  // ��������� ����� ����������� (��� ��������� ������)
  UARTx[uart_number].RemoteAdr = Pack->SrcAdr;
  
  // ��������� CRC
  Crc = MakeCRC16(&Pack->DstAdr, Pack->Size + HEAD_PACKET);
  Pcrc = *((uint *) &Pack->Data[Pack->Size]);
  if (Crc != Pcrc) {
    ErrCode = ERR_BAD_CRC16;
    sendPacket_UARTx(uart_number, Pack->SrcAdr, CMD_ERROR, &ErrCode, 1);
    UARTx[uart_number].RxComplete = 0; // ���������� ������� ��������� ������ ������            
	return;
  }
  
  // ������� ���������� �����
  UARTx[uart_number].Request = 1;
  // ������� ���������� ����� �� GPS ���������
  if (uart_number == UART_GPS) { fl_AnsCmdGPS = 1; }
  // �������� ������������� �� ������� ���
  if ((Pack->Cmd == CMD_SEND_SMS) && (Pack->Size == 0)) { fl_AckSMS = 1; }
}

/** 
  * @brief      ������ �������� ������� �� UART
  * @param      uart_number - ����� ������������� UART�: 
  *             0 => UART0 (USART1) 
  *             1 => UART1 (USART2)
  * @retval     None
  */
void parserPacket_UARTx(uchar uart_number)
{
  //uint Crc, Pcrc;
  uchar ErrCode = NO_ERROR;

  PackUart_TypeDef *Pack = (PackUart_TypeDef *) &UARTx[uart_number].InBuf;
  DataTime_TypeDef *DT = (DataTime_TypeDef *) &->;
  //TGPSStatus *StatusGPS = (TGPSStatus *) &Pack->Data;
  //DataTime_TypeDef *DateTime = (DataTime_TypeDef *) &StatusGPS[1];  // Byte #1
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

  if (!UARTx[uart_number].Request) return;  // ���������� ����� �� ������ - �������
  
/* ������ ������ ������ ------------------------------------------------------*/ 
  switch (Pack->Cmd) {
    case 102:                           //������� 
      STM32vldiscovery_LEDOn(LED3);    
    break;
    case CMD_GETSTATE_GPS:  // ����� �� ������� "������ ������� GPS ���������"
      if (Pack->Size != sizeof(TGPSStatus)) { ErrCode = ERR_BAD_SIZE; break; }
      // ====DEBUG GPS====
      //viewDump_Consol(8, (uchar *)StatusGPS, sizeof(TGPSStatus));
      break;
      
    case CMD_GET_LOCATION:  // ����� �� ������� "�������� ���������� �������� ��������������"
      if (Pack->Size != sizeof(TLocation)) { ErrCode = ERR_BAD_SIZE; break; }
      convertLocM2M(&CurrLoc, Location);
      printf_Consol((const char *) "���������� �������� ��������������:\r\n");
      printf_Consol((const char *) "%02d�%02d'%02d.%02d\"N, ", 
          CurrLoc.Lat_Degree, CurrLoc.Lat_Min, CurrLoc.Lat_Sec, CurrLoc.Lat_FrSec);
      printf_Consol((const char *) "%d%02d�%02d'%02d.%02d\"E\r\n", 
          CurrLoc.Lon_Degree1, CurrLoc.Lon_Degree2, CurrLoc.Lon_Min, CurrLoc.Lon_Sec, CurrLoc.Lon_FrSec);
      break;
      
    case CMD_SEND_SMS:  // ����� �� ������� "�������� SMS"
      if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      printf_Consol("SMS ������� � �������!\n");
      break; 
      
    case CMD_GET_DATETIME:  // ����� �� ������� "�������� ���� � ����� GPS-���������"
      if (Pack->Size != sizeof (DataTime_TypeDef)) { ErrCode = ERR_BAD_SIZE; break; }
      printf_Consol("�������� ����� ���� � �����:");
      printf_Consol((const char *) "Date: %02d-%02d-20%02d\n", 
                    DT->Day, DT->Month, DT->Year);
      printf_Consol((const char *) "Time: %02d:%02d:%02d\n", 
                    DT->Hour, DT->Min, DT->Sec);
      break;
    
  // ----------- ������� ��������� ����������� �� ������ ��������� -------------  
  case REQ_GET_LAST_EVN:  // ����� �� ������ "�������� ��������� ������� �� ������� ���������"
      printf_Consol("������� ������ REQ_GET_LAST_EVN\n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      if (fl_SendingSMS)    { ErrCode = ERR_NOTSENT_SMS; break; } // ���������� SMS ��� �� ��������
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      /*
        ����� ���� ����������� ����������� ����� ������� 
        ������� �� ��������� SMS ���������, 
        � ���� ������ �������� ���������� ������ ���������� ������������� ������� 
        �� Flash ������, 
        �������� ��������� �� ������ �� ���� ������� � 
        ���������������� ���������� ������������ �������
      */
      if(countQtyEvents() == 0) {
        ErrCode = ERR_NO_EVENTS;
        break;
      }
      if(getEvent(&CurrEvent, SysParams.IndexRead, MOVE) == ERROR) {
        ErrCode = ERR_READ_DF;
        break;
      }
      ErrCode = handler_SendSMS(UART_GPS, GPSTERM, Pack->Cmd, (uchar *)&CurrEvent);
      break;
      
    case REQ_GET_SPEC_EVN:  // ����� �� ������ "�������� ������ ������������ �������"
      printf_Consol("������� ������ REQ_GET_SPEC_EVN/n");
      if (Pack->Size != 4)  { ErrCode = ERR_BAD_SIZE; break; }
      if (fl_SendingSMS)    { ErrCode = ERR_NOTSENT_SMS; break; } // ���������� SMS ��� �� ��������
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      /*
        ����� ���� ����������� ����������� ����� ������� 
        ������� �� ��������� SMS ���������, 
        � ���� ������ �������� ���������� ������ ���������� ������������� ������� 
        �� Flash ������, 
        ��������� �� ������ � ���������� ������������ ������� �� ��������������.
      */
      if(getEvent(&CurrEvent, *Event_ID, STAY) == ERROR) {
        ErrCode = ERR_READ_DF;
        break;
      }
      ErrCode = handler_SendSMS(UART_GPS, GPSTERM, Pack->Cmd, (uchar *)&CurrEvent);
      break;
      
    case REQ_QTY_EVN:   // ����� �� ������ "�������� ���������� ������������ �������"
      printf_Consol("������� ������ REQ_QTY_EVN/n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      if (fl_SendingSMS)    { ErrCode = ERR_NOTSENT_SMS; break; } // ���������� SMS ��� �� ��������
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      /*
       ������� ����������� ����� ������� ������� �� ��������� SMS ��������� (CMD_SEND_SMS),
       � ���� ������ �������� ���������� �������� ���������� ������������ ������� 
      */
      SysStat.NonReadEvents = countQtyEvents();
      ErrCode = handler_SendSMS(UART_GPS, GPSTERM, Pack->Cmd, (uchar *)&SysStat.NonReadEvents);
      break;
      
    case REQ_CLEAR_HIS: // ����� �� ������ "�������� ������� �������"
      printf_Consol("������� ������ REQ_CLEAR_HIS/n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      /* �������� ��������� ������ � ������ ������� */
      if (clearHistory() == ERROR) { ErrCode = ERR_EXEC; break;}
      break;
      
    case REQ_GET_DATETIME:  // ����� �� ������ "�������� ������� ���� � �����"
      printf_Consol("������� ������ REQ_GET_DATETIME/n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      if (fl_SendingSMS)    { ErrCode = ERR_NOTSENT_SMS; break; } // ���������� SMS ��� �� ��������
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      /* �������� � SMS ������� ���� � ����� */
      if (getDateTime(&SysStat.DataTime) == ERROR) { ErrCode = ERR_GET_DT; break; }
      ErrCode = handler_SendSMS(UART_GPS, GPSTERM, Pack->Cmd, (uchar *)&SysStat.DataTime);
      break;
      
    case REQ_SET_DATETIME: // ����� �� ������ "���������� ���� � �����"
      printf_Consol("������� ������ REQ_SET_DATETIME/n");
      if (Pack->Size != sizeof (DataTime_TypeDef))  { ErrCode = ERR_BAD_SIZE; break; }
      /* ���������� ����� ���� � ����� */
      // ������������ �������� �������, �.�. GPS �������� "�������" �� ��������� ������� �� 3 ����
      if (DT->Hour < 21) { DT->Hour += 3; };
      if (setDateTime(*DT) == ERROR) { ErrCode = ERR_SET_DT; break; }
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      break;

    case REQ_SET_SMSINT: // ����� �� ������ "���������� ��������� �������� ������� SMS"
      printf_Consol("������� ������ REQ_SET_SMSINT/n");
      if (Pack->Size != sizeof (TSMSInt))  { ErrCode = ERR_BAD_SIZE; break; }
      if (fl_SendingSMS) { ErrCode = ERR_NOTSENT_SMS; break; } // ���������� SMS ��� �� ��������
      /* ��������������� ����� �������� �������������� ������� SMS ��������� 
      �� �������� �����. ���� �������� CelNumber ����� "\n", ����� ������ 
      ������������ �� ������. ���� �������� SMS_Interval ����� 0, 
      �� SMS ��������� � �������� ������ ������� ������������� �� ������������ */
      strcpy(SysParams.CelNumber, (const char *)SMSInt->CelNumber);
      SysParams.SMSInt = (uint32_t)SMSInt->SMS_Interval;
      if ((setData((uchar *)SysParams.CelNumber, ADR_CELNUMBER, 32) == ERROR) ||
          (setData((uchar *)&SysParams.SMSInt, ADR_SMSINT, 4) == ERROR)) {
        ErrCode = ERR_WRITE_FLASH;
        break;
      }
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      break;
      
    case REQ_GET_STATE: // ����� �� ������ "�������� ��������� �����������"
      printf_Consol("������� ������ REQ_GET_STATE/n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      if (fl_SendingSMS) { ErrCode = ERR_NOTSENT_SMS; break; } // ���������� SMS ��� �� ��������
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      // �������� ����� ������ �������
      memset((uchar *)&SysStat.Errors, 0, 4);   
      // ������ �� ������ ��������� ���������� �������
      if(SysParams.IndexWrite != 0) {
        if(getEvent(&CurrEvent, SysParams.IndexWrite - 1, STAY) == ERROR) {
          ErrCode = ERR_READ_DF;
          SysStat.Errors.ErrorsExist = 1;
          SysStat.Errors.ErrGetEvent = 1;
          break;
        }
      }
      // 1) ������ ��
      SysStat.Version = SysParams.BitVars.Version;
      // 2) ��������� ���� � �����
      if(getDateTime(&SysStat.DataTime) == ERROR) { 
        ErrCode = ERR_GET_DT;
        SysStat.Errors.ErrorsExist = 1; 
        SysStat.Errors.ErrSetDT = 1;
        break; 
      }
      // 3) ���������� ��������������
      /* ... */
      // 4) ���-�� ������� ����
      SysStat.OccupSeats = CurrEvent.OccupSeats;
      // 5) ���-�� ��������� ����
      SysStat.FreeSeats = CurrEvent.FreeSeats;
      // 6) ���-�� ��������� ��������
      SysStat.GoodPPD = SysParams.QtyPPD - CurrEvent.FailedPPD;
      // 7) ���-�� ����������� ��������
      SysStat.FailedPPD = CurrEvent.FailedPPD;
      // 8) ���-�� ������������ �������
      SysStat.NonReadEvents = countQtyEvents();
      // 9) ���� ������, ����� ����������� �����������, ����� �������� ���������� � ��������
      /* ...��� ���� */ 
      /* �������� � SMS ������ ��������� �������  */
      //ErrCode = handler_SendSMS(UART_GPS, GPSTERM, Pack->Cmd, (uchar *)&SysStat);
      break;
      
    case REQ_EXT_IRQ: // ����� �� ������ "���������� �� �������� ����������"
      printf_Consol("������� ������ REQ_EXT_IRQ/n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      break;

    case CMD_ERROR:    // ������ ������� � �������
      break;
    
    case REQ_GET_SEATS_INFO:    // �������� �� DriverMonitor ���������� � ��������
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ���������� ����� �� ������� �������� ������� */
      //sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      fl_ShowSeats = 1;
      break;

    case REQ_AWAKE_PPD:         // ������� ��� ������� �� ��������� "�������� ���"
                                // � �������� �� DriverMonitor ���������� � ��������
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ���������� ����� �� ������� �������� ������� */
      //sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      /* ���������� ������ ��� ����������� ��������� �������: */
      // C������ ����� ���������� �������� �� ����� 
      ErrCode = handler_CountPPD(SysParams.QtyPPD, WAKE);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("Alive: %u, Dead: %u\r\n", (u16)SysStat.GoodPPD, (u16)SysStat.FailedPPD);
      }
      // ���������� ���� � �������� �� DriveMonitor 
      /* ...���� DriveMonitor ��������� � UART1 */
      //ErrCode = handler_giveCmdGPS(cmdDISPL_SEAT, (uchar *)&IS[1], NULL);             
      /* ...���� DriveMonitor ��������� � UART0 */
      sendPacket_UARTx(uart_number, Pack->SrcAdr, cmdDISPL_SEAT, (uchar *)&IS[1], sizeof(TInfoSeats));   
      break;
      
    case REQ_REINIT_RF:         // �������� ����������������� �����������
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ���������� ����� �� ������� �������� ������� */
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      init_MRF49XA();
      break;
      
    // ������ ���������� � ��������� ������� �� ������ ���������� �������
    case REQ_GET_INFO_DT:
      if (Pack->Size != sizeof(TReqGetInfoDT))  { ErrCode = ERR_BAD_SIZE; break; }
      if (ReqGIDT->Board != SysParams.BoardNumber) { ErrCode = ERR_BADPARAM; break; }
      printf_Consol("������� ������ REQ_GET_INFO_DT\r\n");
      ErrCode = handler_GetInfoDT(ReqGIDT);
      if (ErrCode == NO_ERROR) {
        /*
        printf_Consol("Board: %4d, Start: %02d/%02d/20%02d %02d:%02d:%02d, End: %02d/%02d/20%02d %02d:%02d:%02d, StartID: %d, EndID: %d, Stops: %d, Time: %d[s], Dist: %d[m], Passengers: %d, PasTime: %d[p*s], PasDist: %d[p*m]\r\n", 
                      AIDT.Board, AIDT.DT_StartEvn.Day, AIDT.DT_StartEvn.Month, AIDT.DT_StartEvn.Year, 
                      AIDT.DT_StartEvn.Hour, AIDT.DT_StartEvn.Min, AIDT.DT_StartEvn.Sec,      
                      AIDT.DT_EndEvn.Day, AIDT.DT_EndEvn.Month, AIDT.DT_EndEvn.Year, 
                      AIDT.DT_EndEvn.Hour, AIDT.DT_EndEvn.Min, AIDT.DT_EndEvn.Sec,
                      AIDT.ID_StartEvn, AIDT.ID_EndEvn, AIDT.QtyStops, 
                      (u32)AIDT.Time, (u32)AIDT.Distance, (u32)AIDT.Passengers,
                      (u32)AIDT.TotalPasTime, (u32)AIDT.TotalPasDistance);
        */
        sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&AIDT, sizeof(TAnswerInfoDT));
      }
      break;
      
    case REQ_GET_EVENT:
      if (Pack->Size != sizeof(TReqGetEvn))  { ErrCode = ERR_BAD_SIZE; break; }
      if (ReqGE->Board != SysParams.BoardNumber) { ErrCode = ERR_BADPARAM; break; }
      printf_Consol("������� ������ REQ_GET_EVENT\r\n");
      if(getEvent(&Event, ReqGE->NumEvn, STAY) == ERROR) {
        ErrCode = ERR_READ_DF;
        break;
      }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&Event, sizeof(TEvent));
      break;

    case REQ_GET_WEB_DATA: // ����� �� ������ "�������� ������ ��� Web-�������"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      // ������ �� ������ ��������� ���������� �������
      if(SysParams.IndexWrite != 0) {
        if(getEvent(&CurrEvent, SysParams.IndexWrite - 1, STAY) == ERROR) {
          ErrCode = ERR_READ_DF;
          SysStat.Errors.ErrorsExist = 1;
          SysStat.Errors.ErrGetEvent = 1;
          break;
        }
        // ��������� ������ ��� ����������� �� Web-�������
        WD.ID = (uint)CurrEvent.ID;
        memcpy(&WD.DataTime, &CurrEvent.DataTime, sizeof(DataTime_TypeDef));
        memcpy(&WD.Coordinates, &CurrEvent.Coordinates, sizeof(TGoogleGPS));
        WD.OccupSeats = CurrEvent.OccupSeats;
        WD.FreeSeats = CurrEvent.FreeSeats;
        WD.FailedPPD = CurrEvent.FailedPPD;
        WD.WorkRadioCh = CurrEvent.IS.Ch;
        WD.Speed = CurrEvent.Speed;
        WD.Flags = CurrEvent.Flags;
        WD.Reserve_1 = WD.Reserve_2 = 0;
        WD.Reserve_34 = WD.Reserve_56 = 0;
        // �������� ����� � �������
        sendPacket_UARTx(uart_number, Pack->SrcAdr, REQ_GET_WEB_DATA, (uchar *)&WD, sizeof(TWebData));
      }
      else { ErrCode = ERR_NODATA; break; }  
      break;
      
//----- ������� �� ������������ ���������� ��� ������� -------------------------
    case REQ_PPD_PING_REQUEST:  // �������� ��������� PPD
      printf_Consol("������� ������ REQ_PPD_PING_REQUEST\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdPING_REQUEST, NULL, &StatusPPD, NOENCRIPT, VR->CheckNet);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("FlagsWord: 0x%02x (Rtc:%1x; Sensor:%1x; DataFlash:%1x; Battery:%1x; Passenger1:%1x; Passenger2:%1x; EnableSleep:%1x), DevId: 0x%08x, DateTime: %02u/%02u/20%02u-%02u:%02u:%02u, Qty_NonreadTSA: %u, Qty_Index: %u, C1: %u, C2: %u\r\n",
                      (u8)StatusPPD.FlagsWord, 
                      (u8)StatusPPD.Rtc, (u8)StatusPPD.Sensor, (u8)StatusPPD.DataFlash,
                      (u8)StatusPPD.Battery, (u8)StatusPPD.Passenger1, (u8)StatusPPD.Passenger2, (u8)StatusPPD.EnableSleep,
                      (u32)StatusPPD.DevId, (u8)StatusPPD.DateTime.Day,
                      (u8)StatusPPD.DateTime.Month, (u8)StatusPPD.DateTime.Year, (u8)StatusPPD.DateTime.Hour,
                      (u8)StatusPPD.DateTime.Min, (u8)StatusPPD.DateTime.Sec, (u32)StatusPPD.Qty_NonreadTSA,
                      (u32)StatusPPD.Qty_Index, (u16)StatusPPD.C1, (u16)StatusPPD.C2);
      }
      break;

    case REQ_PPD_READ_LTSA:     // ��������� ��������� ���������� PPD
      printf_Consol("������� ������ REQ_PPD_READ_LTSA\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdREAD_LTSA, NULL, &TSA, NOENCRIPT, VR->CheckNet);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("Id: %u, Passenger1: %01x, Passenger2: %01x, DateTime: %02u/%02u/20%02u-%02u:%02u:%02u, C1: %u, C2: %u\r\n",
                      (u32)TSA.Id, (u8)TSA.Passenger1, (u8)TSA.Passenger2,
                      (u8)TSA.Dt.Day, (u8)TSA.Dt.Month, (u8)TSA.Dt.Year, 
                      (u8)TSA.Dt.Hour, (u8)TSA.Dt.Min, (u8)TSA.Dt.Sec,
                      (u16)TSA.C1, (u16)TSA.C2);
      }
      break;

    case REQ_PPD_READ_STSA:     // ��������� ����������� ���������� PPD
      printf_Consol("������� ������ REQ_PPD_READ_STSA\n");
      if (Pack->Size != sizeof(TReqTSA))  { ErrCode = ERR_BAD_SIZE; break; } // uchar + ulong
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(ReqTSA->VReq.NumPPD, cmdREAD_STSA, &NumTSA, &TSA, NOENCRIPT, ReqTSA->VReq.CheckNet);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("Id: %u, Passenger1: %01x, Passenger2: %01x, DateTime: %02u/%02u/20%02u-%02u:%02u:%02u, C1: %u, C2: %u\r\n",
                      (u32)TSA.Id, (u8)TSA.Passenger1, (u8)TSA.Passenger2,
                      (u8)TSA.Dt.Day, (u8)TSA.Dt.Month, (u8)TSA.Dt.Year, 
                      (u8)TSA.Dt.Hour, (u8)TSA.Dt.Min, (u8)TSA.Dt.Sec,
                      (u16)TSA.C1, (u16)TSA.C2);
      }
      break;
      
    case REQ_PPD_SET_DATATIME:  // ���������� ���� � ����� � PPD
      printf_Consol("������� ������ REQ_PPD_SET_DATATIME\r\n");
      if (Pack->Size != sizeof(TReqSetDT))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      _RTC.Day = SetDT->DT.Day;
      _RTC.Month = SetDT->DT.Month;
      _RTC.Year = SetDT->DT.Year;
      _RTC.Hour = SetDT->DT.Hour;
      _RTC.Min = SetDT->DT.Min;
      _RTC.Sec = SetDT->DT.Sec;
      ErrCode = handler_giveCmdPPD(SetDT->VReq.NumPPD, cmdSET_DATETIME, &_RTC, 
                                   NULL, SysParams.BitVars.EncryptRF, SetDT->VReq.CheckNet);
      break;

    case REQ_PPD_GETUIN:        // ��������� ���������� ����� PPD
      printf_Consol("������� ������ REQ_PPD_GETUIN\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdGETNUM, NULL, &DevID, NOENCRIPT, VR->CheckNet);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("PPD: %u, ID: 0x%x\r\n", VR->NumPPD, DevID);
      }
      break;
      
    case REQ_PPD_REDUCE_PWR:     
      printf_Consol("������� ������ REQ_PPD_REDUCE_PWR\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdREDUCE_PWR, NULL, NULL, NOENCRIPT, VR->CheckNet);
      break;
        
    case REQ_PPD_GET_DT:     
      printf_Consol("������� ������ REQ_PPD_GET_DT\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdGET_DATETIME, NULL, &_RTC, NOENCRIPT, VR->CheckNet);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("Date: %02u/%02u/20%02u,\rTime: %02u:%02u:%02u\r\n",
                      (u8)_RTC.Day, (u8)_RTC.Month, (u8)_RTC.Year,
                      (u8)_RTC.Hour, (u8)_RTC.Min, (u8)_RTC.Sec);
      }
      break;

    case REQ_PPD_GET_PARAMS:     
      printf_Consol("������� ������ REQ_PPD_GET_PARAMS\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdGET_PARAMS, NULL, &ParamsPPD, NOENCRIPT, VR->CheckNet);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("SelfAddr: 0x%04x, RemoteAddr: 0x%04x, Sensitivity1: %u, Sensitivity2: %u, Tresh1: %u, Tresh2: %u, SensTime1: %u, SensTime2: %u, BatteryTresh: %u, Power: %u\r\n",
                      (u16)ParamsPPD.SelfAddr, (u16)ParamsPPD.RemoteAddr, (u16)ParamsPPD.Sensitivity1,
                      (u16)ParamsPPD.Sensitivity2, (u16)ParamsPPD.Threshold1, (u16)ParamsPPD.Threshold2, 
                      (u16)ParamsPPD.SensTime1, (u16)ParamsPPD.SensTime2,
                      (u16)ParamsPPD.BatteryTresh, (u8)ParamsPPD.TransmitPower);
      }
      break;
      
    case REQ_PPD_SET_PARAMS:  
      printf_Consol("REQ_PPD_SET_PARAMS\r\n");
      if (Pack->Size != sizeof(TReqSetParamsPPD))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ParamsPPD.SelfAddr = SetParamsPPD->Params.SelfAddr;
      ParamsPPD.RemoteAddr = SetParamsPPD->Params.RemoteAddr;
      ParamsPPD.Sensitivity1 = SetParamsPPD->Params.Sensitivity1;
      ParamsPPD.Sensitivity2 = SetParamsPPD->Params.Sensitivity2;
      ParamsPPD.Sensitivity1 = SetParamsPPD->Params.Sensitivity1;
      ParamsPPD.Threshold1 = SetParamsPPD->Params.Threshold1;
      ParamsPPD.Threshold2 = SetParamsPPD->Params.Threshold2;
      ParamsPPD.SensTime1 = SetParamsPPD->Params.SensTime1;
      ParamsPPD.SensTime2 = SetParamsPPD->Params.SensTime2;
      ParamsPPD.BatteryTresh = SetParamsPPD->Params.BatteryTresh;
      ParamsPPD.TransmitPower = SetParamsPPD->Params.TransmitPower;
      ParamsPPD.Crc = MakeCRC_RF((uchar *)&ParamsPPD, sizeof(TParamsPPD) - 2);
      ErrCode = handler_giveCmdPPD(SetParamsPPD->VReq.NumPPD, cmdCONFIG_PDD, &ParamsPPD, NULL, 
                                   SysParams.BitVars.EncryptRF, SetParamsPPD->VReq.CheckNet);
      break;
        
    case REQ_PPD_CALIBRATE:  // ����������� ������(�) � PPD
      printf_Consol("������� ������ REQ_PPD_CALIBRATE\r\n");
      if (Pack->Size != sizeof(TReqCalibr))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      CS.Sensor1 = CalibPar->Sensor1;
      CS.Sensor2 = CalibPar->Sensor2;
      ErrCode = handler_giveCmdPPD(CalibPar->VReq.NumPPD, cmdCALIBRATE, &CS, NULL, NOENCRIPT, CalibPar->VReq.CheckNet);
      break;                        
      
    case REQ_PPD_CLEAR_PTRS:     
      printf_Consol("������� ������ REQ_PPD_CLEAR_PTRS\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdCLEAR_POINTERS, NULL, NULL, NOENCRIPT, VR->CheckNet);
      break;
      
    case REQ_COUNT_PPD:
      /* ������� �������� �� "������" ������ */  
      printf_Consol("������� ������ REQ_COUNT_PPD\r\n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* C������ ����� ���������� �������� �� ����� */
      ErrCode = handler_CountPPD(SysParams.QtyPPD, WAKE);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("Alive: %u, Dead: %u\r\n", (u16)SysStat.GoodPPD, (u16)SysStat.FailedPPD);
        /* �������� "�������" ����� */
        sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&CP, sizeof(TCountPPD));
      }
      break;
      
    case REQ_RESYNCH_PPD:     
      /* ������� �������� �� ������� ������ */
      printf_Consol("������� ������ REQ_RESYNCH_PPD\r\n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      // ==== DEBUG ====
      //handler_giveCmdPPD(getMaxNumPPD(), cmdREDUCE_PWR, NULL, NULL, NOENCRIPT, NOCHECKNET);
      ErrCode = handler_CountPPD(SysParams.QtyPPD/*getMaxNumPPD()*/, NOWAKE);
      if (ErrCode == NO_ERROR) {  
        printf_Consol("Alive: %u, Dead: %u\r\n", (u16)SysStat.GoodPPD, (u16)SysStat.FailedPPD);
        sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&CP, sizeof(TCountPPD));
      }
      break;
      
    case REQ_STOP_SYNCH_PPD:     
      printf_Consol("������� ������ REQ_STOP_SYNCH_PPD\r\n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ��������� ������������� */
      fl_SendSynchroRF = 0;
      /* ���������� ���� ��������� �������� �� ��������� "��������� ���" */
      //fl_NoDeepSleep = 0;
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      break;

    case REQ_NEXT_CH:     
      printf_Consol("������� ������ REQ_NEXT_CH\r\n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      if (SysStat.WorkRadioCh == Radio.NumChannel - 1) { SysStat.WorkRadioCh = 0; }
      else { SysStat.WorkRadioCh++; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      break;
      
    case REQ_PREV_CH:     
      printf_Consol("������� ������ REQ_PREV_CH\r\n");
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      if (SysStat.WorkRadioCh == 0) { SysStat.WorkRadioCh = Radio.NumChannel - 1; }
      else { SysStat.WorkRadioCh--; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      break;

    case REQ_PPD_SWITCH_SLEEP:     
      printf_Consol("������� ������ REQ_PPD_SWITCH_SLEEP\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdSWITCH_SLEEP, NULL, NULL, NOENCRIPT, VR->CheckNet);
      break;

    case REQ_PPD_ENABLE_SLEEP:     
      printf_Consol("������� ������ REQ_PPD_ENABLE_SLEEP\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdENABLE_SLEEP, NULL, NULL, NOENCRIPT, VR->CheckNet);
      break;
      
    case REQ_PPD_DISABLE_SLEEP:     
      printf_Consol("������� ������ REQ_PPD_DISABLE_SLEEP\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdDISABLE_SLEEP, NULL, NULL, NOENCRIPT, VR->CheckNet);
      break;
      
    case REQ_PPD_RESET:     
      printf_Consol("������� ������ REQ_PPD_RESET\r\n");
      if (Pack->Size != sizeof(TVoidReq))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(VR->NumPPD, cmdRESET, NULL, NULL, NOENCRIPT, VR->CheckNet);
      break;
      
    case REQ_PPD_SET_CHANNEL:  // ���������� ����� ����������� � PPD
      printf_Consol("������� ������ REQ_PPD_SET_CHANNEL\r\n");
      if (Pack->Size != sizeof(TReqSetChannelPPD))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdPPD(SetCh->VReq.NumPPD, cmdSET_CHANNEL, &NumCh, 
                                   NULL, SysParams.BitVars.EncryptRF, SetCh->VReq.CheckNet);
      // ������������� � ���� ���� ����� 
      setRadioChannel(NumCh);
      break;
      
    case REQ_GPS_GETSTATE:      // �������� ��������� GPS
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdGPS(CMD_GETSTATE_GPS, NULL, (uchar *)&CurrStatGPS);
      break;

    case REQ_GPS_GET_LOCATION:  // �������� ���������� �������������� GPS
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdGPS(CMD_GET_LOCATION, NULL, (uchar *)&SysStat.Coordinates); 
      break;

    case REQ_GPS_SEND_SMS:      // �������� ��� ����� GPS
      if (Pack->Size != sizeof(TSendSMS))  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdGPS(CMD_SEND_SMS, (uchar *)SendSMS, NULL); 
      break;

    case REQ_GPS_GET_DATETIME:  // �������� ������� ���� � ����� GPS
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      ErrCode = handler_giveCmdGPS(CMD_GET_DATETIME, NULL, (uchar *)&TmpDT); 
      break;

     // ==== DEBUG ====
    /*
    // �������� �������������  
    case 0x7e:
      sendPacket_UARTx(uart_number, Pack->SrcAdr, 0x7e, (uchar *)&Pack->Data, 6);
      break;
    */

    default:
      // ����������� ������� ��� ������
      ErrCode = ERR_BAD_COMMAND;
      break;
  }
  /* ����� ���������, ���������� ����� */
  UARTx[uart_number].RxComplete = 0;
  UARTx[uart_number].Request = 0;
  if (ErrCode != NO_ERROR) { // ���� ���� ������ � �������/�������
    if (Pack->Cmd >= 0xa0) { // ���� ��� ���� ������� - ��������� ����� �� ������
      sendError_UARTx (uart_number, Pack->SrcAdr, cmdERROR, ErrCode);
      delay_1ms(5); // ���, ����� ����� ����������
    }
    displayError(ErrCode);
    printf_Consol("Error! CMD = 0x%02x: Err = 0x%02x\r\n", Pack->Cmd, ErrCode);
    TimerForAll = 500;
    //while(TimerForAll) { blink(OUTPUTS_GPIO_PORT, LED_ERROR, 30, SND_ON); }
  }
}

/** 
  * @brief      ���������� ��������� ��������� SMS
  * @param      ...
  * @retval     ��� ������ (��. ErrCodes_TypeDef)
  */
uint handler_SendSMS (uchar uart_number, uchar adr_dst, uchar cmd, uchar *data)
{
  SendSmsState_TypeDef State = SMS_Prepare;
  uint ErrCode = NO_ERROR;
  uchar ErrCnt = 0; 
                                        
  /* �������� ��������� LED_WORK �� ������� ������ */
  GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, Bit_RESET);

  while (State != SMS_Complete) {
    _WDR();
    ErrCode = sendSMS(uart_number, adr_dst, cmd, data, &State);
    switch (ErrCode) {
      case NO_ERROR:
        break;
      
      case ERR_NOACK_GPS:
        printf_Consol("��� ������ �� ������� CMD_SEND_SMS = 0x%02x\n", CMD_SEND_SMS);
        break;
        
      case ERR_TRIES_SMS:
        printf_Consol("��������� ���������� ������� ������� SMS! Error 0x%02x\n", ErrCode);
        break;
        
      case ERR_SEND_SMS:
        printf_Consol("������� ����� �������� ������ �� ������� 0x%02x. Error 0x%02x\n", CMD_SEND_SMS , ErrCode);
        break;
        
        
      default:
        if(++ErrCnt > 5) {
          printf_Consol("Send SMS: ����������� ������ 0x%02x\n", ErrCode);
          break;
        }
        else { ErrCode = NO_ERROR; }
        break;
    }
    if (ErrCode != NO_ERROR) { break; }
    delay_1ms(1);
  }
  /* ��������� ��������� LED_WORK �� ������� ������ */
  GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, Bit_SET);
  return ErrCode;
}

/** 
  * @brief      ������� �������� ��������� SMS
  * @param      *state - ��������� �� ��������� �������� (��. SendSmsState_TypeDef)
  * @retval     ��� ������ (��. ErrCodes_TypeDef)
  */
uint sendSMS (uchar uart_number, uchar adr_dst, uchar cmd, uchar *data, SendSmsState_TypeDef *state)
{ 
  uint ErrCode = NO_ERROR;
  
  switch (*state)
  {
    case SMS_Idle:
      fl_AckSMS = 0;   // ��������� ���� ������������� �������� SMS (��� ���������, �.�. �� ������ �� ����� ����� ����������)
      ErrCode = NO_ERROR;
      *state = SMS_Complete;
      break;
    
    case SMS_Prepare:
      if(fl_SendingSMS) break;       // ��� �������� SMS
      prepareSMS(cmd, data, (uchar *)&SMS);
      *state = SMS_Send;
      break;
      
    case SMS_Send:
      fl_SendingSMS = 1;
      fl_AckSMS = 0;              // ���������� ���� �������� ������
      sendPacket_UARTx(uart_number, adr_dst, CMD_SEND_SMS, (uchar *)&SMS, sizeof(TSendSMS));
      printf_Consol((const char *) "����� SMS: %s\n", SMS.ShortMes);
      TimerWaitAckSendSMS = WAITACKSENDSMS; // ...ms �� �������� ������ �� �������� ������� 
      *state = SMS_WaitAck;
      break;
    
    case SMS_WaitAck:
      if (TimerWaitAckSendSMS == 0) {
        ErrCode = ERR_SEND_SMS;
        fl_SendingSMS = 0;
        break;
      }
      else if (fl_AckSMS) {
        fl_AckSMS = 0;
        fl_SendingSMS = 0;
        *state = SMS_Idle;
      }
      break;
      
    default:
      break;
  }
  return ErrCode;
}

/** 
  * @brief      ���������� ������� ������� ������� ����������� ��������� PPD
  * @param      num - ����� ������� (PPD_1, PPD_2, ..., PPD_96)
  *             cmd - ��� �������
  *             *idata - ��������� �� ������, ������������ � �������
  *             *rdata - ��������� �� ����� ��� ���������� ������ ����� ���������� �������
  *             enc - �� ��������� ������ (0)
  *                   ��� ��������� ������ (1) 
  *             checknet - ��������� ������� ������� � ���� ��� ���
  * @retval     ��� ������ (��. ErrCodes_TypeDef)
  */
uint handler_giveCmdPPD (uchar num, uchar cmd, void *idata, void *rdata, uchar enc, uchar checknet)
{
  GiveCmdState_TypeDef State = GC_PrepareData;
  uint ErrCode = NO_ERROR;
  uchar ErrCnt = 0;
  
  if ((num < 1) || (num > SysParams.QtyPPD))
  { 
    printf_Consol("����� ������� ��� ��������� ���������� �������: %d\n", num);
    return ERR_RANGE_PPD; 
  }
  
  /* ��������� ������� ������� � ���� */
  if (checknet) {
    //if (cmd != cmdREDUCE_PWR) {   // ���� �������� ������������ ������� ��������
      if (checkNumPPD(num) == ERROR) { return ERR_NOEXIST_PPD; }
    //}
    /* ��������� ����� ������������, �.�. ������ ��� ��������� � ���� */
    fl_NoChangeChannel = 1;
    /* ������������� � ��������� ������� ���������� */
    setRadioChannel(SysStat.WorkRadioCh);
  }
  
  /* ��������� ������ ������������� � ���������� �� �������� ����� */
  fl_SendSynchroRF = 0;

  /* �������� ��������� LED_WORK �� ������� ������ */
  GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, Bit_RESET);

  while (State != GC_Complete) {
    _WDR();
    /* ������������� ����� ���������� */
    Radio.RemoteAddr = UIN(num);
    /* ������� ������� ������� */
    ErrCode = giveCmdPPD(cmd, idata, rdata, &State, enc);
    /* ������������ ���������� */
    taskTransiver();

    /* ������� ��������� */
    switch (ErrCode) {
      case NO_ERROR:
        break;
      
      case ERR_NOACK_PPD:
        printf_Consol("��� ������ �� ������� Cmd: 0x%02x, PPD:%u\r\n", cmd, num);
       break;
        
      case ERR_RF_SEND:
        printf_Consol("ERR_RF_SEND: ������ �������� ������ � ����������!\r\n");
        break;

      case ERR_NOCONNECT_PPD:
        printf_Consol("������� ����� �������� ���������� � �������� �%u\r\n", num);  
        break;
        
      case ERR_ERRANSWER:
        printf_Consol("����� � �������! PPD �%u\r\n", num);  
        break;

    default:
        if(++ErrCnt > 5) {
          printf_Consol("����������� ������ 0x%02x\r\n", ErrCode);
          ErrCode = ERR_UNKNOWN;
          break;
        }
        else { ErrCode = NO_ERROR; }
        break;
    }
    if (ErrCode != NO_ERROR) { 
      break; 
    }
    delay_1ms(1);
  }
  /* ��������� ������� ������� ������������� � ���������� */
  fl_SendSynchroRF = 1;
  /* ��������� ����� ������������ */
  //fl_NoChangeChannel = 0;
  /* ��������� ��������� LED_WORK �� ������� ������ */
  GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, Bit_SET);
  return ErrCode;
}

/** 
  * @brief      ������ ������� ������� ����������� ��������� PPD
  * @param      cmd - ��� �������
  *             *idata - ��������� �� ������, ������������ � �������
  *             *rdata - ��������� �� ����� ��� ���������� ������ ����� ���������� �������
  *             *state - ��������� �� ��������� ����������
  *             enc - �� ��������� ������ (0)
  *                   ��� ��������� ������ (1)  
  * @retval     ��� ������ (��. ErrCodes_TypeDef)
  */
uint giveCmdPPD (uchar cmd, void *idata, void *rdata, GiveCmdState_TypeDef *state, uchar enc)
{
  uint ErrCode = NO_ERROR;
  PackHeader_TypeDef *RxPack = (PackHeader_TypeDef *) &Radio.RxBuf;

  switch (*state)
  {
    case GC_Idle:
      fl_AnsCmdPPD = 0;
      fl_PPDPingConfirm = 0;
      ErrCode = NO_ERROR;
      Radio.TaskState = IDLE_STATE;
      *state = GC_Complete;
      break;
      
    case GC_PrepareData:
      switch (cmd)
      {
        case cmdPING_REQUEST:
          idata = NULL;             
          LenOfInputData = 0;
          //rdata = (TStatusPDD *)rdata;
          rdata = &StatusPPD;
          LenOfReceivedData = sizeof(TStatusPDD);
          fl_DecryptRxData = 0;    // ���������� ������������ � [void parserPacket_MRF49XA(void)]
          break;
          
        case cmdREAD_LTSA:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = (TTranz *)rdata;
          LenOfReceivedData = sizeof(TTranz);
          fl_DecryptRxData = 1;       
          break;

        case cmdREAD_STSA:
          idata = (ulong *)idata;             
          LenOfInputData = 4;
          rdata = (TTranz *)rdata;
          LenOfReceivedData = sizeof(TTranz);
          fl_DecryptRxData = 1;       
          break;

        case cmdGET_DATETIME:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = (TRtcPPD *)rdata;
          LenOfReceivedData = sizeof(TRtcPPD);
          fl_DecryptRxData = 1;    // ������������� ���������� ������    
          break;
          
        case cmdSET_DATETIME:
          idata = (TRtcPPD *)idata;             
          LenOfInputData = sizeof(TRtcPPD);
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;        
          break;
          
        case cmdCONFIG_PDD:
          idata = (TParamsPPD *)idata;             
          LenOfInputData = sizeof(TParamsPPD);
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;        
          break;
            
        case cmdGETNUM:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = (u32 *)rdata;
          LenOfReceivedData = 4;
          fl_DecryptRxData = 0;         // �� ������������� ���������� ������        
          break;
          
        case cmdCALIBRATE:
          idata = (TCalibrSens *)idata;             
          LenOfInputData = sizeof(TCalibrSens);
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;        
          break;
          
        case cmdCLEAR_POINTERS:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;                 
          break;

        case cmdREDUCE_PWR:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;                 
          break;
          
        case cmdGET_PARAMS:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = (TParamsPPD *)rdata;
          LenOfReceivedData = sizeof(TParamsPPD);
          fl_DecryptRxData = 1;                 
          break;

        case cmdSWITCH_SLEEP:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;        
          break;
          
        case cmdENABLE_SLEEP:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;        
          break;

        case cmdDISABLE_SLEEP:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;        
          break;
          
        case cmdRESET:
          idata = NULL;             
          LenOfInputData = 0;
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;        
          break;
          
        case cmdSET_CHANNEL:
          idata = (uint *)idata;             
          LenOfInputData = sizeof(uint);
          rdata = NULL;
          LenOfReceivedData = 0;
          fl_DecryptRxData = 0;        
          break;
          
        default:
          printf_Consol("������: ����������� �������! CMD = 0x%02x\n", cmd);
          return ERR_BAD_COMMAND;
      }
      if((idata == NULL) && (LenOfInputData != 0)) 
      { // ������ ��������� ������ �������� malloc
        printf_Consol("Error malloc for CMD = 0x%02x\n", cmd);
        return ERR_MALLOC; 
      }
      *state = GC_StartConnect;
      break;
      
    case GC_StartConnect:
      /* ������������ ��������� ������ ��������� ���������� - ������������ */
      Radio.TaskState = SCAN_STATE;
      TimerWaitAnswer = SysStat.TimeWaitConnectRF;
      fl_PPDPingConfirm = 0;
      printf_Consol("\r\nWait connection...\r\n");
      *state = GC_WaitConnect;
      break;
      
    case GC_WaitConnect:
      if (TimerWaitAnswer == 0) {
        ErrCode = ERR_NOCONNECT_PPD;
        break;
      }
      if (fl_PPDPingConfirm) { // ����������� ���������� � ��������
        printf_Consol("\r\nConnection OK! PPD: %u, Ch: %u\r\n\n", 
                      NUM(Radio.RemoteAddr), Radio.Channel);
        fl_PPDPingConfirm = 0;
        if (cmd == cmdPING_REQUEST) { *state = GC_GetData; }
        else { *state = GC_SendCmd; }
      }
      break;
      
    case GC_SendCmd:
        printf_Consol("Send Cmd: 0x%02x\r\n", cmd);
        fl_AnsCmdPPD = 0;                   // ���������� ���� �������� ������
        if (sendPacket_MRF49XA(cmd, idata, LenOfInputData, enc) == ERROR) 
        { return ERR_RF_SEND; }
        TimerWaitAnswer = WAITANSWER_RF;    // - �������� ������ �� �������
        *state = GC_WaitAns;
      break;
      
    case GC_WaitAns:
      if (TimerWaitAnswer == 0) {
        ErrCode = ERR_NOACK_PPD;
        *state = GC_Idle;
        break;
      }
      if (fl_ErrPPD) { // ������� ����� � �������
        fl_ErrPPD = 0;
        ErrCode = ERR_ERRANSWER;
        break;
      }
      if (fl_AnsCmdPPD) { // ������� ����� �� �������
        fl_AnsCmdPPD = 0;
        SysStat.WorkRadioCh = Radio.Channel;    // ������ ������� �����
        Radio.TaskState = IDLE_STATE;
        *state = GC_GetData;
      }
      break;
      
    case GC_GetData:
      if(fl_DecryptRxData) {
        fl_DecryptRxData = 0;
        decryptData((uchar *)&RxPack->Data, LenOfReceivedData);
      }
      memcpy(rdata, &RxPack->Data, LenOfReceivedData);
      *state = GC_ReducePwr;
      break;
      
    case GC_ReducePwr:
      if(cmd != cmdREDUCE_PWR) {
        //sendPacket_MRF49XA(cmdREDUCE_PWR, NULL, NULL, NOENCRIPT);
      }
      *state = GC_Idle;
      break;
    
    default:
      break;
  }  
  return ErrCode;
}

/** 
  * @brief      ���������� ������� ������� ��������� GPS
  * @param      cmd - ��� �������
  *             *idata - ��������� �� ������, ������������ � �������
  *             *rdata - ��������� �� ����� ��� ���������� ������ ����� ���������� �������
  * @retval     ��� ������ (��. ErrCodes_TypeDef)
  */
uint handler_giveCmdGPS (uchar cmd, uchar *idata, uchar *rdata)
{
  //GiveCmdState_TypeDef State = GC_PrepareData;
  uint ErrCode = ERR_IDLE;
  uchar ErrCnt = 0;
  
  while (StateGPS != GC_Complete) {
    _WDR();
    ErrCode = giveCmdGPS(cmd, idata, rdata, &StateGPS);
    parserPacket_UARTx(UART_GPS);
    switch (ErrCode) {
      case NO_ERROR:
        break;
        
      case ERR_WAITING_ANSWER:
        break;
        
      case ERR_NOACK_GPS:
        break;

      default:
        if(++ErrCnt > 5) {
          printf_Consol("Give Cmd GPS: �����. ������ 0x%02x\n", ErrCode);
          ErrCode = ERR_UNKNOWN;
          break;
        }
        else { ErrCode = NO_ERROR; }
        break;
    }
    if (ErrCode != NO_ERROR) { break; }
  }
  //delay_1ms(10); //====DEBUG HANG====
  return ErrCode;
}

/** 
  * @brief      ������ ������� ��������� GPS
  * @param      cmd - ��� �������
  *             *idata - ��������� �� ������, ������������ � �������
  *             *rdata - ��������� �� ����� ��� ���������� ������ ����� ���������� �������
  *             *state - ��������� �� ��������� ����������
  * @retval     ��� ������ (��. ErrCodes_TypeDef)
  */
uint giveCmdGPS (uchar cmd, void *idata, void *rdata, GiveCmdState_TypeDef *state)
{
  uint ErrCode = ERR_WAITING_ANSWER;

  switch (*state)
  {
    case GC_Idle:
      break;
      
    case GC_PrepareData:
      switch (cmd)
      {
        case CMD_GETSTATE_GPS:
          idata = NULL;
          LenOfInputData = 0;
          rdata = (TGPSStatus *)rdata;
          LenOfReceivedData = sizeof(TGPSStatus);
          break;

        case CMD_GET_LOCATION:
          idata = NULL;
          LenOfInputData = 0;
          rdata = (TLocation *)rdata;
          LenOfReceivedData = sizeof(TLocation);
          break;

        case CMD_SEND_SMS:
          idata = (TSendSMS *)idata;
          LenOfInputData = sizeof(TSendSMS);
          rdata = NULL;
          LenOfReceivedData = 0;
          break;

        case CMD_GET_DATETIME:
          idata = NULL;
          LenOfInputData = 0;
          rdata = (DataTime_TypeDef *)rdata;
          LenOfReceivedData = sizeof(DataTime_TypeDef);
          break;

        case cmdDISPL_SEAT:
          idata = (TInfoSeats *)idata;
          LenOfInputData = sizeof(TInfoSeats);
          rdata = NULL;
          LenOfReceivedData = 0;
          break;

        default:
          printf_Consol("������: ����������� �������! CMD = 0x%02x\n", cmd);
          return ERR_BAD_COMMAND;
      }
      *state = GC_SendCmd;
      break;
      
    case GC_SendCmd:
      /* �������� ��� ������� */
      if(cmd != cmdDISPL_SEAT) { 
      /* -------------------- */  
        sendPacket_UARTx(UART_GPS, GPSTERM, cmd, idata, LenOfInputData);
        fl_AnsCmdGPS = 0;                   // ���������� ���� �������� ������
        TimerWaitAnswer = WAITANSWER_GPS;   // ... ms �� �������� ������ �� �������
        if(cmd == CMD_GETSTATE_GPS)
        { // GPS �������� "M2M-mini" �������� �� ������� 0x11 �� 10 ���  
          TimerWaitAnswer = 10000;
        }  
        *state = GC_WaitAns;
      /* �������� ��� ������� */
      } 
      else {
        printf_Consol("Send to GPS\r\n");
        sendPacket_UARTx(UART_GPS, DRIVERUNIT, cmd, idata, LenOfInputData);
        fl_AnsCmdGPS = 0;
        *state = GC_GetData;
      }
      /* -------------------- */
      break;
      
    case GC_WaitAns:
      if (TimerWaitAnswer == 0) {
        printf_Consol("������� ����� �������� ������ �� ������� 0x%02x\n", cmd);  
        ErrCode = ERR_NOACK_GPS;
        *state = GC_Idle;
        break;
      }
      if (fl_AnsCmdGPS) { // ������� ����� �� �������
        fl_AnsCmdGPS = 0;
        *state = GC_GetData;
        break;
      }
      /* ���� ���� ����� - ����������� ����� ����� ��������� */
      STimerPingGPS = 10;   // 10s
      break;
      
    case GC_GetData:
      memcpy(rdata, &UART_PACKDATA(UART_GPS), LenOfReceivedData);
      *state = GC_Exit;
      break;
    
    case GC_Exit:
      fl_AnsCmdGPS = 0;
      ErrCode = NO_ERROR;
      *state = GC_Complete;
      break;
      
    default:
      break;
  }  
  return ErrCode;
}

/** 
  * @brief      ������ �������� ������� �� UART (���������� �������)
  * @param      uart_number - ����� ������������� UART�: 
  *             0 => UART0 (USART1) 
  *             1 => UART1 (USART2)
  * @retval     None
  * @Note       ��� ������� ������������ � ������ �������� ������� GPS ���������
  */
void GPS_parserPacket_UARTx(uchar uart_number)
{
  //uint Crc, Pcrc;
  uchar ErrCode = NO_ERROR;
  uchar Arr[] = {0x7d, 0x7d, 0x7d, 0x7e, 0x7d, 0x7e, 0x7e, 0x7e};

  PackUart_TypeDef *Pack = (PackUart_TypeDef *) &UARTx[uart_number].InBuf;

  //if(!UARTx[uart_number].RxComplete) return;

  if (!UARTx[uart_number].Request) return;  // ���������� ����� �� ������ - �������

/* �������� ��������� ������ -------------------------------------------------*/  
  
  // ���� ���������� �� � GPS ��������� - �����
  /*
  if (Pack->DstAdr != GPSTERM) {
    UARTx[uart_number].RxComplete = 0; // ���������� ������� ��������� ������ ������            
    return;
  }
  */

  // ��������� ����� ����������� (��� ��������� ������)
  //UARTx[uart_number].RemoteAdr = Pack->SrcAdr;
  
  // ��������� CRC
  /*
  Crc = MakeCRC16(&Pack->DstAdr, Pack->Size + HEAD_PACKET);
  Pcrc = *((uint *) &Pack->Data[Pack->Size]);
  if (Crc != Pcrc) {
    ErrCode = ERR_BAD_CRC16;
    sendPacket_UARTx(uart_number, Pack->SrcAdr, CMD_ERROR, &ErrCode, 1);
    UARTx[uart_number].RxComplete = 0; // ���������� ������� ��������� ������ ������            
	return;
  }
  */

/* ������ ������ ������ ------------------------------------------------------*/ 
  switch (Pack->Cmd) {
    // ������� GPS ��������� �� ��������� �����������
    case CMD_GETSTATE_GPS:  // ����� �� ������� "������ ������� GPS ���������"
      if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      DebugStatGPS.Antenna = 1;	        // ������� GPS ���������� (1), ��������� (0)
      DebugStatGPS.AntennaFailed = 1;	// ������� ���������� (1), ������� �� (0)
      DebugStatGPS.IntBatt = 0x05;		// ������� ������ ���������� �������
      DebugStatGPS.LevelGSM = 0x03;     // ������� GSM �������
      getDateTime(&DebugStatGPS.DataTime);	// ������� ���� � ����� (��. �. 16.5.2)
      DebugStatGPS.Latitude_IntPart = (uint)rnd();	    // ����� ����� �������� ������
      DebugStatGPS.Latitude_FrPart = (uint)rnd();	    // ������� ����� �������� ������
      DebugStatGPS.Longitude_IntPart1 = (uint)rnd();	// ����� ����� �������� ������� 1
      DebugStatGPS.Longitude_IntPart2 = (uint)rnd();	// ����� ����� �������� ������� 2
      DebugStatGPS.Longitude_FrPart = (uint)rnd();	    // ������� ����� �������� �������
      DebugStatGPS.Speed = 0x16;			    // �������� ��������
      DebugStatGPS.Qty_VisibleSats = 0x08;		// ����� ������� ���������
      DebugStatGPS.StartMoving = 1;		    // ���� ������ ��������
      DebugStatGPS.EndMoving = 0;		    // ���� ��������� ��������
      DebugStatGPS.SenGPIO_1 = 1;		    // �������� ����������� ������� 1
      DebugStatGPS.SenGPIO_2 = 0;		    // �������� ����������� ������� 2
      DebugStatGPS.SenGPIO_3 = 1;		    // �������� ����������� ������� 3
      DebugStatGPS.SenGPIO_4 = 0;		    // �������� ����������� ������� 4
      DebugStatGPS.SenGPIO_5 = 1;		    // �������� ����������� ������� 5
      DebugStatGPS.SenGPIO_6 = 0;		    // �������� ����������� ������� 6
      DebugStatGPS.ExtBatt = 0;			    // ������� ���. ����-�� ���������� (1),
      DebugStatGPS.DataExists = 0;		    // � ������ ���� ������ ��� �������� (1),
      DebugStatGPS.ReserveBits = 0x16;		    // ������
      DebugStatGPS.SenAnalog_1 = 0x7e;		    // �������� ����������� ������� 1
      DebugStatGPS.SenAnalog_2 = 0x7d;		    // �������� ����������� ������� 2
      DebugStatGPS.SenAnalog_3 = 0x7e;		    // �������� ����������� ������� 3
      DebugStatGPS.ReserveByte = 0x7d;		    // ������
      // �������� - �������� ���������� ������
      delay_1ms(1000);
      // �������� �������� �����
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&DebugStatGPS, sizeof(TGPSStatus));
      break;
      
    case CMD_GET_LOCATION:  // ����� �� ������� "�������� ���������� �������� ��������������"
      if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      DebugLoc.Latitude_IntPart = (uint)rnd();	    // ����� ����� �������� ������
      DebugLoc.Latitude_FrPart = (uint)rnd();	    // ������� ����� �������� ������
      DebugLoc.Longitude_IntPart1 = (uint)rnd();	// ����� ����� �������� ������� 1
      DebugLoc.Longitude_IntPart2 = (uint)rnd();	// ����� ����� �������� ������� 2
      DebugLoc.Longitude_FrPart = (uint)rnd();	    // ������� ����� �������� �������
      // �������� - �������� ���������� ������
      delay_1ms(800);
      // �������� ��������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&DebugLoc, sizeof(TLocation));
      break;
      
    case CMD_SEND_SMS:  // ����� �� ������� "�������� SMS"
      if (Pack->Size != sizeof(TSendSMS)) { ErrCode = ERR_BAD_SIZE; break; }
      DebugStatGPS.DataExists = 1;		    // � ������ ���� ������ ��� ��������
      // �������� - �������� ���������� �������� SMS
      delay_1ms(2000);
      // ������������� �� ������� ����������� �������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
      break; 
      
    case CMD_GET_DATETIME:  // ����� �� ������� "�������� ���� � ����� GPS-���������"
      if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      if (getDateTime(&DTI) == ERROR) { ErrCode = ERR_GET_DT; break; }
      // �������� - �������� ��������� ���������� ������� � ����
      delay_1ms(600);
      // �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&DTI, sizeof(DataTime_TypeDef));
      break;
    
    case CMD_CHECK_BTSTF:  // ����� �� ������� "�������� �������������"
      delay_1ms(300);
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, Arr, sizeof(Arr));
      break; 

    // ----------- ������ �� ������� GPS-��������� �� ��������� ����������� ------  
    case REQ_GET_LAST_EVN:  // ����� �� ������ "�������� ��������� ������� �� ������� ���������"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ����� ���� ������������ � �������� ����-��, ��� ����� �������! */
      break;
      
    case REQ_GET_SPEC_EVN:  // ����� �� ������ "�������� ������ ������������ �������"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ����� ���� ������������ � �������� ����-��, ��� ����� �������! */
      break;
      
    case REQ_QTY_EVN:   // ����� �� ������ "�������� ���������� ������������ �������"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ����� ���� ������������ � �������� ����-��, ��� ����� �������! */
      break;
      
    case REQ_CLEAR_HIS: // ����� �� ������ "�������� ������� �������"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ����� ���� ������������ � �������� ����-��, ��� ����� �������! */
      break;
      
    case REQ_GET_DATETIME:  // ����� �� ������ "�������� ������� ���� � �����"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ����� ���� ������������ � �������� ����-��, ��� ����� �������! */
      break;
      
    case REQ_SET_DATETIME: // ����� �� ������ "���������� ���� � �����"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ����� ���� ������������ � �������� ���� ����, ��� ����� �������! */
      break;

    case REQ_SET_SMSINT: // ����� �� ������ "���������� ��������� �������� ������� SMS"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ����� ���� ������������ � �������� ���� ����, ��� ����� �������! */
      break;
      
    case REQ_GET_STATE: // ����� �� ������ "�������� ��������� �����������"
      if (Pack->Size != 0)  { ErrCode = ERR_BAD_SIZE; break; }
      /* ����� ���� ������������ � �������� ���� ����, ��� ����� �������! */
      break;
      
    case REQ_EXT_IRQ: // ����� �� ������ "���������� �� �������� ����������"
      break;
      
    case REQ_GET_WEB_DATA: // ����� �� ������ "�������� ������ ��� Web-�������"
      if (Pack->Size != sizeof(TWebData))  { ErrCode = ERR_BAD_SIZE; break; }
      break;

    case CMD_ERROR:    // ������ ������� � �������
      break;

    default:
      // ����������� ������� ��� ������
      ErrCode = ERR_BAD_COMMAND;
      break;
  }
  
  if (ErrCode != NO_ERROR) { // ���� ���� ������ � �������/�������
    if (Pack->Cmd >= 0xa0) { // ���� ��� ���� ������� - ��������� ����� �� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, CMD_ERROR, &ErrCode, 1);
    }
    displayError(ErrCode);
    printf_Consol((const char *)"Error! CMD = 0x%02x, Err = 0x%02x\n", Pack->Cmd, ErrCode);
    TimerForAll = 2000;
    while(TimerForAll) { _WDR(); blink(OUTPUTS_GPIO_PORT, LED_ERROR, 30, SND_ON); }
  }
  
  /* ����� ���������, ���������� ����� */
  UARTx[uart_number].RxComplete = 0;
  UARTx[uart_number].Request = 0;
}

/******************* (C) COPYRIGHT 2013 SDK-SYSTEMS *****END OF FILE****/
