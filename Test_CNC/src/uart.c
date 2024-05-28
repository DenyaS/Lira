
 
/* Includes ------------------------------------------------------------------*/
#include "uart.h"
#include "includes.h"
#include "stm32f10x_usart.h"

/**  
  * @brief      Функция для инициализации UART0
  * @param      None
  * @retval     None
  */
void init_UART0(void)
{
  GPIO_InitTypeDef gpio;
  USART_InitTypeDef uart;
  
  // Включаем тактирование порта A и USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

  // Настраиваем ногу Tx0 (PA9) как выход push-pull c альтернативной функцией
  gpio.GPIO_Pin = TX0;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(UART_GPIO_PORT, &gpio);
  
  // Rx0 - высокоимпендансный вход (PA10)
  gpio.GPIO_Pin = RX0;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(UART_GPIO_PORT, &gpio);

  USART_StructInit(&uart);     // Параметры по умолчанию: 9600 бод, 8 бит данных, 1 стоп-бит

  // Настраиваем UART под себя
  uart.USART_BaudRate = 9600;
  /*
  uart.USART_WordLength = USART_WordLength_8b;
  uart.USART_StopBits = USART_StopBits_1;
  uart.USART_Parity = USART_Parity_No ;
  uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  */

  // Инициализируем UART 
  USART_Init(USART1, &uart);   // Initialize the USARTx peripheral
  
  //USART1->BRR = 0xd0;       //   (24000000 + 115200 / 2) / 115200 = 208.8 => 0xd0

  // Настраиваем UART под себя
  //USART1->BRR = 0x9c4;            // BaudRate 9600
  /*
  Для получения значения, которое нужно записать в регистр BRR, нужно воспользоваться формулой:
  USART_BRR = (fck + baudrate / 2) / baudrate,
  где fck - это частота тактирования UART1; 
  baudrate - это желаемая скорость передачи/приёма. 
  (fck равна 24 Мгц если использовать кварц на 8 МГц и ничего не менять
  в настройках тактирования, а оставить их по умолчанию) 
  ***
  Пример расчётов: 
  Скорость 9600. 
  Получаем (24000000 + 9600 / 2) / 9600 = 2500.5 
  Отсекаем дробную часть, переводим число в шестнадцатеричную систему счисления 
  и получаем 0x9c4.  
  */

  /* Все остальные параметры (стоп биты, чётность,кол-во байт данных) уже настроены
  как надо, (во все эти биты по умолчанию записаны нули), таким образом мы имеем
  скорость 9600 1 стоп бит, 8 бит данных, без проверки чётности */

  //USART1->CR1 |= USART_CR1_UE; // Разрешаем работу USART1
  //USART1->CR1 |= USART_CR1_TE; // Включаем передатчик

  // Включаем прерывания по передаче и приёму байта
  USART_ITConfig(USART1, USART_IT_TC, ENABLE);      // Transmission complete interrupt
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    // Receive Data register not empty interrupt 

  // Включаем UART
  USART_Cmd(USART1, ENABLE);    // Enable the specified USART peripheral

  // Разрешаем прерывания от USART1
  NVIC_EnableIRQ(USART1_IRQn); 
  
  // Обнуляем структуру
  memset(&UARTx[0], 0x00, sizeof(UART_TypeDef));
}

/**  
  * @brief      Функция для инициализации UART1
  * @param      None
  * @retval     None
  */
void init_UART1(void)
{
  GPIO_InitTypeDef gpio;
  USART_InitTypeDef uart;
  
  // Включаем тактирование порта A и С "DIR"
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

  // Включаем тактирование порта USART2
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  // Настраиваем ногу Tx1 (PA2) как выход push-pull c альтернативной функцией
  gpio.GPIO_Pin = TX1;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(UART_GPIO_PORT, &gpio);

  // Rx1 - высокоимпендансный вход (PA3)
  gpio.GPIO_Pin = RX1;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(UART_GPIO_PORT, &gpio);
  
     // Настраиваем ногу DIR (PC12) как выход push-pull c основной функцией
  gpio.GPIO_Pin = DIR;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &gpio);
  
  USART_StructInit(&uart);     // Параметры по умолчанию: 9600 бод, 8 бит данных, 1 стоп-бит
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
  * @brief      Функция для отправления байта в UART0
  * @param      data - отправляемый в UART0 байт
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
  
  // Отсылаем байт через UART
  USART_SendData(USART1, data);

  // Reset Transmit data register empty flag
  USART_ClearFlag(USART1, USART_FLAG_TXE);
}

/**  
  * @brief      Функция для отправления байта в UART1
  * @param      data - отправляемый в UART1 байт
  * @retval     None
  */
void sendByte_UART1(uint8_t data) 
{
  // Включаем передатчик RS-485
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

  // Выключаем передатчик RS-485
  off(TX485);  
}

/**  
  * @brief      Функция для отправления строки в UART0
  * @param      string - отправляемая в UART0 строка
  * @retval     None
  */
void sendStr_UART0 (char *string) 
{
	uint8_t i = 0;
	
    // Выключаем прерывания
    NVIC_DisableIRQ(USART1_IRQn);
    
    while(string[i]) {
		sendByte_UART0(string[i]);
		i++;
	}
	sendByte_UART0('\n');
    
    // Включаем прерывания
    NVIC_EnableIRQ(USART1_IRQn);
}

/**  
  * @brief      Функция для отправления строки в UART1
  * @param      data - отправляемая в UART1 строка
  * @retval     None
  */
void sendStr_UART1 (char *string) 
{
	uint8_t i = 0;

    // Выключаем прерывания
    NVIC_DisableIRQ(USART2_IRQn);

    off(TX485);
	while(string[i]) {
		sendByte_UART1(string[i]);
		i++;
	}
	sendByte_UART1('\n');
    on(TX485);

    // Включаем прерывания
    NVIC_EnableIRQ(USART2_IRQn);
}

/**  
  * @brief      Функция чтения байта из UART0
  * @param      None 
  * @retval     Прочитанный из UART0 байт
  */
uint8_t readByte_UART0(void) 
{
  uint8_t Data;

  // Выключаем прерывание по приёму байта
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // Receive Data register not empty interrupt

      while(1) // Бесконечный цикл в нем мы проверяем ...
  {
  	if (USART1->SR & USART_SR_RXNE) {   // ... не пришло ли что-то в UART ?
      
      Data = USART1->DR;                // Считываем то что пришло в переменную...
      USART_ClearITPendingBit(USART1, USART_IT_RXNE); // Очищаем флаг в регистре
      
      // Включаем прерывание по приёму байта
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
      
      return Data;
    }
  }
}

/**  
  * @brief      Функция чтения байта из UART1
  * @param      None 
  * @retval     Прочитанный из UART1 байт
  */
uint8_t readByte_UART1(void) 
{
  uint8_t Data;

  // Выключаем прерывание по приёму байта
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // Receive Data register not empty interrupt
  
  while(1) // Бесконечный цикл в нем мы проверяем ...
  {
  	if (USART2->SR & USART_SR_RXNE) {   // ... не пришло ли что-то в UART ?
      
      Data = USART2->DR;                // Считываем то что пришло в переменную...
      USART_ClearITPendingBit(USART2, USART_IT_RXNE); // Очищаем флаг в регистре
      
      // Включаем прерывание по приёму байта
      USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
      
      return Data;
    }
  }
}

/**  
  * @brief      Сформировать пакет данных и передать его через UART
  * @param      uart_number - номер используемого UARTа: 0 = UART0 или 1 = UART1
  *             adr_dst - адрес получателя            
  *             cmd - код команды
  *             *ptr - указатель на данные
  *             len - размер данных
  * @retval     None
  */
void sendPacket_UARTx(uchar uart_number, uchar adr_dst, uchar cmd, 
                             uchar *ptr, uint len)
{
  uint Crc;

  // Защита от повторной посылки пакета
  //if (UARTx[uart_number].SendPack) return;  
  while(UARTx[uart_number].SendPack) { ; } 
  ADDR_DST_OUT(uart_number) = adr_dst;      // Адрес устройства - инициатора запроса

  ADDR_SRC_OUT(uart_number) = MY_ADDRESS;	// Адрес бортового контроллера в сети
  
  SIZE_OUT(uart_number) = len & 0xff;		// Размер данных
  CMD_OUT(uart_number) = cmd;				// Команда

  if(len)                                   // Если размер данных (len) > 0 
    memcpy(&DATA_OUT(uart_number), /*TmpArr*/ptr, len);  // формируем данные  
  Crc = MakeCRC16(&ADDR_DST_OUT(uart_number), len + HEAD_PACKET);  // Расчёт CRC
  *((uint *)&UARTx[uart_number].OutBuf[len + HEAD_PACKET]) = Crc;  // Формируем CRC
  
  UARTx[uart_number].Ptr = UARTx[uart_number].OutBuf;
  UARTx[uart_number].TxCnt = HEAD_PACKET + SIZE_OUT(uart_number) + 2;  	// Размер пакета: заголовок [ADDR_DST + ADDR_SRC + CMD] + данные [SIZE] + 2 байта CRC 
  UARTx[uart_number].SendPack = 1;                   			// Выставляем признак посылки пакета 
  
  if (uart_number == 0)  { // Выбран UART0 
    USART_SendData(USART1, FLAG_PACKET);
  }
  else if (uart_number == 1) { // Выбран UART1
	on(TX485);	  						// Включаем передатчик RS-485
    USART_SendData(USART2, FLAG_PACKET);
  }
  // Далее весь пакет передаётся по прерываниям
}

/** 
  * @brief      Отправить пакет с кодом ошибки в UART
  * @param      uart_number - номер СОМ порта
  *             cmd - код команды, которая вызвала ошибку
  *             err - код ошибки
  * @retval     None
  */
void sendError_UARTx (uchar uart_number, uchar adr_dst, uchar cmd, uchar err)
{
	TError Error;
	
    Error.Cmd = cmd;
	Error.Code = err;
    sendPacket_UARTx(uart_number, adr_dst, CMD_ERROR, (uchar *)&Error, sizeof(TError));
}



