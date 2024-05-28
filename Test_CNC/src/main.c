

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "stm32F10x.h"
#include "STM32vldiscovery.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  LSE_FAIL_FLAG  0x80
#define  LSE_PASS_FLAG  0x100

#define WORK_DIR      ROTDIR_CCW       // ����������� �������� "������ �������"
#define RETURN_DIR    ROTDIR_CW        // ����������� �������� "������� �������"

#define START_ENABLE    !GPIO_ReadOutputDataBit(OUT_GPIO_PORT, LEDSTART_DOUT)
#define WORK_ENABLE     !GPIO_ReadOutputDataBit(OUT_GPIO_PORT, WORK_DOUT)
#define ERROR_ENABLE    !GPIO_ReadOutputDataBit(OUT_GPIO_PORT, ERROR_DOUT)  
    
//��������� �������� ������ ������
   
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
  
  // �������������� ������ 6
  init_TimerTIM6(1, 100);    // 100 us
    
  // �������������� ������ 7                 
 // init_TimerTIM7(10, 10);    // 100 us  
  
  // ������������� ������� 3 ��� �������� Z
  init_TimerTIM3();
  
  // ������������� ������� 4 ��� �������� X
  init_TimerTIM4();  
  
    // ������������� ������� 2 ��� �������� Y
  init_TimerTIM2();
 
    // ������������� ������� 1 ��� �������� A
  init_TimerTIM1(); 
  
  
  // �������� ����� ��� ���������  
  init_Encoders();  
  
  // ������������� ������ ��� ��������� ��
  init_OutputsDriversSM();
   // �������������� UART�
 // init_UART0();       //������������ � ��������� ��� � pin PA8 "AA"
  init_UART1();
  
  // �������������� ����� (������, �������)
  init_Inputs();
  
  // ������������� �������
  init_Outputs();
  
  // �������� ����� �������
  Flags.Flw = 0;
  
  // ��������� �������
  for (int i = 0; i < 3; i++) { Motor[i].State = MRS_Idle;}
  MotorZ.Dir = WORK_DIR;
  MotorZ.MaxError = 100;
  
// ���������� �������� ���������
          TIM_SetCounter(TIM4, 0);        // ���������� ������� �������� 
        TIM_SetCounter(TIM2, 0);          // ���������� ������� �������� 
     TIM_SetCounter(TIM3, 0);       // ���������� ������� �������� 
 TIM_SetCounter(TIM1, 0);          // ���������� ������� �������� 
  
  // �������� WatchDog
//  setup_IWDG();
  /* main while */
  while(1)
  {

    _WDR();     // reset Watchdog
    
    cpl();      // ��������� �������� �����
    // ��������� ����������
    Con_air_cyl_Man();
    Con_air_cyl_Zah();
    Con_air_cyl_Fin();
    
    
    // ��������� ������
 //   handler_Buttons();
    
    // ��������� �������
//    handler_Sensors();
    
    // ������ �����
    if (handler_RotateMotor(_Z_) == ERR_ROTATION) {
      on(_ERROR);
      on(_BUZZER);
//      MotorZ.State = MRS_Complete;     // ���������������  
      ErrCodeAll = ERR_ROTATION_Z;
    }
     if (handler_RotateMotor(_X_) == ERR_ROTATION) {
      on(_ERROR);
      on(_BUZZER);
      MotorX.State = MRS_Complete;     // ���������������  
      ErrCodeAll = ERR_ROTATION_X;
    }   
     if (handler_RotateMotor(_Y_) == ERR_ROTATION) {
      on(_ERROR);
      on(_BUZZER);
      MotorY.State = MRS_Complete;     // ��������������� 
     ErrCodeAll = ERR_ROTATION_Y; 
    } 
     if (handler_RotateMotor(_A_) == ERR_ROTATION) {
      on(_ERROR);
      on(_BUZZER);
      MotorA.State = MRS_Complete;     // ��������������� 
      ErrCodeAll = ERR_ROTATION_A; 
    }   
    
    //-----------------------------������������� ����-----------------
    switch (state_init)
    {
    case init_A:
    ErrCode = init_axis(_A_, 100); // ������������� ��� �
    if (State_Init_Axis == init_Complete) {State_Init_Axis = start_init; state_init = init_Y;} //��������� � ����. ���� ���
    break;
//    case init_X:
//    ErrCode = init_axis(_X_, 100); // ������������� ��� X
//    if (State_Init_Axis == init_Complete) {State_Init_Axis = start_init; state_init = init_Y;} //��������� � ����. ���� ���
//    break;    
    case init_Y:  
    ErrCode = init_axis(_Y_, 200); // ������������� ��� Y
    break;
    }

 
    //---------------------���� ���� Y � A----------------------------------
    if((MessegSM==ReInitSM))
    { 
      /*
      MessegSM=NewTaskSM;        
         MotorA.MaxError = 10;        //���� ���-�� ������
         MotorA.Start = 10;          // ��������� ��������    
         MotorA.Imp = 10;               // �������� �������� 
         MotorA.NextCoor = 3000;
         go_to_poin (_A_);
      */
      
//         coordA = TIM_GetCounter(TIM1);
//         go_to_poin (_Y_, 1000, 15);
         //Y
         /*
         MotorY.MaxError = 10;        //���� ���-�� ������
         MotorY.Steps = 3000;     // 
         MotorY.Start = 10;          // ��������� ��������
         MotorY.Dir = forwardSM;    // ����������� ��������        
         MotorY.Imp = 3;                // �������� �������� 
         MotorY.State = MRS_Start; // ��������� ����� 
        MessegSM=NewTaskSM; 
         */
    }
    /*------------------������� ������ ������-----------------------------*/
  switch (StateSM){
    case InitSM:      //--------------------------------��������� ��������� ��� ��������� ��� ��������� ������ �������---
      if (MessegSM==NewTaskSM)   // �������� ����� �������
      {
         MessegSM=NoNewTaskSM;      //
        if (fl_SEN_SM3)             //��������� ������ �� ������ ������ ������������ - ������ �� �������
        { 
          CodeErrorSM=bahd_exit; 
          StateSM=ErrorSM;
         break;
        } 
        if (fl_SEN_SM2) //������ ��� ������� ������ ������ ��������
        {
         MotorZ.MaxError = 10;        //���� ���-�� ������
         MotorZ.Steps = LengthPol*400*100/314/25;     // ������ 200 ����� 0.5 ������� 180�� = 200*1,8/2
         MotorZ.Start = 10;          // ��������� ��������
         MotorZ.Dir = forwardSM;    // ����������� ��������        
         MotorZ.Imp = speedSM;                // �������� �������� 
         MotorZ.State = MRS_Start; // ��������� ����� 
         StateSM=Main_pull;      //���� ������ �������� ����� ����� �������� �� ��������� � ��������� ���������
          break;
        }
        if (fl_SEN_SM1)
        {
         MotorZ.MaxError = 2400;        //���� ���-�� ������, ����� ����� �������� ����������?
         MotorZ.Steps = 2500;          //�� ������� ��������� ��������, ����� ����� �������� ����������?
         MotorZ.Start = 10;          // ��������� ��������
         MotorZ.Dir = forwardSM;    // ����������� ��������         
         MotorZ.Imp = speedSM;                // �������� ��������     
         MotorZ.State = MRS_Start; // ��������� �����
         Timer_Sup_roller = 15;    //������������� ������ �� ����������� ��� �����
         StateSM=Sup_roller;      //���� ���� �� ����� �� ��������� � �������� ��� ����� ��������
         break;
        }
        else  {StateSM=Wait_strip; Timer_inputSM = 30; break;}          //������ ��� �� ����� ��������� � �������� ������ ������ ��������
      }            
      break;
    case Wait_strip:            //----------------------------------�������� ������-------------------------
      if (fl_SEN_SM1)
      {
         MotorZ.MaxError = 2400;        //���� ���-�� ������, ����� ����� �������� ����������?
         MotorZ.Steps = 2500;          //�� ������� ��������� ��������, ����� ����� �������� ����������?
         MotorZ.Start = 10;          // ��������� ��������
         MotorZ.Dir = forwardSM;    // ����������� ��������         
         MotorZ.Imp = speedSM;                // �������� ��������     
         MotorZ.State = MRS_Start; // ��������� �����
         Timer_Sup_roller = 15;    //������������� ������ �� ����������� ��� �����
         StateSM=Sup_roller;   //������ ��������� ��������� � ����������� ��� �����
         break;
      }
      else         
      {
        if(Timer_inputSM);
        else {StateSM=ErrorSM; CodeErrorSM=NoStrip;break; } //������ ����� � ������ ���              
      }
      break;
    case Sup_roller:    //-------------����������� ������ ��� ������ �����-----------------
        if (fl_SEN_SM2) //�������� ��� ����� ������������� �������� ��������
        {
         MotorZ.MaxError = 10;        //���� ���-�� ������
         MotorZ.Steps = LengthPol*400*100/314/25;     // ������ 200 ����� 0.5 ������� 180�� = 200*1,8/2
         MotorZ.Start = 10;          // ��������� ��������
         MotorZ.Dir = forwardSM;    // ����������� ��������        
         MotorZ.Imp = speedSM;                // �������� �������� 
 //        MotorZ.State = MRS_Start; // ��������� ����� 
         StateSM = Main_pull;   //�������� � �������� ������ ��������
        }
        else 
        {
          if (handler_RotateMotor(_Z_) == ERR_ROTATION) //���� ���������� ��� ����� � �� �������� ������ �� ���-�� ������
          {
           fl_ERR_ROT_Z = 0;
           CodeErrorSM=not_tightened_strip; 
           StateSM=ErrorSM; 
           break;
          }
          if(Timer_Sup_roller);
          else {StateSM=ErrorSM; CodeErrorSM=not_tightened_strip_timer; break; }//���� ���������� ��� ����� � �� �������� ������ �� �������
        }
      break;
    case Main_pull:     //----------------------------�������� ��������------------------------------------
      if (handler_RotateMotor(_Z_) == ERR_ROTATION)
      {
        CodeErrorSM=ErrRotSM;
        StateSM=ErrorSM;  //���� ����������� �� �������� � �� ���������
      }
      if (handler_RotateMotor(_Z_)==OK_COMPLETE){StateSM = CompleteSM;break;}
      break; 
    case ErrorSM:       //---------------��������� ������-----------------------------------------
      if (MessegSM==ReInitSM){ StateSM = InitSM; MotorZ.State = MRS_Complete;}   // ����������������� ������ !!!!!!�������� ������ ��������!!!!!!!!
      break;
    case CompleteSM:    //-------------------������� ���������--------------------------------------------
       if (MessegSM==NewTaskSM)StateSM = InitSM;   //�������� ����� �������
      break;
    default:  
    break;  
    }
  
/*----------------------������� ������ ������� CN------------------------------*/
switch (StateCM){
      case InitCM:       //�������� ��������� � ������������ �������
        if (fl_SEN_CM)   //������ �������� - �� � �������� ���������
        {
         CodeErrorCM=not_initCM;        //������ �� � �������� ��������� 
         StateCM= ErrorCM;              // � ��������� ������
         break;
        }
        else
        {
         if (MessegCM==NewTaskCM)
         {
         MessegCM = NoNewTaskCM;  
         SEN_CM_old_state=0;    //��������� ������� ��������� �������
         on(_RELAY_CM);         //�������� ���������
         Timer_RunCM=10;        //������������� ������ ���� ������� �������
         StateCM=CuttingSM;         //��������� � ��������� "�������"
         }
        }
        break;
      case CuttingSM:       //������� �����������
        if(Timer_RunCM)
          {
          if ((fl_SEN_CM == 1) && (SEN_CM_old_state == 0))SEN_CM_old_state = 1;       //������� ������� �������
          else if ((fl_SEN_CM == 0) && (SEN_CM_old_state == 1)) 
          { 
            off(_RELAY_CM);         //��������� ���������
            StateCM=CompleteCM;        // ������� ������� ��������   
          }
          break;
          }       
        else
          {
          off(_RELAY_CM);         //��������� ���������
          CodeErrorCM=Exceeded_TimeCM;          //����� �� ������� �����! 
          StateCM=ErrorCM;                              
          }
      break;  
    case CompleteCM:       //������� ���������
      if (MessegCM==NewTaskCM)StateCM = InitCM;   //�������� ����� �������
      break;  
    case ErrorCM:       //��������� ������
      if (MessegCM==ReInitCM)StateCM=InitCM;   // ����������������� ������ 
      if (MessegCM==ResetCM)    //���������� � ��������
      {
         SEN_CM_old_state =0;
         on(_RELAY_CM);         //�������� ���������
         Timer_RunCM=10;        //������������� ������ ���� ������� �������
         StateCM=CuttingSM;         //��������� � ��������� "�������"
      }
      break;
    default:  
    break;    
  }

   /*���� UART DEBUG*/
//    if(readByte_UART1()== 101)STM32vldiscovery_LEDOn(LED3);
     /*���� UART END DEBUG*/
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
          /* 400 = 1 ������; �� ���. �������, 5 = �������� ��������
             20 = ���. ��������, 10 - ���� ������� */
           
 
          //��� �������
  //         sendByte_UART1(102);
          /**  
  * @brief      ������������ ����� ������ � �������� ��� ����� UART
  * @param      uart_number - ����� ������������� UART�: 0 = UART0 ��� 1 = UART1
  *             adr_dst - ����� ����������            
  *             cmd - ��� �������
  *             *ptr - ��������� �� ������
  *             len - ������ ������
  * @retval     None
  */
   //       sendPacket_UARTx(1,0,102, 0, 0);  
          
/*                        // ������ ��������� �������� Z
         Motor[2].State = MRS_Idle;
         MotorZ.MaxError = 10;        //���� ���-�� ������ 
         MotorZ.Steps = 1000;     // ������ 200 ����� 0.5 ������� 180�� = 200*1,8/2
         MotorZ.Start = 10;          // ��������� ��������
         MotorZ.Dir = WORK_DIR;    // ����������� ��������
         MotorZ.Imp = 1;             // �������� ��������                   
         MotorZ.State = MRS_Start;// ��������� �����
          
          
                    // ������ ��������� �������� X
         MotorX.MaxError = 10;        //���� ���-�� ������ 
         MotorX.Steps = 1000;     // ������ 200 ����� 0.5 ������� 180�� = 200*1,8/2
         MotorX.Start = 10;          // ��������� ��������
         MotorX.Dir = WORK_DIR;    // ����������� ��������
         MotorX.Imp = 10;             // �������� ��������
         MotorX.State = MRS_Start;// ��������� �����
  */        
          //��� ������� the end 
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
 //       MotorZ.State = MRS_Complete;     // ���������������  
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
  * @brief      ������� ���������� ��������� ��������� ���������� ������������  
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
  case Cyl_Behind_Man:  //������� ��������
    if (fl_Man_Behind == 0)ErrCodeAll = ERR_BEHIND_MAN_SEN;       //������ ������ �� �������, ����� �� ��������
    if(fl_Air_Cyl_Man)         
    {
      Timer_Man_Ahead = 5;      //����� �� ���������� ��������
      state_Man = Cyl_Comes_Out_Man;
      on(_RELAY_MAN);   //��������� ������������
    }
    break;
  case Cyl_Ahead_Man:   //������� ��������
     if (fl_Man_Ahead == 0)ErrCodeAll = ERR_AHEAD_MAN_SEN;       //������ ������ �� �������, ����� �� �������� 
     if(fl_Air_Cyl_Man == 0)
    {
      Timer_Man_Behind = 5;      //����� �� ���������� ��������
      state_Man = Cyl_Comes_Man;
      off(_RELAY_MAN);   //��������� ������������
    }
    break;
  case Cyl_Comes_Out_Man: //������� �������
    if (fl_Man_Ahead)state_Man = Cyl_Ahead_Man;
    else {if(Timer_Man_Ahead == 0) ErrCodeAll = ERR_AHEAD_MAN;}
    break;
  case Cyl_Comes_Man:     //������� �������
    if (fl_Man_Behind)state_Man = Cyl_Behind_Man;
    else {if(Timer_Man_Behind == 0) ErrCodeAll = ERR_BEHIND_MAN;}
    break;    
   }
}
/** 
  * @brief      ������� ���������� ��������� ��������� ������ ������������
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
  case Close_Zah:  //������� ��������
    if (fl_Zah_Close == 0)ErrCodeAll = ERR_CLOSE_ZAH_SEN;       //������ ������ �� �������, ����� �� ��������
    if(fl_Air_Cyl_Zah)         
    {
      Timer_Zah_Open = 5;      //����� �� ���������� ��������
      state_Zah = Cyl_Comes_Out_Zah;
      on(_RELAY_ZAH);   //��������� ������������
    }
    break;
  case Open_Zah:   //������� ��������
     if (fl_Zah_Open == 0)ErrCodeAll = ERR_OPEN_ZAH_SEN;       //������ ������ �� �������, ����� �� �������� 
     if(fl_Air_Cyl_Zah == 0)
    {
      Timer_Zah_Close = 5;      //����� �� ���������� ��������
      state_Zah = Cyl_Comes_Zah;
      off(_RELAY_ZAH);   //��������� ������������
    }
    break;
  case Cyl_Comes_Out_Zah: //������� �������
    if (fl_Zah_Open)state_Zah = Open_Zah;
    else {if(Timer_Zah_Open == 0) ErrCodeAll = ERR_OPEN_ZAH;}
    break;
  case Cyl_Comes_Zah:     //������� �������
    if (fl_Zah_Close)state_Zah = Close_Zah; 
    else {if(Timer_Zah_Close == 0) ErrCodeAll = ERR_CLOSE_ZAH;}
    break;    
   }
}
/** 
  * @brief      ������� ���������� ��������� ��������� ���������� ������  
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
  case Cyl_Behind_Fin:  //������� ��������
    if (fl_Fin_Behind == 0)ErrCodeAll = ERR_BEHIND_FIN_SEN;       //������ ������ �� �������, ����� �� ��������
    if(fl_Air_Cyl_Fin)         
    {
      Timer_Fin_Ahead = 5;      //����� �� ���������� ��������
      state_Fin = Cyl_Comes_Out_Fin;
      on(_RELAY_FIN);   //��������� ������������
    }
    break;
  case Cyl_Ahead_Fin:   //������� ��������
     if (fl_Fin_Ahead == 0)ErrCodeAll = ERR_AHEAD_FIN_SEN;       //������ ������ �� �������, ����� �� �������� 
     if(fl_Air_Cyl_Fin == 0)
    {
      Timer_Fin_Behind = 5;      //����� �� ���������� ��������
      state_Fin = Cyl_Comes_Fin;
      off(_RELAY_FIN);   //��������� ������������
    }
    break;
  case Cyl_Comes_Out_Fin: //������� �������
    if (fl_Fin_Ahead)state_Fin = Cyl_Ahead_Fin;
    else {if(Timer_Fin_Ahead == 0) ErrCodeAll = ERR_AHEAD_FIN;}
    break;
  case Cyl_Comes_Fin:     //������� �������
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

  //---------------- ��������� �������� 1ms -----------------
  if (TimerCPL != 0x00) TimerCPL--;
  if (TimerLED != 0x00) TimerLED--;
  if (TimeForDelay != 0x00) TimeForDelay--;

  /*------------------- 1-��������� ��������� -----------------*/
  if (SecDel != 0x00) SecDel--;
  else {
	SecDel = 1000;
    if (STimer1 != 0x00) STimer1--;
    if (STimer2 != 0x00) STimer2--;
    if (Timer_inputSM != 0x00) Timer_inputSM--;         //������ ������ ������������ �������� ������
    if (Timer_RunCM != 0x00) Timer_RunCM--;             //������ ������ ������� ������������ ����� ������ ��������� �������
    if (Timer_Sup_roller != 0x00) Timer_Sup_roller--;        //������ �������� �� ����������� ��� �����
    if (Timer_Init_axis != 0x00) Timer_Init_axis--;           //������ ����� ��������
    if (Timer_Man_Ahead != 0x00) Timer_Man_Ahead--;           //������ ���������� ������������
    if (Timer_Man_Behind != 0x00) Timer_Man_Behind--;           //������ ���������� ������������
    if (Timer_Zah_Open != 0x00) Timer_Zah_Open--;           //������ �������� ������� ������������
    if (Timer_Zah_Close != 0x00) Timer_Zah_Close--;           //������ �������� ������� ������������
    if (Timer_Fin_Ahead != 0x00) Timer_Fin_Ahead--;           //������ �������� ������� �����������
    if (Timer_Fin_Behind != 0x00) Timer_Fin_Behind--;           //������ �������� ������� �����������
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
  * @brief      ��������� ������ � ���������
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


