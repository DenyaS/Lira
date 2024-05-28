
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

//Лира
volatile uint32_t TimerStateCheck = 0;        //Таймер между опросом автоматов Лира 1 ms
// Interfaces 
volatile uint8_t data_SPI = 0;
volatile uint8_t data_UART0 = 0;
volatile uint8_t data_UART1 = 0;
UART_TypeDef UARTx[2];  
Radio_TypeDef Radio;
RfRegisters_TypeDef RF;

// Parameters
SystemParams_TypeDef SysParams;      // Параметры системы  
DevFault_TypeDef DevFault;           // Неисправности устройств
SMSParams_Typedef SMSParams;
RTCParams_TypeDef SysRTC;            // Real Time Clock   

// Counters
volatile uint32_t TickCnt = 0;

// Таймеры 1 ms
volatile uint32_t TimerCPL = 0;
volatile uint32_t TimerMenuExit = 0;
volatile uint32_t TimeForDelay = 0;
volatile uint32_t TimerKeyboard = 0;
volatile uint32_t TimerWaitAnswer = 0;
volatile uint32_t TimerSPI = 0;
volatile uint32_t TimerBlink = 0;
volatile uint32_t TimerBeep = 0;
volatile uint32_t TimerFLASH = 0;
volatile uint32_t TimerGetDT = 0;
volatile uint32_t TimerRF = 0;
volatile uint32_t TimerReceivePack = 0;
volatile uint32_t TimerForAll = 0;
volatile uint32_t TimerWaitAckSendSMS = 0;
volatile uint32_t TimerSynchroRF = 0;
volatile uint32_t TimerAwake = 0;
volatile uint32_t TimerAutoSMS = 0;
// Таймеры 1 s
volatile uint32_t STimerShowSeats = 0;       
volatile uint32_t STimerPingGPS = 0;
volatile uint32_t STimerMakeEvent = 0;
volatile uint32_t STimerAutoSMS = 0;
volatile uint32_t STimerBacklight = 0;
volatile uint32_t STimerStopShowTime = 0;

// Flags
volatile _FLAGS Flags;   

// Pointers
volatile ulong StartAddrEventDF;
volatile ulong CurrentAddrEventDF;

// Код нажатой клавиши
volatile uchar LastKey;
volatile uchar PreviKey;

// Меню
MenuDisplayState_TypeDef MenuPos = MD_Idle;
volatile uchar MenuStack = 0;
volatile uchar MenuDirection;     // 00 - UP; 01 -  DOWN; 10 - ENTER; 11 - ESCAPE
 
// Состояния 
volatile uchar ErrorI2C = 0;
GiveCmdState_TypeDef StateGPS = GC_Idle;

// Radio
#ifdef __RF_MASTER_MODE__
  vu8 TryNum = 0;
  vu8 Replay = 0;
  vu16 Try = 10;       // К-во попыток пинга
#endif /* __RF_MASTER_MODE__ */

// Display
DisplayState_TypeDef CurrentlyDisplayed;
DisplayState_TypeDef PreviosDisplayed;
MenuDisplayState_TypeDef BeforeDisplayed = MD_Idle;
char DString[16];

// Данные для хранения и передачи
TSystemStatus SysStat;
TSendSMS SMS;
TGoogleGPS CurrLoc;
TGPSStatus CurrStatGPS;
TEvent CurrEvent;
TCheckSensor CurrSensor;
TStatusPDD StatusPPD;
DataTime_TypeDef DTI;
TCountPPD CP;
TGPSStatus DebugStatGPS;
TLocation DebugLoc;
TInfoSeats IS[2];       // Информация о состоянии датчиков и занятости сидений
                        // IS[0] - предыдущая считанная информация
                        // IS[1] - последняя считанная информация (самая свежая)    
volatile double Odometer = 0;         // Общий пробег в метрах
TAnswerInfoDT AIDT;

// ==== DEBUG ====
vu16 NumsOfPackets = 0;
vu16 NumsOfIPackets = 0;
u8 TmpU08_1;
u8 TmpU08_2;
u16 TmpU16_1;
u16 TmpU16_2;
u32 TmpU32_1;
u32 TmpU32_2;

