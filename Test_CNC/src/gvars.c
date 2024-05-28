
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

// Counters
volatile uint32_t TickCnt = 0;

// ������� 1 ms
volatile uint32_t TimerCPL = 0;
volatile uint32_t TimerLED = 0;
volatile uint32_t TimeForDelay = 0;

// ������� 1 s
volatile uint32_t STimer1 = 0;       
volatile uint32_t STimer2 = 0;       

// Flags
volatile _FLAGS Flags;   

// Interfaces 
UART_TypeDef UARTx[2];  

// ������
StepMotor_TypeDef Motor[4];

// ��������
Encoder_TypeDef Enc[4];

//���������� ������ ������ ����
volatile uint8_t ErrCodeAll;            //��� ������ ������
volatile uint8_t StateSM;             //��������� ������ ������
volatile uint8_t MessegSM;            //��������� ������ ������
volatile uint32_t LengthPol;             //������ ������ �� ������� ���� ���
volatile uint8_t speedSM;                 //�������� �������
volatile uint8_t CodeErrorSM;            //��������� ������ ������ ������
volatile uint32_t Timer_inputSM;           //������ �������� ������ �� ����� � ���
volatile uint32_t Timer_Sup_roller;        //������ �������� �� ����������� ��� �����
//���������� ������ ������� ����
volatile uint8_t StateCM;               //��������� ������ �������
volatile uint8_t MessegCM;               //��������� ������ �������
volatile uint8_t CodeErrorCM;            //��������� ������ ������ �������
volatile uint32_t Timer_RunCM;           //������ ������������� ������� ������� ������ � ���
//������� ������������� ����
volatile uint8_t State_Init_Axis;                //��������� ������������� ����
volatile uint8_t Code_Error_init_axis;                    //���� ������
//������������� ����
volatile uint8_t State_Init_Axis;                 //��������� �������� ������������� ��� 
volatile uint8_t Timer_Init_axis;        //������ ����� ��������
//��������� ��������� ������ ���������
volatile uint8_t state_Man = 0;          //��������� �������� �������������� ���������� ������������
volatile uint8_t state_Zah = 0;          //��������� �������� �������������� ������� ������������
volatile uint8_t state_Fin = 0;          //��������� �������� �������������� ������ ������������
//������� ������ ���������
volatile uint8_t Timer_Man_Ahead;        //������ ����������� ������� ���������� ������������
volatile uint8_t Timer_Man_Behind;       //������ ����������� ������� ���������� ������������
volatile uint8_t Timer_Zah_Open;         //������ ����������� ������� �������� �������
volatile uint8_t Timer_Zah_Close;        //������ ����������� ������� �������� �������
volatile uint8_t Timer_Fin_Ahead;        //������ ����������� ������� ���������� ������ ������������
volatile uint8_t Timer_Fin_Behind;       //������ ����������� ������� ���������� ������ ������������
//�������
volatile uint8_t MessegX;               //��������� �������� ��� X
volatile uint8_t CodeErrorX;            //��������� ������ �������� ��� X
volatile uint32_t LengthX;              //������ �� ������� ���� ������������� X
volatile uint8_t speedX;                //�������� ����������� ��� X


