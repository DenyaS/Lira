 
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"
#include "includes.h"
#include "stm32f10x_i2c.h"

/* Private defines -----------------------------------------------------------*/
#define I2C_STOP_MASK        ((uint16_t)0x0200)
#define I2C_POS_SET          ((uint16_t)0x0800)
#define I2C_POS_RESET        ((uint16_t)0xF7FF)
/**  
  * @brief      Функция инициализации интерфейса I2C
  * @param      None
  * @retval     None
  */
void init_I2C (void)
{
  GPIO_InitTypeDef gpio;
  I2C_InitTypeDef i2c;

  // Тактирование аппаратного модуля I2C1, порта B (SCL и SDA)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  // I2C1 использует Remap (PB8, PB9 вместо PB6, PB7)
  // чтобы работал remap, необходимо включить тактирование AFIO
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  /* I2C1 GPIO Configuration	
    PB8	 ------> I2C1_SCL
	PB9	 ------> I2C1_SDA
  */
  
  gpio.GPIO_Pin = SCL | SDA;

  gpio.GPIO_Mode = GPIO_Mode_AF_OD;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(I2C_GPIO_PORT, &gpio);      
  
  /* Configure peripheral I/O remapping */
  GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

  // Deinitialize the I2C1 peripheral registers to their default reset values
  I2C_DeInit(I2C1);
  
  // Заполняем структуру I2C модуля параметрами по умолчанию 
  I2C_StructInit(&i2c);
  
  // Конфигурируем I2C1 "под себя"
  i2c.I2C_ClockSpeed = SPEED_100KHZ;
  i2c.I2C_Mode = I2C_Mode_I2C;
  i2c.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c.I2C_OwnAddress1 = OWN_I2C_ADR;
  i2c.I2C_Ack = I2C_Ack_Disable;
  i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  // Настраиваем I2C1
  I2C_Init(I2C1, &i2c); 

  // Включаем модуль I2C1
  I2C_Cmd(I2C1, ENABLE); 

  /*
  // Включаем прерывание [Buffer interrupt mask] 
  I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
  // ... и [Event interrupt mask]
  I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE); 
  
  // Разрешаем прерывания от I2C1
  NVIC_EnableIRQ(I2C1_EV_IRQn); 
  */
}

/**  
  * @brief      Передать данные в SLAVE устройство шины I2C 
  * @param      adr - адрес устройства на шине I2C
  *             *buf - указатель на данные для передачи
  *             len - количество байт данных
  * @retval     0 - ERROR
  *             1 - SUCCESS
  */
ErrorStatus sendData_I2C (uchar adr, uchar *buf, uchar len)
{
  vu32 Timeout = 0;     // vu = volatile uint
  
  /* Enable Error IT */
  I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
  
  /* 1. Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* 1.1. Wait until SB flag is set: EV5 */
  Timeout = 500;//0xffff;
  while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
  {
    if (Timeout-- == 0)
    { return ERROR; }  
  }

  /* 2. Send slave address */
  I2C_Send7bitAddress(I2C1, adr, I2C_Direction_Transmitter);
  /* 2.1. Wait until ADDR is set: EV6 */
  Timeout = 500;//0xffff;
  while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
  {
    if (Timeout-- == 0)
    { return ERROR; }   
  }

  /* 3. Write the first byte in DR register (EV8_1) */
  I2C_SendData(I2C1, *buf);
  /* 3.1. Increment buffer*/
  buf++;
  /* 3.2. Decrement the number of bytes to be written */
  len--;

  /* 4. While there is data to be written */
  while (len--)
  {
    /* 4.1. Poll on BTF to receive data because in polling mode we can not guarantee the
    EV8 software sequence is managed before the current byte transfer completes */
    Timeout = 500;//0xffff;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
    {
      if (Timeout-- == 0)
      { return ERROR; }   
    }
    /* 4.2. Send the current byte */
    I2C_SendData(I2C1, *buf);
    buf++;
  }
  /* 4.3. EV8_2: Wait until BTF is set before programming the STOP */
  Timeout = 500;//0xffff;
  while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
  {
    if (Timeout-- == 0)
    { return ERROR; }   
  }

  /* 5. Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
  /* 4.1. Make sure that the STOP bit is cleared by Hardware */
  Timeout = 500;//0xffff;
  while ((I2C1->CR1 & I2C_STOP_MASK) == I2C_STOP_MASK)
  {
    if (Timeout-- == 0)
    { return ERROR; }   
  }
  return SUCCESS;
}

/**  
  * @brief      Получить данные из SLAVE устройства шины I2C 
  * @param      adr - адрес устройства на шине I2C
  *             *buf - указатель на буфер для копирования
  *             len - количество байт данных
  * @retval     Код состояния приёма данных (см. ErrorStatus)
  */
ErrorStatus receiveData_I2C (uchar adr, uchar *buf, uchar len)
{
  vu32 Timeout = 0;     // vu = volatile uint
  vu32 Temp;
  
  /* Enable Error IT */
  I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);

  //------------------------ Need to receive 1 Byte ----------------------------
  if (len == 1)
  {
    /* 1. Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* 1.1. Wait until SB flag is set: EV5  */
    Timeout = 500;//0xffff;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) {
      if (Timeout-- == 0)
      { return ERROR; }
    }
  
    /* 2. Send slave address */
    I2C_Send7bitAddress(I2C1, adr, I2C_Direction_Receiver);
    /* 2.2. Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
    and program the STOP just after ADDR is cleared. The EV6_3 
    software sequence must complete before the current byte end of transfer.*/
    Timeout = 500;//0xffff;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR) {
      if (Timeout-- == 0)
      { return ERROR; }
    }
    /* 4. Clear ADDR flag, ACK bit and program the STOP */
    /* 4.1. Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
    software sequence must complete before the current byte end of transfer */
    disableInterrupts();
    /* 4.2. Clear ADDR flag */
    Temp = I2C1->SR2;
    /* 4.3. Clear ACK bit - disable the acknowledgement */
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    /* 4.4. Program the STOP */
    I2C_GenerateSTOP(I2C1, ENABLE);
    /* 4.5. Re-enable IRQs */
    enableInterrupts();
    
    /* 5. Read the data */
    /* 5.1. Wait until a data is received in DR register (RXNE = 1) EV7 */
    Timeout = 500;//0xffff;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR) {
      if (Timeout-- == 0)
      { return ERROR; }
    }
    /* 5.2. Receive the data */
    *buf = I2C_ReceiveData(I2C1);
    /* 5.3. Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    Timeout = 500;//0xffff;
    while ((I2C1->CR1 & I2C_STOP_MASK) == I2C_STOP_MASK) {
      if (Timeout-- == 0)
      { return ERROR; }
    }
    
    /* 6. Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);
  }
  //------------------ Need to receive 2 Bytes ---------------------------------
  else if (len == 2)
  {
    /* 1. Set POS bit */
    I2C1->CR1 |= I2C_POS_SET;
    
    /* 2. Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* 2.1. Wait until SB flag is set: EV5  */
    Timeout = 500;//0xffff;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) {
      if (Timeout-- == 0)
      { return ERROR; }
    }

    /* 3. Send slave address */
    I2C_Send7bitAddress(I2C1, adr, I2C_Direction_Receiver);
    /* 3.1. Wait until ADDR is set: EV6 */
    Timeout = 500;//0xffff;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR) {
      if (Timeout-- == 0)
      { return ERROR; }
    }
    
    /* 4. EV6_1: The acknowledge disable should be done just after EV6,
    that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
    ACK clearing */
    disableInterrupts();
    /* 4.1. Clear ADDR flag */
    Temp = I2C1->SR2;
    /* 4.2. Clear ACK bit - disable the acknowledgement */
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    /* 4.3. Re-enable IRQs */
    enableInterrupts();
    
    /* 5. Wait until BTF is set */
    Timeout = 500;//0xffff;
    while (I2C_GetITStatus(I2C1, I2C_IT_BTF) != SET) {
      if (Timeout-- == 0)
      { return ERROR; }
    }
    
    /* 6. Receive the 1st Byte */
    /* 6.1. Disable IRQs around STOP programming and data reading because of the limitation ?*/
    __disable_irq();
    /* 6.2. Program the STOP */
    I2C_GenerateSTOP(I2C1, ENABLE);
    /* 6.3. Read first byte */
    *buf = I2C_ReceiveData(I2C1);
    /* 6.4. Re-enable IRQs */
    __enable_irq();
    /* 6.5. Increment pointer */
    buf++;
    
    /* 7. Receive the 2nd Byte */
    *buf = I2C_ReceiveData(I2C1);
    /* 7.1. Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    Timeout = 500;//0xffff;
    while ((I2C1->CR1 & I2C_STOP_MASK) == I2C_STOP_MASK) {
      if (Timeout-- == 0)
      { return ERROR; }
    }
    
    /* 8. Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    /* 9. Clear POS bit */
    I2C1->CR1  &= I2C_POS_RESET;
  }
  //------------------ Need to receive more than 2 Bytes ----------------------
  else
  {
    /* 1. Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* 1.1. Wait until SB flag is set: EV5  */
    Timeout = 500;//0xffff;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) {
      if (Timeout-- == 0)
      { return ERROR; }
    }

    /* 2. Send slave address */
    I2C_Send7bitAddress(I2C1, adr, I2C_Direction_Receiver);
    /* 2.1. Wait until ADDR is set: EV6 */
    Timeout = 500;//0xffff;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR) {
      if (Timeout-- == 0)
      { return ERROR; }
    }
    
    /* 3. While there is data to be read */
    while (len)
    {
      /* 3.1. Receive bytes from first byte until byte N-3 */
      if (len != 3)
      {
        /* 3.1.1. Poll on BTF to receive data because in polling mode we can not guarantee the
        EV7 software sequence is managed before the current byte transfer completes */
        Timeout = 500;//0xffff;
        while (I2C_GetITStatus(I2C1, I2C_IT_BTF) != SET) {
          if (Timeout-- == 0)
          { return ERROR; }
        }
        /* 3.1.2. Read data */
        *buf = I2C_ReceiveData(I2C1);
        /* 3.1.3. Increment pointer */
        buf++;
        /* 3.1.4. Decrement the read bytes counter */
        len--;
      }
      /* 3.2. It remains to read three data: data N-2, data N-1, Data N */
      if (len == 3)
      {
        /* 3.2.1. Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
        Timeout = 500;//0xffff;
        while (I2C_GetITStatus(I2C1, I2C_IT_BTF) != SET) {
          if (Timeout-- == 0)
          { return ERROR; }
        }
        /* 3.2.2. Clear ACK */
        I2C_AcknowledgeConfig(I2C1, DISABLE);
        /* 3.2.3. Disable IRQs around data reading and STOP programming because of the
        limitation ? */
        __disable_irq();
        /* 3.2.4. Read Data N-2 */
        *buf = I2C_ReceiveData(I2C1);
        /* 3.2.5. Increment pointer */
        buf++;
        /* 3.2.6. Program the STOP */
        I2C_GenerateSTOP(I2C1, ENABLE);
        /* 3.2.7. Read DataN-1 */
        *buf = I2C_ReceiveData(I2C1);
        /* 3.2.8. Re-enable IRQs */
        __enable_irq();
        /* 3.2.9. Increment pointert */
        buf++;
        /* 3.2.10. Wait until RXNE is set (DR contains the last data) */
        Timeout = 500;//0xffff;
        while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR) {
          if (Timeout-- == 0)
          { return ERROR; }
        }
        /* 3.2.11. Read DataN */
        *buf = I2C_ReceiveData(I2C1);
        /* 3.2.12. Reset the number of bytes to be read by master */
        len = 0;
      }
      
      /* 4. Make sure that the STOP bit is cleared by Hardware before CR1 write access */
      Timeout = 500;//0xffff;
      while ((I2C1->CR1 & I2C_STOP_MASK) == I2C_STOP_MASK) {
        if (Timeout-- == 0)
        { return ERROR; }
      }
      /* 5. Enable Acknowledgement to be ready for another reception */
      I2C_AcknowledgeConfig(I2C1, ENABLE);
    }
  }
  return SUCCESS;
}


