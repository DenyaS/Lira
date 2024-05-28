

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "stm32F10x.h"
#include "STM32vldiscovery.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  LSE_FAIL_FLAG  0x80
#define  LSE_PASS_FLAG  0x100

#define WORK_DIR      ROTDIR_CCW       // Направление вращения "работа тележки"
#define RETURN_DIR    ROTDIR_CW        // Направление вращения "возврат тележки"

#define START_ENABLE    !GPIO_ReadOutputDataBit(OUT_GPIO_PORT, LEDSTART_DOUT)
#define WORK_ENABLE     !GPIO_ReadOutputDataBit(OUT_GPIO_PORT, WORK_DOUT)
#define ERROR_ENABLE    !GPIO_ReadOutputDataBit(OUT_GPIO_PORT, ERROR_DOUT)  
    
//Сообщение автомату модуля подачи
   
/* Private macro -------------------------------------------------------------*/
/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
u32 count = 0;
u32 BlinkSpeed = 0;
u32 KeyState = 0;
static __IO uint32_t TimingDelay;
vu32 SecDel = 0;
vu32 uSecDel = 0;
u8 ErrCode;
u16 TmpVar16 = 0;
u32 TmpVar32 = 0;
u8 SEN_CM_old_state = 0;
u16 test_coord = 1000;       //Debug
u16 coordA = 0;
u8 state_init = 0;

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nTime);
void TimingDelay_Decrement(void);
void handler_Buttons(void);
void handler_Sensors(void);
void Con_air_cyl_Man(void);
void Con_air_cyl_Zah(void);
void Con_air_cyl_Fin(void);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
 int main (void)
{
  /* Enable GPIOx Clock */
  
    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Initialise LEDs LD3&LD4, both off */
  STM32vldiscovery_LEDInit(LED3);
  STM32vldiscovery_LEDInit(LED4);
  STM32vldiscovery_LEDOff(LED3);
  STM32vldiscovery_LEDOff(LED4);
  
  /* Initialise USER Button */
  STM32vldiscovery_PBInit(BUTTON_USER, BUTTON_MODE_GPIO); 
  
  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
  
  // Инициализируем таймер 6
  init_TimerTIM6(1, 100);    // 100 us
    
  // Инициализируем таймер 7                 
 // init_TimerTIM7(10, 10);    // 100 us  
  
  // Инициализация таймера 3 для енкодера Z
  init_TimerTIM3();
  
  // Инициализация таймера 4 для енкодера X
  init_TimerTIM4();  
  
    // Инициализация таймера 2 для енкодера Y
  init_TimerTIM2();
 
    // Инициализация таймера 1 для енкодера A
  init_TimerTIM1(); 
  
  
  // Включаем входы для енкодеров  
  init_Encoders();  
  
  // Конфигурируем выходы для драйверов ШД
  init_OutputsDriversSM();
   // Инициализируем UARTы
 // init_UART0();       //Пересекается с енкодером оси А pin PA8 "AA"
  init_UART1();
  
  // Инициализируем входы (кнопки, датчики)
  init_Inputs();
  
  // Инициализация выходов
  init_Outputs();
  
  // Обнуляем флаги системы
  Flags.Flw = 0;
  
  // Состояния моторов
  for (int i = 0; i < 3; i++) { Motor[i].State = MRS_Idle;}
  MotorZ.Dir = WORK_DIR;
  MotorZ.MaxError = 100;
  
// Сбрасываем счётчики энкодеров
          TIM_SetCounter(TIM4, 0);        // Сбрасываем счетчик енкодера 
        TIM_SetCounter(TIM2, 0);          // Сбрасываем счетчик енкодера 
     TIM_SetCounter(TIM3, 0);       // Сбрасываем счетчик енкодера 
 TIM_SetCounter(TIM1, 0);          // Сбрасываем счетчик енкодера 
  
  // Включаем WatchDog
//  setup_IWDG();
  /* main while */
  while(1)
  {

    _WDR();     // reset Watchdog
    
    cpl();      // Индикатор главного цикла
    // Управляем цилиндрами
    Con_air_cyl_Man();
    Con_air_cyl_Zah();
    Con_air_cyl_Fin();
    
    
    // Проверяем кнопки
 //   handler_Buttons();
    
    // Проверяем датчики
//    handler_Sensors();
    
    // Крутим мотор
    if (handler_RotateMotor(_Z_) == ERR_ROTATION) {
      on(_ERROR);
      on(_BUZZER);
//      MotorZ.State = MRS_Complete;     // останавливаемся  
      ErrCodeAll = ERR_ROTATION_Z;
    }
     if (handler_RotateMotor(_X_) == ERR_ROTATION) {
      on(_ERROR);
      on(_BUZZER);
      MotorX.State = MRS_Complete;     // останавливаемся  
      ErrCodeAll = ERR_ROTATION_X;
    }   
     if (handler_RotateMotor(_Y_) == ERR_ROTATION) {
      on(_ERROR);
      on(_BUZZER);
      MotorY.State = MRS_Complete;     // останавливаемся 
     ErrCodeAll = ERR_ROTATION_Y; 
    } 
     if (handler_RotateMotor(_A_) == ERR_ROTATION) {
      on(_ERROR);
      on(_BUZZER);
      MotorA.State = MRS_Complete;     // останавливаемся 
      ErrCodeAll = ERR_ROTATION_A; 
    }   
    
    //-----------------------------Инициализация Осей-----------------
    switch (state_init)
    {
    case init_A:
    ErrCode = init_axis(_A_, 100); // Инициализация оси А
    if (State_Init_Axis == init_Complete) {State_Init_Axis = start_init; state_init = init_Y;} //Переходим к иниц. след оси
    break;
//    case init_X:
//    ErrCode = init_axis(_X_, 100); // Инициализация оси X
//    if (State_Init_Axis == init_Complete) {State_Init_Axis = start_init; state_init = init_Y;} //Переходим к иниц. след оси
//    break;    
    case init_Y:  
    ErrCode = init_axis(_Y_, 200); // Инициализация оси Y
    break;
    }

 
    //---------------------Тест осей Y и A----------------------------------
    if((MessegSM==ReInitSM))
    { 
      /*
      MessegSM=NewTaskSM;        
         MotorA.MaxError = 10;        //Макс кол-во ошибок
         MotorA.Start = 10;          // Начальная скорость    
         MotorA.Imp = 10;               // Скорость вращения 
         MotorA.NextCoor = 3000;
         go_to_poin (_A_);
      */
      
//         coordA = TIM_GetCounter(TIM1);
//         go_to_poin (_Y_, 1000, 15);
         //Y
         /*
         MotorY.MaxError = 10;        //Макс кол-во ошибок
         MotorY.Steps = 3000;     // 
         MotorY.Start = 10;          // Начальная скорость
         MotorY.Dir = forwardSM;    // Направление вращения        
         MotorY.Imp = 3;                // Скорость вращения 
         MotorY.State = MRS_Start; // Запускаем мотор 
        MessegSM=NewTaskSM; 
         */
    }
    /*------------------Автомат модуля подачи-----------------------------*/
  switch (StateSM){
    case InitSM:      //--------------------------------Начальное состояние при включении или получении нового задания---
      if (MessegSM==NewTaskSM)   // Получено новое задание
      {
         MessegSM=NoNewTaskSM;      //
        if (fl_SEN_SM3)             //Проверяем датчик по выходу модуля выравнивания - полоса не забрана
        { 
          CodeErrorSM=bahd_exit; 
          StateSM=ErrorSM;
         break;
        } 
        if (fl_SEN_SM2) //Полоса под роликом делаем мерную протяжку
        {
         MotorZ.MaxError = 10;        //Макс кол-во ошибок
         MotorZ.Steps = LengthPol*400*100/314/25;     // крутим 200 шагов 0.5 оборота 180гр = 200*1,8/2
         MotorZ.Start = 10;          // Начальная скорость
         MotorZ.Dir = forwardSM;    // Направление вращения        
         MotorZ.Imp = speedSM;                // Скорость вращения 
         MotorZ.State = MRS_Start; // Запускаем мотор 
         StateSM=Main_pull;      //Если полоса затянута через ролик енкодера то переходим к основному состоянию
          break;
        }
        if (fl_SEN_SM1)
        {
         MotorZ.MaxError = 2400;        //Макс кол-во ошибок, может потом добавить переменную?
         MotorZ.Steps = 2500;          //На сколько протянуть прикинул, может потом добавить переменную?
         MotorZ.Start = 10;          // Начальная скорость
         MotorZ.Dir = forwardSM;    // Направление вращения         
         MotorZ.Imp = speedSM;                // Скорость вращения     
         MotorZ.State = MRS_Start; // Запускаем мотор
         Timer_Sup_roller = 15;    //Устанавливаем таймер на затягивание под ролик
         StateSM=Sup_roller;      //Если есть на входе то переходим к протяжки под ролик енкодера
         break;
        }
        else  {StateSM=Wait_strip; Timer_inputSM = 30; break;}          //Полосы нет на входе переходим к ожиданию ставим таймер ожидания
      }            
      break;
    case Wait_strip:            //----------------------------------Ожидание полосы-------------------------
      if (fl_SEN_SM1)
      {
         MotorZ.MaxError = 2400;        //Макс кол-во ошибок, может потом добавить переменную?
         MotorZ.Steps = 2500;          //На сколько протянуть прикинул, может потом добавить переменную?
         MotorZ.Start = 10;          // Начальная скорость
         MotorZ.Dir = forwardSM;    // Направление вращения         
         MotorZ.Imp = speedSM;                // Скорость вращения     
         MotorZ.State = MRS_Start; // Запускаем мотор
         Timer_Sup_roller = 15;    //Устанавливаем таймер на затягивание под ролик
         StateSM=Sup_roller;   //Полоса появилась переходим к затягиванию под ролик
         break;
      }
      else         
      {
        if(Timer_inputSM);
        else {StateSM=ErrorSM; CodeErrorSM=NoStrip;break; } //Таймер вышел а полосы нет              
      }
      break;
    case Sup_roller:    //-------------Затягивание полосы под мерный ролик-----------------
        if (fl_SEN_SM2) //Затянули под ролик устанавливает основную протяжку
        {
         MotorZ.MaxError = 10;        //Макс кол-во ошибок
         MotorZ.Steps = LengthPol*400*100/314/25;     // крутим 200 шагов 0.5 оборота 180гр = 200*1,8/2
         MotorZ.Start = 10;          // Начальная скорость
         MotorZ.Dir = forwardSM;    // Направление вращения        
         MotorZ.Imp = speedSM;                // Скорость вращения 
 //        MotorZ.State = MRS_Start; // Запускаем мотор 
         StateSM = Main_pull;   //Переходи к основной мерной протяжки
        }
        else 
        {
          if (handler_RotateMotor(_Z_) == ERR_ROTATION) //Если затягивали под ролик и не затянули ошибка по кол-ву ошибок
          {
           fl_ERR_ROT_Z = 0;
           CodeErrorSM=not_tightened_strip; 
           StateSM=ErrorSM; 
           break;
          }
          if(Timer_Sup_roller);
          else {StateSM=ErrorSM; CodeErrorSM=not_tightened_strip_timer; break; }//Если затягивали под ролик и не затянули ошибка по таймеру
        }
      break;
    case Main_pull:     //----------------------------Основная протяжка------------------------------------
      if (handler_RotateMotor(_Z_) == ERR_ROTATION)
      {
        CodeErrorSM=ErrRotSM;
        StateSM=ErrorSM;  //Если протягивали на основной и не протянули
      }
      if (handler_RotateMotor(_Z_)==OK_COMPLETE){StateSM = CompleteSM;break;}
      break; 
    case ErrorSM:       //---------------Состояние ошибки-----------------------------------------
      if (MessegSM==ReInitSM){ StateSM = InitSM; MotorZ.State = MRS_Complete;}   // Переинициализация модуля !!!!!!сбросить ошибки вращения!!!!!!!!
      break;
    case CompleteSM:    //-------------------Задание выполнено--------------------------------------------
       if (MessegSM==NewTaskSM)StateSM = InitSM;   //Получено новое задание
      break;
    default:  
    break;  
    }
  
/*----------------------Автомат модуля обрезки CN------------------------------*/
switch (StateCM){
      case InitCM:       //Исходное состояние и приполучении задания
        if (fl_SEN_CM)   //Датчик сработал - не в исходном состоянии
        {
         CodeErrorCM=not_initCM;        //Модуль не в исходном положении 
         StateCM= ErrorCM;              // В состояние ошибка
         break;
        }
        else
        {
         if (MessegCM==NewTaskCM)
         {
         MessegCM = NoNewTaskCM;  
         SEN_CM_old_state=0;    //Сохраняет текущее состояние датчика
         on(_RELAY_CM);         //Включаем двигатель
         Timer_RunCM=10;        //Устанавливаем таймер макс времени обрезки
         StateCM=CuttingSM;         //Переходим в состояние "обрезка"
         }
        }
        break;
      case CuttingSM:       //Обрезка выполняется
        if(Timer_RunCM)
          {
          if ((fl_SEN_CM == 1) && (SEN_CM_old_state == 0))SEN_CM_old_state = 1;       //Начался процесс обрезки
          else if ((fl_SEN_CM == 0) && (SEN_CM_old_state == 1)) 
          { 
            off(_RELAY_CM);         //Выключаем двигатель
            StateCM=CompleteCM;        // процесс обрезки завершён   
          }
          break;
          }       
        else
          {
          off(_RELAY_CM);         //Выключаем двигатель
          CodeErrorCM=Exceeded_TimeCM;          //Время на обрезку вышло! 
          StateCM=ErrorCM;                              
          }
      break;  
    case CompleteCM:       //Обрезка выполнена
      if (MessegCM==NewTaskCM)StateCM = InitCM;   //Получено новое задание
      break;  
    case ErrorCM:       //Состояние ошибки
      if (MessegCM==ReInitCM)StateCM=InitCM;   // Переинициализация модуля 
      if (MessegCM==ResetCM)    //Приведение в исходное
      {
         SEN_CM_old_state =0;
         on(_RELAY_CM);         //Включаем двигатель
         Timer_RunCM=10;        //Устанавливаем таймер макс времени обрезки
         StateCM=CuttingSM;         //Переходим в состояние "обрезка"
      }
      break;
    default:  
    break;    
  }

   /*Приём UART DEBUG*/
//    if(readByte_UART1()== 101)STM32vldiscovery_LEDOn(LED3);
     /*Приём UART END DEBUG*/
/*----------------------------------------------------------------------------*/    
/*------------------- DEBUG with USER_BUTTON -------------------------------- */
    checkPacket_UARTx(1);
    parserPacket_UARTx(1);
    if(0 == STM32vldiscovery_PBGetState(BUTTON_USER))
    {
      if(KeyState == 1)
      {
        if(0 == STM32vldiscovery_PBGetState(BUTTON_USER))STM32vldiscovery_LEDOn(LED4);
        {
          /* USER Button released */
          KeyState = 0;
          /* Turn Off LED4 */
          //STM32vldiscovery_LEDOff(LED4);
        }       
      }
    }
    else if(STM32vldiscovery_PBGetState(BUTTON_USER))
    { 
      if(KeyState == 0)
      {
        if(STM32vldiscovery_PBGetState(BUTTON_USER))
        {
          //--------- DEBUG MOTOR ROTATE ---------------
          /* 400 = 1 оборот; по час. стрелке, 5 = скорость вращения
             20 = нач. скорость, 10 - темп разгона */
           
 
          //Моя добавка
  //         sendByte_UART1(102);
          /**  
  * @brief      Сформировать пакет данных и передать его через UART
  * @param      uart_number - номер используемого UARTа: 0 = UART0 или 1 = UART1
  *             adr_dst - адрес получателя            
  *             cmd - код команды
  *             *ptr - указатель на данные
  *             len - размер данных
  * @retval     None
  */
   //       sendPacket_UARTx(1,0,102, 0, 0);  
          
/*                        // Задаем параметры вращения Z
         Motor[2].State = MRS_Idle;
         MotorZ.MaxError = 10;        //Макс кол-во ошибок 
         MotorZ.Steps = 1000;     // крутим 200 шагов 0.5 оборота 180гр = 200*1,8/2
         MotorZ.Start = 10;          // Начальная скорость
         MotorZ.Dir = WORK_DIR;    // Направление вращения
         MotorZ.Imp = 1;             // Скорость вращения                   
         MotorZ.State = MRS_Start;// Запускаем мотор
          
          
                    // Задаем параметры вращения X
         MotorX.MaxError = 10;        //Макс кол-во ошибок 
         MotorX.Steps = 1000;     // крутим 200 шагов 0.5 оборота 180гр = 200*1,8/2
         MotorX.Start = 10;          // Начальная скорость
         MotorX.Dir = WORK_DIR;    // Направление вращения
         MotorX.Imp = 10;             // Скорость вращения
         MotorX.State = MRS_Start;// Запускаем мотор
  */        
          //Моя добавка the end 
  //        TIM_SetCounter(TIM3, 0);
  //        rotateSpecParam_Motor(_Z_, 400 * 10, MotorZ.Dir, 5, 20, 10);
          //--------------------------------------------
          
          /* USER Button released */
          KeyState = 1;
          /* Turn ON LED4 */
          //STM32vldiscovery_LEDOn(LED4);
          //Delay(1000);
          /* Turn OFF LED4 */
          //STM32vldiscovery_LEDOff(LED4);
        }
      }
      else 
     {
        if(STM32vldiscovery_PBGetState(BUTTON_USER))
        {       
 //       MotorZ.State = MRS_Complete;     // останавливаемся  
        }
     }
    }
    count++;
    //Delay(100);
    /* BlinkSpeed: 0 */ 
    if(BlinkSpeed == 0)
    {
      /*
      if(4 == (count % 8))
        STM32vldiscovery_LEDOn(LED3);
      if(0 == (count % 8))
        STM32vldiscovery_LEDOff(LED3);
      */
    }
    /* BlinkSpeed: 1 */ 
    if(BlinkSpeed == 1)
    {
      /*
      if(2 == (count % 4))
        STM32vldiscovery_LEDOn(LED3);
      if(0 == (count % 4))
        STM32vldiscovery_LEDOff(LED3);
      */
    }  
    /* BlinkSpeed: 2 */        
    if(BlinkSpeed == 2)
    {
      /*
      if(0 == (count % 2))
        STM32vldiscovery_LEDOn(LED3);
      else
        STM32vldiscovery_LEDOff(LED3);
      */
    }     
     /* BlinkSpeed: 3 */ 
    else if(BlinkSpeed == 3)
      BlinkSpeed = 0;
/*------------------ DEBUG with USER_BUTTON ----------------------------------*/
/*----------------------------------------------------------------------------*/    
  }
}
/** 
  * @brief      Функция управление воздушным цилиндром выдвижение манипулятора  
  * @param      None
  * @retval     
  */
void Con_air_cyl_Man(void)
{
   switch (state_Man)
   {
   case Cyl_init_Man:
     if (fl_Man_Ahead)state_Man = Cyl_Ahead_Man;
     if (fl_Man_Behind)state_Man = Cyl_Behind_Man; 
    break; 
  case Cyl_Behind_Man:  //цилиндр задвинут
    if (fl_Man_Behind == 0)ErrCodeAll = ERR_BEHIND_MAN_SEN;       //Цилинд отошёл от датчика, когда он задвинут
    if(fl_Air_Cyl_Man)         
    {
      Timer_Man_Ahead = 5;      //Время на выдвижение цилиндра
      state_Man = Cyl_Comes_Out_Man;
      on(_RELAY_MAN);   //Открываем пневмоклапан
    }
    break;
  case Cyl_Ahead_Man:   //цилиндр выдвинут
     if (fl_Man_Ahead == 0)ErrCodeAll = ERR_AHEAD_MAN_SEN;       //Цилинд отошёл от датчика, когда он выдвинут 
     if(fl_Air_Cyl_Man == 0)
    {
      Timer_Man_Behind = 5;      //Время на задвижение цилиндра
      state_Man = Cyl_Comes_Man;
      off(_RELAY_MAN);   //закрываем пневмоклапан
    }
    break;
  case Cyl_Comes_Out_Man: //цилиндр выходит
    if (fl_Man_Ahead)state_Man = Cyl_Ahead_Man;
    else {if(Timer_Man_Ahead == 0) ErrCodeAll = ERR_AHEAD_MAN;}
    break;
  case Cyl_Comes_Man:     //цилиндр заходит
    if (fl_Man_Behind)state_Man = Cyl_Behind_Man;
    else {if(Timer_Man_Behind == 0) ErrCodeAll = ERR_BEHIND_MAN;}
    break;    
   }
}
/** 
  * @brief      Функция управление воздушным цилиндром захват манипулятора
  * @param      None
  * @retval     
  */
void Con_air_cyl_Zah(void)
{
   switch (state_Zah)
   {
   case Cyl_init_Zah:
     if (fl_Zah_Open)state_Zah = Open_Zah;
     if (fl_Zah_Close)state_Zah = Close_Zah; 
    break; 
  case Close_Zah:  //цилиндр задвинут
    if (fl_Zah_Close == 0)ErrCodeAll = ERR_CLOSE_ZAH_SEN;       //Цилинд отошёл от датчика, когда он задвинут
    if(fl_Air_Cyl_Zah)         
    {
      Timer_Zah_Open = 5;      //Время на выдвижение цилиндра
      state_Zah = Cyl_Comes_Out_Zah;
      on(_RELAY_ZAH);   //Открываем пневмоклапан
    }
    break;
  case Open_Zah:   //цилиндр выдвинут
     if (fl_Zah_Open == 0)ErrCodeAll = ERR_OPEN_ZAH_SEN;       //Цилинд отошёл от датчика, когда он выдвинут 
     if(fl_Air_Cyl_Zah == 0)
    {
      Timer_Zah_Close = 5;      //Время на задвижение цилиндра
      state_Zah = Cyl_Comes_Zah;
      off(_RELAY_ZAH);   //закрываем пневмоклапан
    }
    break;
  case Cyl_Comes_Out_Zah: //цилиндр выходит
    if (fl_Zah_Open)state_Zah = Open_Zah;
    else {if(Timer_Zah_Open == 0) ErrCodeAll = ERR_OPEN_ZAH;}
    break;
  case Cyl_Comes_Zah:     //цилиндр заходит
    if (fl_Zah_Close)state_Zah = Close_Zah; 
    else {if(Timer_Zah_Close == 0) ErrCodeAll = ERR_CLOSE_ZAH;}
    break;    
   }
}
/** 
  * @brief      Функция управление воздушным цилиндром выдвижение пальца  
  * @param      None
  * @retval     
  */
void Con_air_cyl_Fin(void)
{
   switch (state_Fin)
   {
   case Cyl_init_Fin:
     if (fl_Fin_Ahead)state_Man = Cyl_Ahead_Fin;
     if (fl_Fin_Behind)state_Man = Cyl_Behind_Fin; 
    break; 
  case Cyl_Behind_Fin:  //цилиндр задвинут
    if (fl_Fin_Behind == 0)ErrCodeAll = ERR_BEHIND_FIN_SEN;       //Цилинд отошёл от датчика, когда он задвинут
    if(fl_Air_Cyl_Fin)         
    {
      Timer_Fin_Ahead = 5;      //Время на выдвижение цилиндра
      state_Fin = Cyl_Comes_Out_Fin;
      on(_RELAY_FIN);   //Открываем пневмоклапан
    }
    break;
  case Cyl_Ahead_Fin:   //цилиндр выдвинут
     if (fl_Fin_Ahead == 0)ErrCodeAll = ERR_AHEAD_FIN_SEN;       //Цилинд отошёл от датчика, когда он выдвинут 
     if(fl_Air_Cyl_Fin == 0)
    {
      Timer_Fin_Behind = 5;      //Время на задвижение цилиндра
      state_Fin = Cyl_Comes_Fin;
      off(_RELAY_FIN);   //закрываем пневмоклапан
    }
    break;
  case Cyl_Comes_Out_Fin: //цилиндр выходит
    if (fl_Fin_Ahead)state_Fin = Cyl_Ahead_Fin;
    else {if(Timer_Fin_Ahead == 0) ErrCodeAll = ERR_AHEAD_FIN;}
    break;
  case Cyl_Comes_Fin:     //цилиндр заходит
    if (fl_Fin_Behind)state_Fin = Cyl_Behind_Fin;
    else {if(Timer_Fin_Behind == 0) ErrCodeAll = ERR_BEHIND_FIN;}
    break;    
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
  * @brief  Decrements the Timers variables.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }

  //---------------- Обработка таймеров 1ms -----------------
  if (TimerCPL != 0x00) TimerCPL--;
  if (TimerLED != 0x00) TimerLED--;
  if (TimeForDelay != 0x00) TimeForDelay--;

  /*------------------- 1-секундные интервалы -----------------*/
  if (SecDel != 0x00) SecDel--;
  else {
	SecDel = 1000;
    if (STimer1 != 0x00) STimer1--;
    if (STimer2 != 0x00) STimer2--;
    if (Timer_inputSM != 0x00) Timer_inputSM--;         //Таймер модуля выравнивания ожидание полосы
    if (Timer_RunCM != 0x00) Timer_RunCM--;             //Таймер модуля обрезки максимальное время работы двигателя обрезки
    if (Timer_Sup_roller != 0x00) Timer_Sup_roller--;        //Таймер ожидания на затягивание под ролик
    if (Timer_Init_axis != 0x00) Timer_Init_axis--;           //Таймер перед реверсом
    if (Timer_Man_Ahead != 0x00) Timer_Man_Ahead--;           //Таймер выдвижения манипулятора
    if (Timer_Man_Behind != 0x00) Timer_Man_Behind--;           //Таймер задвижения манипулятора
    if (Timer_Zah_Open != 0x00) Timer_Zah_Open--;           //Таймер открытия захвата манипулятора
    if (Timer_Zah_Close != 0x00) Timer_Zah_Close--;           //Таймер закрытия захвата манипулятора
    if (Timer_Fin_Ahead != 0x00) Timer_Fin_Ahead--;           //Таймер закрытия захвата манипулятор
    if (Timer_Fin_Behind != 0x00) Timer_Fin_Behind--;           //Таймер закрытия захвата манипулятор
  }
}

/**
  * @brief  Decrements the Timers for Stepper Motors Drivers variables.
  * @param  None
  * @retval None
  */
void stepMotorsTimers_Decrement (void)
{
   if (MotorX.Timer != 0x00) MotorX.Timer--;
   if (MotorY.Timer != 0x00) MotorY.Timer--;
   if (MotorZ.Timer != 0x00) MotorZ.Timer--;
   if (MotorA.Timer != 0x00) MotorA.Timer--;
}

/**
  * @brief  Decrements the Timers for Encoders.
  * @param  None
  * @retval None
  */

void encodersTimers_Decrement (void)
{
  EncX.TickA++;
  EncX.TickB++;
  EncY.TickA++;
  EncY.TickB++;
  EncZ.TickA++;
  EncZ.TickB++;
  EncA.TickA++;
  EncA.TickB++;

  if (uSecDel != 0x00) { uSecDel--; }
  else {
	uSecDel = 10;
  }
  
  if (EncX.Timer != 0x00) EncX.Timer--;
  if (EncY.Timer != 0x00) EncY.Timer--;
  if (EncZ.Timer != 0x00) EncZ.Timer--;
  if (EncA.Timer != 0x00) EncA.Timer--;
}

/** 
  * @brief      Обработка кнопок и джойстика
  * @param      None
  * @retval     None
  */
#define STATE_ZERO      0
#define STATE_ERROR     1
#define STATE_START     2
#define STATE_ERRSTART  3






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


