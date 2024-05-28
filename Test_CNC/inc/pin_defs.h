

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PIN_DEFS_H
#define __PIN_DEFS_H

/* Defines GPIO --------------------------------------------------------------*/

// Using by STM32VLD
#define USERBUTTON_GPIO_PORT    GPIOA
#define USERBUTTON              GPIO_Pin_0      // PA0

#define RCC_LEDS_GPIO       RCC_APB2Periph_GPIOC
#define LEDS_GPIO_PORT      GPIOC           
#define LEDBLUE             GPIO_Pin_8      // PC8 - Blue LED
#define LEDGREEN            GPIO_Pin_9      // PC9 - Green LED
//����� �������� ����
#define Sen1SM              GPIO_Pin_4      // PC4 - ���� ������� ������� ������ �� �����
#define Sen2SM              GPIO_Pin_4      // PA4 - ���� ������� ������ �������� ����� �������
#define Sen3SM              GPIO_Pin_6      // PA6 - ���� ������� ������ �������� ����� �������
#define SenCM               GPIO_Pin_5      // PA5 - ���� ������� ������ ������� 
#define Home                GPIO_Pin_7      // PA7 - ���� Home 
#define Limit               GPIO_Pin_11     // PC11 - ���� Limit   
#define CloseZah            GPIO_Pin_10     // PC10 - ���� ZZ  FingerBeh
#define OpenZah             GPIO_Pin_13     // PC13 - ���� ZA OpenZah   
#define FingerAhed          GPIO_Pin_5      // PC5 - ���� ZY CloseZah 
#define FingerBeh           GPIO_Pin_2      // PC2 - ���� ZX FingerBeh 
#define Man_Ahead           GPIO_Pin_11      //PA11 ������ Manipulator Ahead
#define Man_Behind          GPIO_Pin_12      //PA12 ������ Manipulator Behind
//������ ����
#define RELAY_CM            GPIO_Pin_11     // PB11 - ���� ��������� ������ ������� ������ �������
#define Vid_Man             GPIO_Pin_13     // PB13 - ������������ ���������� ������� ������������
#define FingerMan           GPIO_Pin_14     // PB14 - ������������ ���������� ������ ������������
#define ZahvMan             GPIO_Pin_12     // PB12 - ������������ ������ ������������

// UART
#define UART_GPIO_PORT      GPIOA           
#define TX0                 GPIO_Pin_9      // PA9 - UART0 Tx
#define RX0                 GPIO_Pin_10     // PA10 - UART0 Rx
#define TX1                 GPIO_Pin_2      // PA2 - UART1 Tx
#define RX1                 GPIO_Pin_3      // PA3 - UART1 Rx
#define DIR                 GPIO_Pin_12      // P�12 - Direction UART1 for RS-485 interface

// ����� ���������
#define RCC_ENCODERS_GPIO   RCC_APB2Periph_GPIOC
#define ENC_PortSourceGPIO  GPIO_PortSourceGPIOC
#define ENCODERS_GPIO_PORT  GPIOC
        // Encoder X
#define ENCX_A              GPIO_Pin_6      // PB6 - ���� � �������� ��� X
#define ENCX_B              GPIO_Pin_7      // PB7 - ���� � �������� ��� X
#define ENCX_Z              GPIO_Pin_2      // PC2 - ���� ZERO �������� ��� X
#define ENCX_A_PinSource    GPIO_PinSource0
#define ENCX_B_PinSource    GPIO_PinSource1
#define ENCX_Z_PinSource    GPIO_PinSource2
#define ENCX_A_EXTI_LINE    EXTI_Line0      // ���� ���������� A �� EncoderX
#define ENCX_B_EXTI_LINE    EXTI_Line1      // ���� ���������� B �� EncoderX
#define ENCX_Z_EXTI_LINE    EXTI_Line2      // ���� ���������� ZERO �� EncoderX
#define ENCX_A_IRQn         EXTI0_IRQn      // EXTI Line0 Interrupt (A)
#define ENCX_B_IRQn         EXTI1_IRQn      // EXTI Line1 Interrupt (B)
#define ENCX_Z_IRQn         EXTI2_IRQn      // EXTI Line2 Interrupt (ZERO)
        // Encoder Y
#define ENCY_A              GPIO_Pin_0      // PA0 - ���� � �������� ��� Y
#define ENCY_B              GPIO_Pin_1      // PA1 - ���� � �������� ��� Y
#define ENCY_Z              GPIO_Pin_5      // PC5 - ���� ZERO �������� ��� Y
#define ENCY_A_PinSource    GPIO_PinSource3
#define ENCY_B_PinSource    GPIO_PinSource4
#define ENCY_Z_PinSource    GPIO_PinSource5
#define ENCY_A_EXTI_LINE    EXTI_Line3      // ���� ���������� A �� EncoderY
#define ENCY_B_EXTI_LINE    EXTI_Line4      // ���� ���������� B �� EncoderY
#define ENCY_Z_EXTI_LINE    EXTI_Line5      // ���� ���������� ZERO �� EncoderY
#define ENCY_A_IRQn         EXTI3_IRQn      // EXTI Line3 Interrupt (A)
#define ENCY_B_IRQn         EXTI4_IRQn      // EXTI Line4 Interrupt (B)
#define ENCY_Z_IRQn         EXTI9_5_IRQn    // EXTI Line5...9 Interrupt (ZERO)
        // Encoder Z
#define ENCZ_A              GPIO_Pin_6      // PC6 - ���� � �������� ��� Z
#define ENCZ_B              GPIO_Pin_7      // PC7 - ���� � �������� ��� Z
#define ENCZ_Z              GPIO_Pin_10     // PC10 - ���� ZERO �������� ��� Z
#define ENCZ_A_PinSource    GPIO_PinSource6
#define ENCZ_B_PinSource    GPIO_PinSource7
#define ENCZ_Z_PinSource    GPIO_PinSource10
#define ENCZ_A_EXTI_LINE    EXTI_Line6      // ���� ���������� A �� EncoderZ
#define ENCZ_B_EXTI_LINE    EXTI_Line7      // ���� ���������� B �� EncoderZ
#define ENCZ_Z_EXTI_LINE    EXTI_Line10     // ���� ���������� ZERO �� EncoderZ
#define ENCZ_A_IRQn         EXTI9_5_IRQn    // EXTI Line5...9 Interrupt (A)
#define ENCZ_B_IRQn         EXTI9_5_IRQn    // EXTI Line5...9 Interrupt (B)
#define ENCZ_Z_IRQn         EXTI15_10_IRQn  // EXTI Line10...15 Interrupt (ZERO)
        // Encoder A
#define ENCA_A              GPIO_Pin_8     // PA8 - ���� � �������� ��� A
#define ENCA_B              GPIO_Pin_9     // PA9 �����,     �� ����� PC12 - ���� � �������� ��� A!!!!
#define ENCA_Z              GPIO_Pin_13     // PC13 - ���� ZERO �������� ��� A
#define ENCA_A_PinSource    GPIO_PinSource11
#define ENCA_B_PinSource    GPIO_PinSource12
#define ENCA_Z_PinSource    GPIO_PinSource13
#define ENCA_A_EXTI_LINE    EXTI_Line11     // ���� ���������� A �� EncoderA
#define ENCA_B_EXTI_LINE    EXTI_Line12     // ���� ���������� B �� EncoderA
#define ENCA_Z_EXTI_LINE    EXTI_Line13     // ���� ���������� ZERO �� EncoderA
#define ENCA_A_IRQn         EXTI15_10_IRQn  // EXTI Line10...15 Interrupt (A)
#define ENCA_B_IRQn         EXTI15_10_IRQn  // EXTI Line10...15 Interrupt (B)
#define ENCA_Z_IRQn         EXTI15_10_IRQn  // EXTI Line10...15 Interrupt (ZERO)

// ������ �� �������� ��
#define RCC_SMOTORS_GPIO    RCC_APB2Periph_GPIOB
#define SMOTORS_GPIO_PORT   GPIOB
      // Driver X
#define PUL_X               GPIO_Pin_0      // PB0 - ����� PUL(STEP) �������� �� ��� �
#define DIR_X               GPIO_Pin_1      // PB1 - ����� DIR �������� �� ��� �
      // Driver Y
#define PUL_Y               GPIO_Pin_5      // PB5 - ����� PUL(STEP) �������� �� ��� Y
#define DIR_Y               GPIO_Pin_0      // PC0 -  ����� DIR �������� �� ��� Y
      // Driver Z
#define PUL_Z               GPIO_Pin_1      // PC1 - ����� PUL(STEP) �������� �� ��� Z
#define DIR_Z               GPIO_Pin_8      // PB8 - ����� DIR �������� �� ��� Z
      // Driver A
#define PUL_A               GPIO_Pin_9      // PB9 - ����� PUL(STEP) �������� �� ��� A
#define DIR_A               GPIO_Pin_10     // PB10 - ����� DIR �������� �� ��� A

// ����� ���������� ����������
#define RCC_INPUTS_GPIO         RCC_APB2Periph_GPIOA
#define INPUTS_GPIO_PORT        GPIOA
      // Driver X
//#define IPUL_X                  GPIO_Pin_0      // PB0 - ���� PUL(STEP) ��� �������� ��� X
//#define IDIR_X                  GPIO_Pin_1      // PB1 - ���� DIR ��� �������� ��� X
      // Driver Y
//#define IPUL_Y                  GPIO_Pin_5      // PA5 - ���� PUL(STEP) ��� �������� ��� Y
//#define IDIR_Y                  GPIO_Pin_6      // PA6 - ���� DIR ��� �������� ��� Y
      // Driver Z
//#define IPUL_Z                  GPIO_Pin_7      // PA7 - ���� PUL(STEP) ��� �������� ��� Z
//#define IDIR_Z                  GPIO_Pin_8      // PA8 - ���� DIR ��� �������� ��� Z
      // Driver A
//#define IPUL_A                  GPIO_Pin_11     // PA11 - ���� PUL(STEP) ��� �������� ��� A
//#define IDIR_A                  GPIO_Pin_12     // PA12 - ���� DIR ��� �������� ��� A

// ������ �� ����������� ���������
#define RCC_STATUS_GPI          RCC_APB2Periph_GPIOB
#define STATUS_GPIO_PORT        GPIOB
//#define INPOS_X                 GPIO_Pin_11     // PB11 - ����� "������ � � �������" 
#define INPOS_Y                 GPIO_Pin_12     // PB12 - ����� "������ Y � �������" 
#define INPOS_Z                 GPIO_Pin_13     // PB13 - ����� "������ Z � �������"
#define INPOS_A                 GPIO_Pin_14     // PB14 - ����� "������ A � �������"
#define FAULT                   GPIO_Pin_15     // PB15 - ����� "������ ���������������� �������"

// ����� �� ������
#define RCC_BUTTON_GPIO         RCC_APB2Periph_GPIOD
#define BUTTON_GPIO_PORT        GPIOD
#define BUT_1                   GPIO_Pin_2      // PD2 - ������ "Button 1"
#define BUT_2                   USERBUTTON      // PA0 - ������ "Button 2" = UserButton �� ����� STM32VLD

// ������ �� ���������� �� �������� ������
#define LEDWORK                 LEDBLUE         // PC8 - ��������� "������"
#define LEDERROR                LEDGREEN        // PC9 - ��������� "������"

// X1
#define X1_GPIO_PORT        GPIOC

// X2
#define X2_GPIO_PORT        GPIOA

// X3
#define X3_GPIO_PORT        GPIOB

// X4
#define X4_GPIO_PORT        GPIOD

// SPI
#define SPI_GPIO_PORT       GPIOA

// I2C Interface 
#define I2C_GPIO_PORT       GPIOB

// Redefine encoder inputs to general-purpose inputs
#define JOYSTICK_GPIO_PORT  ENCODERS_GPIO_PORT  // Port C
#define JOYSTICK_UP         ENCX_A              // PC0
#define JOYSTICK_DN         ENCX_B              // PC1
//#define JOYSTICK_LEFT       ENCX_Z              // PC2
#define JOYSTICK_RIGHT      ENCY_Z              // PC5

// Redefine step/dir inputs to general-purpose inputs
#define SIGNALS_GPIO_PORT   INPUTS_GPIO_PORT    // Port A
#define ESTOP_SIGNAL        IPUL_X              // PA1
#define ENABLE_SIGNAL       IDIR_X              // PA4
//#define SEN_HOME            IPUL_Y              // PA5
//#define SEN_LIMIT           IDIR_Y              // PA6
//#define SEN_AD1             IPUL_Z              // PA7
#define SEN_AD2             IDIR_Z              // PA8
#define BUT_START           IPUL_A              // PA11
#define BUT_STOP            IDIR_A              // PA12

// Redefine general-purpose outputs
#define OUT_GPIO_PORT       STATUS_GPIO_PORT    // Port B
//#define WORK_DOUT           INPOS_X             // PB11       
#define BUZZER_DOUT         INPOS_Y             // PB12
#define LITTER_DOUT         INPOS_Z             // PB13
#define SEWAGE_DOUT         INPOS_A             // PB14
#define ERROR_DOUT          FAULT               // PB15
//#define LEDSTART_DOUT       PUL_X               // PB0

/* Type defenitions --------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PIN_DEFS_H */


