
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXTERNS_H
#define __EXTERNS_H

#include "flags.h"
#include "functions.h"
#include "stepper.h"
#include "uart.h"

extern volatile uint32_t TickCnt;
extern volatile uint32_t TimerCPL;
extern volatile uint32_t TimerLED;
extern volatile uint32_t TimeForDelay;
extern volatile uint32_t STimer1;
extern volatile uint32_t STimer2;
extern volatile _FLAGS Flags;         
extern UART_TypeDef UARTx[2];  

extern StepMotor_TypeDef Motor[4];
extern Encoder_TypeDef Enc[4];
//���������� ������ ������ ����
extern volatile uint8_t ErrCodeAll;            //��� ������ ������
extern volatile uint8_t StateSM;                  //��������� ������ ������
extern volatile uint8_t MessegSM;               //��������� ������ ������
extern volatile uint32_t LengthPol;             //������ ������ �� ������� ���� ��������
extern volatile uint8_t speedSM;                 //�������� �������
extern volatile uint8_t CodeErrorSM;            //��������� ������ ������ ������
extern volatile uint32_t Timer_inputSM;           //������ �������� ������ �� ����� � ���
extern volatile uint32_t Timer_Sup_roller;        //������ �������� �� ����������� ��� �����
//���������� ������ ������� ����
extern volatile uint8_t StateCM;                 //��������� ������ �������
extern volatile uint8_t MessegCM;               //��������� ������ �������
extern volatile uint8_t CodeErrorCM;            //��������� ������ ������ �������
extern volatile uint32_t Timer_RunCM;           //������ ������������� ������� ������� ������ � ���
//������� ������������� ����
extern volatile uint8_t State_Init_Axis;        //��������� ������������� ����
extern volatile uint8_t Code_Error_init_axis;   //���� ������
extern volatile uint8_t Timer_Init_axis;        //������ ����� ��������
//��������� ��������� ������ ���������
extern volatile uint8_t state_Man;          //��������� �������� �������������� ���������� ������������
extern volatile uint8_t state_Zah;          //��������� �������� �������������� ������� ������������
extern volatile uint8_t state_Fin;          //��������� �������� �������������� ������ ������������
//������� ������ ���������
extern volatile uint8_t Timer_Man_Ahead;        //������ ����������� ������� ���������� ������������
extern volatile uint8_t Timer_Man_Behind;       //������ ����������� ������� ���������� ������������
extern volatile uint8_t Timer_Zah_Open;         //������ ����������� ������� �������� �������
extern volatile uint8_t Timer_Zah_Close;        //������ ����������� ������� �������� �������
extern volatile uint8_t Timer_Fin_Ahead;        //������ ����������� ������� ���������� ������ ������������
extern volatile uint8_t Timer_Fin_Behind;       //������ ����������� ������� ���������� ������ ������������
//�������
extern volatile uint8_t MessegX;               //��������� �������� ��� X
extern volatile uint8_t CodeErrorX;            //��������� ������ �������� ��� X
extern volatile uint32_t LengthX;              //������ �� ������� ���� ������������� X
extern volatile uint8_t speedX;                //�������� ����������� ��� X
#endif /* __EXTERNS_H */


