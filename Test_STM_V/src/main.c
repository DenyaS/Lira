
/* Includes ------------------------------------------------------------------*/
#include "stm32F10x.h"
#include "STM32vldiscovery.h"
#include "main.h"
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  LSE_FAIL_FLAG  0x80
#define  LSE_PASS_FLAG  0x100
/* Private macro -------------------------------------------------------------*/
/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
u32 LSE_Delay = 0;
u32 count = 0;
u32 BlinkSpeed = 0;
u32 KeyState = 0;
static __IO uint32_t TimingDelay;
vu32 SecDel = 0;
double GotCnt = 0;
double FixedCnt = 0;
double SpeedMS = 0;
uchar Turn = 0;

// ==== DEBUG ==== 
struct sup_motor {               //Тест посылки прокрутки мотора
  u16 protazka;
  u8 intens;
} ;
u8 StateTest = 0;
/*--------------------- ПОТОМ УДАЛИТЬ! ---------------------------------------*/
volatile uchar TmpBt = 0;
volatile ulong TmpAddr;
uchar TmpArr[256];
extern TEvent TmpEventSet;
extern TEvent TmpEventGet;
u32 TmpVarSet;
u32 TmpVarGet;
u32 TmpArrSet[32];
u32 TmpArrGet[32];
u8 TmpArrSet_2[10];
u8 TmpArrGet_2[10];
TRtcPPD _RTC;
DataTime_TypeDef TmpDT;
u8 TmpFlag = 0;
/*----------------------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nTime);
void TimingDelay_Decrement(void);

/* Private functions ---------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  vu8 Var08;
  vu16 Var16;
  vu32 Var32;

  /* Enable GPIOx Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  // Инициализация выходов
  init_Outputs();
  on(SND);

  /* Initialise LEDs LD3&LD4, both off */
  init_Leds();
  
  /* Initialise USER Button */
  init_UserButton(INPUT_MODE_GPIO);
  
  // Setup SysTick Timer for 1 ms interrupts
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    // Capture error  
    while (1) { on(SND); }
  }

  // Init internal structure [TSystemStatus SysStat]
  uint8_t *pSysStat = (uint8_t *)&SysStat;
  for(uint8_t i = 0; i < sizeof(TSystemStatus); i++) 
  { *(pSysStat + i) = 0; }

  // Инициализируем Базовый Таймер TIM7:
  // срабатывание каждую 0,01 мс, 
  // обработка входов - кнопок клавиатуры и датчиков
  init_TimerTIM7(1, 10);

  // Конфигурируем порт для дисплея MTC-S16204XR
  pinConfig_LCD_MTC();

  // Инициализируем UARTы
  init_UART0();
  init_UART1();
  
  // Инициализация I2C
  init_I2C();
  
  // Инициализация таймера
  if (init_PCF8563T() == ERROR) {
    while (1) { blink(LEDS_GPIO_PORT, LEDGREEN, 50, SND_OFF); blink(OUTPUTS_GPIO_PORT, LED_ERROR, 50, SND_ON); }
  }
  
  // Инициализация выводов управления платой DebugBoard
  init_DebugBoard();
  
  // Инициализируем SPI
  init_SPI();
  
  // Инициализация внешней DataFlash AT45DB161D
  //init_AT45(); 

  // Инициализация радиомодуля
 // init_MRF49XA();
  
  // Инициализация входов
  init_Sensors();
  init_Keyboard();
    /* Initialise USER Button */
  STM32vldiscovery_PBInit(BUTTON_USER, BUTTON_MODE_GPIO); 
  
  // Debug: erase 127 page of FLASH
  /*
  FLASH_Unlock();
  FLASH_ErasePage(ADR_DEFAULTVALS_BASE);
  FLASH_Lock();
  */

  // Запись во FLASH параметров по умолчанию (при первом запуске)
 // writeDefaultParams();

  // Установка параметров пользователя
  // setUserParams();
  
  // Инициализируем Базовый Таймер TIM6:
  // отвечает за контрастность дисплея
  /** Внимание! Параметры таймера подобраны исходя из 
  схемотехники узла питания дисплея*/
  init_TimerTIM6(18, SysParams.LCD_Contrast);       
  
  // Инициализируем дисплей
  lcd_init();
  lcd_set_state(LCD_ENABLE, CURSOR_DISABLE, NO_BLINK);

  // Init pointers for Events in DataFlash
  //StartAddrEventDF = ADR_START_EXTFLASH + (DF_SIZE - (__MAXQTYEVENTS * sizeof(TEvent)) - 1); 
  //CurrentAddrEventDF = StartAddrEventDF + (SysParams.IndexWrite * sizeof(TEvent)); 

  // Обнуляем флаги системы
  Flags.Flw = 0;
  
  // Проверяем указатели 
  if (SysParams.IndexWrite > __MAXQTYEVENTS) { SysParams.IndexWrite = 0; }
  if (SysParams.IndexRead > __MAXQTYEVENTS) { SysParams.IndexRead = 0; }
  SysStat.NonReadEvents = countQtyEvents();
  setData((uchar *)&SysParams.IndexRead, ADR_INDEXREAD, 4);
  setData((uchar *)&SysParams.IndexWrite, ADR_INDEXWRITE, 4);

  // Разрешаем отображение времени на дисплее
  fl_ShowTime = 1;

  // Очищаем дисплей
  lcd_clear();

  /* Re-init LCD */
  lcd_init(); lcd_init(); 
  lcd_set_state(LCD_ENABLE, CURSOR_DISABLE, NO_BLINK);    // При первом запуске контроллера надо выставлять признак, что двери открыты

  /* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  { 
    /* IWDGRST flag set */
    fl_WDreset = 1;
    /* Clear reset flags */
    RCC_ClearFlag();
    /* Enable synchro Radio */
    fl_SendSynchroRF = 1;
    /* Enable Timer for Driver Unit */
    STimerShowSeats = __TIMEOUTSHOWSEATS;//0;
  }
  else
  { /* IWDGRST flag is not set */
    // Только включили контроллер, поэтому надо разбудить датчики и записать событие
    fl_SendSynchroRF = 0;
    fl_OpenCar = 1;
#ifdef __SHOWHAIL__
    // Показать сообщение-приветствие
    on(BKL);
#endif
  }
  
  // Показываем часы через 1 сек
  TimerGetDT = 1000;    // 1s

  // Устанавливаем таймаут синхронизации радиоканала
  TimerSynchroRF = __TIMERSYNCHRORF;
  
  // Включаем на 10 сек подсветку дисплея
  STimerBacklight = 10;
  
  // Начинаем пинговать GPS терминал через 20 сек после включения
  STimerPingGPS = 20;
  
  // Событие автоматически сгенерируется через 1 мин. после включения
  STimerMakeEvent = 60;  

  /* Устанавливаем время ожидания соединения с датчиками PPD */
  SysStat.TimeWaitConnectRF = WAITCONNECT_RF;
  
  /* Читаем из памяти ExtFlash последнее событие, не сдвигая указатель, 
  и инициализируем одометр */
  if (SysParams.IndexWrite == 0) { SysParams.IndexWrite = __MAXQTYEVENTS; }
  getEvent(&CurrEvent, SysParams.IndexWrite - 1, STAY);
  Odometer = CurrEvent.Mileage;
  
  if (fl_WDreset) {
    fl_WDreset = 0;
    /* Turn on LED_ERROR and display Error */
    displayError(ERR_WDT_RESET);
  }
 //Тест LCD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  
      sprintf(DString, "Занято     %u", SysStat.OccupSeats);
    displayString(1, 1, DString);
    fl_EnDisp = 1;        // razreshaem vyvod na display Glavnogo Menu
  // Включаем WatchDog
 // setup_IWDG();                               Включить после отладки!!!!!!!!!
  
  // ====================== MAIN while ===============================
  while(1)
  {
    // Reload the WatchDog
    _WDR();                                           

    // Индикатор главного цикла (мигаем синим светодиодом)
    cpl();
    checkError(1);      //Проверка состояний автоматов Лира
  
    // Управление подсветкой дисплея
 //   bkl();

    // Читаем время и дату из внешнего таймера и выводим на дисплей
 //   showTime();
  
//Вывод меню
    handler_Menu();
    // Обработка клавиатуры
 //   handler_Keyboard();

#ifndef __GPS_EMULATOR__
    // Смотрим пакеты от управляющего PC
 //   parserPacket_UARTx(UART_PC);
    
    // Отображаем информацию на дисплее
 //   showInfo(5, 0); // (5) - на 5 сек, (0) - не подсвечивать дисплей 

    // Синхронизируем радиоканал на выбранном номере канала
 //   if (fl_SendSynchroRF) { synchroRF(SysStat.WorkRadioCh, NOSWITCH); }
    
    // Анализируем событие
 //   checkEvent();
    
    // Запрашиваем состояние GPS терминала каждые 10 сек 
 //   checkGPS(10);

    // Отсылаем SMS с данными состояния системы 
    /** --- ВОССТАНОВИТЬ! - (временно не отсылаем, чтобы не тратить деньги)---*/
    //sendStatusSMS();

   

#else   /* Работа в режиме эмуляции GPS-терминала */

#endif /* __GPS_EMULATOR__ */    
    
/*----------------------------------------------------------------------------*/
/* ---------(BEGIN)------- TO REMOVE! ----------------------------------------*/
/*----------------------------------------------------------------------------*/
//    checkPacket_UARTx(1);
    parserPacket_UARTx(RS485);      //Приём пакетов по СОМ1 RS485
 //   if(readByte_UART1()== 102)STM32vldiscovery_LEDOn(LED3); //Приём байта тест UART 
    if(0 == STM32vldiscovery_PBGetState(BUTTON_USER))
    {  
      if(KeyState == 1)
      {
         if(0 == STM32vldiscovery_PBGetState(BUTTON_USER))
         {
            /* USER Button released */
            KeyState = 0;
            /* Turn Off LED4 */
 //           STM32vldiscovery_LEDOff(LED4);
         }       
      }
     }
    else if(STM32vldiscovery_PBGetState(BUTTON_USER))
    {  
      if(KeyState == 0)
      {
        if(STM32vldiscovery_PBGetState(BUTTON_USER))
        {
          
          
         struct sup_motor my_motor;          
 switch (StateTest) {
        case 0:
        sendPacket_UARTx(RS485,SUPPLYUNIT,200,NULL, 0);  //Запрос состояния модуля выравнивания  
//        sendPacket_UARTx(RS485,SUPPLYUNIT,105,NULL, 0);  //Привести всё в исходное
        StateTest++;
        break;  
        case 1:
         my_motor.intens = 20;            //Протянуть с интенсивностью 1
         my_motor.protazka = 300;       //Протянуть на 300 мм
          sendPacket_UARTx(RS485,SUPPLYUNIT,101,(uchar *)&my_motor, 3);
          StateTest++; 
          break;
          case 2:
          sendPacket_UARTx(RS485,SUPPLYUNIT,102,NULL, 0);  //Обрезать 
         StateTest++; 
          break;
          case 3:
         my_motor.intens = 10;            //Протянуть с интенсивностью 1
         my_motor.protazka = 3000;       //Занять позицию
         sendPacket_UARTx(RS485,SUPPLYUNIT,103,(uchar *)&my_motor, 3);
         StateTest++; 
         break;    
          case 4:       //Выдвинуть цилиндр
         sendPacket_UARTx(RS485,SUPPLYUNIT,104,NULL, 0);
         StateTest++; 
         break;   
          case 5:       //Задвинуть цилиндр
         sendPacket_UARTx(RS485,SUPPLYUNIT,105,NULL, 0);
         StateTest++; 
         break;  
          case 6:       //Сбросить ошибки - переинициализация
         sendPacket_UARTx(RS485,SUPPLYUNIT,106,NULL, 0);
         StateTest=0; 
         break;             
 }
  //        sendByte_UART1(101);  //Test UART передача байта       //Debug
   //       sendPacket_UARTx(1,3,102, 0, 0);          //Debug
          /* USER Button released */
          KeyState = 1;
          /* Turn ON LED4 */
          STM32vldiscovery_LEDOn(LED4);
          /* Turn OFF LED4 */
          STM32vldiscovery_LEDOff(LED4);
          /* BlinkSpeed: 0 -> 1 -> 2, then re-cycle */    
          BlinkSpeed ++ ;
        }
      }
    }
    count++;
    //delay_1ms(50);
    /* BlinkSpeed: 0 */ 
    if(BlinkSpeed == 0)
    {
      if(8 == (count % 16))
      {
 //       STM32vldiscovery_LEDOn(LED3);
        //on(SND);
      }
      if(0 == (count % 16))
      {
 //       STM32vldiscovery_LEDOff(LED3);
 //       off(SND);
      }
    }
    /* BlinkSpeed: 1 */ 
    if(BlinkSpeed == 1)
    {
      if(4 == (count % 8))
      {
 //       STM32vldiscovery_LEDOn(LED3);
        //on(SND);
      }
      if(0 == (count % 8))
      {
  //      STM32vldiscovery_LEDOff(LED3);
 //       off(SND);
      }
    }  
    /* BlinkSpeed: 2 */        
    if(BlinkSpeed == 2)
    {
      if(2== (count % 4))
      {
 //       STM32vldiscovery_LEDOn(LED3);
        //on(SND);
      }
      if(0 == (count % 4))
      {
 //       STM32vldiscovery_LEDOff(LED3);
  //      off(SND);
      }
    }     
    /* BlinkSpeed: 3 */ 
    if(BlinkSpeed == 3)
    {
      if(0 == (count % 2))
      {
 //       STM32vldiscovery_LEDOn(LED3);
        //on(SND);
      }
      else
      {
 //       STM32vldiscovery_LEDOff(LED3);     
 //       off(SND);
      }
    }
    /* BlinkSpeed: 4 */ 
    else if(BlinkSpeed == 4)
      _NOP();
    //BlinkSpeed = 0;
/*----------------------------------------------------------------------------*/
/* ---------(END)------- TO REMOVE! ------------------------------------------*/
/*----------------------------------------------------------------------------*/
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  * @note   Interval of decrement set to 1ms
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }

  //---------------- Обработка таймеров -----------------
  if (TimerCPL != 0x00) TimerCPL--;
  if (TimerMenuExit != 0x00) TimerMenuExit--;
  if (TimeForDelay != 0x00) TimeForDelay--;
  if (TimerKeyboard != 0x00) TimerKeyboard--;
  if (TimerWaitAnswer != 0x00) TimerWaitAnswer--;
  if (TimerSPI != 0x00) TimerSPI--;
  if (TimerBlink != 0x00) TimerBlink--;
  if (TimerBeep != 0x00) TimerBeep--;
  if (TimerFLASH != 0x00) TimerFLASH--;
  if (TimerGetDT != 0x00) TimerGetDT--;
  if (TimerRF != 0x00) TimerRF--;
  if (TimerReceivePack != 0x00) TimerReceivePack--;
  if (TimerForAll != 0x00) TimerForAll--;
  if (TimerWaitAckSendSMS != 0x00) TimerWaitAckSendSMS--;
  if (TimerSynchroRF != 0x00) TimerSynchroRF--;
  if (TimerAwake != 0x00) TimerAwake--;
  if (TimerAutoSMS != 0x00) TimerAutoSMS--;

  if (Radio.ScanTimer != 0x00) Radio.ScanTimer--;
  if (Radio.Timer != 0x00) Radio.Timer--;
  if (UARTx[0].Timer != 0x00) UARTx[0].Timer--;
  if (UARTx[1].Timer != 0x00) UARTx[1].Timer--;
  if (TimerStateCheck != 0x00)TimerStateCheck--;

  /* 1-секундные интервалы */
  if (SecDel != 0x00) SecDel--;
  else {
	SecDel = 1000;
    if (STimerShowSeats != 0x00) STimerShowSeats--;
    if (STimerPingGPS != 0x00) STimerPingGPS--;
    if (STimerMakeEvent != 0x00) STimerMakeEvent--;
    if (STimerAutoSMS != 0x00) STimerAutoSMS--;
    if (STimerBacklight != 0x00) STimerBacklight--;
    if (STimerStopShowTime != 0x00) STimerStopShowTime--;
  }
}

/** 
  * @brief      Управление меню
  * @param      None
  * @retval     None
  */
void handler_Menu (void)
{
  if ((fl_EnterMenu == 0) && (LastKey == ENTER)) {
    fl_EnterMenu = 1;
    MenuPos = MD_1;
    LastKey = 0;
  }
  if (fl_EnterMenu) {   
    retMenuPos();
    displayMenu(MenuPos);
    execMenu();
  }
}

/** 
  * @brief      Обработать данные GPS терминала
  * @param      None
  * @retval     None
  */
void convertDataGPS (void)
{
  TLocation Loc;
  
  Loc.Latitude_IntPart = CurrStatGPS.Latitude_IntPart;
  Loc.Latitude_FrPart = CurrStatGPS.Latitude_FrPart;
  Loc.Longitude_IntPart1 = CurrStatGPS.Longitude_IntPart1;
  Loc.Longitude_IntPart2 = CurrStatGPS.Longitude_IntPart2;
  Loc.Longitude_FrPart = CurrStatGPS.Longitude_FrPart;
  
  // Переводим координаты местоположения в стандартное представление
  convertLocM2M(&SysStat.Coordinates, &Loc);
  
  // Копируем данные в структуру текущих координат
  memcpy(&CurrLoc, &SysStat.Coordinates, sizeof(TGoogleGPS));
  
  // Переводим скорость из узлов в м/с
      /** По международному определению, один узел (1 морская миля в час) 
      равен 1,852 км/ч или 0,514 м/с */
  SpeedMS = (double)CurrStatGPS.Speed * 0.514;
  // Поправляем значение одометра в зависимости от скорости
  GotCnt = (double)getTickCnt();
  if (GotCnt < FixedCnt) { GotCnt = FixedCnt; } // если вдруг счётчик тиков перешел через 0
  Odometer += SpeedMS * ((double)(GotCnt - FixedCnt) / 1000);     // s = v * t
  // Фиксируем время измерения скорости
  FixedCnt = (double)getTickCnt();
  
  // Можно отображать инфо на дисплее
  fl_ShowInfoGPS = 1;
}

/** 
  * @brief      Отправить SMS с данными о состоянии системы
  * @param      None
  * @retval     None
  */
void sendStatusSMS (void)
{
    if(SysParams.SMSInt > 0) {
      if (STimerAutoSMS == 0) {
        STimerAutoSMS = SysParams.SMSInt; // интервал отправки - в секундах
        memset((uchar *)&SysStat.Errors, 0, 4);   // обнуляем флаги ошибок системы
        if(getDateTime(&SysStat.DataTime) == ERROR) { 
          SysStat.Errors.ErrorsExist = 1; 
          SysStat.Errors.ErrSetDT = 1; 
        }
        if(SysParams.IndexWrite != 0) {
          if(getEvent(&CurrEvent, SysParams.IndexWrite - 1, STAY) == ERROR) {
            SysStat.Errors.ErrorsExist = 1;
            SysStat.Errors.ErrGetEvent = 1;
          }
        }
        else {
          memset((uchar *)&CurrEvent, 0, sizeof(CurrEvent));
        }
        SysStat.Version = SysParams.BitVars.Version;
        //SysStat.OccupSeats = CurrEvent.OccupSeats;
        //SysStat.FreeSeats = CurrEvent.FreeSeats;
        //SysStat.GoodPPD = SysParams.QtyPPD - CurrEvent.FailedPPD;
        //SysStat.FailedPPD = CurrEvent.FailedPPD;
        SysStat.NonReadEvents = countQtyEvents();
        handler_SendSMS(UART_GPS, GPSTERM, REQ_GET_STATE, (uchar *)&SysStat);
      }
    }
}

/** 
  * @brief      Отобразить необходимую информацию на дисплее
  * @param      sec - время отображения информации в секундах
  *             bkl - включать (1) или не включать (0) подсветку дисплея
  * @retval     None
  */
void showInfo (uchar sec, uchar bkl)
{
  if (!fl_ShowInfoGPS) return;
  fl_ShowInfoGPS = 0;

  // Включаем на 3 сек подсветку дисплея
  if (bkl) {
    fl_ShowTime = 0; 
    STimerBacklight = sec;
  }
  else {
  // ...или не подсвечиваем
    STimerStopShowTime = sec;
  }

  if (++Turn == 1) {
    sprintf(DString, "%02d^%02d'%02d.%02d\"N", 
          CurrLoc.Lat_Degree, CurrLoc.Lat_Min, CurrLoc.Lat_Sec, CurrLoc.Lat_FrSec);
    displayString(1, 1, DString);
    sprintf(DString, "%d%02d^%02d'%02d.%02d\"E", 
          CurrLoc.Lon_Degree1, CurrLoc.Lon_Degree2, CurrLoc.Lon_Min, CurrLoc.Lon_Sec, CurrLoc.Lon_FrSec);
    displayString(2, 1, DString);
  }
  else if (Turn == 2) {
    sprintf(DString, "%.1f km/h", (double)CurrStatGPS.Speed * 1.852);
    displayString(1, 2, DString);
    displayString(2, 1, "");
  }
  else if (Turn == 3) {
    Turn = 0;
    sprintf(DString, "Занято     %u", SysStat.OccupSeats);
    displayString(1, 1, DString);
    sprintf(DString, "Свободно   %u", SysStat.FreeSeats);
    displayString(2, 1, DString);
  }
}

/** 
  * @brief      Обработка клаыиатуры
  * @param      None
  * @retval     None
  */
void handler_Keyboard (void)
{
  u8 EC;
  
  /** Нажатие кнопки [ENTER] */
  if (LastKey == ENTER) {
    LastKey = 0;
#ifndef __GPS_EMULATOR__
    // Enable synchro Radio
    fl_SendSynchroRF = 1;
#else  /* Работа в режиме эмуляции GPS-терминала */ 
    // Отсылаем запрос на получение данных для отображения на Web-Сервере
    sendPacket_UARTx(UART_PC, ONBOARDCONTR, REQ_GET_WEB_DATA, NULL, 0);
#endif /* __GPS_EMULATOR__ */    
  }
  /** Нажатие кнопки [ESCAPE] */
  if (LastKey == ESCAPE) {
    LastKey = 0;
    /* Запрещаем синхронизацию */
    fl_SendSynchroRF = 0;
  }
  /** Нажатие кнопки [UP] */
  if (LastKey == UP) {
    LastKey = 0;
    EC = handler_CountPPD(SysParams.QtyPPD, WAKE);
    if (EC == NO_ERROR) {  
      printf_Consol("Found: %u, No answer: %u\r\n", (u16)SysStat.GoodPPD, (u16)SysStat.FailedPPD);
    }
  }
  /** Нажатие кнопки [DOWN] */
  if (LastKey == DOWN) {
    LastKey = 0;
    // Сброс по WatchDog
    while(1);
    // ...
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
