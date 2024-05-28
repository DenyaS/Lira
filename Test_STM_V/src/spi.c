 
/* Includes ------------------------------------------------------------------*/
#include "spi.h"
#include "includes.h"
#include "stm32f10x_spi.h"

/**  
  * @brief      ������� ������������� ���������� SPI
  * @param      None
  * @retval     None
  */
void init_SPI(void)
{
  GPIO_InitTypeDef gpio;
  SPI_InitTypeDef spi;

  // ������������ ������ SPI1, ����� � (EE_CS), ����� C (RF_CS)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  // ������ CS ��������� AT45DB161D � MRF49XA
  gpio.GPIO_Pin = EE_CS;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DBOARD_GPIO_PORT_1 , &gpio);      // port A
  gpio.GPIO_Pin = RF_CS;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DBOARD_GPIO_PORT_2 , &gpio);      // port C

  // ������ SCK, MISO, MOSI
  gpio.GPIO_Pin = SCK | MISO | MOSI;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_GPIO_PORT, &gpio);
  
  // Fill each SPI_InitStruct member with its default value
  SPI_StructInit(&spi);

  // ��������� ��������� � ����������� SPI ������
  spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // ������ �������
  spi.SPI_Mode = SPI_Mode_Master;                       // ����� - ������
  spi.SPI_DataSize = SPI_DataSize_8b;                   // �������� �� 8 ���
  spi.SPI_CPOL = SPI_CPOL_Low;                          // ���������� �
  spi.SPI_CPHA = SPI_CPHA_1Edge;                        // ���� ��������� �������
  spi.SPI_NSS = SPI_NSS_Soft;      // ��������� ���������� ������� NSS ����������
  spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // ������������ SCK
  spi.SPI_FirstBit = SPI_FirstBit_MSB;                  // ������ ������������ ������� ���
  spi.SPI_CRCPolynomial = 7;
  
  // ����������� SPI1
  SPI_Init(SPI1, &spi); 
  
  // �������� ������ SPI1
  SPI_Cmd(SPI1, ENABLE); 
  
  // �������� ���������� �� ������ ����� 
  //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE); 
  // ... � �� �������� �����
  //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE); 

  // ��������� ������ NSS �������������� ����������, ��������� ��� � �������
  // ���� �������� ��� � ����, �� ��� SPI ������ ��������, ���
  // � ��� ��������������� ��������� � ��� ������ ���������� �������.
  SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);

  // ������� ������� ������ ��� ��������
  SPI1->DR = 0;

  // Clear the SPIx CRC Error flags
  SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_TXE);
  SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
  
  // ��������� ���������� �� SPI1
  //NVIC_EnableIRQ(SPI1_IRQn); 
 
  // ��� �������� �� �����������...
}

/** 
  * @brief      ����� ������ �� SPI
  * @param      data - ���������� ����
  * @retval     ���������� �������� ����
  */
uchar send2SPI(uchar data) 
{
  SPI_I2S_SendData(SPI1, data);       // �������� ���� [data] ����� SPI1
  TimerSPI = 100;
  while(TimerSPI) 
  {  
    /* Check SPI1 Busy flag */
    if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == RESET) {
      return (uchar)(SPI_I2S_ReceiveData(SPI1)); // ������ �������� ������
    }
  }
  displayError(ERR_SPI);
  while(1) { blink(OUTPUTS_GPIO_PORT, LED_ERROR, 30, SND_ON); }
}
