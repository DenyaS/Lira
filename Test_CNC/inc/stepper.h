

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STEPPER_H
#define __STEPPER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
// Определение выходов MCU
#define _PULX   GPIOB, PUL_X, LOW   // Акт. уровень сигнала - LOW (0)
#define _DIRX   GPIOB, DIR_X, HIGH
#define _PULY   GPIOB, PUL_Y, LOW   
#define _DIRY   GPIOC, DIR_Y, HIGH
//#define _PULZ   SMOTORS_GPIO_PORT, PUL_Z, LOW   
//#define _DIRZ   SMOTORS_GPIO_PORT, DIR_Z, HIGH
#define _PULZ   GPIOC, PUL_Z, LOW   
#define _DIRZ   GPIOB, DIR_Z, HIGH
#define _PULA   GPIOB, PUL_A, LOW   
#define _DIRA   GPIOB, DIR_A, HIGH

#define MXMINSPEED      5      // (min 10) чем больше значение, тем меньше скорость вращения ШД 
#define MXMAXSPEED      30      // (max 30) по сути - это минимальная скорость вращения ШД
#define MXSTARTSPEED    30      // Start speed of MotorX (inverse value)
#define MXACCELERATION  5       // Acceleration of MotorX (inverse value) - чем больше значение, тем медленнее разгоняется

#define MotorX          Motor[0]
#define MotorY          Motor[1]
#define MotorZ          Motor[2]
#define MotorA          Motor[3]

/* Type definitions ----------------------------------------------------------*/
enum { ROTDIR_CW = 0, ROTDIR_CCW = !ROTDIR_CW };

typedef enum { 
  Rotation_CW = 0, 
  Rotation_CCW  
} RotationDirection;

typedef enum 
{
  MRS_Idle,
  MRS_Start,
  MRS_CheckSteps,
  MRS_SetTimerON,
  MRS_PinON,
  MRS_SetTimerOFF,
  MRS_PinOFF,
  MRS_CheckEncoder,
  MRS_Error,
  MRS_Complete
} TMotorRotateState;

typedef struct _stepmotor 
{
  union
  {
    u8 Signals;
    struct {
      u8 ReserveS:8;
    };
  };
  union
  {
    u8 Flags;
    struct {
      u8 fl_NegativeFeedback:1;
      u8 fl_TaskBusy:1;
      u8 fl_Braking:1;
      u8 f1_init_axis:5;
    };
  };
  u32                       Timer;
  u16                       Counter;
  u16                       Steps;
  u8                        Dir;
  u32                       Imp;
  u32                       Start;
  u16                       MaxError;
  u16                       Ratio;
  u8                        Task;
  u32                       Tmin;
  TMotorRotateState         State;
  RotationDirection         RotDir;
  u16                       PrimCoor;
  u16                       NextCoor;
} StepMotor_TypeDef;

enum 
{
  start_init,
  Wait_Home,
  Outgo_Home,
  Wait_Reset_Home,
  Error_init_axis,
  Complete_Outgo_fix_Home,
  init_Complete,
  Outgo_fix_Home,
}; 
/* Exported functions ------------------------------------------------------- */
void init_OutputsDriversSM(void);
void startMotor_X(u8 num, FunctionalState newstate);
void setRotationDirectionMotor(u8 num, u8 rotdir);
void rotateSpecParam_Motor(u8 num, u32 steps, u8 dir, u32 tclk, u16 sts, u16 acc);
u8 rotateMotor_3(u8 num, u16 *steps, TMotorRotateState *state);
u8 handler_RotateMotor(u8 num);
u8 go_to_poin (u8 num); //Занять координату 
u8 init_axis (u8 num, u16 point);  //Инициализировать ось

#endif /* __ENCODER_H */


