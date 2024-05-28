
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLAGS_H
#define __FLAGS_H

#include "unsigned.h"

typedef struct _tflags 
{
	uint  _bit0:1;
	uint  _bit1:1;
	uint  _bit2:1;
	uint  _bit3:1;
	uint  _bit4:1;
	uint  _bit5:1;
	uint  _bit6:1;
	uint  _bit7:1;

	uint  _bit8:1;
	uint  _bit9:1;
	uint  _bit10:1;
	uint  _bit11:1;
	uint  _bit12:1;
	uint  _bit13:1;
	uint  _bit14:1;
	uint  _bit15:1;

	uint  _bit16:1;
	uint  _bit17:1;
	uint  _bit18:1;
	uint  _bit19:1;
	uint  _bit20:1;
	uint  _bit21:1;
	uint  _bit22:1;
	uint  _bit23:1;

	uint  _bit24:1;
	uint  _bit25:1;
	uint  _bit26:1;
	uint  _bit27:1;
	uint  _bit28:1;
	uint  _bit29:1;
	uint  _bit30:1;
	uint  _bit31:1;
} _TFLAGS;

typedef union 
{
	_TFLAGS		Fl;
	ulong		Flw;
} _FLAGS;

// Flags
#define fl_HandleRF         Flags.Fl._bit0   // ���� ��������� �����������
#define fl_packRF           Flags.Fl._bit1   // ���� ���������� ��������� ��������� ������ �� �����������
#define fl_EntrOnce         Flags.Fl._bit2   // ���� ������� ���������� ����� (��������, ������ ��������� �� �������)
#define fl_EnDisp   		Flags.Fl._bit3   // ���� ���������� ������ ������� �� ������� 
#define fl_KeyOn  		    Flags.Fl._bit4   // ���� ������� ������� �������   
#define fl_GoExit  		    Flags.Fl._bit5   // ���� ���������� ������ � ���������� ����� ���� 
#define fl_ByteStuf	        Flags.Fl._bit6   // ���� ��������� ������������� 
#define fl_KeyFixed         Flags.Fl._bit7   // ���� uderjaniya klavishi
#define fl_PPDPingConfirm   Flags.Fl._bit8   // ���� ������ �� ������� ����������� ���������      
#define fl_DecryptRxData    Flags.Fl._bit9   // ���� ������������ ������� ������      
#define fl_EnterMenu        Flags.Fl._bit10  // ���� vhoda v Menu
#define fl_StartPacket      Flags.Fl._bit11  // ���� ������ ������ 
#define fl_AckSMS	        Flags.Fl._bit12  // SMS �������� 
#define fl_SendingSMS   	Flags.Fl._bit13  // ���� ���������� � �������� SMS
#define fl_SendToServer     Flags.Fl._bit14  // ���������� ������ �� ������ 
#define fl_SendSynchroRF    Flags.Fl._bit15  // �������� ����� ������������� � ���������� 
#define fl_ErrPPD           Flags.Fl._bit16  // ������� ����� � ������� �� PPD   
#define fl_NoChangeChannel	Flags.Fl._bit17  // �� ����������� ����������  
#define fl_AckAwake         Flags.Fl._bit18  // ������������� ������ ������� �� ������ ����������������  
#define fl_EmptyBus         Flags.Fl._bit19  // ��� ���������� � ��������     
#define fl_ShowSeats        Flags.Fl._bit20  // ���� �������� � ���, ��� ��������� �������� ���������� � ��������� ���� ������� 
#define fl_OpenFrontDoor    Flags.Fl._bit21  // ���� ������ ������ "������� �������� �����" 
#define fl_OpenRearDoor     Flags.Fl._bit22  // ���� ������ ������ "������� ������ �����"
#define fl_EventOccured     Flags.Fl._bit23  // ���� "������� ���������" (����� ��������� � ������� ������)
#define fl_ZeroID	        Flags.Fl._bit24  // ���� ��������, ��� ID ������� ����� 0   
#define fl_WDreset          Flags.Fl._bit25  // �������� WatchDog Timer  
#define fl_AnsCmdPPD        Flags.Fl._bit26  // ������� ����� �� ������� ��� PPD   
#define fl_ShowTime  		Flags.Fl._bit27  // ���������� ����� �� ������� ��� ��� 
#define fl_AnsCmdGPS        Flags.Fl._bit28  // ������� ����� �� ������� ��� GPS  
#define fl_ShowInfoGPS      Flags.Fl._bit29  // ���������� ���������� �� ��������� GPS 
#define fl_OpenCar	        Flags.Fl._bit30  // ������� ���� �� ������ ���������� 
#define fl_32  		        Flags.Fl._bit31   

#endif /* __FUNCTIONS_H */


