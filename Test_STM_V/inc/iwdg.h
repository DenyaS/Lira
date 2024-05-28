
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __IWDG_H
#define __IWDG_H

/* Includes ----------------------------------------------------------------- */
/* Defines------------------------------------------------------------------- */
/** Independent Watchdog Configuration
@ IWDG period [us] <125-32000000:125>
@ Set the timer period for Independent Watchdog.
@ Default: 1000000  (1s) */
#define __IWDG_SETUP              1
#define __IWDG_PERIOD             0x001E8480    // 2000000 us = 2 s

#define _WDR()   IWDG->KR = 0xAAAA

/** Define  IWDG PR and RLR settings */
#if   (__IWDG_PERIOD >  16384000UL)
  #define __IWDG_PR             (6)
  #define __IWDGCLOCK (32000UL/256)
#elif (__IWDG_PERIOD >   8192000UL)
  #define __IWDG_PR             (5)
  #define __IWDGCLOCK (32000UL/128)
#elif (__IWDG_PERIOD >   4096000UL)
  #define __IWDG_PR             (4)
  #define __IWDGCLOCK  (32000UL/64)
#elif (__IWDG_PERIOD >   2048000UL)
  #define __IWDG_PR             (3)
  #define __IWDGCLOCK  (32000UL/32)
#elif (__IWDG_PERIOD >   1024000UL)
  #define __IWDG_PR             (2)
  #define __IWDGCLOCK  (32000UL/16)
#elif (__IWDG_PERIOD >    512000UL)
  #define __IWDG_PR             (1)
  #define __IWDGCLOCK   (32000UL/8)
#else
  #define __IWDG_PR             (0)
  #define __IWDGCLOCK   (32000UL/4)
#endif
#define __IWGDCLK  (32000UL/(0x04<<__IWDG_PR))
#define __IWDG_RLR (__IWDG_PERIOD*__IWGDCLK/1000000UL-1)


/* Type definitions ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#if __IWDG_SETUP
void setup_IWDG(void);
#endif /* __IWDG_SETUP */

#endif /* __IWDG_H */

