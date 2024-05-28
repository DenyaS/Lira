

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UNSIGNED_H
#define __UNSIGNED_H

#define uchar   __UINT8_T_TYPE__        //unsigned char     // 1 Byte => uint8_t
#define uint    __UINT16_T_TYPE__       //unsigned int      // 2 Byte => uint16_t
#define ushort  unsigned short                              // 2 Byte (?)
#define ulong   __UINT32_T_TYPE__       //unsigned long     // 4 Byte => uint32_t
#define ullint  unsigned long long int                      // 8 Byte (?)

#ifndef NULL
#define NULL _NULL
#endif

/*
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  // Read Only 
typedef const int16_t sc16;  // Read Only 
typedef const int8_t sc8;    // Read Only 

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  // Read Only 
typedef __I int16_t vsc16;  // Read Only 
typedef __I int8_t vsc8;   // Read Only 

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  // Read Only
typedef const uint16_t uc16;  // Read Only
typedef const uint8_t uc8;    // Read Only

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  // Read Only
typedef __I uint16_t vuc16;  // Read Only
typedef __I uint8_t vuc8;    // Read Only
*/

#endif /* __UNSIGNED_H */


