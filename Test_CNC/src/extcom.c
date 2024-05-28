
 
/* Includes ------------------------------------------------------------------*/
#include "extcom.h"
#include "includes.h"
#include "STM32vldiscovery.h"

/* Private variables ---------------------------------------------------------*/
vu8 LenOfInputData;
vu8 LenOfReceivedData;
u32 Protag;     // ����� �������� ������

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
  uchar ErrCode = NO_ERROR;

  PackUart_TypeDef *Pack = (PackUart_TypeDef *) &UARTx[uart_number].InBuf;
  //DataTime_TypeDef *DT = (DataTime_TypeDef *) &Pack->Data;

  if (!UARTx[uart_number].Request) return;  // ���������� ����� �� ������ - �������
  
/* ������ ������ ������ ------------------------------------------------------*/ 

SupMotor_TypeDef *DT = (SupMotor_TypeDef *) &Pack->Data;

  switch (Pack->Cmd) {
  case REQ_STATUS_SM:   //����� �� ������ ��������� ������ ������������
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&StateSM, 1); 
  break; 
  case REQ_ERROR_SM:   //����� �� ������ ��������� ������ ������������
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&CodeErrorSM, 1); 
  break;
    case REQ_STATUS_CM:   //����� �� ������ ��������� ������ �������
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&StateCM, 1); 
  break; 
  case REQ_ERROR_CM:   //����� �� ������ ��������� ������ �������
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&CodeErrorCM, 1); 
  break;
    case REQ_STATUS_Man:   //������ ��������� ���������� �������� ���������� ������������
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&state_Man, 1); 
  break; 
    break;
    case REQ_STATUS_Zah:   //������ ��������� ���������� �������� ������ ������������
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&state_Zah, 1); 
  break;
    case REQ_STATUS_Fin:   //������ ��������� ���������� �������� ������ ������������
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&state_Fin, 1); 
  break;
  
  case REQ_ERROR_Code:   //������ ����� ������ 
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&ErrCodeAll, 1); 
  break;  
  
  case 101:
    if (Pack->Size != sizeof (SupMotor_TypeDef)) { ErrCode = ERR_BAD_SIZE; break; }
      LengthPol = DT->length;     // �����
      speedSM = DT->speed;                 //�������� �������
      MessegSM = NewTaskSM;       //�������� ����� �������
    break;
  case 102:
     if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      MessegCM = NewTaskCM;       //�������� ����� �������
    break; 
  case 103:
    if (Pack->Size != sizeof (SupMotor_TypeDef)) { ErrCode = ERR_BAD_SIZE; break; }
      Motor[_A_].NextCoor = DT->length;     // ����������
      Motor[_A_].Imp = DT->speed;                 //��������
      go_to_poin (_A_);
    break;
  case 104:
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      fl_Air_Cyl_Man = 1;
    break; 
  case 105:
     if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      fl_Air_Cyl_Man = 0;
    break;    
  case 106:             //����������������� 
     if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      MessegSM = ReInitSM;
      MessegCM = ReInitCM;
      MessegCM = ResetCM;
      
    break;    
   
//    sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&Event, sizeof(TEvent)); // ������ �� STM-V
 /*   
    case SUP_ALIGN_MOVE_LEFT:
      if (Pack->Size != sizeof (SupMotor_TypeDef)) { ErrCode = ERR_BAD_SIZE; break; }
//      LengthPol=DT->length;     // �����
//      speedSM = DT->speed;                 //�������� �������
//      MessegSM=NewTaskSM;
//        MessegCM=ResetCM;    
//      MessegCM=NewTaskCM;
      LengthX = DT->length;     // �����
      speedX = DT->speed;                 //�������� �������
       MessegX=motion_rightX;       //�������� ����� �������
    */
            /*
         StateSM = WorkSM;
         Protag = DT->length;
         
         MotorZ.Steps = Protag*400*100/314/25;     // ������ 200 ����� 0.5 ������� 180�� = 200*1,8/2
         MotorZ.Start = 10;          // ��������� ��������
         MotorZ.Dir = 1;    // ����������� ��������         
         MotorZ.Imp = DT->speed;                // �������� ��������  
          // ��������� �����
          MotorZ.State = MRS_Start;
         */
 //     STM32vldiscovery_LEDOn(LED3);
    break;  
    case CMD_TEST:  
      if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      // ������������� �� ������� �������� ������
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
     break;
      
    case CMD_ERROR:    // ������ ������� � �������
      break;
    
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
  /*
  if (ErrCode != NO_ERROR) { // ���� ���� ������ � �������/�������
      sendError_UARTx (uart_number, Pack->SrcAdr, CMD_ERROR, ErrCode);
      delay_1ms(5); // ���, ����� ����� ����������
      printf_Consol("Error! CMD = 0x%02x: Err = 0x%02x\r\n", Pack->Cmd, ErrCode);
  }
  */
  
}


