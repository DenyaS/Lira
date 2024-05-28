
 
/* Includes ------------------------------------------------------------------*/
#include "extcom.h"
#include "includes.h"
#include "STM32vldiscovery.h"

/* Private variables ---------------------------------------------------------*/
vu8 LenOfInputData;
vu8 LenOfReceivedData;
u32 Protag;     // Длина протяжки полосы

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
  uchar ErrCode = NO_ERROR;

  PackUart_TypeDef *Pack = (PackUart_TypeDef *) &UARTx[uart_number].InBuf;
  //DataTime_TypeDef *DT = (DataTime_TypeDef *) &Pack->Data;

  if (!UARTx[uart_number].Request) return;  // Корректный пакет не принят - возврат
  
/* Анализ данных пакета ------------------------------------------------------*/ 

SupMotor_TypeDef *DT = (SupMotor_TypeDef *) &Pack->Data;

  switch (Pack->Cmd) {
  case REQ_STATUS_SM:   //Ответ на запрос состояния модуля выравнивания
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&StateSM, 1); 
  break; 
  case REQ_ERROR_SM:   //Ответ на запрос состояния модуля выравнивания
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&CodeErrorSM, 1); 
  break;
    case REQ_STATUS_CM:   //Ответ на запрос состояния модуля обрезки
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&StateCM, 1); 
  break; 
  case REQ_ERROR_CM:   //Ответ на запрос состояния модуля обрезки
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&CodeErrorCM, 1); 
  break;
    case REQ_STATUS_Man:   //Запрос состояния воздушного цилиндра выдвижение манипулятора
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&state_Man, 1); 
  break; 
    break;
    case REQ_STATUS_Zah:   //Запрос состояния воздушного цилиндра захват манипулятора
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&state_Zah, 1); 
  break;
    case REQ_STATUS_Fin:   //Запрос состояния воздушного цилиндра пальца манипулятора
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; } 
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&state_Fin, 1); 
  break;
  
  case REQ_ERROR_Code:   //Запрос обших ошибок 
    if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&ErrCodeAll, 1); 
  break;  
  
  case 101:
    if (Pack->Size != sizeof (SupMotor_TypeDef)) { ErrCode = ERR_BAD_SIZE; break; }
      LengthPol = DT->length;     // Длина
      speedSM = DT->speed;                 //Скорость затяжки
      MessegSM = NewTaskSM;       //Получено новое задание
    break;
  case 102:
     if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      MessegCM = NewTaskCM;       //Получено новое задание
    break; 
  case 103:
    if (Pack->Size != sizeof (SupMotor_TypeDef)) { ErrCode = ERR_BAD_SIZE; break; }
      Motor[_A_].NextCoor = DT->length;     // Координата
      Motor[_A_].Imp = DT->speed;                 //Скорость
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
  case 106:             //Переинициализация 
     if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      MessegSM = ReInitSM;
      MessegCM = ReInitCM;
      MessegCM = ResetCM;
      
    break;    
   
//    sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, (uchar *)&Event, sizeof(TEvent)); // ПРИМЕР ИЗ STM-V
 /*   
    case SUP_ALIGN_MOVE_LEFT:
      if (Pack->Size != sizeof (SupMotor_TypeDef)) { ErrCode = ERR_BAD_SIZE; break; }
//      LengthPol=DT->length;     // Длина
//      speedSM = DT->speed;                 //Скорость затяжки
//      MessegSM=NewTaskSM;
//        MessegCM=ResetCM;    
//      MessegCM=NewTaskCM;
      LengthX = DT->length;     // Длина
      speedX = DT->speed;                 //Скорость затяжки
       MessegX=motion_rightX;       //Получено новое задание
    */
            /*
         StateSM = WorkSM;
         Protag = DT->length;
         
         MotorZ.Steps = Protag*400*100/314/25;     // крутим 200 шагов 0.5 оборота 180гр = 200*1,8/2
         MotorZ.Start = 10;          // Начальная скорость
         MotorZ.Dir = 1;    // Направление вращения         
         MotorZ.Imp = DT->speed;                // Скорость вращения  
          // Запускаем мотор
          MotorZ.State = MRS_Start;
         */
 //     STM32vldiscovery_LEDOn(LED3);
    break;  
    case CMD_TEST:  
      if (Pack->Size != 0) { ErrCode = ERR_BAD_SIZE; break; }
      // Подтверждение на успешно принятый запрос
      sendPacket_UARTx(uart_number, Pack->SrcAdr, Pack->Cmd, NULL, 0);
     break;
      
    case CMD_ERROR:    // Анализ ответов с ошибкой
      break;
    
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
  /*
  if (ErrCode != NO_ERROR) { // Если была ошибка в команде/запросе
      sendError_UARTx (uart_number, Pack->SrcAdr, CMD_ERROR, ErrCode);
      delay_1ms(5); // Ждём, чтобы пакет отправился
      printf_Consol("Error! CMD = 0x%02x: Err = 0x%02x\r\n", Pack->Cmd, ErrCode);
  }
  */
  
}


