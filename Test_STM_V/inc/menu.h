
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_H
#define __MENU_H

/* Includes ------------------------------------------------------------------*/
//#include <ina90.h>
#include <string.h>
//#include <pgmspace.h>
//#include <comp_a90.h>
#include <stdlib.h>
#include <stdio.h>
#include "unsigned.h"

/* Defines ------------------------------------------------------------------*/
#define MAIN_MENU_SIZE          3       // размерность Главного меню       
#define START_MENU_SIZE         2
#define TEST_MENU_SIZE          2
#define SET_MENU_SIZE           4
#define AUTO_MENU_SIZE          1
#define MANUAL_MENU_SIZE        1
#define INPUTS_MENU_SIZE        1
#define OUTPUTS_MENU_SIZE       1

#define BEGIN           0
#define PREVI           1

#define VOIDFUNC        0x10
#define EXEFUNC         0x11

#define GOBACK          0x00
#define STAYHERE        0x01
#define MOVEON          0x02

#define flash   const

/* Type definitions --------------------------------------------------------*/
typedef struct _menu {
	const char *menustr;    // Строка меню
	int (*func)(void);	    // Указатель на функцию
} _MENU;

typedef enum 
{
    MD_Idle,
    MD_1        = 1,
    MD_2,
    MD_3,
    MD_4,
    MD_5,
    MD_6,
    MD_7,
    MD_8,
    MD_9,
    MD_1_1      = 10,
    MD_1_2,
    MD_1_3,
    MD_1_4,
    MD_1_5,
    MD_1_6,
    MD_1_7,
    MD_1_8,
    MD_1_9,
    MD_1_10,
    MD_2_1      = 20,
    MD_2_2,
    MD_3_1      = 30,
    MD_3_2,
    MD_3_3,
    MD_3_4,
    MD_1_1_1    = 100,
    MD_1_2_1    = 110,
    MD_2_1_1    = 200,
    MD_2_2_1    = 210,
    MD_2_2_2,
    MD_2_2_3,
    MD_3_1_1    = 300,
    MD_3_1_2,
    MD_3_1_3,
    MD_3_1_4,
    MD_3_1_5,
    MD_3_1_6,
    MD_3_1_7,
    MD_3_1_8,
    MD_3_2_1    = 310,
    MD_3_2_2,
    MD_3_2_3,
    MD_3_2_4,
    MD_3_2_5,
    MD_3_2_6,
    MD_3_2_7,
    MD_3_2_8,
    MD_3_3_1    = 320,
    MD_3_3_2,
    MD_3_3_3,
    MD_3_3_4,
    MD_3_3_5,
    MD_3_3_6,
    MD_3_3_7,
    MD_3_3_8,
    MD_3_1_1_1  = 3000,
    MD_3_1_1_2,
    MD_3_1_2_1  = 3010,
    MD_3_1_2_2,
    MD_3_1_3_1  = 3020,
    MD_3_1_3_2,
    MD_3_1_4_1  = 3030,
    MD_3_1_4_2,
    MD_3_2_1_1  = 3100,
    MD_3_2_1_2,
    MD_3_2_2_1  = 3110,
    MD_3_2_2_2,
    MD_3_2_3_1  = 3120,
    MD_3_2_3_2,
    MD_3_2_4_1  = 3130,
    MD_3_2_4_2,
    MD_3_2_5_1  = 3140,
    MD_3_2_5_2,
    MD_3_3_1_1  = 3200,
    MD_3_3_1_2,
    MD_3_3_2_1  = 3210,
    MD_3_3_2_2,
    MD_3_3_3_1  = 3220,
    MD_3_3_3_2,
    MD_3_3_4_1  = 3230,
    MD_3_3_4_2,
    MD_DontUse  = 0xffffffff        // Чтобы заставить компилятор использовать
                                    // тип больше по размеру, чем он автоматически 
                                    // использовал бы, определите в enum константу 
                                    // больше по размеру, чем это необходимо 
} MenuDisplayState_TypeDef;

typedef enum 
{
    ME_Idle,
    ME_Auto,
    ME_StopAuto,
    ME_Manual,
    ME_StopManual,
    ME_Inputs,
    ME_Outputs,
    ME_MMParams,
    ME_MMDirection,
    ME_MMTemp,
    ME_MMTempOtm,
    ME_MMTimeOfHold,
    ME_RMParams,
    ME_RMMainSteps,
    ME_RMSideSteps,
    ME_RMDirection,
    ME_RMTemp,
    ME_RMHold,
    ME_PullParams,
    ME_PullMainSteps,
    ME_PullSideSteps,
    ME_PullNumber,
    ME_RewSteps,
    ME_Contrast
} MenuExecState_TypeDef;

/* Exported functions ------------------------------------------------------- */
int noFunc(void); 
void retMenuPos(void); 
void displayMenu(MenuDisplayState_TypeDef Pos); 
void execMenu(void); 

#endif /* __MENU_H */

