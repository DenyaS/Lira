
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PIN_DEFS_H
#define __PIN_DEFS_H

/* Defines GPIO --------------------------------------------------------------*/
// CAN Interface
#define CAN_GPIO_PORT       GPIOC
//#define CAN_TX              GPIO_Pin_5      // PC5 - CAN Tx
//#define CAN_RX              GPIO_Pin_6      // PC6 - CAN Rx
#define CAN_RV              GPIO_Pin_7      // PC7 - CAN Reference Voltage 

// UART
#define UART_GPIO_PORT      GPIOA           
#define TX0                 GPIO_Pin_9      // PA9 - UART0 Tx       
#define RX0                 GPIO_Pin_10     // PA10 - UART0 Rx         
#define TX1                 GPIO_Pin_2      // PA2 - UART1 Tx           
#define RX1                 GPIO_Pin_3      // PA3 - UART1 Rx
#define DIR                 GPIO_Pin_4      // PA4 - Direction UART1 for RS-485 interface

// Debug Board
#define DBOARD_GPIO_PORT_1  GPIOA
#define RF_FSEL             GPIO_Pin_1      // PA1 - Frequency Shift Keying; Data; FIFO Select ок 
#define RF_IRO              GPIO_Pin_8      // PA8 - Interrupt request output   ок
#define EE_CS               GPIO_Pin_11     // PA11 - SPI chip select for AT45DB161D ок
#define RF_INT              GPIO_Pin_12     // PA12 - Interrupt; Data indicator output ок

#define DBOARD_GPIO_PORT_2  GPIOC
#define RF_FINT             GPIO_Pin_0      // PC0 - Recovery Clock Output; Filter Capacitor; FIFO Interrupt ок
#define EE_RST              GPIO_Pin_1      // PC1 - Reset signal for AT45DB161D ок
#define RF_RST              GPIO_Pin_2      // PC2 - Reset signal for MRF49XA ок 
#define RF_CLK              GPIO_Pin_3      // PC3 - Clock output MRF49XA ок 
#define RF_CS               GPIO_Pin_4      // PC4 - SPI chip select for MRF49XA ок

// SPI
#define SPI_GPIO_PORT       GPIOA
#define SCK                 GPIO_Pin_5      // PA5 - SPI clock ок 
#define MISO                GPIO_Pin_6      // PA6 - SPI master input signal ок 
#define MOSI                GPIO_Pin_7      // PA7 - SPI master output signal ок

// INPUTS
#define SENSORS_GPIO_PORT_1 GPIOB
#define SEN_1               GPIO_Pin_0      // PB0 - Sensor 1 (кнопка открытия передней двери) ок 
#define SEN_2               GPIO_Pin_1      // PB1 - Sensor 2 (кнопка открытия задней двери) ок 
/** Примечание. Ошибочно использовал задействованный в плате STM32VLD порт PB3 
#define SEN_4               GPIO_Pin_3      // PB3 - Sensor 4 не задействован в  схеме !!!!!!!!!!!!!!!!!!!!!!!!!!1
------------------------------------------------------------------------------*/

/** Примечание. Ошибочно использовал задействованный в плате STM32VLD порт PA15 
#define SENSORS_GPIO_PORT_2 GPIOA
#define SEN_3               GPIO_Pin_15     // PA15 - Sensor 3 не задействован в  схеме !!!!!!!!!!!!!!!!!!!!!!!!!!1
------------------------------------------------------------------------------*/
#define SENSORS_GPIO_PORT_2 GPIOC
#define SEN_3               GPIO_Pin_5      // PC5 - Sensor 3   ок 
#define SEN_4               GPIO_Pin_6      // PC6 - Sensor 4  ок 

#define KEYBOARD_GPIO_PORT  GPIOB
/** Примечание. Ошибочно использовал задействованный в плате STM32VLD порт PB4 */
//#define KEY_UP              GPIO_Pin_4      // PB4 - Button "UP" - НЕ РАБОТАЕТ!!!   ок 
#define KEY_DOWN            GPIO_Pin_5      // PB5 - Button "DOWN"      ок 
#define KEY_OK              GPIO_Pin_6      // PB6 - Button "OK"        ок
#define KEY_ESC             GPIO_Pin_7      // PB7 - Button "ESCAPE"    ок
#define KEY_UP              GPIO_Pin_7      // PC7 - Button "UP" GPIOC  ок

#define KEYBOARD_EXTI_PORT      EXTI_PortSourceGPIOC    
#define KEY_UP_EXTI_PIN         EXTI_PinSource0
#define KEY_DOWN_EXTI_PIN       EXTI_PinSource1
#define KEY_OK_EXTI_PIN         EXTI_PinSource2
#define KEY_ESC_EXTI_PIN        EXTI_PinSource3

#define USERBUTTON_GPIO_PORT    GPIOA
#define USERBUTTON              GPIO_Pin_0      // PA0

// OUTPUTS
#define OUTPUTS_GPIO_PORT   GPIOC
#define OUT_1               GPIO_Pin_10     // PC10 - Output 1
#define OUT_2               GPIO_Pin_11     // PC11 - Output 2
#define OUT_3               GPIO_Pin_12     // PC12 - Output 3  
#define LED_WORK            OUT_1
#define LED_ERROR           OUT_2

#define SND_GPIO_PORT       GPIOD
#define OUT_SND             GPIO_Pin_2      // PD2 - Sound (buzzer)
#define SND                 SND_GPIO_PORT, OUT_SND

#define LEDS_GPIO_PORT          GPIOC           
#define LEDBLUE                 GPIO_Pin_8      // PC8 - Blue LED 
#define LEDGREEN                GPIO_Pin_9      // PB9 - Green LED

// LCD MTC-S16204XRGHS
#define MTC_GPIO_PORT           GPIOB
#define MTC_RS                  GPIO_Pin_10     // PB10 - Data/Instruction Register Select ок
#define MTC_E                   GPIO_Pin_11     // PB11 - Enable Signal 
#define MTC_DB4                 GPIO_Pin_12     // PB12 - I/O Data Bus
#define MTC_DB5                 GPIO_Pin_13     // PB13 - I/O Data Bus
#define MTC_DB6                 GPIO_Pin_14     // PB14 - I/O Data Bus
#define MTC_DB7                 GPIO_Pin_15     // PB15 - I/O Data Bus
#define MTC_BKL                 OUT_3           // подсветка: поключен к OUTPUTS_GPIO_PORT [OUT_3]
#define MTCBITS                 MTC_RS | MTC_E | MTC_DB4 | MTC_DB5 | MTC_DB6 | MTC_DB7

#define MTC_GPIO_PORT_V0        GPIOC
#define MTC_V0                  GPIO_Pin_13      // PC13 - Power supply for LCD panel "OP4"

// I2C Interface 
#define I2C_GPIO_PORT           GPIOB
#define SCL                     GPIO_Pin_8      // PB8 - SCL
#define SDA                     GPIO_Pin_9      // PB9 - SDA

/* Type defenitions --------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PIN_DEFS_H */

