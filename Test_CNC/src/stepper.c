
 
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
  //------------����������� ���� C---------------------------------
  GPIO_InitTypeDef gpio;
  /* Enable the GPIOs Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE); //��������� ���� B � � 
  /* Configure the output pins */
  gpio.GPIO_Pin = PUL_Z | DIR_Y;

  // Init I/O port 
  gpio.GPIO_Mode = GPIO_Mode_Out_OD;    // �������� ���������
  gpio.GPIO_Speed = GPIO_Speed_50MHz;

  // �������������� GPIO �� �����, � �������� ���������� �������� �� 
  GPIO_Init(GPIOC, &gpio);
 //------------����������� ���� B--------------------------------- 
   GPIO_InitTypeDef gpio2;

  /* Configure the output pins */
  gpio2.GPIO_Pin = DIR_Z | PUL_X | DIR_X | PUL_Y | PUL_A | DIR_A;

  // Init I/O port 
  gpio2.GPIO_Mode = GPIO_Mode_Out_OD;    // �������� ���������
  gpio2.GPIO_Speed = GPIO_Speed_50MHz;

  // �������������� GPIO �� �����, � �������� ���������� �������� �� 
  GPIO_Init(GPIOB, &gpio2);
  
   
  // ���������� ������ � 1, ����� ������ ���������� � 3-� ���������
  //GPIO_SetBits(SMOTORS_GPIO_PORT, gpio.GPIO_Pin);
  
  // ��������� ������
  off(_PULZ); off(_DIRZ); off(_PULX); off(_DIRX); off(_PULY); off(_DIRY); off(_PULA); off(_DIRA);
}

/**
  * @brief      Starts or Stops the Motor 
  * @param      num - ����� ������ (0 ... 3)
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
  * @param      num - ����� ������ (0 ... 3)
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
  * @param  num - ����� ������ (0 ... 3)
  *         steps: Specifies the number of steps to rotate
  *         dir: Specifies the rotation direction
  *         tclk: Specifies the time period of CLK signal (determines the speed of rotation)
  *         sts: Specifies the start speed of rotation
  *         acc: Specifies the acceleration (the acceleration rate)
  * @retval None
  * @note   1) ��� ������ �������� tclk, ��� ������ �������� ��������
  *         2) ��� ������ �������� starts, ��� ������ ��������� �������� ������ 
  *         3) ��� ������ �������� acc, ��� ������ ��������� 
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

    // ������ �� [sts] �� [tclk] � ����� [acc]
    if (!Motor[num].fl_Braking) { // ���� ��� ���������� 
      if ((Time > tclk) && ((i % acc) == 0)) { 
        Time--;
      }
    }
    // ���������� (�������� ��������� �� 100 ����� �� ���������)
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
  * @param      num - ����� ������ (0...3)
  *             *steps - ��������� �� ���������� �����
  *             *state - ��������� �� ��������� ������
  * @retval     ��� ������ (��. ErrCodes_TypeDef)
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
      Motor[num].Timer = Motor[num].Start;    // ������������ �������� PUL(STEP) 
      switch (num) 
      {
        case 0: on(_PULX); break;
        case 1: on(_PULY); break;
        case 2: on(_PULZ); break;
        case 3: on(_PULA); break;
      }
      Motor[num].Counter++;
      if (*steps > 0) { (*steps)--; }
      
      // ������ �� [Motor.Start] �� [Motor.Imp] � ����� [Acc]
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
      Motor[num].Timer = 3;         // �������� ����� ���������� PUL(STEP)
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
      // ������ �����
      Motor[num].Steps = 0;
      Motor[num].Counter = 0;
      Motor[num].fl_TaskBusy = 0;
            switch (num) 
      {
 //       case 0: TIM_SetCounter(TIM4, 0); break;         // ���������� ������� �������� 
 //       case 1: TIM_SetCounter(TIM2, 0); break;         // ���������� ������� �������� 
        case 2: TIM_SetCounter(TIM3, 0); break;       // ���������� ������� �������� 
 //       case 3: TIM_SetCounter(TIM1, 0); break;         // ���������� ������� �������� 
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
  * @param      num - ����� ������ (0...3) "���"
  * @retval     ��� ������ (��. ErrCodes_TypeDef) ������ 
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
    switch (num) // ���������� ������� ����������  
      {
        case 0: Motor[num].PrimCoor = TIM_GetCounter(TIM4); break;       
        case 1: Motor[num].PrimCoor = TIM_GetCounter(TIM2); break;     
        case 2: break;
        case 3: Motor[num].PrimCoor = TIM_GetCounter(TIM1); break;              
      }
 //���������� ����������� � ���-�� �����
        if (Motor[num].NextCoor!=Motor[num].PrimCoor)
        {
         if (Motor[num].NextCoor>Motor[num].PrimCoor)
         {
           Motor[num].Dir = 0;    // ����������� �������� 
           Motor[num].Steps=Motor[num].NextCoor-Motor[num].PrimCoor; // ��������� ���-�� �����
         }
         else 
         {
           Motor[num].Dir = 1;    // ����������� �������� 
           Motor[num].Steps=Motor[num].PrimCoor-Motor[num].NextCoor; // ��������� ���-�� �����
         }
         Motor[num].State = MRS_Start; // ��������� ����� 
        }
   }
 }
 else ErrCode = ERR_NOT_INIT_AXIS;       //����������� �� ���, ����� ��� �� ����������������  
         return ErrCode;
}
/**
  * @brief      Init axis 
  * @param      num - ����� ������ (0...3) "���"
                point - �������� �� �����
  * @retval     ��� ������ (��. ErrCodes_TypeDef) ������ 
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
    Code_Error_init_axis = ERR_HOME_ON_AT_INIT;        //������ Home ��������
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
     Motor[num].MaxError = 10;        //���� ���-�� ������
     Motor[num].Start = 10;          // ��������� ��������    
     Motor[num].Imp = 30;               // �������� ��������
     Motor[num].Dir = ROTDIR_CCW;      //���� � Home
     Motor[num].Steps = 60000;          // ���- �� ����� � Home
     Motor[num].State = MRS_Start; // ��������� �����
     State_Init_Axis = Wait_Home;       //��������� � �������� ������������� Home
    }
    break;
  case Wait_Home:
    if(fl_Home)         //�������� Home
    {
    Timer_Init_axis = 1; //������ ������ �� 2 ��� ����� ��������
    State_Init_Axis = Outgo_Home;
    }
    break;
  case Outgo_Home:   // ��������� �� Home
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
     Motor[num].Dir = ROTDIR_CW;      //��������� �� Home
     Motor[num].Steps = 6000;          // ���- �� ����� � Home 
     Motor[num].Start = 10;          // ��������� ��������    
     Motor[num].Imp = 29;               // �������� �������� ���������
     Motor[num].State = MRS_Start; // ��������� �����  
     State_Init_Axis = Wait_Reset_Home;       //��������� � �������� ���������� Home  
   } 
   break;
  case Wait_Reset_Home: //������� ���������� Home
    if(fl_Home);         
    else                        // ����� Home
      {
      switch (num) 
      {
        case 0: TIM_SetCounter(TIM4, 0); break;
        case 1: TIM_SetCounter(TIM2, 0); break;
//        case 2: TIM_SetCounter(TIM3, 0); break; 
      case 3: TIM_SetCounter(TIM1, 0); break;
      }
     Motor[num].State = MRS_Complete;
     Motor[num].f1_init_axis = 1;   //������������� ��� �������
     State_Init_Axis = Outgo_fix_Home;
    }
     break;   
  case Outgo_fix_Home:        //������� �� ���� ���- �� �����
     Motor[num].Start = 10;          // ��������� ��������    
     Motor[num].Imp = 29;               // �������� �������� ���������
     Motor[num].NextCoor = point;     //��������� �� point ����� 
     go_to_poin (num); 
     State_Init_Axis = Complete_Outgo_fix_Home;
     break;
  case Complete_Outgo_fix_Home: //������� ���������� �������
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


