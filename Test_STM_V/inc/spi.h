
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __SPI_H
#define __SPI_H

/* Includes ----------------------------------------------------------------- */
#include "stm32F10x.h" 
#include "unsigned.h"

/* Exported functions ------------------------------------------------------- */
void init_SPI(void);
uchar send2SPI(uchar data);

#endif /* __SPI_H */

