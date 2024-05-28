
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

/* Includes ----------------------------------------------------------------- */
#include "extcom.h"
#include "stm32F10x.h" 
#include "unsigned.h"

/* Define ------------------------------------------------------------------- */
#define FACTORY_DIFF    4
#define EMPTY_STRING    "\n"

#define LEDS           LEDBLUE | LEDGREEN
#define LEDS_GPIO_CLK  RCC_APB2Periph_GPIOC

#define KEYBOARD       KEY_DOWN | KEY_OK | KEY_ESC 

#define KEYBOARD_EXTI_LINES     KEY_UP_EXTI_PIN | KEY_DOWN_EXTI_PIN| KEY_OK_EXTI_PIN | KEY_ESC_EXTI_PIN

#define MTC_GPIO_CLK        RCC_APB2Periph_GPIOB
#define MTC_GPIO_V0_CLK     RCC_APB2Periph_GPIOC

#define SEN_1_STATE   GPIO_ReadInputDataBit(SENSORS_GPIO_PORT_1, SEN_1)
#define SEN_2_STATE   GPIO_ReadInputDataBit(SENSORS_GPIO_PORT_1, SEN_2)
#define SEN_3_STATE   GPIO_ReadInputDataBit(SENSORS_GPIO_PORT_2, SEN_3)
#define SEN_4_STATE   GPIO_ReadInputDataBit(SENSORS_GPIO_PORT_2, SEN_4)

// Коды клавиш
#define UP              0x10        // port B: 0000 0000 0001 0000
#define DOWN            0x20        // port B: 0000 0000 0010 0000
#define ENTER           0x40        // port B: 0000 0000 0100 0000
#define ESCAPE          0x80        // port B: 0000 0000 1000 0000

// Внешние прерывания
#define RF_EXTI_LINE    EXTI_Line8  // Вход прерывания от микросхемы MRF49XA

/* Type definitions ----------------------------------------------------------*/
typedef enum 
{  
  INPUT_MODE_GPIO = 0,
  INPUT_MODE_EXTI = 1
} InputMode_TypeDef;              

typedef struct
{
    uint8_t Version;        // Версия ПО
    
    uint8_t EncryptRF:1;    // Шифровать (0) или не шифровать (1) данные пакеты УКВ 
    uint8_t Reserve_2:1;          
    uint8_t Reserve_3:1;          
    uint8_t Reserve_4:1;          
    uint8_t Reserve_5:1;          
    uint8_t Reserve_6:1;          
    uint8_t Reserve_7:1;          
    uint8_t Reserve_8:1;          

    uint8_t Reserve_9:1;          
    uint8_t Reserve_10:1;          
    uint8_t Reserve_11:1;          
    uint8_t Reserve_12:1;          
    uint8_t Reserve_13:1;          
    uint8_t Reserve_14:1;          
    uint8_t Reserve_15:1;          
    uint8_t Reserve_16:1;          

    uint8_t Reserve_17:1;          
    uint8_t Reserve_18:1;          
    uint8_t Reserve_19:1;          
    uint8_t Reserve_20:1;          
    uint8_t Reserve_21:1;          
    uint8_t Reserve_22:1;          
    uint8_t Reserve_23:1;          
    uint8_t Reserve_24:1;          
} SystemBitVars_TypeDef;

typedef struct 
{
    SystemBitVars_TypeDef BitVars;
    uint32_t BoardNumber;       // Номер транспортного средства 
    uint32_t IndexRead;			// Индекс события для чтения
    uint32_t IndexWrite;		// Индекс события для записи
    char CelNumber[32];         // Номер телефона для отправки SMS
    uint32_t SMSInt;            // Интервал автоматической отсылки SMS (minutes)
    uint32_t QtyPPD;            // Общее количество датчиков присутствия пассажира 
    uint32_t BatTresh;          // ? 
    uint32_t TimeSeats;         // Время, через которое после нажатия на кнопку открытия двери происходит запись события 
    uint32_t DVal_10; 
    uint32_t DVal_11; 
    uint32_t LCD_Contrast;
    uint32_t DVal_13; 
    uint32_t DVal_14; 
    uint32_t DVal_15;
    uint32_t DVal_16;
    uint32_t DVal_17;
    uint32_t DVal_18;
    uint32_t DVal_19;
    uint32_t DVal_20;
    uint32_t DVal_21;
    uint32_t DVal_22;
    uint32_t DVal_23;
    uint32_t DVal_24;
    uint32_t DVal_25;
    uint32_t DVal_26;
    uint32_t DVal_27;
    uint32_t DVal_28;
    uint32_t DVal_29;
    uint32_t DVal_30;
    uint32_t Minuend;
    uint32_t Subtrahend;
} SystemParams_TypeDef;

typedef struct
{
    uchar CmdCode;
    uchar UartNumber;
    uchar SrcAdr;
} SMSParams_Typedef;
    
typedef enum 
{
  CP_Idle,
  CP_WakeUp,
  CP_IntoLine,
  CP_Scream,
  CP_Pause,
  CP_Alarm,
  CP_Start,
  CP_Connect,
  CP_Synchro,
  CP_GoSleep,
  CP_AtEase,
  CP_Complete
} CountPPDState_TypeDef;
    
typedef enum 
{
  GI_Idle,
  GI_FindEndEvent,
  GI_FindStartEvent,
  GI_CalculateStops,
  GI_CalculatePassengers,
  GI_WriteOtherData,
  GI_Complete
} GetInfoDTState_TypeDef;

enum { STAY = 0, MOVE = !STAY };
enum { SND_ON = 0, SND_OFF = !SND_ON };
enum { NOSWITCH = 0, SWITCH = !NOSWITCH };
enum { NOCHECKNET = 0, CHECKNET = !NOCHECKNET };
enum { NOWAKE = 0, WAKE = !NOWAKE };
enum { FIRST_EARLIER = 0, FIRST_LATER, DT_EQUAL };

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void init_Leds(void);
void init_UserButton(InputMode_TypeDef Button_Mode);
void init_TimerTIM6(uint32_t tick_value, uint32_t period);
void init_TimerTIM7(uint32_t tick_value, uint32_t period);
void init_Sensors(void);
void init_Keyboard(void);
void init_Outputs(void);
void init_DebugBoard(void);
void cpl(void);
void blink(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t period, uchar snd);
void beep(uint time);
uint32_t readWord_FLASH(uint32_t Address); 
FLASH_Status writeWord_FLASH(uint32_t Address, uint32_t Data); 
void writeSystemParams (uint32_t adr, SystemParams_TypeDef* defaultStruct);
void writeUserParams(SystemParams_TypeDef* defaultStruct);
void pinConfig_LCD_MTC(void);
void set_ByteOnPins(GPIO_TypeDef* GPIOx, uint16_t PinsSelected, uint8_t Mask, uint8_t Byte);
uchar getKey(void);
uint setCharValue(uchar h, uchar p, uchar val);
uint setIntValue(uchar h, uchar p, uint val);
void showInputs(void);
void writeDefaultParams(void);
void setUserParams(void);
FunctionalState testUserData(void);
FunctionalState testDefaultData(void);
void setMMDirection(void);
void setMMTemp(void);
void setMMTempOtm(void);
void setMMTimeOfHold(void);
void setRMMainSteps(void);
void setRMSideSteps(void);
void setRMDirection(void);
void setRMTemp(void);
void setRMTimeOfHold(void);
void setPullMainSteps(void);
void setPullSideSteps(void);
void setPullNumber(void);
void setRewSteps(void);
ulong getTickCnt(void); 
ulong rnd(void); 
ErrorStatus getData(uchar *buf, ulong adr, uint size); 
ErrorStatus setData(uchar *buf, ulong adr, uint size); 
void printf_Consol(const char *pcFmt, ...);
void enableRFInterrupts(void);
void disableRFInterrupts(void);
void displayError(uint err);
ErrorStatus addEvent(TEvent *pevent);
ulong countQtyEvents(void);
ErrorStatus getEvent(TEvent *pevent, uint32_t id, uint8_t move_index);
void prepareSMS(uchar cmd, uchar *idata,  uchar* odata);
ErrorStatus clearHistory(void);
ErrorStatus clearEvents(void);
ErrorStatus getDateTime(DataTime_TypeDef *dt);
ErrorStatus setDateTime(DataTime_TypeDef dt);
uchar bcd2bin(uchar value);
uchar bin2bcd(uchar value);
void showTime(void);
void encryptData(uchar *buf, uint size);
void decryptData(uchar *buf, uint size);
void viewDump_Consol(uchar sym, uchar *adr, uint len);
uint crcEnc(uchar bt, uint ocrc);
u32 random (u32 min, u32 max);
u16 handler_CountPPD(u8 num, u8 wake);
u16 countPPD(CountPPDState_TypeDef *state);
u8 getMaxNumPPD(void);
u8 getNextNumPPD(u8 curnum);
ErrorStatus checkNumPPD(u8 num);
void awakePPD(void);
void checkEvent(void);
void getInfoSeats(void);
void bkl(void);
u16 handler_GetInfoDT(TReqGetInfoDT *rgidt);
u16 getInfoDT(TReqGetInfoDT *rgidt, u32 *index, GetInfoDTState_TypeDef *state);
u8 compareDateTime(DataTime_TypeDef *dt1, DataTime_TypeDef *dt2);
double calculateTime(DataTime_TypeDef *dt1, DataTime_TypeDef *dt2);
void convertLocM2M(TGoogleGPS *gps, TLocation *loc);
void checkGPS(uchar sec);

#endif /* __FUNCTIONS_H */

