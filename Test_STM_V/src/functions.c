 
/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include "functions.h"
#include "includes.h"
#include "stm32f10x_it.h"
#include "stm32f10x_usart.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum 
{
  FAILED = 0, 
  PASSED = !FAILED
} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t EraseCounter = 0x00;
__IO uint32_t NbrOfPage = 0x00;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;
const int32_t FactoryMinuend;
const int32_t FactorySubtrahend;
uint8_t Sen_1_pr = 0; // predydushee sostoyanie datchika
uint8_t Sen_2_pr = 0;
uint8_t Sen_3_pr = 0;
uint8_t Sen_4_pr = 0;
vu8 Times = 0;
vu16 GoodPPD = 0;
vu8 RepeatSleep = 0;
double OdStart = 0; 
double OdEnd = 0;
double PasCount = 0;
double QtyAutoEvns = 0;
DataTime_TypeDef FixedTimeOfStop;
double FixedOdoOfStop;
TEvent Evn;
double AverPas;
u8 ErrN = 0xff;//NO_ERROR;

// Нелинейная функция
const uchar NLF[32] = {
  0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
  0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
  0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00
};

/* Extern variables ----------------------------------------------------------*/

/** 
  * @brief      Инициализация выходов для управления светодиодами
  * @param      None
  * @retval     None
  */
void init_Leds(void)
{
  GPIO_InitTypeDef gpio;

  /* Enable the GPIOs Clock  - port C */
  RCC_APB2PeriphClockCmd(LEDS_GPIO_CLK, ENABLE);
  
  /* Configure the LEDS pins */
  gpio.GPIO_Pin = LEDS;

  // Init I/O port 
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;

  // Инициализируем GPIO на порту C 
  GPIO_Init(LEDS_GPIO_PORT, &gpio);

  // Гасим
  GPIO_ResetBits(LEDS_GPIO_PORT, LEDS);
}

/** 
  * @brief      Инициализация входа для кнопки UserButton
  * @param  Button_Mode: Specifies Button mode.
  *         This parameter can be one of following parameters:   
  *     @arg INPUT_MODE_GPIO: Button will be used as simple IO 
  *     @arg INPUT_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                             generation capability  
  * @retval     None
  */
void init_UserButton(InputMode_TypeDef Button_Mode)
{
  GPIO_InitTypeDef gpio;
  EXTI_InitTypeDef exti;
  NVIC_InitTypeDef nvic;
  
  /* Enable the BUTTON Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  /* Конфигурируем порт А на вход (Button pin as input floating) */
  GPIO_StructInit(&gpio);           // Fills each [gpio] with its default value
//  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  gpio.GPIO_Pin = USERBUTTON;
  //gpio.GPIO_Speed = GPIO_Speed_2MHz;

  // Инициализируем GPIO на порту A 
  GPIO_Init(USERBUTTON_GPIO_PORT, &gpio);  
  
  // Configure UserButton as EXTI line
  if (Button_Mode == INPUT_MODE_EXTI)
  {
    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    /* Configure Button EXTI line */
    exti.EXTI_Line = EXTI_Line0;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Rising;  
    exti.EXTI_LineCmd = ENABLE;

    EXTI_Init(&exti);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    nvic.NVIC_IRQChannel = EXTI0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0x0F;
    nvic.NVIC_IRQChannelSubPriority = 0x0F;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvic); 
  }
}

/** 
  * @brief      Инициализация базового таймера TIM6
  *             Использеуется для регулировки контрастности дисплея
  * @param      tick_value - время инкрементирования счётчика таймера [10 us]
  *             period - период срабатывания таймера
  * @retval     None
  */
void init_TimerTIM6(uint32_t tick_value, uint32_t period)
{
  // Затактируем таймер
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  // Вычисляем делитель
  uint32_t Divisor = (SystemCoreClock / 1000 / 100) * tick_value;   // срабатывание каждые [10 us * tick_value]

  // Корректируем значения period по необходимости
  if (period < LCDCONTRASTMINVALUE) period = LCDCONTRASTMINVALUE;
  if (period > LCDCONTRASTMAXVALUE) period = LCDCONTRASTMAXVALUE;
  
  // Задаём период срабатывания таймера для обеспечения контрастности дисплея
  uint PeriodM;
  
  switch (period)
  {
    case 1: PeriodM = 50; break;
    case 2: PeriodM = 25; break;
    case 3: PeriodM = 18; break;
    case 4: PeriodM = 12; break;
    case 5: PeriodM = 10; break;
    case 6: PeriodM = 9; break;
    case 7: PeriodM = 8; break;
    case 8: PeriodM = 7; break;
    case 9: PeriodM = 6; break;
    case 10: PeriodM = 4; break;
    default: PeriodM = 10; break;
  }
  
  // Инициализируем базовый таймер TIM6: 
  // делитель , период.
  // Другие параметры структуры TIM_TimeBaseInitTypeDef
  // не имеют смысла для базовых таймеров.
  TIM_TimeBaseInitTypeDef base_timer;
  TIM_TimeBaseStructInit(&base_timer);
  
  base_timer.TIM_Prescaler = Divisor - 1; // Делитель учитывается как TIM_Prescaler + 1, поэтому отнимаем 1
  base_timer.TIM_Period = PeriodM;
  TIM_TimeBaseInit(TIM6, &base_timer);

  // Разрешаем прерывание по обновлению (в данном случае -
  // по переполнению) счётчика таймера TIM6
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  
  // Включаем таймер
  TIM_Cmd(TIM6, ENABLE); 
 
  // Разрешаем обработку прерывания по переполнению счётчика таймера TIM6
  NVIC_EnableIRQ(TIM6_DAC_IRQn); 
}

/** 
  * @brief      Инициализация базового таймера TIM7
  *             Использеуется для обработки клавиатуры
  * @param      tick_value - время в мс инкрементирования счётчика таймера [10 us]
  *             period - период срабатывания таймера
  * @retval     None
  */
void init_TimerTIM7(uint32_t tick_value, uint32_t period)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  
  uint32_t divisor = ((SystemCoreClock / 1000) / 100) * tick_value;   // срабатывание каждые [0,01 ms * tick_value]
  
  TIM_TimeBaseInitTypeDef base_timer;
  TIM_TimeBaseStructInit(&base_timer);
  
  base_timer.TIM_Prescaler = divisor - 1; 
  base_timer.TIM_Period = period;
  TIM_TimeBaseInit(TIM7, &base_timer);

  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  
  TIM_Cmd(TIM7, ENABLE); 
 
  NVIC_EnableIRQ(TIM7_IRQn); 
}

/** 
  * @brief      Инициализация входов для датчиков SenReady и SenStart
  * @param      None
  * @retval     None
  */
void init_Sensors(void)
{
  GPIO_InitTypeDef gpio;

  /* Enable the GPIOs clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Fill each [gpio] with its default value */
  GPIO_StructInit(&gpio);            

  /* Конфигурируем порт B на вход с подтяжкой на "-" (открытый эмиттер оптопар)*/
  gpio.GPIO_Pin = SEN_1 | SEN_2; //| SEN_4; // Ошибочное использование порта 
  gpio.GPIO_Mode = GPIO_Mode_IPD;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(SENSORS_GPIO_PORT_1, &gpio);

  /* Конфигурируем порт A на вход с подтяжкой на "-" (открытый эмиттер оптопар)*/
  /** Примечание. Ошибочно использовал задействованный в плате STM32VLD порт PA15 */
  /*
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  gpio.GPIO_Pin = SEN_3;
  gpio.GPIO_Mode = GPIO_Mode_IPD;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(SENSORS_GPIO_PORT_2, &gpio);
  */

  /* Конфигурируем порт C на вход с подтяжкой на "-" (открытый эмиттер оптопар)*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  gpio.GPIO_Pin = SEN_3 | SEN_4;
  gpio.GPIO_Mode = GPIO_Mode_IPD;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(SENSORS_GPIO_PORT_2, &gpio);
}

/** 
  * @brief      Инициализация входов для кнопок клавиатуры
  * @param      None
  * @retval     None
  */
void init_Keyboard(void)
{
  GPIO_InitTypeDef gpio;

  /* Enable the GPIOs clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_StructInit(&gpio);            // Fills each [gpio] with its default value

  /* Конфигурируем порт B на вход с подтяжкой на "-" (открытый эмиттер оптопар)*/
  gpio.GPIO_Pin = KEYBOARD;
  gpio.GPIO_Mode = GPIO_Mode_IPD;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(KEYBOARD_GPIO_PORT, &gpio);  

  /* Конфигурируем порт C на вход с подтяжкой на "-" для кнопки [UP] */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  gpio.GPIO_Pin = KEY_UP;
  gpio.GPIO_Mode = GPIO_Mode_IPD;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &gpio);  
}

/** 
  * @brief      Инициализация дискретных выходов и буззера
  * @param      None
  * @retval     None
  */
void init_Outputs(void)
{
  GPIO_InitTypeDef gpio;

  /* Enable the GPIOs Clock  - port C, port D */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  
  /* Configure the output pins */
  gpio.GPIO_Pin = OUT_1 | OUT_2 | OUT_3;

  // Init I/O port 
  gpio.GPIO_Mode = GPIO_Mode_Out_OD;    // открытый коллектор
  gpio.GPIO_Speed = GPIO_Speed_50MHz;

  // Инициализируем GPIO на порту C 
  GPIO_Init(OUTPUTS_GPIO_PORT, &gpio);
  
  // Выставляем выходы в 1, чтобы выходы находились в 3-м состоянии
  GPIO_SetBits(OUTPUTS_GPIO_PORT, (OUT_1 | OUT_2 | OUT_3));

   /* Configure the sound pin */
  gpio.GPIO_Pin = OUT_SND;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;    // push-pull
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SND_GPIO_PORT, &gpio);
  off(SND);
  off(BKL);
}

/** 
  * @brief      Инициализация портов для управления платой DebugBoard
  *             (кроме SPI интерфейса)
  *             На плате DebugBoard: УКВ модуль MRF49XA 
  *             и внешняя Flash память AT45DB161D
  * @param      None
  * @retval     None
  */
void init_DebugBoard(void)
{
  GPIO_InitTypeDef gpio;
#ifdef __RFINT_ENABLED__
  EXTI_InitTypeDef exti;
  NVIC_InitTypeDef nvic;
#endif  

  /* Enable the GPIOs Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE); 

  // Прерывания - это альтернативная функция порта
  // поэтому надо необходимо включить тактирование AFIO
#ifdef __RFINT_ENABLED__
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
#endif  

  /* Fill each [gpio] with its default value */
  GPIO_StructInit(&gpio);     //  GPIO_Pin_All, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING

  // Конфигурируем входы порта А и C значениями по умолчанию
  gpio.GPIO_Pin = RF_INT;
  GPIO_Init(DBOARD_GPIO_PORT_1, &gpio);  
  // Конфигурируем входы порта C
  gpio.GPIO_Pin = RF_FINT | RF_CLK;
  GPIO_Init(DBOARD_GPIO_PORT_2, &gpio);  
//------------ Этот выход ломает IRO -------------------------------------------
  // Конфигурируем выход RF_RST порта С 
  GPIO_StructInit(&gpio);
  gpio.GPIO_Pin = RF_RST; 
  //gpio.GPIO_Mode = GPIO_Mode_Out_OD;      // не получается (
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // работает как вход для POR
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &gpio);
  // Устанавливаем 1 (неактивный уровень сигнала RESET)
  GPIO_SetBits(GPIOC, RF_RST);
//------------------------------------------------------------------------------
  // Конфигурируем выход RF_FSEL (порт А) 
  GPIO_StructInit(&gpio);
  gpio.GPIO_Pin = RF_FSEL; 
  gpio.GPIO_Mode = GPIO_Mode_Out_OD;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DBOARD_GPIO_PORT_1, &gpio);
  GPIO_ResetBits(DBOARD_GPIO_PORT_1, RF_FSEL);
  /* Конфигурируем вход внешнего прерывания от RF модуля*/
  GPIO_StructInit(&gpio);
  gpio.GPIO_Pin = RF_IRO;
  gpio.GPIO_Mode = GPIO_Mode_IPU;       // Pull-up, вход с подтяжкой к питанию
  GPIO_Init(DBOARD_GPIO_PORT_1, &gpio);  

#ifdef __RFINT_ENABLED__
  /* Конфигурируем вход прерывания от микросхемы MRF49XA как EXTI line */
  // Connect EXTI Line to RF_IRO GPIO Pin (port A line 8 
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);

  // Deinitialize the EXTI peripheral registers to their default reset values
  EXTI_DeInit();
  
  // Configure RF_IRO EXTI line */
  exti.EXTI_Line = RF_EXTI_LINE;
  exti.EXTI_Mode = EXTI_Mode_Interrupt;
  exti.EXTI_Trigger = EXTI_Trigger_Falling;  // Срабатывание по заднему фронту
  exti.EXTI_LineCmd = ENABLE;

  EXTI_Init(&exti);

  /* Enable and set RF_IRO EXTI Interrupt to the lowest priority */
  nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 0x0F;
  nvic.NVIC_IRQChannelSubPriority = 0x0F;
  nvic.NVIC_IRQChannelCmd = ENABLE;

  // Initialize the NVIC peripheral
  NVIC_Init(&nvic); 
  
  NVIC_EnableIRQ(EXTI9_5_IRQn);
#endif
}

/** 
  * @brief      Тестовый индикатор главного цикла программы
  * @param      None
  * @retval     None
  */
void cpl (void)
{
  BitAction BitVal;
  
  if (TimerCPL == 0) {
    // Мигание светодиодом LEDBLUE каждую секунду
    BitVal = !GPIO_ReadOutputDataBit(LEDS_GPIO_PORT, LEDBLUE) ? Bit_SET : Bit_RESET;
    GPIO_WriteBit(LEDS_GPIO_PORT, LEDBLUE, BitVal);
    // Мигание светодиодом LED_WORK каждую секунду
    BitVal = !GPIO_ReadOutputDataBit(OUTPUTS_GPIO_PORT, LED_WORK) ? Bit_SET : Bit_RESET;
    GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, BitVal);
    TimerCPL = 500;      // = 500 ms = 0.5 s
  }
}

/** 
  * @brief      Мигание светодиодом
  * @param      GPIOx - выходной порт, к которому подключен светодиод
  *             GPIO_Pin - номер вывода порта, к которому подключен светодиод
  *             period - период мигания светодиода (в мс)
  *             snd - 1 (буззер включается с периодом [period]), 0 (буззер не включается)
  * @retval     None
  */
void blink (GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t period, uchar snd)
{
  BitAction BitVal;
    
  if (TimerBlink == 0) {
    BitVal = !GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin) ? Bit_SET : Bit_RESET;
    GPIO_WriteBit(GPIOx, GPIO_Pin, BitVal);
    if ((BitVal == Bit_SET) && (snd == SND_ON)) { on(SND); }
    else { off(SND); }
    TimerBlink = period;      
  }
}

/** 
  * @brief      Включение буззера
  * @param      time - продолжительность звукового сигнала (в мс)
  * @retval     None
  */
void beep (uint time)
{
   on(SND);
   delay_1ms(time);
   off(SND);
}

/** 
  * @brief      Чтение данных из Flash 
  * @param      Address - адрес, где будет расположено слово
  * @retval     None
  */
uint32_t readWord_FLASH (uint32_t Address) 
{
  return (*(__IO uint32_t*) Address);
}

/** 
  * @brief      Запись 4 байт данных во внутреннюю Flash процессора 
  * @param      Address - адрес, где будет расположено слово
  *             Data - данные для записи
  * @retval     FLASH Status: The returned value can be: 
  *             FLASH_ERROR_PG - ошибка: запись в Flash запрещена,
  *             FLASH_ERROR_WRP - ошибка: , 
  *             FLASH_COMPLETE - запись завершена успешно 
  *             or FLASH_TIMEOUT - ошибка: закончилось время для записи.
  */
FLASH_Status writeWord_FLASH (uint32_t Address, uint32_t Data)
{
  FLASH_Status Res;
  uchar PageNumber;
  static uint32_t SaveData[256];
  uint32_t AddrSaveData;
  uint i;
  uint32_t adr;

  // Проверка адреса хранения данных
  if ((Address < ADR_DEFAULTVALS_BASE) || ((Address + 4) > ADR_END_FLASH)) 
  { return FLASH_ERROR_PG; }

  // Выделение номера страницы
  PageNumber = (Address - ADR_START_FLASH) / FLASH_PAGE_SIZE;

  AddrSaveData = (PageNumber * FLASH_PAGE_SIZE) + ADR_START_FLASH;
  adr = AddrSaveData;
  i = 0;
  // Запоминание всех предыдущих данных на странице, включая новое слово
  while(adr < (AddrSaveData + 1024))
  {
    if (adr == Address) { SaveData[i] = Data; }
    else { SaveData[i] = readWord_FLASH(adr); }
    adr += 4;
    i++;
  }
  /* Unlock the FLASH PECR register */
  FLASH_Unlock();
  /* Clear All pending flags */
  //FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
  // Erase a specified FLASH page
  FLASH_ErasePage(Address);
  // Записываем сохранённые данные
  adr = AddrSaveData;
  i = 0;
  while(adr < (AddrSaveData + 1024))
  {
    Res = FLASH_ProgramWord(adr, SaveData[i++]); 
    if (Res  != FLASH_COMPLETE) { return FLASH_ERROR_PG; }
    adr += 4;
  }
  
  /* Lock the FLASH PECR register */
  FLASH_Lock();
  
  return Res;
} 

/** 
  * @brief      Запись параметров по умолчанию в память FLASH
  * @param      adr - адрес для записи данных
  *             defaultStruct - указатель на структуру типа SystemParams_TypeDef
  * @retval     None
  */
void writeSystemParams (uint32_t adr, SystemParams_TypeDef* defaultStruct)
{
  uint32_t  *dataToWrite = (uint32_t *)&defaultStruct->BitVars;
  uint32_t  Address = adr;
  FLASH_Status Res;
  
  while (Address < (adr + sizeof(SystemParams_TypeDef))) {
    Res = writeWord_FLASH(Address, *dataToWrite++);
    if (Res != FLASH_COMPLETE) {
      displayError(Res);
      while(1) { // Stay in loop in case of critical system error
        blink(LEDS_GPIO_PORT, LEDGREEN, 50, SND_ON);
        blink(OUTPUTS_GPIO_PORT, LED_ERROR, 50, SND_ON);
      }
    }
    Address += 4;
  } 
}

/** 
  * @brief      Запись параметров пользователя в память FLASH
  * @param      defaultStruct - указатель на структуру типа SystemParams_TypeDef
  * @retval     None
  */
void writeUserParams (SystemParams_TypeDef* defaultStruct)
{
  writeSystemParams(ADR_USERPARAMS_BASE, defaultStruct);
}

/** 
  * @brief      Инициализация выходов для управления LCD дисплеем MTC-S16204XRGHS
  * @param      None
  * @retval     None
  */
void pinConfig_LCD_MTC (void)
{
  GPIO_InitTypeDef gpio;
  
  // Enable the GPIOs clocks - port B 
  RCC_APB2PeriphClockCmd(MTC_GPIO_CLK, ENABLE);
  
  // Configure pins as outputs open-drain nopull
  gpio.GPIO_Pin = MTCBITS;
  gpio.GPIO_Mode = GPIO_Mode_Out_OD; 
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  
  // Инициализируем GPIO на порту B
  GPIO_Init(MTC_GPIO_PORT, &gpio);  

  // Для V0 - аналогично
  RCC_APB2PeriphClockCmd(MTC_GPIO_V0_CLK, ENABLE);
  gpio.GPIO_Pin = MTC_V0;
  GPIO_Init(MTC_GPIO_PORT_V0, &gpio);  
 
  // Сбрасываем биты
  GPIO_ResetBits(MTC_GPIO_PORT, MTCBITS);
  GPIO_ResetBits(MTC_GPIO_PORT_V0, MTC_V0);
}

/** [duke] 
  * @brief      Установка на любые 8 выводов (16-pins GPIO) байта: 8 бит
  * @param      GPIOx - выбранный порт
  *             PinsSelected - выбранные выводы порта
  *             Mask - маска на порт (определяет 8 бит для установки байта)
                Byte - выводимый байт
  * @retval     None
  */
void set_ByteOnPins (GPIO_TypeDef* GPIOx, uint16_t PinsSelected, uint8_t Mask, uint8_t Byte)
{
  uint16_t tmp_16;
  uint8_t tmp_8;
  BitAction BitVal;     // Bit_SET или Bit_RESET
  static uint16_t Mask16Array[16] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, 
                            GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7,
                            GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11,
                            GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
  static uint8_t Mask8Array[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
  uint16_t PinsArray[8] = NULL;
  volatile uint8_t i;  
  uint8_t j;  
  uint8_t k = 0;
  
  // Определяем, какие пины необходимо использовать
  for (i = 0; i < 8; i++) {
    for (j = k; j < 16; j++) {
      tmp_16 = PinsSelected & Mask16Array[j];
      if (tmp_16 != 0) {
        PinsArray[i] = Mask16Array[j];
        k = j + 1;
        break;
      }
    }
  }
  // Получили массив PinsArray[8], содержащий значения используемых пинов 
  // Теперь выставляем на пины нужные биты из Byte
  k = 0;
  for (i = 0; i < 8; i++) {
    if ((Mask & Mask8Array[i]) == 0) continue;     // Определяем размерность шины данных
    tmp_8 = Byte & Mask8Array[i];
    if (tmp_8 != 0)
      BitVal = Bit_SET;
    else
      BitVal = Bit_RESET;
    GPIO_WriteBit(GPIOx, PinsArray[k], BitVal);
    k++;
  }
}

/** [duke] 
  * @brief      Дать код клавиши
  * @param      None
  * @retval     1
  */
uchar getKey(void)
{
  //uchar value = 0;
  
  while(fl_KeyOn) {
    if(LastKey != ESCAPE) TimerMenuExit = 5000;     // 5s
  }
  while(1) {
    if(fl_GoExit) {
      fl_GoExit = 0; 
      //value = ESCAPE;
      return ESCAPE;
    }
    TimerMenuExit = 5000;
    if(LastKey) {
      //onBacklight(2000); // 2s					
      //value = LastKey;
      return LastKey;
    }
  }
    //return value;
}

/**  
  * @brief      Pokazyvaet na displee sostoyanie diskretnyh vhodov
  * @param      None
  * @retval     None
  */
void showInputs(void)
{
      uint8_t Sen_1 = SEN_1_STATE;
      uint8_t Sen_2 = SEN_2_STATE;
      uint8_t Sen_3 = SEN_3_STATE;
      uint8_t Sen_4 = SEN_4_STATE;
      
      if ((Sen_1 == 1) && (Sen_1_pr == 0)) {
        lcd_set_xy(0, 1);
        lcd_out("1");
      }
      else if ((Sen_1 == 0) && (Sen_1_pr == 1)) {
        lcd_set_xy(0, 1);
        lcd_out("0");
      }
      
      if ((Sen_2 == 1) && (Sen_2_pr == 0)) {
        lcd_set_xy(2, 1);
        lcd_out("1");
      }
      else if ((Sen_2 == 0) && (Sen_2_pr == 1)) {
        lcd_set_xy(2, 1);
        lcd_out("0");
      }
      
      if ((Sen_3 == 1) && (Sen_3_pr == 0)) {
        lcd_set_xy(4, 1);
        lcd_out("1");
      }
      else if ((Sen_3 == 0) && (Sen_3_pr == 1)) {
        lcd_set_xy(4, 1);
        lcd_out("0");
      }

      if ((Sen_4 == 1) && (Sen_4_pr == 0)) {
        lcd_set_xy(5, 1);
        lcd_out("1");
      }
      else if ((Sen_4 == 0) && (Sen_4_pr == 1)) {
        lcd_set_xy(5, 1);
        lcd_out("0");
      }

      Sen_1_pr = Sen_1;
      Sen_2_pr = Sen_2;
      Sen_3_pr = Sen_3;
      Sen_4_pr = Sen_4;
}

/** 
  * @brief      Сохраняет во FLASH параметры по умолчанию, если они не записаны
  * @param      None
  * @retval     None
  */
void writeDefaultParams(void)
{
  // Проверить, хранятся ли во FLASH параметры по умолчанию 
  if (testDefaultData() == DISABLE) {
    memset(&SysParams.BitVars, 0x00, 4);

    SysParams.BitVars.Version = VERSION;
    SysParams.BitVars.EncryptRF = ENCRYPT_RF;
    SysParams.BoardNumber = BOARD;
    SysParams.IndexRead = 0;
    SysParams.IndexWrite = 0;
    strcpy(SysParams.CelNumber, CELNUMBER);
    SysParams.SMSInt = SMSINT;
    SysParams.QtyPPD = QTYPPD;
    SysParams.BatTresh = BATTRESH;
    SysParams.TimeSeats = TIMEFORSEATS;    

    //----- reserve -----//
    SysParams.DVal_10 = 0; 
    SysParams.DVal_11 = 0;
    //--------------------//  

    SysParams.LCD_Contrast = LCDCONTRAST;

    //----- reserve -----//
    SysParams.DVal_13 = 0; 
    SysParams.DVal_14 = 0; 
    SysParams.DVal_15 = 0;
    SysParams.DVal_16 = 0;
    SysParams.DVal_17 = 0;
    SysParams.DVal_18 = 0;
    SysParams.DVal_19 = 0;
    SysParams.DVal_20 = 0;
    SysParams.DVal_21 = 0;
    SysParams.DVal_22 = 0;
    SysParams.DVal_23 = 0;
    SysParams.DVal_24 = 0;
    SysParams.DVal_25 = 0;
    SysParams.DVal_26 = 0;
    SysParams.DVal_27 = 0;
    SysParams.DVal_28 = 0;
    SysParams.DVal_29 = 0;
    SysParams.DVal_30 = 0;
   //--------------------//  

    SysParams.Minuend = MINUEND;
    SysParams.Subtrahend = SUBTRAHEND;
    
    // Записать в FLASH параметры по умолчанию
    writeSystemParams(ADR_DEFAULTVALS_BASE, &SysParams);
  }
}

/** 
  * @brief      Читает из FLASH параметры, установленные пользователем
  *             Если они не были установлены, используются параметры по умолчанию
  * @param      None
  * @retval     None
  */
void setUserParams(void)
{
   
  if (testUserData() == DISABLE) 
  { // Если пользовательем не были установлены параметры 
    SysParams = *((SystemParams_TypeDef *) ADR_DEFAULTVALS_BASE);
    writeSystemParams(ADR_USERPARAMS_BASE, &SysParams);
  }
  SysParams = *((SystemParams_TypeDef *) ADR_USERPARAMS_BASE);
}

/** 
  * @brief      Проверка наличия в FLASH заводских установок 
  * @param      None
  * @retval     ENABLE - в памяти FLASH хранятся заводские установки
  *             DISABLE - память "чистая"
  */
FunctionalState testDefaultData(void)
{
  int32_t testdiff;
  FunctionalState retval = DISABLE;
  
  // Читаем параметры из FLASH
  //SysParams = *DEFAULTVALS_DATA;
  SysParams = *((SystemParams_TypeDef *) ADR_DEFAULTVALS_BASE);
  
  // Проверяем реальность данных
  testdiff = SysParams.Minuend - SysParams.Subtrahend;
  // При самом первом запуске программы условие не будет соблюдено
  if (testdiff == DIFFERENCE) { retval = ENABLE; }
  
  return retval;
}

/** 
  * @brief      Проверка наличия в FLASH пользовательских установок 
  * @param      None
  * @retval     ENABLE - в памяти FLASH хранятся установки пользователя
  *             DISABLE - в памяти ещё не сохранены установки пользователя
  */
FunctionalState testUserData(void)
{
  int32_t testdiff;
  FunctionalState retval = DISABLE;
  
  // Читаем параметры из FLASH
  //SysParams = *USERPARAMS_DATA;
  SysParams = *((SystemParams_TypeDef *) ADR_USERPARAMS_BASE);
  
  // Проверяем реальность данных
  testdiff = SysParams.Minuend - SysParams.Subtrahend;
  if (testdiff == DIFFERENCE) { retval = ENABLE; }
  
  return retval;
}

/**  
  * @brief      Ustanovit' napravlenie vrasheniya Main Motor
  * @param      None
  * @retval     None
  */
void setMMDirection(void)
{
/*
  uint32_t param = Machine_params.MM_Direction;
  uint val = 0;
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu

  displayString(2, 1, "         000/001");
  
  do {
    val = setCharValue(0, 1, param);
  } while (val > 0x01);      // Ogranichivaem znachenie (0 ili 1)
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.MM_Direction = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' temp vrasheniya Main Motor (obratnaya velichina skorosti)
  * @param      None
  * @retval     None
  */
void setMMTemp(void)
{
/*
  uint32_t param = Machine_params.MM_SpeedInv;;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "        %d...%d", MAINMOTORMINSPEED, MAINMOTORMAXSPEED);
  displayString(2, 1, strtmp);
  
  do {
    val = setIntValue(0, 1, param);
  } while ((val < MAINMOTORMINSPEED) || (val > MAINMOTORMAXSPEED));      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.MM_SpeedInv = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' temp otmotki Main Motor (obratnaya velichina skorosti)
  * @param      None
  * @retval     None
  */
void setMMTempOtm(void)
{
/*
  uint32_t param = Machine_params.MM_RewindSpeed;;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "        %d...%d", MAINMOTORMINSPEED, MAINMOTORMAXSPEED);
  displayString(2, 1, strtmp);
  
  do {
    val = setIntValue(0, 1, param);
  } while ((val < MAINMOTORMINSPEED) || (val > MAINMOTORMAXSPEED));      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.MM_RewindSpeed = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' vremya uderjaniya Main Motor
  * @param      None
  * @retval     None
  */
void setMMTimeOfHold(void)
{
/*
  uint32_t param = Machine_params.MM_TimeOfHold;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "       %d-%d ms", 1, 5000);
  displayString(2, 1, strtmp);
  param /= 10;      // peres4ityvaem v [ms]
  
  do {
    val = setIntValue(0, 0, param);
  } while ((val < 1) || (val > 5000));      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    val = val * 10;       // TimeOfHold izmeryaetsya v [100 us]
    Machine_params.MM_TimeOfHold = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' kol-vo osnovnyh shagov Roll Motor
  * @param      None
  * @retval     None
  */
void setRMMainSteps(void)
{
/*
  uint32_t param = Machine_params.RM_MainSteps;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "        %d-%d", 0, 60000);
  displayString(2, 1, strtmp);
  
  do {
    val = setIntValue(0, 0, param);
  } while (val > 60000);      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.RM_MainSteps = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' kol-vo pobochnyh shagov Roll Motor
  * @param      None
  * @retval     None
  */
void setRMSideSteps(void)
{
/*
  uint32_t param = Machine_params.RM_SideSteps;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "        %d-%d", 0, 60000);
  displayString(2, 1, strtmp);
  
  do {
    val = setIntValue(0, 0, param);
  } while (val > 60000);      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.RM_SideSteps = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' napravlenie vrasheniya Roll Motor
  * @param      None
  * @retval     None
  */
void setRMDirection(void)
{
/*
  uint32_t param = Machine_params.RM_Direction;
  uint val = 0;
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu

  displayString(2, 1, "         000/001");
  
  do {
    val = setCharValue(0, 1, param);
  } while (val > 0x01);      // Ogranichivaem znachenie (0 ili 1)
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.RM_Direction = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' temp vrasheniya Roll Motor (obratnaya velichina skorosti)
  * @param      None
  * @retval     None
  */
void setRMTemp(void)
{
/*
  uint32_t param = Machine_params.RM_SpeedInv;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "        %d...%d", ROLLMOTORMINSPEED, ROLLMOTORMAXSPEED);
  displayString(2, 1, strtmp);
  
  do {
    val = setIntValue(0, 1, param);
  } while ((val < ROLLMOTORMINSPEED) || (val > ROLLMOTORMAXSPEED));      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.RM_SpeedInv = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' vremya uderjaniya Roll Motor
  * @param      None
  * @retval     None
  */
void setRMTimeOfHold(void)
{
/*
  uint32_t param = Machine_params.RM_TimeOfHold;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "       %d-%d ms", 1, 5000);
  displayString(2, 1, strtmp);
  param /= 10;      // peres4ityvaem v [ms]
  
  do {
    val = setIntValue(0, 0, param);
  } while ((val < 1) || (val > 5000));      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    val = val * 10;       // TimeOfHold izmeryaetsya v [100 us]
    Machine_params.RM_TimeOfHold = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' kol-vo osnovnyh shagov Main Motor
  * @param      None
  * @retval     None
  */
void setPullMainSteps(void)
{
/*
  uint32_t param = Machine_params.MM_MainSteps;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "        %d-%d", 0, 60000);
  displayString(2, 1, strtmp);
  
  do {
    val = setIntValue(0, 0, param);
  } while (val > 60000);      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.MM_MainSteps = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' kol-vo korotkih shagov Main Motor
  * @param      None
  * @retval     None
  */
void setPullSideSteps(void)
{
/*
  uint32_t param = Machine_params.MM_SideSteps;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "        %d-%d", 0, 60000);
  displayString(2, 1, strtmp);
  
  do {
    val = setIntValue(0, 0, param);
  } while (val > 60000);      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.MM_SideSteps = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' kol-vo korotkih protyajek
  * @param      None
  * @retval     None
  */
void setPullNumber(void)
{
/*
  uint32_t param = Machine_params.NumberOfBroaches;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu

  sprintf(strtmp, "          %d-%d", 0, 100);
  displayString(2, 1, strtmp);
  
  do {
    val = setCharValue(0, 1, param);
  } while (val > 0x64);      // Ogranichivaem znachenie (0 - 100)
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.NumberOfBroaches = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Ustanovit' kol-vo shagov otmotki nazad Main Motor pri na4ale dvijeniya mehanizma
  * @param      None
  * @retval     None
  */
void setRewSteps(void)
{
/*
  uint32_t param = Machine_params.MM_RewindSteps;
  uint val = 0;
  char strtmp[16];
  
  fl_EnDisp = 0;    // zapreshaem vyvod na display Glavnogo Menu
  sprintf(strtmp, "         %d-%d", 0, 1000);
  displayString(2, 1, strtmp);
  
  do {
    val = setIntValue(0, 0, param);
  } while (val > 1000);      // Ogranichivaem znachenie
  
  LastKey = ESCAPE;     // vosstanavlivaem kod najatoi klavishi
  fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Esli parametr byl izmenen, zapisyvaem v Flash
  if (val != param) {
    Machine_params.MM_RewindSteps = val;
    write_UserParams(&Machine_params);
  } 
*/
}

/**  
  * @brief      Izmenit' znachenie chisla razmerom 1 byte
  * @param      h - "hide": 0 - otobrajayutsya cifry
  *                         1 - otobrajayutsya "*"
  *             p - nachal'naya poziciya kursora 
  *             val - izmenyaemyi parametr
  * @retval     Ustanovlennoe znachenie parametra [uint]
  */
uint setCharValue(uchar h, uchar p, uchar val)
{
  uchar poz = 0;
  char str[4];
  char str1[4];
  char tmp[4];
    
    lcd_send(0x0d, COMMAND);	// vkl. miganie kursora										
	sprintf(str, "%03u", val);  
	memset(str1, 0, 6);         // obnulyaem str1
        
	while(1) {
		lcd_set_xy(p, 1);   // poziciya p vo vtoroi stroke
		if(!h) sprintf(tmp, "%s", str);
		else sprintf(tmp, "%s", str1);
        lcd_out(tmp);
		lcd_set_xy(poz + p, 1);
		switch(LastKey) {
			case UP:
				if(++str[poz] > 0x39) str[poz] = 0x30;
				str1[poz] = '*';
			break;
			case DOWN:
				if(--str[poz] < 0x30) str[poz] = 0x39;
				str1[poz] = '*';
			break;
			case ENTER:
				str1[poz] = '*';
				if((++poz) > 2) poz = 0;
			break;
			case ESCAPE:
				lcd_send(0x0c, COMMAND);	// vyklyuchili miganie kursora							
                LastKey = 0;                // sbrasyvaem kod klavishi, 4toby syuda ne vozvrashatsya
                return atoi((char *) str);
		}
        if(LastKey) LastKey = 0;    // sbrasyvaem kod klavishi, chtoby ne zaciklitsya
	}
}

/**  
  * @brief      Izmenit' znachenie chisla razmerom 4 byte
  * @param      h - "hide": 0 - otobrajayutsya cifry
  *                         1 - otobrajayutsya "*"
  *             p - nachal'naya poziciya kursora 
  *             val - izmenyaemyi parametr
  * @retval     Ustanovlennoe znachenie parametra [uint]
  */
uint setIntValue(uchar h, uchar p, uint val)
{
  uchar poz = 0;
  char str[6];
  char str1[6];
  char tmp[6];

    lcd_send(0x0d, COMMAND);	// vkl. miganie kursora										
	sprintf(str, "%05u", val);
	memset(str1, 0, 6);
    
	while(1) {
		lcd_set_xy(p, 1);   // poziciya p vo vtoroi stroke
		if(!h) sprintf(tmp, "%s", str);
		else sprintf(tmp, "%s", str1);
        lcd_out(tmp);
		lcd_set_xy(poz + p, 1);
		switch(LastKey) {
			case UP:
				if(++str[poz] > 0x39) str[poz] = 0x30;
				str1[poz] = '*';
			break;
			case DOWN:
				if(--str[poz] < 0x30) str[poz] = 0x39;
				str1[poz] = '*';
			break;
			case ENTER:
				str1[poz] = '*';
				if((++poz) > 4) poz = 0;
			break;
			case ESCAPE:
				lcd_send(0x0c, COMMAND);	// vyklyuchili miganie kursora							
                LastKey = 0;                // sbrasyvaem kod klavishi, 4toby syuda ne vozvrashatsya
                return atoi((char *) str);
		}
        if(LastKey) LastKey = 0;
	}
}

/** 
  * @brief      Запретить прерывания и дать значение системных тиков
  * @param      None
  * @retval     Значение системных тиков TickCnt
  */
ulong getTickCnt (void) 
{
  ulong value;

  disableInterrupts();  // Disable general interrupts
  value = TickCnt;
  enableInterrupts();   // Enable general interrupts
  return value;
}

/** 
  * @brief      Генерация псевдослучайного 4 байтного числа
  * @param      None
  * @retval     4-байтное псевдослучайное число
  */

ulong _Randseed = 1;
#define MKRND(a)	((a) * 1103515245L + 12345L)

ulong rnd (void) 
{
  ulong Val;
  
  Val = (ulong)TIM6->CNT;
  Val *= (ulong)TIM7->CNT;
  _Randseed = MKRND(_Randseed + Val);
  Val = MKRND(_Randseed * (ulong)TIM6->CNT);
  return Val;
}

/** 
  * @brief      Прочитать данные размером [size] по адресу [adr] в буфер [*buf]   
  * @param      *buf - указатель на буфер, куда копировать данные
  *             adr - адрес для чтения данных
  *             size - размер данных для чтения
  * @retval     SUCCESS (1) - данные успешно прочитаны
  *             ERROR (0) - ошибка
  */
ErrorStatus getData(uchar *buf, ulong adr, uint size) 
{
	ErrorStatus Res;
	uchar Hadr;
    
    // Выделение байта, содержащего информацию о физическом адресе
    Hadr = (uchar) ((adr >> 16) & 0xff);        

//--------------- Чтение из внутренней FLASH или SRAM ---------------------    
    
    // if (0x00 =< Hadr <= 0x01) -> read internal FLASH
    // Если диапазон адресов лежит в пределах 0x08000000...0x0801FFFF 
    // - обращение к внутренней FLASH MCU 128 kByte

    uint Size = size;
    ulong Adr = adr;
    char *Buf = (char *)buf;
    
    if (Hadr <= ENDPOINT_FLASH) 
    {
      for (; 0 < Size; ++Buf, ++Adr, --Size) {
        *Buf = (*(__IO uint8_t*) Adr);          // читаем побайтно
      }
      return SUCCESS;
    }

//----------------- Чтение из внешней DataFlash --------------------------

    // if (0x40 =< Hadr <= 0x60) -> read external DataFlash
    // Если диапазон адресов лежит в пределах 0x40400000...0x40600000 
    // - обращение к внешней DataFlash AT45DB161D 16 Mbit
    
    if ((Hadr >= STARTPOINT_EXTFLASH) && (Hadr <= ENDPOINT_EXTFLASH)) 
    {
#ifdef __RFINT_ENABLED__
      disableRFInterrupts();
#endif
      Adr = adr - ADR_START_EXTFLASH; // Корректируем значение адреса для чтения из DataFlash
      Res = readData_AT45(buf, Adr, size);
#ifdef __RFINT_ENABLED__
      enableRFInterrupts();
#endif      
      return Res;
    }
      
//-------------- Указан неверный адрес для чтения данных ----------------
    disableInterrupts();   // Запретили прерывания
    displayError(ERR_ADR_READ_MEMORY);
    while(1) {               // Stay in loop in case of critical system error
      blink(LEDS_GPIO_PORT, LEDGREEN, 50, SND_ON);
      blink(OUTPUTS_GPIO_PORT, LED_ERROR, 50, SND_ON);
    }
}

/** 
  * @brief      Записать данные размером [size] по адресу [adr] из буфера [*buf]   
  * @param      *buf - указатель на буфер, откуда копировать данные
  *             adr - адрес памяти, куда записываются данные
  *             size - размер данных 
  * @retval     0 - ERROR (ошибка записи)
  *             1 - SUCCESS (данные успешно записаны в память)
  */
ErrorStatus setData(uchar *buf, ulong adr, uint size) 
{
	ErrorStatus Res;
	uchar Hadr;
    
    Hadr = (uchar) ((adr >> 16) & 0xff); // Выделяем байт физического адреса

//--------------- Запись во внутреннюю FLASH или SRAM ---------------------    
    
    // if (0x00 =< Hadr <= 0x01) -> write into internal FLASH
    // Если диапазон адресов лежит в пределах 0x08000000...0x0801FFFF 
    // - обращение к внутренней FLASH MCU 128 kByte
    if (Hadr <= ENDPOINT_FLASH) 
    {
      /*
      uint Size = size;
      char *Adr = (char *)adr; 
      const char *Buf = (const char *)buf;
      for (; 0 < Size; ++Buf, ++Adr, --Size) { 
        *Adr = *Buf;          // записываем побайтно
      }
      */
      ulong Adr = adr;
      uint Size = size;
      ulong *Buf = (ulong *)buf;
      
      for ( ; Size > 0; Buf++) {
        if (writeWord_FLASH(Adr, *Buf) != FLASH_COMPLETE) return ERROR;
        Adr += 4; 
        Size -= 4;
      }
      return SUCCESS;
    }

//----------------- Запись во внешнюю DataFlash --------------------------

    // if (0x40 =< Hadr <= 0x60) -> write external DataFlash
    // Если диапазон адресов лежит в пределах 0x40400000...0x40600000 
    // - обращение к внешней DataFlash AT45DB161D 16 Mbit
    
    if ((Hadr >= STARTPOINT_EXTFLASH) && (Hadr <= ENDPOINT_EXTFLASH)) 
    {
#ifdef __RFINT_ENABLED__
      disableRFInterrupts();
#endif
      // Корректируем значение адреса для записи в DataFlash
      Res = writeData_AT45(buf, (adr - ADR_START_EXTFLASH), size);
#ifdef __RFINT_ENABLED__
      enableRFInterrupts();
#endif      
      return Res;
    }

//-------------- Указан неверный адрес для записи данных -----------------
    disableInterrupts();   // Запретили прерывания
    displayError(ERR_ADR_WRITE_MEMORY);
    printf_Consol("Bad Address to write: Hadr 0x%08x", Hadr);
    while(1) {               // Stay in loop in case of critical system error
      blink(LEDS_GPIO_PORT, LEDGREEN, 50, SND_ON);
      blink(OUTPUTS_GPIO_PORT, LED_ERROR, 50, SND_ON);
    }
}

/**  
  * @brief      Writes output to the consol stream, under control of the string pointed to by
  *             format that specifies how subsequent arguments are converted for output. If 
  *             there are insufficient arguments for the format, the behavior is undefined.
  *             If the format is exhausted while arguments remain, the excess arguments are
  *             ignored.
  * @param      pcFmt - Format string. It can contain all the format specifies.
  *             ...   - Are the passed parameters (pointers to the objects to receive the
  *                      converted input).
  * @retval     None
  */
void printf_Consol(const char *pcFmt, ...)
{
#if __DEBUGPORT > 1
  return;
#endif

//====DEBUG HANG====
#if __DEBUGPORT == 0
  if ((UARTx[0].ReceivePack) || (UARTx[0].SendPack)) return; // Пакет в процессе приема или передачи
#elif __DEBUGPORT == 1 
  if ((UARTx[1].ReceivePack) || (UARTx[1].SendPack)) return; 
#endif
  
  va_list ap;
  char pbString[256];
  
  va_start(ap, pcFmt);
  vsprintf(pbString, pcFmt, ap);

#if __DEBUGPORT == 0
  sendStr_UART0(pbString);
#elif __DEBUGPORT == 1 
  sendStr_UART1(pbString);
#endif

  va_end(ap);
}

/** 
  * @brief      Разрешить прерывания от радиомодуля MRF49XA
  * @param      None
  * @retval     None
  */
void enableRFInterrupts (void)
{
  EXTI_InitTypeDef exti;

  // Configure RF_IRO EXTI line 
  exti.EXTI_Line = RF_EXTI_LINE;
  exti.EXTI_Mode = EXTI_Mode_Interrupt;
  exti.EXTI_Trigger = EXTI_Trigger_Falling;  // Срабатывание по заднему фронту
  exti.EXTI_LineCmd = ENABLE;

  EXTI_Init(&exti);
}

/** 
  * @brief      Запретить прерывания от радиомодуля MRF49XA
  * @param      None
  * @retval     None
  */
void disableRFInterrupts(void)
{
  EXTI_InitTypeDef exti;

  // Configure RF_IRO EXTI line 
  exti.EXTI_Line = RF_EXTI_LINE;
  exti.EXTI_LineCmd = DISABLE;

  EXTI_Init(&exti);
}

/** 
  * @brief      Высветить номер ошибки на дисплей и в консоль
  * @param      err - код ошибки (в соответствии с таблицей в main.h)
  * @retval     None
  */
void displayError(uint err)
{
  char String[16];
  
  // Включаем светодиод
  GPIO_ResetBits(OUTPUTS_GPIO_PORT, LED_ERROR);
  
  // Включаем подсветку дисплея на 5с
  fl_ShowTime = 0;
  STimerBacklight = 5;
  bkl();
  
  sprintf(String, "ERROR: %03d", err);
  displayString(1, 1, String);
  
  switch (err) {
    case ERR_EXEC:
      displayString(2, 1, "Ошибка выполн-я!");
      printf_Consol((const char *)"Ошибка выполнения команды (запроса). \r\n", String);
      break;
    
    case ERR_WRITE_FLASH: 
      displayString(2, 1, "Запись Flash");
      printf_Consol((const char *)"Ошибка записи во внешнюю FLASH. ", String);
      break;

    case ERR_READ_FLASH:
      displayString(2, 1, "Чтение Flash");
      printf_Consol((const char *)"Ошибка чтения из внешней FLASH. \r\n", String);
      break;

    case ERR_ADR_READ_MEMORY:
      displayString(2, 1, "Неверный адрес");    // неверный адрес для чтения данных
      printf_Consol((const char *)"Ошибка адреса для чтения данных из памяти. \r\n", String);
      break;
  
    case ERR_ADR_WRITE_MEMORY:
      displayString(2, 1, "Неверный адрес");    // неверный адрес для записи данных
      printf_Consol((const char *)"Ошибка адреса для записи данных в память. \r\n", String);
      break;

    case ERR_BAD_SIZE:
      displayString(2, 1, "Размер DATA");
      printf_Consol((const char *)"Ошибка размера данных в пакете. \r\n", String);
      break;

    case ERR_NODATA:
      displayString(2, 1, "Нет данных!");
      printf_Consol((const char *)"Нет данных для передачи!\r\n", String);
      break;
      
    case ERR_BAD_COMMAND:
      displayString(2, 1, "Неизв. команда");
      printf_Consol((const char *)"Ошибка: неизвестная команда. \r\n", String);
      break;
      
    case ERR_NOTSENT_SMS:
      displayString(2, 1, "SMS не отправл.");
      printf_Consol((const char *)"Ошибка: предыдущее SMS ещё не отправлено. \r\n", String);
      break;

    case ERR_SPI:
      displayCString(2, 1, (const char *)"SPI read/write");
      printf_Consol((const char *)"Ошибка обмена данными по SPI!\r\n", String);
      break;
      
    case ERR_I2C_WRITE:
      displayCString(2, 1, (const char *)"I2C write");
      printf_Consol((const char *)"Ошибка записи по I2C!\r\n", String);
      break;
      
    case ERR_I2C_READ:
      displayCString(2, 1, (const char *)"I2C read");
      printf_Consol((const char *)"Ошибка чтения по I2C!\r\n", String);
      break;

    case ERR_DF_WRITE:
      displayCString(2, 1, (const char *)"DataFlash write");
      printf_Consol((const char *)"Ошибка записи во внешнюю DataFlash!\r\n", String);
      break;
        
    case ERR_DF_READ:
      displayCString(2, 1, (const char *)"DataFlash read");
      printf_Consol((const char *)"Ошибка чтения из внешней DataFlash!\r\n", String);
      break;
      
    case ERR_GET_DT:
      displayCString(2, 1, (const char *)"Get Date-Time");
      printf_Consol((const char *)"Ошибка чтения даты и времени из часы-календаря!\r\n", String);
      break;
      
    case ERR_SET_DT:
      displayCString(2, 1, (const char *)"Set Date-Time");
      printf_Consol((const char *)"Ошибка записи даты и времени в часы-календарь!\r\n", String);
      break;
      
    case ERR_INIT_RF:
      displayCString(2, 1, (const char *)"Init MRF49XA");
      printf_Consol((const char *)"Ошибка инициалиации радиомодуля!\r\n", String);
      break;
      
    case ERR_RF_SEND:
      displayCString(2, 1, (const char *)"Send Data via RF");
      printf_Consol((const char *)"Ошибка отправки данных в радиомодуль!\r\n", String);
      break;
      
    case ERR_UART0_SB:
      displayCString(2, 1, (const char *)"Send Byte UART0");
      printf_Consol((const char *)"Ошибка отправки байта через UART0!\r\n", String);
      break;

    case ERR_UART1_SB:
      displayCString(2, 1, (const char *)"Send Byte UART1");
      printf_Consol((const char *)"Ошибка отправки байта через UART1!\r\n", String);
      break;
      
    case ERR_NOACK_PPD:
      displayCString(2, 1, (const char *)"PPD: no answer");
      printf_Consol((const char *)"Не получен ответ от PPD! \r\n", String);
      break;
      
    case ERR_NOACK_GPS:
      displayCString(2, 1, (const char *)"GPS: no answer");
      printf_Consol((const char *)"Не получен ответ от терминала GPS!\r\n", String);
      break;
      
    case ERR_FROM_PPD:
      ;
      break;
      
    case ERR_ERRANSWER:
      displayCString(2, 1, (const char *)"Error answer");
      break;
      
    case ERR_NOEXIST_PPD:
      displayCString(2, 1, (const char *)"No PPD synchro");
      break;

    case ERR_SEND_SMS:
      displayCString(2, 1, (const char *)"Can't send SMS");
      break;
      
    case ERR_NOCONNECT_PPD:
      displayCString(2, 1, (const char *)"No answer PPD");
      break;
      
    case ERR_READ_DF:
      displayCString(2, 1, (const char *)"Can't read Event");
      break;
      
    case ERR_FIND_EVENT:
      displayCString(2, 1, (const char *)"Can't find Event");
      break;
      
    case ERR_BADPARAM:
      displayCString(2, 1, (const char *)"Bad parameter");
      break;
      
    case ERR_WDT_RESET:
      displayCString(2, 1, (const char *)"WatchDog reset");
      printf_Consol("Сброс по WatchDog таймеру!\r\n");
      break;

    default:
      displayString(2, 1, "Неизв. ошибка");
      printf_Consol((const char *)"Неизвестная ошибка! Err %03d\r\n", err);
      break;
  }
}

/** 
  * @brief      Записать Событие в DataFlash
  * @param      *pevent - указатель на данные для записи в DataFlash
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus addEvent(TEvent *pevent)
{
  
  // Записываем ID события
  pevent->ID = SysParams.IndexWrite;
  
  // Записываем дату и время события
  getDateTime(&pevent->DataTime);
  
  // Считаем CRC события
  pevent->Crc = MakeCRC16((uchar *)pevent, (sizeof(TEvent) - 2));
  
  // Записываем данные События в DataFlash
  if (setData((uchar *)pevent, CurrentAddrEventDF, sizeof(TEvent)) == ERROR) 
  { return ERROR; }
  
  // Инкрементируем IndexWrite и записываем во FLASH
  if(++SysParams.IndexWrite > (__MAXQTYEVENTS - 1)) 
  { SysParams.IndexWrite = 0; }
  if (setData((uchar *)&SysParams.IndexWrite, ADR_INDEXWRITE, 4) == ERROR)
  { return ERROR; }

  // Подсчитываем новый адрес для записи транзакций
  CurrentAddrEventDF = StartAddrEventDF + (SysParams.IndexWrite * sizeof(TEvent));

  // Если индекс для записи "догнал" индекс для чтения, сдвигаем IndexRead
  // и записываем его во FLASH
  if (SysParams.IndexWrite == SysParams.IndexRead) {
    SysParams.IndexRead++;
    if (SysParams.IndexRead > (__MAXQTYEVENTS - 1)) { SysParams.IndexRead = 0; }
    if (setData((uchar *)&SysParams.IndexRead, ADR_INDEXREAD, 4) == ERROR)
    { return ERROR; }
  }
 
  // Подсчитываем количество невычитанных событий
  SysStat.NonReadEvents = countQtyEvents();
 
  return SUCCESS;
}

/** 
  * @brief      Посчитать количество невычитанных Событий 
  * @param      None
  * @retval     Количество невычитанных событий (4 Byte)
  */
ulong countQtyEvents(void)
{
    ulong Qty;

    if (SysParams.IndexWrite < SysParams.IndexRead) 
      Qty = (__MAXQTYEVENTS - SysParams.IndexRead + SysParams.IndexWrite);
    else Qty = SysParams.IndexWrite - SysParams.IndexRead;

    return Qty;
}

/** 
  * @brief      Прочитать Событие из DataFlash
  * @param      *pevent - указатель на область памяти, куда скопировать Событие
  *             id - идентификационный номер события
  *             move_index -    "0" указатель по чтению IndexRead не сдвигается
  *                             "1" указатель по чтению сдвигается
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus getEvent(TEvent *pevent, uint32_t id, uint8_t move_index)
{
  vu32 Adr = StartAddrEventDF + (id * sizeof(TEvent));
  
  // Проверяем ID 
  if (id > __MAXQTYEVENTS - 1) { return ERROR; }
  
  if (getData((uchar *)pevent, Adr, sizeof(TEvent)) == ERROR)
  { return ERROR; }
  
  if (id != 0)
  { // Проверяем, то ли мы вычитали
    if (pevent->ID != id)  { 
      return ERROR; 
    }
  }
  
  // Считаем CRC и проверяем
  if (MakeCRC16((uchar *)pevent, (sizeof(TEvent) - 2)) != pevent->Crc) { return ERROR; }
  
  // Инкрементируем IndexRead и записываем во FLASH
  if (move_index) {
    if (++SysParams.IndexRead > (__MAXQTYEVENTS - 1)) { SysParams.IndexRead = 0; }
    if (setData((uchar *)&SysParams.IndexRead, ADR_INDEXREAD, 4) == ERROR)
    { return ERROR; }
  }

  // Подсчитываем количество невычитанных событий
  SysStat.NonReadEvents = countQtyEvents();
 
  return SUCCESS;
}

/** 
  * @brief      Подготовить SMS 
  * @param      *ptr - указатель на данные для формирования SMS
  *             cmd - код команды для формирования SMS
  *             uart_number -   номер UART, откуда пришёл запрос на SMS
  *             src_adr - сетевой адрес отправителя запроса
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
void prepareSMS (uchar cmd, uchar *idata,  uchar* odata)
{   
  char Message[140];
  TEvent *Event = (TEvent *) idata;
  ulong *QtyNRE = (ulong *) idata;
  DataTime_TypeDef *DT = (DataTime_TypeDef *) idata;
  TSystemStatus *SysStat = (TSystemStatus *) idata;
  TSendSMS *DataForSMS = (TSendSMS *) odata;
  
  // Очистка буфера для вывода
  memset(Message, 0x00, sizeof(Message));
  memset(DataForSMS, 0x00, sizeof(TSendSMS));
  
  // Готовим текст сообщения
  switch (cmd)
  { 
    case REQ_GET_LAST_EVN:  // Передать Cобытие
    case REQ_GET_SPEC_EVN:
      sprintf(Message, 
"ID %d; Date %02u/%02u/%02u; Time %02u:%02u:%02u; Coord %02d°%02d'%02d.%02d\"N, %d%02d°%02d'%02d.%02d\"E; Occup %u; Free %u; Failed %u; Odometer %fkm; Speed %.1fkm/h; Doors %1x\n", 
Event->ID,
Event->DataTime.Day, Event->DataTime.Month, Event->DataTime.Year,
Event->DataTime.Hour, Event->DataTime.Min, Event->DataTime.Sec,
Event->Coordinates.Lat_Degree, Event->Coordinates.Lat_Min, 
Event->Coordinates.Lat_Sec, Event->Coordinates.Lat_FrSec,
Event->Coordinates.Lon_Degree1, 
Event->Coordinates.Lon_Degree2, Event->Coordinates.Lon_Min,
Event->Coordinates.Lon_Sec, Event->Coordinates.Lon_FrSec,
Event->OccupSeats, Event->FreeSeats, Event->FailedPPD,
Event->Mileage / 1000, (double)Event->Speed * 1.852, !Event->AutoSave);
      break;
    
    case REQ_QTY_EVN:   // Передать количество невычитанных событий
      sprintf(Message, "Qty of non-read Events = %u\n", *QtyNRE); 
      break;
    
    case REQ_GET_DATETIME:  // Передать системные дату и время
      sprintf(Message, "Date %02u/%02u/20%02u, Time %02u:%02u:%02u\n",
             DT->Day, DT->Month, DT->Year, DT->Hour, DT->Min, DT->Sec); 
      break;

    case REQ_GET_STATE:  // Передать состояние системы
      sprintf(Message, 
"Version %u.%1u.%1u; Date %02d/%02d/20%02d; Time %02d:%02d:%02d; Coord %02d°%02d'%02d.%02d\"N, %d%02d°%02d'%02d.%02d\"E; Speed %.1fkm/h; Occup %u; Free %u; Good %u; Failed %u; Non-read evn %u\n",
SysStat->Version/100,(SysStat->Version%100)/10, SysStat->Version%10,
SysStat->DataTime.Day, SysStat->DataTime.Month, SysStat->DataTime.Year,
SysStat->DataTime.Hour, SysStat->DataTime.Min, SysStat->DataTime.Sec,
SysStat->Coordinates.Lat_Degree, SysStat->Coordinates.Lat_Min,
SysStat->Coordinates.Lat_Sec, SysStat->Coordinates.Lat_FrSec,
SysStat->Coordinates.Lon_Degree1,
SysStat->Coordinates.Lon_Degree2, SysStat->Coordinates.Lon_Min,
SysStat->Coordinates.Lon_Sec, SysStat->Coordinates.Lon_FrSec, 
(double)SysStat->Speed * 1.852,
SysStat->OccupSeats, SysStat->FreeSeats, 
SysStat->GoodPPD, SysStat->FailedPPD, SysStat->NonReadEvents); 
      break;
    
    default:
      memset(&DataForSMS->ShortMes, '\n', 1);
      memset(&DataForSMS->CelNumber, '\n', 1);
      break;
  }
  
  // Заполняем структуру для передачи SMS
  DataForSMS->Len = strlen(Message) - 1; // Не включая символ "\n"
  memcpy(&DataForSMS->ShortMes, Message, strlen(Message));
  memcpy(&DataForSMS->CelNumber, &SysParams.CelNumber, strlen(SysParams.CelNumber));
}

/** 
  * @brief      Очистить историю Cобытий 
  * @param      None
  * @retval     0 - ERROR
  *             1 - SUCCESS
  * @notes      Сбрасываются указатели чтения и записи Событий.
  *             Физически все данные Событий в памяти остаются.
  */
ErrorStatus clearHistory(void)
{
  // Обнулить IndexTARead  и записать во FLASH
  SysParams.IndexRead = 0;
  if (setData((uchar *)&SysParams.IndexRead, ADR_INDEXREAD, 4) == ERROR)
  { return ERROR; }
  // Обнулить IndexTAWrite  и записать в DATAFLASH
  SysParams.IndexWrite = 0;
  if (setData((uchar *)&SysParams.IndexWrite, ADR_INDEXWRITE, 4) == ERROR)
  { return ERROR; }
  // Сбросить адрес начала записи транзакций
  CurrentAddrEventDF = StartAddrEventDF;
  // Сбросить счётчик невычитанных транзакций
  SysStat.NonReadEvents = 0;
  printf_Consol("История Событий очищена!");
  return SUCCESS;
}

/** 
  * @brief      Очистить События 
  * @param      None
  * @retval     0 - ERROR
  *             1 - SUCCESS
  * @notes      Сбрасываются указатели чтения и записи Событий.
  *             Все данные Событий в памяти стираются.
  */
ErrorStatus clearEvents(void)
{
  ulong i;
  ulong QtyErased = 0;
  TEvent TmpEvent;
  uchar *pTmpEvent = (uchar *)&TmpEvent;

  // Стираем События в памяти
  for(i = 0; i < sizeof(TEvent); i++) { *(pTmpEvent + i) = 0; }
  for(i = StartAddrEventDF; i < ADR_END_EXTFLASH; i += sizeof(TEvent))
  {
    if (setData(pTmpEvent, i, sizeof(TEvent))) { return ERROR; }
    QtyErased++;
  }
  printf_Consol("Количество стертых Событий: %d\n", QtyErased);
  // Обнуляем указатели и счётчики
  return clearHistory(); 
}

/** 
  * @brief      Прочитать системные дату и время  
  * @param      *dt - указатель на переменную, куда передаются дата и время
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus getDateTime (DataTime_TypeDef *dt)
{
  ushort Year;
  ErrorStatus RetVal;
  
  disableInterrupts();  //====DEBUG HANG====
  
  if ((getDate_PCF8563T(&dt->Day, &dt->Month, &Year) == ERROR) ||
       (getTime_PCF8563T(&dt->Hour, &dt->Min, &dt->Sec) == ERROR))
  { RetVal =  ERROR; }
  else
  { 
    if (Year < 2000) { Year -= 1900; }
    else { Year -= 2000; }
    RetVal = SUCCESS; 
  }
  dt->Year = (uchar)Year;

  enableInterrupts();  //====DEBUG HANG====

  return RetVal;
}

/** 
  * @brief      Установить системные дату и время  
  * @param      dt - данные для установки
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus setDateTime(DataTime_TypeDef dt)
{
  if ((setDate_PCF8563T(dt.Day, dt.Month, (ushort)(dt.Year + 2000)) == ERROR) ||
      (setTime_PCF8563T(dt.Hour, dt.Min, dt.Sec) == ERROR)) 
  {
    return ERROR;
  }
  return SUCCESS;
}

/** 
  * @brief      Переводит число из двоично-десятичного формата в бинарный  
  * @param      value - числов формате BCD (binary-coded decimal): от 0 до 99
  * @retval     Числов формате BIN
  */
uchar bcd2bin (uchar value)
{
  uchar High = 0;
  uchar Low = 0;
  uchar Res;
	
  Low = value & 0x0f;
  if(Low >= 10) { Low = 0; }
	
  High = (value >> 4) & 0x0f;
  if(High >= 10) { High = 0; }
  Res = High * 10 + Low;
    
  return Res;
}

/** 
  * @brief      Переводит число из бинарного формата в двоично-десятичный  
  * @param      value - числов формате BIN (от 0 до 99)
  * @retval     Число в формате BCD (binary-coded decimal)
  */
uchar bin2bcd (uchar value)
{
  uchar High = 0;
  uchar Low = 0;
  uchar Res;
	
  if(value >= 100) { value = 0; }
	
  High = value / 10;
  Low  = value % 10;
	
  High = (High << 4) & 0xf0;
  Low  = Low & 0x0f;
  Res  = High | Low;   
  
  return Res;
}

/** 
  * @brief      Отображение даты и времени на дисплее  
  * @param      None
  * @retval     None
  */
void showTime (void)
{
  char String[16];
  
  if (!fl_ShowTime) return;
  if (STimerStopShowTime > 0) return;
  if (TimerGetDT == 0) {
    TimerGetDT = 1000;
    if (getDateTime(&DTI) == ERROR) {
      displayError(ERR_GET_DT);
      return;
    }
    sprintf(String, "%02d/%02d/%04d", DTI.Day, DTI.Month, (2000 + DTI.Year));
    displayString(1, 1, String);
    sprintf(String, "%02d:%02d:%02d", DTI.Hour, DTI.Min, DTI.Sec);
    displayString(2, 1, String);
  }
}

/** 
  * @brief      Зашифровать данные  
  * @param      *buf - указатель данные
  *             size - размер данных
  * @retval     None
  */
void encryptData (uchar *buf, uint size) 
{
	uchar	Vnlf, Carr;
	uchar	i;
	ulong	Key;
	uint	Mcrc16, Hop;

	if (buf == NULL) return;
	if (size == 0) return;
	
    Key = *(ulong *) &SysStat.KSES[0];
	Hop = *(uint *) &SysStat.KSES[4];
	Mcrc16 = *(uint *) &SysStat.KSES[6];
	for(i = 0; i < size; i++) {
      Carr = buf[i];
      buf[i] ^= (Mcrc16 & 255);
      Mcrc16 = crcEnc(Carr, Mcrc16);
      Vnlf = 0;
      if(Hop & 0x0001) Vnlf |= 0x01;
      if(Hop & 0x0004) Vnlf |= 0x02;
      if(Hop & 0x0800) Vnlf |= 0x04;
      if(Hop & 0x4000) Vnlf |= 0x08;
      if(Key & 0x04000000L)	Vnlf |= 0x10;
      Carr = NLF[Vnlf];
      if(Hop & 0x0080) Carr = ~Carr;
      if(Hop & 0x8000) Carr = ~Carr;
      if(Key & 0x00008000L)	Carr = ~Carr;
      Hop <<= 1; 
      if(Carr) Hop |= 1;
      Carr = 0;
      if(Key & 0x80000000L) Carr = ~Carr;
      Key <<= 1;
      if(Carr) Key |= 1L;
      buf[i] ^= (Hop & 255);
	}
}

/** 
  * @brief      Расшифровать данные  
  * @param      *buf - указатель данные
  *             size - размер данных
  * @retval     None
  */
void decryptData (uchar *buf, uint size) 
{
	uchar	Vnlf, Carr;
	uchar	i;
	ulong	Key;
	uint	Mcrc16, Hop;

	if (buf == NULL) return;
	if (size == 0) return;
	
    Key    = *(ulong *) &SysStat.KSES[0];
	Hop    = *(uint *)  &SysStat.KSES[4];
	Mcrc16 = *(uint *)  &SysStat.KSES[6];
	
    for (i = 0; i < size; i++) {
		Vnlf = 0;
		if(Hop & 0x0001) Vnlf |= 0x01;
		if(Hop & 0x0004) Vnlf |= 0x02;
		if(Hop & 0x0800) Vnlf |= 0x04;
		if(Hop & 0x4000) Vnlf |= 0x08;
		if(Key & 0x04000000L) Vnlf |= 0x10;
		Carr = NLF[Vnlf];
		if(Hop & 0x0080) Carr = ~Carr;
		if(Hop & 0x8000) Carr = ~Carr;
		if(Key & 0x00008000L) Carr = ~Carr;
		Hop <<= 1; 
        if(Carr) Hop |= 1;
		Carr = 0;
		if(Key & 0x80000000L) Carr = ~Carr;
		Key <<= 1;
		if(Carr) Key |= 1L;
		buf[i] ^= (Hop & 255);
		buf[i] ^= (Mcrc16 & 255);
		Mcrc16 = crcEnc(buf[i], Mcrc16);
	}
}

/** 
  * @brief      Просмотр дампа  
  * @param      sym - количество симвволов байт (bb), отображаемых в одной строке
  *             *adr - указатель на данные для просмотра
  *             len - размер дампа 
  * @retval     None
  */
void viewDump_Consol (uchar sym, uchar *adr, uint len) 
{
	uchar i = 0;
	uchar *Ptr = adr;

	if(!len) return;
	do {
      //_WDR();													
      if((++i % (sym + 1)) == 0)
      {	
        i = 1;	
        printf_Consol((const char *) "\r\n");	
      }
      printf_Consol((const char *) "%02x ", *Ptr++);
	} while(--len);
	printf_Consol((const char *) "\r\n");
}

/** 
  * @brief      Подсчёт CRC для функций шифровки и дешифровки данных  
  * @param      bt - 
  *             ocrc
  * @retval     CRC (2 Bytes)
  */
uint crcEnc(uchar bt, uint ocrc)
{
	ocrc = ocrc ^ (bt << 8);
	if (ocrc & 0x8000) { ocrc <<= 1; ocrc ^= 0x1021; }
	else ocrc <<= 1;
	return ocrc;
}

/** 
  * @brief      Генерация псевдослучайного числа в заданном диапазоне  
  * @param      min - нижняя граница диапазона 
  *             max - верхняя граница диапазона
  * @retval     Псевдослучайное число
  */
u32 random (u32 min, u32 max)
{
  //u32 RetVal = min + (max-min) * rnd() / 0xffffffff;
  u32 RetVal = ((rnd()%(u32)(((max) + 1)-(min)))+ (min));
  return RetVal;
}

/** 
  * @brief      Определение количества датчиков присутствия пассажира на связи  
  * @param      num - максимальный номер датчика для подсчёта общего количества  
  *             wake - выводить датчики из состояния сна (1) или нет (0)
  * @retval     Код ошибки (см. ErrCodes_TypeDef)
  * @Note       Подсчёт датчиков начинается с номера [num] в обратном порядке.
  *             Таким образом, датчики желательно располагать в пространстве 
  *             по такому правилу:
  *             чем дальше находится датчик от бортового контроллера,
  *             тем большее значение должен иметь его порядкловый номер
  */
u16 handler_CountPPD (u8 num, u8 wake)
{
  CountPPDState_TypeDef State; 
  u16 ErrCode = NO_ERROR;
  u8 ErrCnt = 0;
  
  if (wake == WAKE) { 
    /* Устанавливаем время соединения с датчиками */
    SysStat.TimeWaitConnectRF = SCAN_WAIT_TIME + WAITCONNECT_RF;    // больше на 8 с чем время удержания канала
    State = CP_WakeUp; 
  }
  else if (wake == NOWAKE) { 
    /* Устанавливаем время соединения с датчиками */
    SysStat.TimeWaitConnectRF = SCAN_WAIT_TIME - 300;    // меньше на 300 мс, чем время удержания канала
    State = CP_IntoLine; 
  }
  
  /* Чтобы нормально работал подсчёт датчиков, надо (не понятно почему) 
  ** сначала отработать любую команду; потом всё ОК */
  
  /* Не отсылаем пакеты синхронизации в радиоканал */
  fl_SendSynchroRF = 0;
  /* Устанавливаем таймер для синхронизации найденных датчиков */
  TimerSynchroRF = __TIMERSYNCHRORF;
  /* Устанавливаем таймер для следующей отсылки информации на DM */
  STimerShowSeats = __TIMEOUTSHOWSEATS;
  /* Обнуляем структуру "где какой датчик" */
  //memset(&CP, 0, sizeof(TCountPPD));
  CP.Total = 0;
  CP.Alive = 0;
  CP.Dead = 0;
  CP.WhereIs_1 = 0;
  /* Подготавливаем структуру для передачи информации по датчикам на DriveUnit */
  getDateTime(&IS[1].DT);
  IS[1].Show.Total = SysParams.QtyPPD;
  for (int i = SysParams.QtyPPD; i > 0; i--) {
    IS[1].Show.Seats[SysParams.QtyPPD - i].Number = i;  
    IS[1].Show.Seats[SysParams.QtyPPD - i].Acked = 0;         
  }
  /* Обнуляем данные в структуре статуса системы */ 
  SysStat.FreeSeats = SysStat.OccupSeats = 0;
  /* Используем как вспомогательную переменную для нумерации опрашиваемых датчиков */
  SysStat.FailedPPD = SysStat.GoodPPD = num + 1;
  
  /* Выводим сообщение */
  fl_ShowTime = 0;
  displayString(1, 1, "ПОИСК ДАТЧИКОВ...");
  displayString(2, 1, "Найдено: ..... 0");
  
  /* Включаем светодиод LED_WORK на лицевой панели */
  GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, Bit_RESET);

  while (State != CP_Complete) {
    _WDR();
    /* Обрабатываем клавиатуру */
    handler_Keyboard();
    /* Передаём команду датчику */
    ErrCode = countPPD(&State);
    /* Обрабатываем радиоканал */
    taskTransiver();
    /* Смотрим результат */
    switch (ErrCode) {
      case NO_ERROR:
        break;
 
    default:
        if(++ErrCnt > 5) {
          printf_Consol("Count PPD: неизв. ошибка 0x%02x\r\n", ErrCode);
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
  /* Разрешаем отображение даты и времени */
  // ... через 2 сек
  STimerBacklight = 2;
  /* Подводим итог */
  CP.Total = SysParams.QtyPPD;
  CP.Dead = CP.Total - CP.Alive;
  SysStat.GoodPPD = CP.Alive;
  SysStat.FailedPPD = CP.Dead;
  /* Восстанавливаем параметры */
  SysStat.TimeWaitConnectRF = WAITCONNECT_RF;
  /* В дальнейшем соединения проводить на рабочем канале */ 
  setRadioChannel(SysStat.WorkRadioCh);
  /* Разрешаем смену радиоканала */
  fl_NoChangeChannel = 0;
  /* Информация о номере рабочего канала в DriveUnit*/
  IS[1].Ch = Radio.Channel;
  /* "Вольно!" (отправляемся отдыхать, если вдруг кто-то не заснул) */
  printf_Consol("Sleep ALL!\r\n");
  Radio.RemoteAddr = BROADCAST_ADDR;
  TimerForAll = 2000;
  while(TimerForAll) {
    delay_1ms(40);
    printf_Consol(".");
    sendPacket_MRF49XA(cmdENABLE_SLEEP, NULL, 0, NOENCRIPT);
  }
  printf_Consol("\r\n");
  /* Выключаем светодиод LED_WORK на лицевой панели */
  GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, Bit_SET);
  return ErrCode;
}

/** 
  * @brief      Установка соединения с датчиками присутствия пассажира  
  * @param      *state - указатель на состояние выполнения  
  * @retval     Код ошибки (см. ErrCodes_TypeDef)
  * @note       В этой функции параметр SysStat.FailedPPD используется как 
  *             "свободная" вспомогательная переменная из глобальной структуры 
  *             для нумерации опрашиваемых датчиков
  */
u16 countPPD (CountPPDState_TypeDef *state)
{
  u16 ErrCode = NO_ERROR;
  char String[16];
  u16 RandomChannel;
  
  switch (*state)
  {
    case CP_Idle:
      TimerSynchroRF = 0;
      fl_EntrOnce = 0;
      *state = CP_Complete;
      break;
      
    case CP_WakeUp:
      /* "Построиться!" - 
      эта команда выводит датчики из состояния "глубокого сна" */
      /* Устанавливаем радиоканал, на котором спят датчики */
      setRadioChannel(__SLEEPCHANNEL);
      /* Запрещаем смену радиоканала */
      fl_NoChangeChannel = 1;
      /* Устанавливаем флаг выведения датчиков из состояния "глубокого сна" */
      //fl_NoDeepSleep = 1;
      /* Устанавливаем широковещательный адрес */
      Radio.RemoteAddr = BROADCAST_ADDR;
      /* Таймер для выведения датчиков из спячки (время сна + 200ms) */
      TimerAwake = __TIMEPPDSLEEP + 200;
      printf_Consol("Wake Up ALL!!!\r\n");
      *state = CP_Scream;
      break;
      
    case CP_IntoLine:
      /* "Построиться!" - 
      эта команда отключает режим энергосбережения датчиков на рабочем канале */
      setRadioChannel(SysStat.WorkRadioCh);
      /* Запрещаем смену радиоканала */ 
      fl_NoChangeChannel = 1;
      Radio.RemoteAddr = BROADCAST_ADDR;
      TimerAwake = __TIMEPPDSLEEP + 200;
      printf_Consol("Fall into Line!!!\r\n");
      *state = CP_Alarm;
      break;

    case CP_Scream:
      /* Используем для выведения из "глубокого сна" */
      if (TimerAwake) { 
        if (TimerSynchroRF == 0) {
          TimerSynchroRF = 20;    // Используем этот таймер для отсылки пакетов "просыпания"
          printf_Consol(".");
          sendPacket_MRF49XA(cmdDISABLE_SLEEP, NULL, 0, NOENCRIPT);
        }
        break;
      }
      printf_Consol("\r\n");
      /* Уходим с "секретного" канала для дальнейшего поиска оптимального канала для связи */
      RandomChannel = (u16)random(0, Radio.NumChannel - 1);
      setRadioChannel(RandomChannel);
      printf_Consol("RandomChannel = %u\r\n", RandomChannel);
      /* Ждём 2с, чтобы датчики начали менять радиоканал */
      TimerAwake = SCAN_WAIT_TIME;
      printf_Consol("Pause 2 seconds...\r\n");
      *state = CP_Pause;
      break;
      
    case CP_Pause:
      if (TimerAwake) { break; }
      /* Разрешаем смену радиоканала */
      fl_NoChangeChannel = 0;
      *state = CP_Start;  
      break;


    case CP_Alarm:
      /* Используем для отключения в датчиках режима энергосбережения
         и выполнения команд на рабочем канале */
      if (TimerAwake) { 
        if (TimerSynchroRF == 0) {
          TimerSynchroRF = 20;    // Используем этот таймер для отсылки пакетов "просыпания"
          printf_Consol(".");
          sendPacket_MRF49XA(cmdDISABLE_SLEEP, NULL, 0, NOENCRIPT);
        }
        break;
      }
      printf_Consol("\r\n");
      *state = CP_Start;  
      break;

    case CP_Start:
      /* Обнуляем количество повторов авторизации с заданным датчиком */
      Times = 0;
      /* Проверяем количество опрошенных датчиков */
      if (--SysStat.FailedPPD < 1) // начинаем с самого дальнего датчика, чтобы выбрать наилучший канал 
      { 
        *state = CP_AtEase;
        break;
      }
      *state = CP_Connect;
      break;
      
    case CP_Connect:
      /* Авторизуемся с датчиком (посылаем команду) */
      ErrCode = handler_giveCmdPPD(SysStat.FailedPPD, cmdPING_REQUEST, NULL, &StatusPPD, NOENCRIPT, NOCHECKNET);
      if (ErrCode == NO_ERROR)
      { // Если датчик ответил - записываем его в битовое поле
        CP.WhereIs_1 = CP.WhereIs_1 | (u32)(1 << (SysStat.FailedPPD - 1));
        // ... и запоминаем номер радиоканала
        if (!fl_EntrOnce) { 
          fl_EntrOnce = 1;
          SysStat.WorkRadioCh = Radio.Channel;
          // ... и запрещаем смену радиоканала 
          fl_NoChangeChannel = 1;
        }
        // ... и записываем инфо для отображения на DriverUnit
        IS[1].Show.Seats[SysParams.QtyPPD - SysStat.FailedPPD].Number = SysStat.FailedPPD;  // порядковый номер датчика
        IS[1].Show.Seats[SysParams.QtyPPD - SysStat.FailedPPD].Acked = 1;                   // датчик отвечает
        IS[1].Show.Seats[SysParams.QtyPPD - SysStat.FailedPPD].RightSeat = StatusPPD.Passenger1;       
        IS[1].Show.Seats[SysParams.QtyPPD - SysStat.FailedPPD].LeftSeat = StatusPPD.Passenger2;       
        IS[1].Show.Seats[SysParams.QtyPPD - SysStat.FailedPPD].PowerLow = StatusPPD.Battery;
        // ... и считаем "живой" датчик
        CP.Alive++;
        // ... и обновляем информацию в структуре статуса системы
        if (StatusPPD.Passenger1) { SysStat.OccupSeats++; }
        else { SysStat.FreeSeats++; }  
        if (StatusPPD.Passenger2) { SysStat.OccupSeats++; }
        else { SysStat.FreeSeats++; }
        // ... и ставим признак необходимости усыпить этот датчик
        RepeatSleep = 1;
        // ... и отображаем на дисплее общее количество найденных датчиков
        displayString(1, 1, "ПОИСК ДАТЧИКОВ");
        sprintf(String, "Найдено: ..... %u", (u8)CP.Alive);
        displayString(2, 1, String);
      }
      else if (ErrCode == ERR_ERRANSWER)
      { // Если ответ получен, но с ошибкой - повторяем авторизацию
        Times++;
        *state = CP_Synchro;
        break;
      }
      /* Если найден первый датчик, уменьшаем время ожидания соединения с датчиком, 
      чтобы не потерять хороший канал */
      if (CP.Alive == 1) 
      {  
        SysStat.TimeWaitConnectRF = SCAN_WAIT_TIME - 300; 
      }    
      Times = 0;
      *state = CP_Synchro;
      break;
      
    case CP_Synchro:
      /* Держим все найденные датчике на синхроканале */
      if (CP.Alive > 0) synchroRF(SysStat.WorkRadioCh, SWITCH);
      /* Если надо повторить опрос датчика - возвращаемся */
      if ((Times > 0) && (Times < 2)) { 
        *state = CP_Connect;
        break;
      }
      /* Должны отправить датчик спать */
      if (RepeatSleep == 0) {       // Признак того, что датчик успешно лёг спать
        // Пингуем следующий датчик
        *state = CP_Start;  
        break;
      }
      else if (RepeatSleep <= 5) {   // Ещё есть попытки уложить датчик спать
        *state = CP_GoSleep;
        break;
      }
      else {                        // Превышено количество попыток уложить датчик спать
        printf_Consol("Не смог усыпить датчик #%u :-(\r\n", SysStat.FailedPPD);
        // Запрещаем синхронизацию
        fl_SendSynchroRF = 0;
        // Запрещаем получение информации по сидениям
        fl_ShowSeats = 0;
        // Сбрасываем признак засыпания
        RepeatSleep = 0;
        // Повторим пересчёт через 2 мин., когда датчики точно перейдут в состояние глубокого сна
        STimerShowSeats = 120;
        *state = CP_Idle;
      }
      break;  
      
    case CP_GoSleep:
      /* Отправляем найденный только что датчик в режим энергосбережения */
      ErrCode = handler_giveCmdPPD(SysStat.FailedPPD, cmdENABLE_SLEEP, NULL, NULL, NOENCRIPT, NOCHECKNET);
      printf_Consol("ЛОЖИСЬ СПАТЬ! RepeatSleep = %u\r\n", RepeatSleep);
      if (ErrCode != NO_ERROR) 
      { // Если была ошибка "усыпления", повторяем операцию
        RepeatSleep++;
      }
      else 
      { // Датчик успешно заснул 
        RepeatSleep = 0; 
      }
      *state = CP_Synchro;
      break;
      
    case CP_AtEase:
      /* Если ни один датчик не найден - увеличиваем время поиска и начинаем сначала */
      if (CP.Alive == 0) {
        SysStat.TimeWaitConnectRF += 500;
        if (SysStat.TimeWaitConnectRF <= WAITCONNECT_RF) {
          SysStat.FailedPPD = SysStat.GoodPPD;
          *state = CP_Start;
          break;
        }
      }
      /* Разрешаем отсылку пакетов синхронизации в радиоканал */
      fl_SendSynchroRF = 1;
      *state = CP_Idle;
      break;
  }
  return NO_ERROR;
}

/** 
  * @brief      Определение максимального порядкового номера датчика, находящегося в сети  
  * @param      None  
  * @retval     Номер датчика, который находится на связи на макс. отдалении
  *             0 - датчики не найдены
  */
u8 getMaxNumPPD (void)
{
  u32 Data = CP.WhereIs_1;
  u8 Num = SysParams.QtyPPD;
  
  if (Num > QTYPPD) return 0;
  if (Data == 0) return SysParams.QtyPPD;
  
  while ((Data >> --Num) == 0) {
    Data = CP.WhereIs_1;
  }
  
  return (Num + 1);
}

/** 
  * @brief      Определение следующего порядкового номера датчика, находящегося в сети  
  * @param      curnum - текущий порядковый номер датчика, с какого начинается поиск  
  * @retval     Порядковый номер датчика, который находится на связи и меньше текущего
  *             0 - датчик не найден
  */
u8 getNextNumPPD (u8 curnum)
{
  if (curnum <= 1) return 0;
  if (curnum > SysParams.QtyPPD) return 0;
  if (CP.WhereIs_1 == 0) return 1;
  
  u8 Num = curnum - 1;
  u32 Data, Var = CP.WhereIs_1 & ~(1 << Num);
  
  while ((Var >> --Num) == 0) {
    Var = Data;
  }
  
  return (Num + 1);
}

/** 
  * @brief      Определение наличия датчика в сети  
  * @param      num - порядковый номер датчика  
  * @retval     0 - ERROR (датчика нет на связи)
  *             1 - SUCCESS
  * @note       Если датчика нет в сети, рекомендуется произвести пересчёт
  *             датчиков при помощи функции handler_CountPPD
  */
ErrorStatus checkNumPPD (u8 num)
{
  u32 Var = CP.WhereIs_1;
  
  if ((num < 1) || (num > SysParams.QtyPPD)) { return ERROR; }

  if ((Var >> (num - 1)) & 0x01) { return SUCCESS; }
  else { return ERROR; }  
  
}

/** 
  * @brief      Вывод датчиков из спячки   
  * @param      None  
  * @retval     None
  */
void awakePPD (void)
{
  u8 Num;
  uint Ch;
  u8 Times = 10;
  TimerAwake = 8000;
  
  for (Num = SysParams.QtyPPD; Num > 0; Num--) {
    while (1) {
      if (TimerAwake) {
        Radio.RemoteAddr = UIN(Num);
        for (Ch = 0; Ch < Radio.NumChannel; Ch++) {
          setRadioChannel(Ch);
          printf_Consol("...PPD#%u-Ch#%u\r\n", Num, Ch);
          while (TimerAwake/*Times*/) {
            if (fl_AckAwake)  break;
            sendPacket_MRF49XA(cmdDISABLE_SLEEP, NULL, 0, NOENCRIPT);
            delay_1ms(100);
            Times--;
          }
          if (fl_AckAwake) break;
          Times = 10;
          TimerAwake = 8000;
        }
        if (fl_AckAwake) {
          fl_AckAwake = 0;
          break;
        }
      }
      else break;
    }
    //TimerAwake = 8000;
  }
  SysStat.WorkRadioCh = Radio.Channel;
}

/** 
  * @brief      Проверка, наступило ли событие, когда надо проверить состояние
  *             всех датчиков системы
  * @param      None  
  * @retval     None
  * @Note       Проверяется факт нажатия кнопок открытия передней и/или
  *             задней двери, а также производится автоматическая проверка
  *             состояния с заданным периодом времени
  */
void checkEvent (void)
{
  u32 FixedTime = STimerShowSeats;

  /* Проверяем, нажата ли была кнопка открытия-закрытия дверей */
  if (fl_OpenFrontDoor || fl_OpenRearDoor || fl_OpenCar) {
    if (CurrStatGPS.Speed > 0) 
    { // Событие наступит только после того, как автобус тронется
      fl_EventOccured = 1;
      fl_OpenFrontDoor = fl_OpenRearDoor = fl_OpenCar = 0;
      // Отображаем надпись на дисплее
      fl_ShowTime = 0; fl_ShowInfoGPS = 0; STimerBacklight = 5; // 5 sec
      displayString(1, 2, "Дверь была"); displayString(2, 2, "открыта!");
      // Событие сгенерируется через 2 мин.
      STimerMakeEvent = SysParams.TimeSeats;
    }
  }
#ifndef __RADIONET__
  return;
#endif /* __RADIONET__ */  
  /**---------------------------------------------------------------------------
    Здесь надо сделать, чтобы по прошествии времени TimerShowSeats либо по нажатию
  кнопки "СЧИТАТЬ" на DriverMonitor считывалось и отсылалось на DriverMonitor
  состояние всех сидений, но не записывалось событие.
  Событие должно записываться по открыванию двери + движение автобуса + 2 мин.
  (SysParams.TimeSeats), и отображаться на DriverMonitor.
    Также надо разделить два времени:
  - TimerShowSeats - это время, стого заданное, для периодической отсылки инфо о
  состоянии сидений на DriverMonitor (если скорость движения > 0);
  - TimerMakeEvent - это время, каждый раз генерируемое случайно, для автомати-
  ческого сбора инфо о сидениях и записи события в память. 
  ----------------------------------------------------------------------------*/

  /* Отсылаем данные о состоянии сидений на DriverMonitor, когда пришло время 
     или поступила команда от DM */
  if ((STimerShowSeats == 0) || fl_ShowSeats) {
    fl_ShowSeats = 0;
    if (fl_SendSynchroRF) 
    { // Получаем информацию, если датчики находятся на синхроканале
      on(BKL);
      getInfoSeats();
      /* Отсылаем на DM инфо о всех датчиках в системе */
      sendPacket_UARTx(UART_DM, DRIVERUNIT, cmdDISPL_SEAT, (uchar *)&IS[1], sizeof(TInfoSeats));   
      // Подсвечиваем дисплей 5с после окончания подсчёта датчиков
      STimerBacklight = 5;
    }
  }

  /* Заполняем данные события */
  if (STimerMakeEvent == 0) {
    // В след. раз событие автоматически сгенерируется в промежутке от 10 до 30 минут
    STimerMakeEvent = random(600, 1800);  
    if (fl_SendSynchroRF) 
    { /* Получаем инфо о сидениях, если датчики находятся на синхроканале
      и выполняется условие:
      или зафиксировано событие, 
      или пришло время автоматичесмкой записи события при скорости движения >0 */
      if (fl_EventOccured || (!fl_EventOccured && (CurrStatGPS.Speed > 0)))
      { getInfoSeats(); }
      /* Формируем Событие */
      // 1) ID и DataTime события записываются в функции [addEvent]
      // ...
      // 2) Записываем текущие координаты 
      memcpy(&CurrEvent.Coordinates, &SysStat.Coordinates, sizeof(TGoogleGPS));
      // 3) Записываем данные о сидениях */ 
      CurrEvent.OccupSeats = SysStat.OccupSeats;
      CurrEvent.FreeSeats = SysStat.FreeSeats;
      CurrEvent.FailedPPD = SysStat.FailedPPD;
      memcpy(&CurrEvent.IS, &IS[1], sizeof(TInfoSeats)); 
      // 4) Отмечаем способ возникновения события
      if (fl_EventOccured) {  
        fl_EventOccured = 0;
        CurrEvent.AutoSave = 0;
      }
      else { CurrEvent.AutoSave = 1; }
      // 5) По необходимости фиксируем начало или конец маршрута
      if (CurrEvent.OccupSeats > 0)
      { // Начало маршрута, если перед все места были свободны
        if (fl_EmptyBus) {
          fl_EmptyBus = 0;
          CurrEvent.StartRoute = 1; 
          displayString(1, 1, "НАЧАЛО МАРШРУТА!");
          STimerBacklight = 2;
        }
      }
      else 
      { // Конец маршрута - нет занятых мест 
        fl_EmptyBus = 1;
        CurrEvent.EndRoute = 1; 
        displayString(1, 1, "КОНЕЦ МАРШРУТА!");
        STimerBacklight = 2;
      }
      // 6) Записываем текущий пробег  
      CurrEvent.Mileage = Odometer;
      // 7) Записываем скорость движения
      CurrEvent.Speed = SysStat.Speed = CurrStatGPS.Speed; 
      /* Записываем событие в память ExtFlash */
      printf_Consol("Add Event...");
      if (addEvent(&CurrEvent) == ERROR) {
        printf_Consol("ERROR!\r\n");
        TimerForAll = 2000;
        while(TimerForAll) { _WDR(); blink(OUTPUTS_GPIO_PORT, LED_ERROR, 30, SND_ON); }
      }
      else { 
        printf_Consol("OK! ID #%u\r\n", (ulong)CurrEvent.ID);
        viewDump_Consol(8, (uchar *)&CurrEvent, sizeof(TEvent));
      }
      /* Обнуляем структуру текущего события */
      memset(&CurrEvent, 0, sizeof(TEvent));
      // Восстанавливаем таймер для отсылки инфо на DM
      STimerShowSeats = FixedTime; 
    }
    else if (CurrStatGPS.Speed > 0)
    { // если вдруг контроллер был сброшен или остановлена синхронизация, 
      // будим и синхронизируем датчики (при условии, что автобус движется)
      /** ТРЕБУЕТ ДОРАБОТКИ */
      do { handler_CountPPD(SysParams.QtyPPD, WAKE); }
      while (SysStat.GoodPPD == 0);
      if (SysStat.GoodPPD < SysParams.QtyPPD)
      { // Пересчитываем, если не все датчики ответили
        STimerMakeEvent = 0; 
      }
    }
  }
}

/** 
  * @brief      Проверка состояния всех датчиков системы
  * @param      None  
  * @retval     None
  * @Note       
  */
void getInfoSeats (void)
{
  /** ТРЕБУЕТ ДОРАБОТКИ */
  /* Запоминаем количество хороших датчиков */
  GoodPPD = SysStat.GoodPPD;
  handler_CountPPD(SysParams.QtyPPD, NOWAKE);
  /* Здесь надо производить пересчёт, если кол-во рабочих датчиков уменьшилось */
  if (SysStat.GoodPPD < GoodPPD) {
    handler_CountPPD(SysParams.QtyPPD, NOWAKE);
    /* ...и если это повторяется - менять канал (найти наилучший) */
    if (SysStat.GoodPPD < GoodPPD) {
      handler_CountPPD(SysParams.QtyPPD, WAKE);
      /*   ...пока не убедимся, что всё-таки это правда (датчик вышел из строя) */
      if (SysStat.GoodPPD < GoodPPD) {
        handler_CountPPD(SysParams.QtyPPD, NOWAKE);
      }
    }
  }
}

/** 
  * @brief      Контроль подсветки дисплея
  * @param      None
  * @retval     None
  */
void bkl (void)
{
  BitAction BitVal;

  // Проверяем состояние выхода BKL
  BitVal = GPIO_ReadOutputDataBit(BKL) ? Bit_SET : Bit_RESET;
  // Разрешаем отображение часов за 1 сек до выключения подсветки
  if ((BitVal == Bit_SET) && (STimerBacklight <= 2)) { fl_ShowTime = 1; }
 
  if (STimerBacklight == 0) {
    // Выключаем светодиод ERROR на лицевой панели, если он случайно включен
    if (GPIO_ReadOutputDataBit(OUTPUTS_GPIO_PORT, LED_ERROR) == Bit_RESET) {
      GPIO_SetBits(OUTPUTS_GPIO_PORT, LED_ERROR);
    }
    if (BitVal == Bit_SET) { off(BKL); }    // Отключаем подсветку
  }
  else { 
    if (BitVal == Bit_RESET) { 
      on(BKL);          // Включаем подсветку
    }   
  }
}

/** 
  * @brief      Управление получения информации о занятости сидений
  *             за определённый промежуток времени
  * @param      rgidt - указатель на исходные данные
  * @retval     Код ошибки (см. ErrCodes_TypeDef)
  */
u16 handler_GetInfoDT(TReqGetInfoDT *rgidt)
{
  u16 ErrCode = NO_ERROR;
  u8 ErrCnt = 0;
  GetInfoDTState_TypeDef State, PrevState;

  /* Устанавливаем индекс на самое "свежее" событие */
  u32 Index = SysParams.IndexWrite - 1; 
  /* Устанавливаем начальную точку выполнения задачи */
  State = PrevState = GI_FindEndEvent;
  /* Обнуляем структуру для ответа */
  memset(&AIDT, 0, sizeof(TAnswerInfoDT));
  /* Включаем светодиод LED_WORK на лицевой панели */
  GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, Bit_RESET);
  
  while (State != GI_Complete) {
    _WDR();
    ErrCode = getInfoDT(rgidt, &Index, &State);
    switch (ErrCode) {
      case NO_ERROR:
        ErrCnt = 0;
        break;
        
      case ERR_FIND_EVENT:
        /* Если ошибка возникла не из одного и того же состояния, 
        сбрасываем счётчик ошибок */
        if (PrevState != State) { ErrCnt = 0; } 
        /* Если не найдено нужное событие 100 раз, то останавливаем выполнение */
        if (++ErrCnt > 100) {
          printf_Consol("handler_GetInfoDT: ошибка 0x%02x - не найдено событие\r\n", ErrCode);
          break;
        }
        else { ErrCode = NO_ERROR; }
        /* Запоминаем, в каком месте выполнения задачи возникла ошибка */
        PrevState = State;
        break;
        
      case ERR_READ_DF:
        break;

      default:  // Неизвестная ошибка
        /* Если ошибка возникла не из одного и того же состояния, 
        сбрасываем счётчик ошибок */
        if (PrevState != State) { ErrCnt = 0; }
        if (++ErrCnt > 5) {
          printf_Consol("handler_GetInfoDT: ошибка 0x%02x\r\n", ErrCode);
          ErrCode = ERR_UNKNOWN;
          break;
        }
        else { ErrCode = NO_ERROR; }
        PrevState = State;
        break;
    }
    if (ErrCode != NO_ERROR) { break; }
    delay_1ms(1);
  }
  /* Выключаем светодиод LED_WORK на лицевой панели */
  GPIO_WriteBit(OUTPUTS_GPIO_PORT, LED_WORK, Bit_SET);
  return ErrCode;
}

/** 
  * @brief      Получение информации о занятости сидений за промежуток времени
  * @param      *rgidt - указатель на исходные данные
  *             *index - указатель на индекс для чтения события из ExtFlash
  *             *state - указатель на состояние выполнения
  * @retval     Код ошибки (см. ErrCodes_TypeDef)
  */
u16 getInfoDT(TReqGetInfoDT *rgidt, u32 *index, GetInfoDTState_TypeDef *state)
{
  vu16 ErrCode = NO_ERROR;
  u32 TmpIndex;

  switch (*state) {
    case GI_Idle:
      break;
      
    case GI_FindEndEvent:
      /* Читаем, начиная с самого последнего события */
      if (getEvent(&Evn, *index, STAY) == ERROR) { ErrCode = ERR_READ_DF; break; }
      /* Если не было ошибок при чтении, анализируем полученное событие */
      /* 1) Проверяем дату и время */
      if (compareDateTime(&Evn.DataTime, &rgidt->DTEnd) == FIRST_LATER) 
      { // Если прочитанное событие произошло позже, чем конечное время в запросе
        // читаем предыдущее событие из памяти
        ErrCode = ERR_FIND_EVENT;
        // Проверяем, не переходит ли указатель на ID события через 0
        TmpIndex = *index;
        if (TmpIndex > 0) { TmpIndex--; }
        else { TmpIndex = __MAXQTYEVENTS - 1; }
        *index = TmpIndex;  
        break; 
      }
      // Если нашли событие, которое произошло раньше, то фиксируем его
      memcpy(&AIDT.DT_EndEvn, &Evn.DataTime, sizeof(DataTime_TypeDef));
      AIDT.ID_EndEvn = Evn.ID;
      OdEnd = Evn.Mileage;
      // Запоминаем время для подсчёта пассажиросекунд
      memcpy(&FixedTimeOfStop, &Evn.DataTime, sizeof(DataTime_TypeDef));
      // Запоминаем пробег для подсчёта пассажирометров
      FixedOdoOfStop = Evn.Mileage;
      // Проверяем, не переходит ли указатель на ID события через 0
      if (AIDT.ID_EndEvn > 0) { TmpIndex = AIDT.ID_EndEvn - 1; }
      else { TmpIndex = __MAXQTYEVENTS - 1; } 
      *index = TmpIndex; 
      *state = GI_FindStartEvent;
      break;
      
    case GI_FindStartEvent:
      /* Продолжаем читать события, пока не найдём его по начальному времени */
      if (getEvent(&Evn, *index, STAY) == ERROR) { ErrCode = ERR_READ_DF; break; }
      /* Если не было ошибок при чтении, анализируем полученное событие */
      /* 1) Проверяем дату и время */
      if (compareDateTime(&Evn.DataTime, &rgidt->DTStart) == FIRST_LATER) 
      { // Если прочитанное событие произошло позже, чем начальное время в запросе
        // читаем предыдущее событие из памяти
        ErrCode = ERR_FIND_EVENT;
        // Проверяем, не переходит ли указатель на ID события через 0
        TmpIndex = *index;
        if (TmpIndex > 0) { TmpIndex--; }
        else { TmpIndex = __MAXQTYEVENTS - 1; }
        *index = TmpIndex;  
        break; 
      }
      // Если нашли событие, которое произошло раньше, то берем предыдущее прочитанное
      // Проверяем, не переходит ли указатель на ID события через __MAXQTYEVENTS
      TmpIndex = *index;
      if (TmpIndex < __MAXQTYEVENTS - 1) { TmpIndex++; }
      else { TmpIndex = 0; }
      *index = TmpIndex;  
      if (getEvent(&Evn, *index, STAY) == ERROR) { ErrCode = ERR_READ_DF; break; }
      memcpy(&AIDT.DT_StartEvn, &Evn.DataTime, sizeof(DataTime_TypeDef));
      AIDT.ID_StartEvn = Evn.ID;
      OdStart = Evn.Mileage;
      // Начинаем подсчёт с последнего события
      TmpIndex = AIDT.ID_EndEvn;
      *index = TmpIndex;
      *state = GI_CalculateStops;
      break;
    
    /* Считаем количество остановок (QtyStops) */  
    case GI_CalculateStops:
      if (AIDT.ID_StartEvn == AIDT.ID_EndEvn) { 
        ErrCode = ERR_FIND_EVENT;
        break;
      }
      // Проверяем индекс события: лежит ли он в пределах между Start и End?
      if (AIDT.ID_StartEvn > AIDT.ID_EndEvn)
      { /** Если ID первого события больше ID последнего события,
        то был переход через 0, и тогда надо не пропустить этот момент
        при считывании событий */
        if (fl_ZeroID) {
          if (*index < AIDT.ID_StartEvn) { 
            fl_ZeroID = 0;
            *state = GI_WriteOtherData;
            break;
          }
        }
      }
      else if ((*index < AIDT.ID_StartEvn) || fl_ZeroID)
      { // Может оказаться, что ID первого события равен 0, поэтому проверяем fl_ZeroID
        fl_ZeroID = 0;
        *state = GI_WriteOtherData;
        break;
      }
      if (getEvent(&Evn, *index, STAY) == ERROR) { ErrCode = ERR_READ_DF; break; }
      // Проверяем, не переходит ли указатель на ID события через 0
      TmpIndex = *index;
      if (TmpIndex > 0) { TmpIndex--; }
      else { TmpIndex = __MAXQTYEVENTS - 1; fl_ZeroID = 1; }
      *index = TmpIndex;  
      PasCount += Evn.OccupSeats;
      if (Evn.AutoSave == 0) 
      { // Событие произошло по открытию двери 
        AIDT.QtyStops++;
        *state = GI_CalculatePassengers;
        break;
      }
      else
      { // Событие создано автоматически
        QtyAutoEvns++;
      }  
      break;
      
    case GI_CalculatePassengers:
      /* Выводим среднее значение количества пассажиров между остановками */
      AverPas = PasCount / (QtyAutoEvns + 1);
      AIDT.Passengers += AverPas;
      /* Обновляем количество пассажиросекунд (TotalPasTime) 
      с учетом времени задержки после начала движения */
      AIDT.TotalPasTime += AverPas * (calculateTime(&Evn.DataTime, &FixedTimeOfStop)
                          + (double)SysParams.TimeSeats);
      /* Фиксируем время остановки */
      memcpy(&FixedTimeOfStop, &Evn.DataTime, sizeof(DataTime_TypeDef));  
      /* Считаем количество пассажирометров (TotalPasDistance) */
      AIDT.TotalPasDistance += AverPas * (FixedOdoOfStop - Evn.Mileage);
      /* Фиксируем пробег на момент остановки */
      FixedOdoOfStop = Evn.Mileage;
      /* Начинаем новый подсчет пассажиров между остановками */
      PasCount = 0; 
      QtyAutoEvns = 0; 
      *state = GI_CalculateStops;
      break;
      
    case GI_WriteOtherData:
      /* Записываем остальные данные */
      // Дописываем пассажиров, если они были, а остановка и открытие двери не зафиксированы
      if (QtyAutoEvns != 0) { AverPas = PasCount / QtyAutoEvns; }
      else { AverPas = 0; }
      AIDT.Passengers += AverPas;
      AIDT.TotalPasTime += AverPas * calculateTime(&Evn.DataTime, &FixedTimeOfStop);
      AIDT.TotalPasDistance += AverPas * (FixedOdoOfStop - Evn.Mileage);
      // Время между двумя крайними событиями в секундах
      AIDT.Time = calculateTime(&AIDT.DT_StartEvn, &AIDT.DT_EndEvn);
      // Пройденное расстояние с учётом задержки
      AIDT.Distance = (OdEnd - OdStart); 
      // Номер автобуса
      AIDT.Board = SysParams.BoardNumber;
      *state = GI_Complete;
      break;
  }
  
  return ErrCode;
}

/** 
  * @brief      Сравнение двух значений Дата-Время
  * @param      *dt1 - указатель на первое сравниваемое значение Даты-Времени
  *             *dt2 - указатель на второе сравниваемое значение Даты-Времени
  * @retval     FIRST_EARLIER (0) - первое значение раньше
  *             FIRST_LATER (1) - первое значение позже
  *             DT_EQUAL (2) - значения идентичны
  */
u8 compareDateTime (DataTime_TypeDef *dt1, DataTime_TypeDef *dt2)
{
    /* 1) Проверяем год */
    if (dt1->Year > dt2->Year) { return FIRST_LATER; }
    else if (dt1->Year < dt2->Year) { return FIRST_EARLIER; }
    
    /* 2) Проверяем месяц */
    if (dt1->Month > dt2->Month) { return FIRST_LATER; }
    else if (dt1->Month < dt2->Month) { return FIRST_EARLIER; }
    
    /* 3) Проверяем день */
    if (dt1->Day > dt2->Day) { return FIRST_LATER; }
    else if (dt1->Day < dt2->Day) { return FIRST_EARLIER; }
    
    /* 4) Проверяем час */
    if (dt1->Hour > dt2->Hour) { return FIRST_LATER; }
    else if (dt1->Hour < dt2->Hour) { return FIRST_EARLIER; }
    
    /* 5) Проверяем минуты */
    if (dt1->Min > dt2->Min) { return FIRST_LATER; }
    else if (dt1->Min < dt2->Min) { return FIRST_EARLIER; }
    
    /* 6) Проверяем секунды */
    if (dt1->Sec > dt2->Sec) { return FIRST_LATER; }
    else if (dt1->Sec < dt2->Sec) { return FIRST_EARLIER; }
    
    return DT_EQUAL;
}

/** 
  * @brief      Вычисление времени в секундах между двумя значениями Даты-Времени
  * @param      *dt1 - указатель на первое значение Даты-Времени
  *             *dt2 - указатель на второе значение Даты-Времени
  * @retval     Разница между двумя значениями Даты-Времени
  */
double calculateTime (DataTime_TypeDef *dt1, DataTime_TypeDef *dt2)
{
  struct tm T1;
  struct tm T2;
  time_t Time1;
  time_t Time2;
  double Res;
  
  /* Заполняем структуры стандартной библиотеки <time.h> */
  T1.tm_sec = dt1->Sec;         // Секунды от начала минуты - [0,60]
  T1.tm_min = dt1->Min;         // Минуты от начала часа - [0,59]
  T1.tm_hour = dt1->Hour;       // Часы от полуночи - [0,23]
  T1.tm_mday = dt1->Day;        // Число месяца - [1,31]
  T1.tm_mon = dt1->Month - 1;   // Месяцы после января - [0,11]
  T1.tm_year = dt1->Year + 100; // Года с 1900
  //T1.tm_wday = 5;                  // Дни с воскресенья - [0,6]
  //T1.tm_yday = 102;                  // Дни с первого января - [0,365]
  T1.tm_isdst = 1;              // Признак летнего времени
                                 /* Поле tm_isdst имеет положительное значение, если активен режим летнего времени, 
                                 нуль в противном случае     
                                 и отрицательное значение, если информация о сезоне времени недоступна/неизвестна*/
  T2.tm_sec = dt2->Sec;         
  T2.tm_min = dt2->Min;         
  T2.tm_hour = dt2->Hour;       
  T2.tm_mday = dt2->Day;        
  T2.tm_mon = dt2->Month - 1;   
  T2.tm_year = dt2->Year + 100;       
  //T2.tm_wday = 1;                 
  //T2.tm_yday = 104;                 
  T2.tm_isdst = 1;             
  
  // Преобразуем время, заданное структурами *T1 и *T2, в календарное
  Time1 = mktime(&T1);
  Time2 = mktime(&T2);
  // Получаем разность Time2 - Time1, выраженную в секундах.
  Res = (u32)fabs(difftime(Time2, Time1));

  return Res;
}

