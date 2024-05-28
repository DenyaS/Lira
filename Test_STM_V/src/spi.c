 
/* Includes ------------------------------------------------------------------*/
#include "spi.h"
#include "includes.h"
#include "stm32f10x_spi.h"

/**  
  * @brief      Функция инициализации интерфейса SPI
  * @param      None
  * @retval     None
  */
void init_SPI(void)
{
  GPIO_InitTypeDef gpio;
  SPI_InitTypeDef spi;

  // Тактирование модуля SPI1, порта А (EE_CS), порта C (RF_CS)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  // Выводы CS микросхем AT45DB161D и MRF49XA
  gpio.GPIO_Pin = EE_CS;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DBOARD_GPIO_PORT_1 , &gpio);      // port A
  gpio.GPIO_Pin = RF_CS;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DBOARD_GPIO_PORT_2 , &gpio);      // port C

  // Выводы SCK, MISO, MOSI
  gpio.GPIO_Pin = SCK | MISO | MOSI;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_GPIO_PORT, &gpio);
  
  // Fill each SPI_InitStruct member with its default value
  SPI_StructInit(&spi);

  // Заполняем структуру с параметрами SPI модуля
  spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // полный дуплекс
  spi.SPI_Mode = SPI_Mode_Master;                       // Режим - мастер
  spi.SPI_DataSize = SPI_DataSize_8b;                   // передаем по 8 бит
  spi.SPI_CPOL = SPI_CPOL_Low;                          // Полярность и
  spi.SPI_CPHA = SPI_CPHA_1Edge;                        // фаза тактового сигнала
  spi.SPI_NSS = SPI_NSS_Soft;      // Управлять состоянием сигнала NSS программно
  spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // Предделитель SCK
  spi.SPI_FirstBit = SPI_FirstBit_MSB;                  // Первым отправляется старший бит
  spi.SPI_CRCPolynomial = 7;
  
  // Настраиваем SPI1
  SPI_Init(SPI1, &spi); 
  
  // Включаем модуль SPI1
  SPI_Cmd(SPI1, ENABLE); 
  
  // Включаем прерывание по приему байта 
  //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE); 
  // ... и по передаче байта
  //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE); 

  // Поскольку сигнал NSS контролируется программно, установим его в единицу
  // Если сбросить его в ноль, то наш SPI модуль подумает, что
  // у нас мультимастерная топология и его лишили полномочий мастера.
  SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);

  // Очищаем регистр данных для передачи
  SPI1->DR = 0;

  // Clear the SPIx CRC Error flags
  SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_TXE);
  SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
  
  // Разрешаем прерывания от SPI1
  //NVIC_EnableIRQ(SPI1_IRQn); 
 
  // все работает на прерываниях...
}

/** 
  * @brief      Обмен байтом по SPI
  * @param      data - посылаемый байт
  * @retval     Полученный ответный байт
  */
uchar send2SPI(uchar data) 
{
  SPI_I2S_SendData(SPI1, data);       // Передаем байт [data] через SPI1
  TimerSPI = 100;
  while(TimerSPI) 
  {  
    /* Check SPI1 Busy flag */
    if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == RESET) {
      return (uchar)(SPI_I2S_ReceiveData(SPI1)); // Читаем принятые данные
    }
  }
  displayError(ERR_SPI);
  while(1) { blink(OUTPUTS_GPIO_PORT, LED_ERROR, 30, SND_ON); }
}
