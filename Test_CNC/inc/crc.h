
#ifndef __CRC_H
#define __CRC_H

#include "unsigned.h"

#define UPDATECRC32(c,crc) (crc32tab[((uint)(crc) ^ (c)) & 0xFF] ^ (((crc) >> 8) & 0x00FFFFFFL))
#define UPDATECRC8(c,crc) (crc8tab[crc ^ c])

uint UPDATECRC16(uchar bt,uint crc);   
uint MakeCRC16(uchar *uk, uint size);   
ulong MakeCRC32(uchar *uk, uint size);
uint MakeCRC_RF(uchar *uk, uint size);   

#endif /* __EXTERNS_H */

