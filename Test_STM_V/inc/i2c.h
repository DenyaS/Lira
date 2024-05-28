
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __I2C_H
#define __I2C_H

/* Includes ----------------------------------------------------------------- */
#include "stm32F10x.h" 
#include "unsigned.h"

/* Define ------------------------------------------------------------------- */
#define SPEED_5KHZ      5000         
#define SPEED_100KHZ    100000         
#define SPEED_400KHZ    400000  
#define OWN_I2C_ADR     0x12


/* Exported functions ------------------------------------------------------- */
void init_I2C(void);
ErrorStatus sendData_I2C(uchar adr, uchar *buf, uchar n);
ErrorStatus receiveData_I2C(uchar adr, uchar *buf, uchar n);

#endif /* __I2C_H */

