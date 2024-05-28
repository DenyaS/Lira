
 
/* Includes ------------------------------------------------------------------*/
#include "uart.h"
#include "includes.h"
#include "stm32f10x_usart.h"

/**  
  * @brief      ������� ��� ������������� UART0
  * @param      None
  * @retval     None
  */
void init_UART0(void)
{
  GPIO_InitTypeDef gpio;
  USART_InitTypeDef uart;
  
  // �������� ������������ ����� A � USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

  // ����������� ���� Tx0 (PA9) ��� ����� push-pull c �������������� ��������
  gpio.GPIO_Pin = TX0;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(UART_GPIO_PORT, &gpio);
  
  // Rx0 - ������������������ ���� (PA10)
  gpio.GPIO_Pin = RX0;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(UART_GPIO_PORT, &gpio);

  USART_StructInit(&uart);     // ��������� �� ���������: 9600 ���, 8 ��� ������, 1 ����-���

  // ����������� UART ��� ����
  uart.USART_BaudRate = 9600;
  /*
  uart.USART_WordLength = USART_WordLength_8b;
  uart.USART_StopBits = USART_StopBits_1;
  uart.USART_Parity = USART_Parity_No ;
  uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  */

  // �������������� UART 
  USART_Init(USART1, &uart);   // Initialize the USARTx peripheral
  
  //USART1->BRR = 0xd0;       //   (24000000 + 115200 / 2) / 115200 = 208.8 => 0xd0

  // ����������� UART ��� ����
  //USART1->BRR = 0x9c4;            // BaudRate 9600
  /*
  ��� ��������� ��������, ������� ����� �������� � ������� BRR, ����� ��������������� ��������:
  USART_BRR = (fck + baudrate / 2) / baudrate,
  ��� fck - ��� ������� ������������ UART1; 
  baudrate - ��� �������� �������� ��������/�����. 
  (fck ����� 24 ��� ���� ������������ ����� �� 8 ��� � ������ �� ������
  � ���������� ������������, � �������� �� �� ���������) 
  ***
  ������ ��������: 
  �������� 9600. 
  �������� (24000000 + 9600 / 2) / 9600 = 2500.5 
  �������� ������� �����, ��������� ����� � ����������������� ������� ��������� 
  � �������� 0x9c4.  
  */

  /* ��� ��������� ��������� (���� ����, ��������,���-�� ���� ������) ��� ���������
  ��� ����, (�� ��� ��� ���� �� ��������� �������� ����), ����� ������� �� �����
  �������� 9600 1 ���� ���, 8 ��� ������, ��� �������� �������� */

  //USART1->CR1 |= USART_CR1_UE; // ��������� ������ USART1
  //USART1->CR1 |= USART_CR1_TE; // �������� ����������

  // �������� ���������� �� �������� � ����� �����
  USART_ITConfig(USART1, USART_IT_TC, ENABLE);      // Transmission complete interrupt
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    // Receive Data register not empty interrupt 

  // �������� UART
  USART_Cmd(USART1, ENABLE);    // Enable the specified USART peripheral

  // ��������� ���������� �� USART1
  NVIC_EnableIRQ(USART1_IRQn); 
  
  // �������� ���������
  memset(&UARTx[0], 0x00, sizeof(UART_TypeDef));
}

/**  
  * @brief      ������� ��� ������������� UART1
  * @param      None
  * @retval     None
  */
void init_UART1(void)
{
  GPIO_InitTypeDef gpio;
  USART_InitTypeDef uart;
  
  // �������� ������������ ����� A � � "DIR"
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

  // �������� ������������ ����� USART2
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  // ����������� ���� Tx1 (PA2) ��� ����� push-pull c �������������� ��������
  gpio.GPIO_Pin = TX1;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(UART_GPIO_PORT, &gpio);

  // Rx1 - ������������������ ���� (PA3)
  gpio.GPIO_Pin = RX1;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(UART_GPIO_PORT, &gpio);
  
     // ����������� ���� DIR (PC12) ��� ����� push-pull c �������� ��������
  gpio.GPIO_Pin = DIR;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &gpio);
  
  USART_StructInit(&uart);     // ��������� �� ���������: 9600 ���, 8 ��� ������, 1 ����-���
  uart.USART_BaudRate = 9600;
  /*
  uart.USART_WordLength = USART_WordLength_8b;
  uart.USART_StopBits = USART_StopBits_1;
  uart.USART_Parity = USART_Parity_No ;
  uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  */

  USART_Init(USART2, &uart);

  USART_ITConfig(USART2, USART_IT_TC, ENABLE);      // Transmission complete interrupt
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);    // Receive Data register not empty interrupt 

  USART_Cmd(USART2, ENABLE);    

  NVIC_EnableIRQ(USART2_IRQn); 

  memset(&UARTx[1], 0x00, sizeof(UART_TypeDef));
}

/**  
  * @brief      ������� ��� ����������� ����� � UART0
  * @param      data - ������������ � UART0 ����
  * @retval     None
  */
void sendByte_UART0(uint8_t data) 
{
  UARTx[0].Timer = 50;
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  { // check the Transmit data register empty flag
    if (UARTx[0].Timer == 0) {
      while(1) { ; }
    }
  }
  
  // �������� ���� ����� UART
  USART_SendData(USART1, data);

  // Reset Transmit data register empty flag
  USART_ClearFlag(USART1, USART_FLAG_TXE);
}

/**  
  * @brief      ������� ��� ����������� ����� � UART1
  * @param      data - ������������ � UART1 ����
  * @retval     None
  */
void sendByte_UART1(uint8_t data) 
{
  // �������� ���������� RS-485
  on(TX485);
  delay_1ms(3);
  UARTx[1].Timer = 50;
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
  {
    if (UARTx[1].Timer == 0) {
      while(1) { ; }
    }
  }
  USART_SendData(USART2, data);
  delay_1ms(1);
  USART_ClearFlag(USART2, USART_FLAG_TXE);

  // ��������� ���������� RS-485
  off(TX485);  
}

/**  
  * @brief      ������� ��� ����������� ������ � UART0
  * @param      string - ������������ � UART0 ������
  * @retval     None
  */
void sendStr_UART0 (char *string) 
{
	uint8_t i = 0;
	
    // ��������� ����������
    NVIC_DisableIRQ(USART1_IRQn);
    
    while(string[i]) {
		sendByte_UART0(string[i]);
		i++;
	}
	sendByte_UART0('\n');
    
    // �������� ����������
    NVIC_EnableIRQ(USART1_IRQn);
}

/**  
  * @brief      ������� ��� ����������� ������ � UART1
  * @param      data - ������������ � UART1 ������
  * @retval     None
  */
void sendStr_UART1 (char *string) 
{
	uint8_t i = 0;

    // ��������� ����������
    NVIC_DisableIRQ(USART2_IRQn);

    off(TX485);
	while(string[i]) {
		sendByte_UART1(string[i]);
		i++;
	}
	sendByte_UART1('\n');
    on(TX485);

    // �������� ����������
    NVIC_EnableIRQ(USART2_IRQn);
}

/**  
  * @brief      ������� ������ ����� �� UART0
  * @param      None 
  * @retval     ����������� �� UART0 ����
  */
uint8_t readByte_UART0(void) 
{
  uint8_t Data;

  // ��������� ���������� �� ����� �����
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // Receive Data register not empty interrupt

      while(1) // ����������� ���� � ��� �� ��������� ...
  {
  	if (USART1->SR & USART_SR_RXNE) {   // ... �� ������ �� ���-�� � UART ?
      
      Data = USART1->DR;                // ��������� �� ��� ������ � ����������...
      USART_ClearITPendingBit(USART1, USART_IT_RXNE); // ������� ���� � ��������
      
      // �������� ���������� �� ����� �����
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
      
      return Data;
    }
  }
}

/**  
  * @brief      ������� ������ ����� �� UART1
  * @param      None 
  * @retval     ����������� �� UART1 ����
  */
uint8_t readByte_UART1(void) 
{
  uint8_t Data;

  // ��������� ���������� �� ����� �����
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // Receive Data register not empty interrupt
  
  while(1) // ����������� ���� � ��� �� ��������� ...
  {
  	if (USART2->SR & USART_SR_RXNE) {   // ... �� ������ �� ���-�� � UART ?
      
      Data = USART2->DR;                // ��������� �� ��� ������ � ����������...
      USART_ClearITPendingBit(USART2, USART_IT_RXNE); // ������� ���� � ��������
      
      // �������� ���������� �� ����� �����
      USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
      
      return Data;
    }
  }
}

/**  
  * @brief      ������������ ����� ������ � �������� ��� ����� UART
  * @param      uart_number - ����� ������������� UART�: 0 = UART0 ��� 1 = UART1
  *             adr_dst - ����� ����������            
  *             cmd - ��� �������
  *             *ptr - ��������� �� ������
  *             len - ������ ������
  * @retval     None
  */
void sendPacket_UARTx(uchar uart_number, uchar adr_dst, uchar cmd, 
                             uchar *ptr, uint len)
{
  uint Crc;

  // ������ �� ��������� ������� ������
  //if (UARTx[uart_number].SendPack) return;  
  while(UARTx[uart_number].SendPack) { ; } 
  ADDR_DST_OUT(uart_number) = adr_dst;      // ����� ���������� - ���������� �������

  ADDR_SRC_OUT(uart_number) = MY_ADDRESS;	// ����� ��������� ����������� � ����
  
  SIZE_OUT(uart_number) = len & 0xff;		// ������ ������
  CMD_OUT(uart_number) = cmd;				// �������

  if(len)                                   // ���� ������ ������ (len) > 0 
    memcpy(&DATA_OUT(uart_number), /*TmpArr*/ptr, len);  // ��������� ������  
  Crc = MakeCRC16(&ADDR_DST_OUT(uart_number), len + HEAD_PACKET);  // ������ CRC
  *((uint *)&UARTx[uart_number].OutBuf[len + HEAD_PACKET]) = Crc;  // ��������� CRC
  
  UARTx[uart_number].Ptr = UARTx[uart_number].OutBuf;
  UARTx[uart_number].TxCnt = HEAD_PACKET + SIZE_OUT(uart_number) + 2;  	// ������ ������: ��������� [ADDR_DST + ADDR_SRC + CMD] + ������ [SIZE] + 2 ����� CRC 
  UARTx[uart_number].SendPack = 1;                   			// ���������� ������� ������� ������ 
  
  if (uart_number == 0)  { // ������ UART0 
    USART_SendData(USART1, FLAG_PACKET);
  }
  else if (uart_number == 1) { // ������ UART1
	on(TX485);	  						// �������� ���������� RS-485
    USART_SendData(USART2, FLAG_PACKET);
  }
  // ����� ���� ����� ��������� �� �����������
}

/** 
  * @brief      ��������� ����� � ����� ������ � UART
  * @param      uart_number - ����� ��� �����
  *             cmd - ��� �������, ������� ������� ������
  *             err - ��� ������
  * @retval     None
  */
void sendError_UARTx (uchar uart_number, uchar adr_dst, uchar cmd, uchar err)
{
	TError Error;
	
    Error.Cmd = cmd;
	Error.Code = err;
    sendPacket_UARTx(uart_number, adr_dst, CMD_ERROR, (uchar *)&Error, sizeof(TError));
}



