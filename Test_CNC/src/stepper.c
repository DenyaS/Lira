
 
/* Includes ------------------------------------------------------------------*/
#include "stepper.h"
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
// DEBUG
vu32 TmpVar;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configures the Stepper Motor Drivers control pins 
  * @param  None
  * @retval None
  */
void init_OutputsDriversSM (void)
{
  //------------Настраиваем порт C---------------------------------
  GPIO_InitTypeDef gpio;
  /* Enable the GPIOs Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE); //Тактируем порт B и С 
  /* Configure the output pins */
  gpio.GPIO_Pin = PUL_Z | DIR_Y;

  // Init I/O port 
  gpio.GPIO_Mode = GPIO_Mode_Out_OD;    // открытый коллектор
  gpio.GPIO_Speed = GPIO_Speed_50MHz;

  // Инициализируем GPIO на порту, к которому подключены драйверы ШД 
  GPIO_Init(GPIOC, &gpio);
 //------------Настраиваем порт B--------------------------------- 
   GPIO_InitTypeDef gpio2;

  /* Configure the output pins */
  gpio2.GPIO_Pin = DIR_Z | PUL_X | DIR_X | PUL_Y | PUL_A | DIR_A;

  // Init I/O port 
  gpio2.GPIO_Mode = GPIO_Mode_Out_OD;    // открытый коллектор
  gpio2.GPIO_Speed = GPIO_Speed_50MHz;

  // Инициализируем GPIO на порту, к которому подключены драйверы ШД 
  GPIO_Init(GPIOB, &gpio2);
  
   
  // Выставляем выходы в 1, чтобы выходы находились в 3-м состоянии
  //GPIO_SetBits(SMOTORS_GPIO_PORT, gpio.GPIO_Pin);
  
  // Выключаем выходы
  off(_PULZ); off(_DIRZ); off(_PULX); off(_DIRX); off(_PULY); off(_DIRY); off(_PULA); off(_DIRA);
}

/**
  * @brief      Starts or Stops the Motor 
  * @param      num - номер мотора (0 ... 3)
  *             newstate: This parameter can be ENABLE (1) or DISABLE (0).
  * @retval     None
  */
void startMotor (u8 num, FunctionalState newstate)
{   
  if (newstate == ENABLE) {
    // Set the ENABLE X pin 
    //on(_ENAX);
  }
  else if (newstate == DISABLE) {   
    // Reset the ENABLE X pin 
    //off(_ENAX);
  }
}

/**
  * @brief      Selects the direction of the rotation of Main Motor
  * @param      num - номер мотора (0 ... 3)
  *             rotdir: Specifies the rotation direction
  *                 This parameter can be one of the following values:
  *             @arg Rotation_CW : sets clockwise direction
  *             @arg Rotation_CCW: sets counterclockwise direction
  * @retval :   None
  */
void setRotationDirectionMotor (u8 num, u8 rotdir)
{
  if (rotdir == ROTDIR_CW) {
    // Set the DIRECTION X pin 
    if (num == 0) on(_DIRX);
    if (num == 1) on(_DIRY);
    if (num == 2) on(_DIRZ);
    if (num == 3) on(_DIRA);
  }
  else if (rotdir == ROTDIR_CCW) {   
    // Reset the DIRECTION X pin 
    if (num == 0) off(_DIRX);
    if (num == 1) off(_DIRY);
    if (num == 2) off(_DIRZ);
    if (num == 3) off(_DIRA);
  }
}

/**
  * @brief  Rotation the MotorX with fixed parametres
  * @param  num - номер мотора (0 ... 3)
  *         steps: Specifies the number of steps to rotate
  *         dir: Specifies the rotation direction
  *         tclk: Specifies the time period of CLK signal (determines the speed of rotation)
  *         sts: Specifies the start speed of rotation
  *         acc: Specifies the acceleration (the acceleration rate)
  * @retval None
  * @note   1) Чем меньше значение tclk, тем больше скорость вращения
  *         2) Чем меньше значение starts, тем больше стартовая скорость мотора 
  *         3) Чем меньше значение acc, тем больше ускорение 
  */
void rotateSpecParam_Motor (u8 num, u32 steps, u8 dir, u32 tclk, u16 sts, u16 acc)
{
  u32 i;
  u8 Time;

  if (acc == 0) { acc = 1; }
  
  // Selection of Rotation direction 
  setRotationDirectionMotor(num, dir); 

  Time = sts;
  for (i = 0; i < steps; i++) {
    Motor[num].Timer = Time;
    if (num == 0) on(_PULX);
    else if (num == 1) on (_PULY);
    else if (num == 2) on (_PULZ);
    else if (num == 3) on (_PULA);
    
    while (Motor[num].Timer) _WDR();
    
    Motor[num].Timer = 3;   // 3 - Critical Value!

    if (num == 0) off(_PULX);
    if (num == 1) off(_PULY);
    if (num == 2) off(_PULZ);
    if (num == 3) off(_PULA);

    while (Motor[num].Timer) _WDR();
    
    EncZ.Cnt = ~TIM_GetCounter(TIM3) + 1;

    //if ((EncZ.Cnt == 0) && (dir == ROTDIR_CCW)) { break; }

    // разгон от [sts] до [tclk] с шагом [acc]
    if (!Motor[num].fl_Braking) { // если нет торможения 
      if ((Time > tclk) && ((i % acc) == 0)) { 
        Time--;
      }
    }
    // торможение (начинаем тормозить за 100 шагов до остановки)
    if (i >= (steps - 100)) {
      Motor[num].fl_Braking = 1;
      if ((Time < sts) && ((i % acc) == 0)) { Time += 2; }
    }
    TmpVar = i;
  }
  // Reset flag of braking
  Motor[num].fl_Braking = 0;
}

/**
  * @brief      Rotates the Stepper Motor
  * @param      num - номер Мотора (0...3)
  *             *steps - указатель на количество шагов
  *             *state - указатель на состояние Мотора
  * @retval     Код ошибки (см. ErrCodes_TypeDef)
  * @note       
  */
u8 rotateMotor_3 (u8 num, u16 *steps, TMotorRotateState *state)
{
  u8 ErrCode = NO_ERROR;
  const u8 Acc = 5;
  
  switch (*state)
  {
    case MRS_Idle:
      break;
    
    case MRS_Start:
      if (*steps > 0) {
        Motor[num].fl_TaskBusy = 1;
        *state = MRS_SetTimerON;
      }
      else {
        *state = MRS_Complete;
      }
      break;
      
    case MRS_SetTimerON:
      Motor[num].Timer = Motor[num].Start;    // Длительность импульса PUL(STEP) 
      switch (num) 
      {
        case 0: on(_PULX); break;
        case 1: on(_PULY); break;
        case 2: on(_PULZ); break;
        case 3: on(_PULA); break;
      }
      Motor[num].Counter++;
      if (*steps > 0) { (*steps)--; }
      
      // разгон от [Motor.Start] до [Motor.Imp] с шагом [Acc]
      if ((Motor[num].Start > Motor[num].Imp) && ((Motor[num].Counter % Acc) == 0)) { 
        Motor[num].Start--;
      }
      
      *state = MRS_PinON;
      break;

    case MRS_PinON:
      if (Motor[num].Timer > 0) {
        break;
      }
      *state = MRS_SetTimerOFF;
      break;
        
    case MRS_SetTimerOFF:
      Motor[num].Timer = 3;         // Задержка между импульсами PUL(STEP)
      switch (num) 
      {
        case 0: off(_PULX); break;
        case 1: off(_PULY); break;
        case 2: off(_PULZ); break;
        case 3: off(_PULA); break;
      }
      *state = MRS_PinOFF;
      break;
        
    case MRS_PinOFF:
      if (Motor[num].Timer > 0) {
        break;
      } 
      *state = MRS_CheckEncoder;
      break;
      
    case MRS_CheckEncoder:
      if (Motor[num].Dir == ROTDIR_CW) {
       switch (num) 
      {
        case 0: Enc[num].Cnt = TIM_GetCounter(TIM4) - Motor[num].PrimCoor + 1; break;
        case 1: Enc[num].Cnt = TIM_GetCounter(TIM2) - Motor[num].PrimCoor + 1; break;
        case 2: Enc[num].Cnt = TIM_GetCounter(TIM3) + 1; break;
        case 3: Enc[num].Cnt = TIM_GetCounter(TIM1) - Motor[num].PrimCoor + 1; break;
      }
      }
      else if (Motor[num].Dir == ROTDIR_CCW) {
              switch (num) 
      {
        case 0: Enc[num].Cnt = Motor[num].PrimCoor - TIM_GetCounter(TIM4) + 1; break;
        case 1: Enc[num].Cnt = Motor[num].PrimCoor - TIM_GetCounter(TIM2) + 1; break;
        case 2: Enc[num].Cnt = ~TIM_GetCounter(TIM3) + 1; break;
        case 3: Enc[num].Cnt = Motor[num].PrimCoor - TIM_GetCounter(TIM1) + 1; break;
      }      
      }
      if (Motor[num].Counter > Enc[num].Cnt + Motor[num].MaxError) {
        *state = MRS_Error;
        break;
      }
      *state = MRS_Start;
      break;
      
    case MRS_Error:
      ErrCode = ERR_ROTATION;
      break;
      
    case MRS_Complete:
      ErrCode = OK_COMPLETE;    //
      // Полный сброс
      Motor[num].Steps = 0;
      Motor[num].Counter = 0;
      Motor[num].fl_TaskBusy = 0;
            switch (num) 
      {
 //       case 0: TIM_SetCounter(TIM4, 0); break;         // Сбрасываем счетчик енкодера 
 //       case 1: TIM_SetCounter(TIM2, 0); break;         // Сбрасываем счетчик енкодера 
        case 2: TIM_SetCounter(TIM3, 0); break;       // Сбрасываем счетчик енкодера 
 //       case 3: TIM_SetCounter(TIM1, 0); break;         // Сбрасываем счетчик енкодера 
      } 
      break;
  }
  return ErrCode;
}

u8 handler_RotateMotor (u8 num)
{
  u8 ErrCode = NO_ERROR;
  
  setRotationDirectionMotor(num, Motor[num].Dir); 
  // rotateMotor_3(num, &Motor[num].Steps, &Motor[num].State);

  ErrCode = rotateMotor_3(num, &Motor[num].Steps, &Motor[num].State);
                          
  return ErrCode;
}
/**
  * @brief      Rotates Stepper Motor to poin axis 
  * @param      num - номер Мотора (0...3) "оси"
  * @retval     Код ошибки (см. ErrCodes_TypeDef) резерв 
  * @note       
  */
u8 go_to_poin (u8 num)
{
 u8 ErrCode = NO_ERROR;
 if (Motor[num].f1_init_axis == 1)
 {
   if (Motor[num].fl_TaskBusy == 1)
   {
     ErrCode = ERR_MOTOR_BUSY;
   }
   else
   {
    switch (num) // Записываем текущую координату  
      {
        case 0: Motor[num].PrimCoor = TIM_GetCounter(TIM4); break;       
        case 1: Motor[num].PrimCoor = TIM_GetCounter(TIM2); break;     
        case 2: break;
        case 3: Motor[num].PrimCoor = TIM_GetCounter(TIM1); break;              
      }
 //Определяем направление и кол-во шагов
        if (Motor[num].NextCoor!=Motor[num].PrimCoor)
        {
         if (Motor[num].NextCoor>Motor[num].PrimCoor)
         {
           Motor[num].Dir = 0;    // Направление вращения 
           Motor[num].Steps=Motor[num].NextCoor-Motor[num].PrimCoor; // Вычисляем кол-во шагов
         }
         else 
         {
           Motor[num].Dir = 1;    // Направление вращения 
           Motor[num].Steps=Motor[num].PrimCoor-Motor[num].NextCoor; // Вычисляем кол-во шагов
         }
         Motor[num].State = MRS_Start; // Запускаем мотор 
        }
   }
 }
 else ErrCode = ERR_NOT_INIT_AXIS;       //Перемещение по оси, когда ось не инициализирована  
         return ErrCode;
}
/**
  * @brief      Init axis 
  * @param      num - номер Мотора (0...3) "оси"
                point - отъехать на точку
  * @retval     Код ошибки (см. ErrCodes_TypeDef) резерв 
  * @note       
  */
u8 init_axis (u8 num, u16 point)
{
   switch (State_Init_Axis)
  {
  case start_init:
    if (fl_Home)
    {
    State_Init_Axis = Error_init_axis;
    Code_Error_init_axis = ERR_HOME_ON_AT_INIT;        //Датчик Home сработал
    }
    else
    {
       switch (num) 
      {
        case 0: TIM_SetCounter(TIM4, 65000); break;
        case 1: TIM_SetCounter(TIM2, 65000); break;
//        case 2: TIM_SetCounter(TIM3, 65000); break; 
      case 3: TIM_SetCounter(TIM1, 65000); break;
      } 
     Motor[num].MaxError = 10;        //Макс кол-во ошибок
     Motor[num].Start = 10;          // Начальная скорость    
     Motor[num].Imp = 30;               // Скорость вращения
     Motor[num].Dir = ROTDIR_CCW;      //Едем к Home
     Motor[num].Steps = 60000;          // Кол- во шагов к Home
     Motor[num].State = MRS_Start; // Запускаем мотор
     State_Init_Axis = Wait_Home;       //Переходим к ожиданию страбатывания Home
    }
    break;
  case Wait_Home:
    if(fl_Home)         //Сработал Home
    {
    Timer_Init_axis = 1; //Ставим таймер на 2 сек перед реверсом
    State_Init_Axis = Outgo_Home;
    }
    break;
  case Outgo_Home:   // Отъезжаем от Home
   if (Timer_Init_axis);
   else 
   {
            switch (num) 
      {
        case 0: TIM_SetCounter(TIM4, 0); break;
        case 1: TIM_SetCounter(TIM2, 0); break;
//        case 2: TIM_SetCounter(TIM3, 0); break; 
      case 3: TIM_SetCounter(TIM1, 0); break;
      }  
     Motor[num].Dir = ROTDIR_CW;      //Отъезжаем от Home
     Motor[num].Steps = 6000;          // Кол- во шагов к Home 
     Motor[num].Start = 10;          // Начальная скорость    
     Motor[num].Imp = 29;               // Скорость вращения медленная
     Motor[num].State = MRS_Start; // Запускаем мотор  
     State_Init_Axis = Wait_Reset_Home;       //Переходим к ожиданию отпусканию Home  
   } 
   break;
  case Wait_Reset_Home: //Ожидаем отпускания Home
    if(fl_Home);         
    else                        // Сброс Home
      {
      switch (num) 
      {
        case 0: TIM_SetCounter(TIM4, 0); break;
        case 1: TIM_SetCounter(TIM2, 0); break;
//        case 2: TIM_SetCounter(TIM3, 0); break; 
      case 3: TIM_SetCounter(TIM1, 0); break;
      }
     Motor[num].State = MRS_Complete;
     Motor[num].f1_init_axis = 1;   //Инициализацию оси провели
     State_Init_Axis = Outgo_fix_Home;
    }
     break;   
  case Outgo_fix_Home:        //Отезжаю на фикс кол- во шагов
     Motor[num].Start = 10;          // Начальная скорость    
     Motor[num].Imp = 29;               // Скорость вращения медленная
     Motor[num].NextCoor = point;     //Отъезжаем на point точку 
     go_to_poin (num); 
     State_Init_Axis = Complete_Outgo_fix_Home;
     break;
  case Complete_Outgo_fix_Home: //ожидаем завершения отъезда
     if(Motor[num].State == MRS_Complete)State_Init_Axis = init_Complete;
      break;
    case Error_init_axis:
      break;
    case init_Complete:
     break; 
   default:  
   break;  
  }
  return Code_Error_init_axis;
}


