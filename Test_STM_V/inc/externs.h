
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXTERNS_H
#define __EXTERNS_H

#include "extcom.h"
#include "flags.h"
#include "functions.h"
#include "main.h"
#include "menu.h"
#include "radio.h"
#include "rtc.h"
#include "uart.h"
//Лира
extern volatile uint32_t TimerStateCheck;        //Таймер между опросом автоматов Лира 

extern volatile uint8_t data_SPI;
extern volatile uint8_t data_UART0;
extern volatile uint8_t data_UART1;
extern UART_TypeDef UARTx[2];  

extern SystemParams_TypeDef SysParams;
extern DevFault_TypeDef DevFault;           
extern SMSParams_Typedef SMSParams;
extern RTCParams_TypeDef SysRTC;              

extern volatile uint32_t TickCnt;

extern volatile uint32_t TimerCPL;
extern volatile uint32_t TimerMenuExit;
extern volatile uint32_t TimeForDelay;
extern volatile uint32_t TimerKeyboard;
extern volatile uint32_t TimerWaitAnswer;
extern volatile uint32_t TimerSPI;
extern volatile uint32_t TimerBlink;
extern volatile uint32_t TimerBeep;
extern volatile uint32_t TimerFLASH;
extern volatile uint32_t TimerGetDT;
extern volatile uint32_t TimerRF;
extern volatile uint32_t TimerReceivePack;
extern volatile uint32_t TimerForAll;
extern volatile uint32_t TimerWaitAckSendSMS;
extern volatile uint32_t TimerSynchroRF;
extern volatile uint32_t TimerAwake;
extern volatile uint32_t TimerAutoSMS;
extern volatile uint32_t STimerShowSeats;
extern volatile uint32_t STimerPingGPS;
extern volatile uint32_t STimerMakeEvent;
extern volatile uint32_t STimerAutoSMS;
extern volatile uint32_t STimerBacklight;
extern volatile uint32_t STimerStopShowTime;

extern volatile _FLAGS Flags;         

extern volatile ulong StartAddrEventDF;
extern volatile ulong CurrentAddrEventDF;

extern volatile uchar LastKey;
extern volatile uchar PreviKey;

extern DisplayState_TypeDef CurrentlyDisplayed;
extern DisplayState_TypeDef PreviosDisplayed;
extern MenuDisplayState_TypeDef BeforeDisplayed;
extern char DString[16];

extern MenuDisplayState_TypeDef MenuPos;
extern volatile uchar MenuStack;
extern volatile uchar MenuDirection;

extern volatile uchar ErrorI2C;
extern GiveCmdState_TypeDef StateGPS;

#ifdef __RF_MASTER_MODE__
extern vu8 TryNum;
extern vu8 Replay;
extern vu16 Try;
#endif /* __RF_MASTER_MODE__ */

extern Radio_TypeDef Radio;
extern RfRegisters_TypeDef RF;

extern TSystemStatus SysStat;
extern TSendSMS SMS;
extern TGoogleGPS CurrLoc;
extern TGPSStatus CurrStatGPS;
extern TEvent CurrEvent;
extern TCheckSensor CurrSensor;
extern TStatusPDD StatusPPD;
extern DataTime_TypeDef DTI;
extern TCountPPD CP;
extern TGPSStatus DebugStatGPS;
extern TLocation DebugLoc;
extern TInfoSeats IS[];
extern volatile double Odometer;
extern TAnswerInfoDT AIDT;

// ==== DEBUG ====
extern vu16 NumsOfPackets;
extern vu16 NumsOfIPackets;
extern u8 TmpU08_1;
extern u8 TmpU08_2;
extern u16 TmpU16_1;
extern u16 TmpU16_2;
extern u32 TmpU32_1;
extern u32 TmpU32_2;


#endif /* __EXTERNS_H */

