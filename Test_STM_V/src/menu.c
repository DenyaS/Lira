 
/* Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "includes.h"

/* Defines ------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
MenuDisplayState_TypeDef NowDisplayed;
char str1[16];
char str2[16];
MenuExecState_TypeDef MenuExecPointer = ME_Idle;

/* Extern variables ----------------------------------------------------------*/

/* Constants for menu display ------------------------------------------------*/
const char _mainm0[] = "Меню    [вход >]";
const char _mainm1[] = "СТАРТ";
const char _mainm2[] = "ТЕСТ";
const char _mainm3[] = "ПАРАМЕТРЫ";

const char _startm0[] = "Старт   [пуск >]";
const char _startm1[] = "АВТОМАТИЧ. РЕЖИМ";
const char _startm2[] = "РУЧНОЙ РЕЖИМ";

const char _testm0[] = "Тест    [вкл. >]";
const char _testm1[] = "ВХОДЫ";
const char _testm2[] = "ВЫХОДЫ";		

char flash _setm0[] = "Парам.  [изм. >]";
char flash _setm1[] = "ГЛАВНЫЙ МОТОР";
char flash _setm2[] = "МОТОР ПОДМОТКИ";
char flash _setm3[] = "ПРОТЯЖКА";
char flash _setm4[] = "КОНТРАСТНОСТЬ";

const char _autom0[] = "Автоматич. режим";
const char _autom1[] = "СТОП [<]";

const char _manualm0[] = "Ручной режим";
const char _manualm1[] = "СТОП [<]";

const char _inputsm0[] = "Входы";
const char _inputsm1[] = "0 0 0 0 0 0 0 0";

const char _outputsm0[] = "Выходы";
const char _outputsm1[] = "0 0 0 0 0 0 0 0";

const char _mainmotorm0[] = "Гл.Мот. [изм. >]";
const char _mainmotorm1[] = "НАПРАВЛЕНИЕ";
const char _mainmotorm2[] = "ТЕМП ПРОТЯЖКИ";
const char _mainmotorm3[] = "ТЕМП НАТЯЖЕНИЯ";
const char _mainmotorm4[] = "ВРЕМЯ УДЕРЖАНИЯ";

const char _rollmotorm0[] = "Мот.П.  [изм. >]";
const char _rollmotorm1[] = "ШАГОВ ПОДМ. ОСН.";
const char _rollmotorm2[] = "ШАГОВ ПОДМ. МАЛ.";
const char _rollmotorm3[] = "НАПРАВЛЕНИЕ";
const char _rollmotorm4[] = "ТЕМП ВРАЩЕНИЯ";
const char _rollmotorm5[] = "ВРЕМЯ УДЕРЖАНИЯ";

const char _pullm0[] = "Протяжка[изм. >]";
const char _pullm1[] = "ШАГОВ ДЛИННАЯ";
const char _pullm2[] = "ШАГОВ КОРОТКАЯ";
const char _pullm3[] = "КОЛ-ВО КОРОТКИХ";
const char _pullm4[] = "ШАГОВ НАТЯЖЕНИЯ";

const char _mmdirm0[] = "Направл-е вращ.";
const char _mmtempm0[] = "Темп протяжки";
const char _mmtempotm0[] = "Темп натяжения";
const char _mmholdm0[] = "Время удержания";

const char _rmmainstepsm0[] = "Шагов подм. осн.";
const char _rmsidestepsm0[] = "Шагов подм. мал.";
const char _rmdirm0[] = "Направл-е вращ.";
const char _rmtempm0[] = "Темп вращения";
const char _rmholdm0[] = "Время удержания";

const char _pmainstepsm0[] = "Длинная (шагов)";
const char _psidestepsm0[] = "Короткая (шагов)";
const char _pnumberm0[] = "Кол-во коротких";
const char _prewstepsm0[] = "Шагов натяжения";

const char str_blank[]  = "                ";

/** 
  * @brief      Opredelit' punkt Menu
  * @param      None
  * @retval     None
  */
void retMenuPos(void)
{
  switch(LastKey) {
    case UP:
      MenuPos--;
      MenuDirection = 0x00;
      break;

    case DOWN: 
      MenuPos++;
      MenuDirection = 0x01;
      break;

    case ENTER:
      MenuPos *= 10;
      MenuDirection = 0x02;
      break;

    case ESCAPE:
      MenuPos /= 10;
      MenuDirection = 0x03;
      /*
      // Sbrasyvaem piny motorov
      start_MainMotor(DISABLE);
      start_RollMotor(DISABLE);
      */
      if (MenuPos == 0) {
        fl_EnterMenu = 0;
        PreviosDisplayed = Display_Idle;    // otobrajaem na displee to, chto bylo do vhoda v Menu
        CurrentlyDisplayed = Display_Hint;
      }
      break;

    default:
      break;
  }
  if(LastKey) LastKey = 0;
}

/** 
  * @brief      Otobrazit' Menu na displee
  * @param      Pos - punkt Menu
  * @retval     None
  */
void displayMenu(MenuDisplayState_TypeDef Pos)
{
  NowDisplayed = Pos;
  
  switch(Pos) {
    // Главное меню
    case MD_1:
      sprintf(str1, _mainm0);
      sprintf(str2, _mainm1);
      break;
        // Подменю первого пункта главного меню
        case MD_1_1:
          sprintf(str1, _startm0);
          sprintf(str2, _startm1);
          MenuExecPointer = ME_StopAuto;
          break;
            case MD_1_1_1:
              sprintf(str1, _autom0);
              sprintf(str2, _autom1);
              MenuExecPointer = ME_Auto;
              break;
        case MD_1_2:
          sprintf(str1, _startm0);
          sprintf(str2, _startm2);
          MenuExecPointer = ME_StopManual;
          break;
            case MD_1_2_1:
              sprintf(str1, _manualm0);
              sprintf(str2, _manualm1);
              MenuExecPointer = ME_Manual;
              break;

    case MD_2:
      sprintf(str1, _mainm0);
      sprintf(str2, _mainm2);
      break;
        // Подменю vtorogo пункта главного меню
        case MD_2_1:
          sprintf(str1, _testm0);
          sprintf(str2, _testm1);
          MenuExecPointer = ME_Idle;
          break;
            case MD_2_1_1:
              sprintf(str1, _inputsm0);
              sprintf(str2, _inputsm1);
              MenuExecPointer = ME_Inputs;
              break;
        case MD_2_2:
          sprintf(str1, _testm0);
          sprintf(str2, _testm2);
          MenuExecPointer = ME_Idle;
          break;
            case MD_2_2_1:
              //sprintf(str1, _outputsm0);
              //sprintf(str2, _outputsm1);
              MenuPos /= 10;        // Vyhodim nazad
              MenuExecPointer = ME_Outputs;
              break;

    case MD_3:
      sprintf(str1, _mainm0);
      sprintf(str2, _mainm3);
      break;
        // Подменю tret'ego пункта главного меню
        // Main Motor
        case MD_3_1:
         sprintf(str1, _setm0);
         sprintf(str2, _setm1);
         break;
            // Main Motor Direction DISPLAY
            case MD_3_1_1:
              sprintf(str1, _mainmotorm0);
              sprintf(str2, _mainmotorm1);
              break;
                // Main Motor Direction SET
                case MD_3_1_1_1:
                  sprintf(str1, _mmdirm0);
                  MenuExecPointer = ME_MMDirection;
                  break;
            // Main Motor TempProtyajki DISPLAY
            case MD_3_1_2:
              sprintf(str1, _mainmotorm0);
              sprintf(str2, _mainmotorm2);
              break;
                // Main Motor TempProtyajki SET
                case MD_3_1_2_1:
                  sprintf(str1, _mmtempm0);
                  MenuExecPointer = ME_MMTemp;
                  break;
            // Main Motor TempOtmotki DISPLAY
            case MD_3_1_3:
              sprintf(str1, _mainmotorm0);
              sprintf(str2, _mainmotorm3);
              break;
                // Main Motor TempOtmotki SET
                case MD_3_1_3_1:
                  sprintf(str1, _mmtempotm0);
                  MenuExecPointer = ME_MMTempOtm;
                  break;
            // Main Motor VremyaUderjaniya DISPLAY
            case MD_3_1_4:
              sprintf(str1, _mainmotorm0);
              sprintf(str2, _mainmotorm4);
              break;
                // Main Motor VremyaUderjaniya SET
                case MD_3_1_4_1:
                  sprintf(str1, _mmholdm0);
                  MenuExecPointer = ME_MMTimeOfHold;
                  break;
        // Roll Motor
        case MD_3_2:
         sprintf(str1, _setm0);
         sprintf(str2, _setm2);
         break;
            // Roll Motor ShagovPodmotkiOsn DISPLAY
            case MD_3_2_1:
              sprintf(str1, _rollmotorm0);
              sprintf(str2, _rollmotorm1);
              break;
                // Roll Motor ShagovPodmotkiOsn SET
                case MD_3_2_1_1:
                  sprintf(str1, _rmmainstepsm0);
                  MenuExecPointer = ME_RMMainSteps;
                  break;
            // Roll Motor ShagovPodmotkiMal DISPLAY
            case MD_3_2_2:
              sprintf(str1, _rollmotorm0);
              sprintf(str2, _rollmotorm2);
              break;
                // Roll Motor ShagovPodmotkiMal SET
                case MD_3_2_2_1:
                  sprintf(str1, _rmsidestepsm0);
                  MenuExecPointer = ME_RMSideSteps;
                  break;
            // Roll Motor Napravlenie DISPLAY
            case MD_3_2_3:
              sprintf(str1, _rollmotorm0);
              sprintf(str2, _rollmotorm3);
              break;
                // Roll Motor Napravlenie SET
                case MD_3_2_3_1:
                  sprintf(str1, _rmdirm0);
                  MenuExecPointer = ME_RMDirection;
                  break;
            // Roll Motor TempVrasheniya DISPLAY
            case MD_3_2_4:
              sprintf(str1, _rollmotorm0);
              sprintf(str2, _rollmotorm4);
              break;
                // Roll Motor TempVrasheniya SET
                case MD_3_2_4_1:
                  sprintf(str1, _rmtempm0);
                  MenuExecPointer = ME_RMTemp;
                  break;
            // Roll Motor VremyaUderjaniya DISPLAY
            case MD_3_2_5:
              sprintf(str1, _rollmotorm0);
              sprintf(str2, _rollmotorm5);
              break;
                // Roll Motor VremyaUderjaniya SET
                case MD_3_2_5_1:
                  sprintf(str1, _rmholdm0);
                  MenuExecPointer = ME_RMHold;
                  break;
        // Protyajka
        case MD_3_3:
         sprintf(str1, _setm0);
         sprintf(str2, _setm3);
         break;
            // Protyajka ShagovOsn DISPLAY
            case MD_3_3_1:
              sprintf(str1, _pullm0);
              sprintf(str2, _pullm1);
              break;
                // Protyajka ShagovOsn SET
                case MD_3_3_1_1:
                  sprintf(str1, _pmainstepsm0);
                  MenuExecPointer = ME_PullMainSteps;
                  break;
            // Protyajka ShagovMal DISPLAY
            case MD_3_3_2:
              sprintf(str1, _pullm0);
              sprintf(str2, _pullm2);
              break;
                // Protyajka ShagovMal SET
                case MD_3_3_2_1:
                  sprintf(str1, _psidestepsm0);
                  MenuExecPointer = ME_PullSideSteps;
                  break;
            // Protyajka Koli4estvoKorotkihProtyajek DISPLAY
            case MD_3_3_3:
              sprintf(str1, _pullm0);
              sprintf(str2, _pullm3);
              break;
                // Protyajka ShagovMal SET
                case MD_3_3_3_1:
                  sprintf(str1, _pnumberm0);
                  MenuExecPointer = ME_PullNumber;
                  break;
            // Protyajka ShagovOtmotki DISPLAY
            case MD_3_3_4:
              sprintf(str1, _pullm0);
              sprintf(str2, _pullm4);
              break;
                // Protyajka ShagovOtmotki SET
                case MD_3_3_4_1:
                  sprintf(str1, _prewstepsm0);
                  MenuExecPointer = ME_RewSteps;
                  break;
        // Contrast
        case MD_3_4:
         sprintf(str1, _setm0);
         sprintf(str2, _setm4);
         break;
    
        default:
          // если не попал ни в один пункт меню, тогда проверяю flag на последнюю операцию 
          // и делаю противоположную                        
          if(MenuDirection == 0x00)       // последняя операция UP
            MenuPos++;
          else if(MenuDirection == 0x01) // последняя операция DOWN
            MenuPos--;
          else if(MenuDirection == 0x02) // последняя операция ENTER
            MenuPos /= 10;
          break;
  }
  
  if (fl_EnDisp) {  
  // Отображаем на дисплее строки один раз
    if (BeforeDisplayed != NowDisplayed) {
      displayString(1, 1, str1);
      displayString(2, 1, str2);
      BeforeDisplayed = NowDisplayed;
    }
  }
}

/** 
  * @brief      Vypolnit' punkt Menu
  * @param      Pos - punkt Menu
  * @retval     None
  */
void execMenu(void)
{
  switch(MenuExecPointer) {
    
    case ME_Idle:
      break;
      
    case ME_Auto:
      break;
      
    case ME_StopAuto:
      break;
      
    case ME_Manual:
      break;
      
    case ME_StopManual:
      break;
      
    case ME_Inputs:
      //showInputs();
      break;
      
    case ME_Outputs:
      break;
      
    case ME_MMParams:
      break;
      
    case ME_MMDirection:
      //setMMDirection();
      MenuExecPointer = ME_Idle;
      break;
      
    case ME_MMTemp:
      //setMMTemp();
      MenuExecPointer = ME_Idle;
      break;

    case ME_MMTempOtm:
      //setMMTempOtm();
      MenuExecPointer = ME_Idle;
      break;
      
    case ME_MMTimeOfHold:
      //setMMTimeOfHold();
      MenuExecPointer = ME_Idle;
      break;

    case ME_RMParams:
      break;
      
    case ME_RMMainSteps:
      //setRMMainSteps();
      MenuExecPointer = ME_Idle;
      break;

    case ME_RMSideSteps:
      //setRMSideSteps();
      MenuExecPointer = ME_Idle;
      break;
      
    case ME_RMDirection:
      //setRMDirection();
      MenuExecPointer = ME_Idle;
      break;

    case ME_RMTemp:
      //setRMTemp();
      MenuExecPointer = ME_Idle;
      break;
      
    case ME_RMHold:
      //setRMTimeOfHold();
      MenuExecPointer = ME_Idle;
      break;
      
    case ME_PullParams:
      break;
      
    case ME_PullMainSteps:
      //setPullMainSteps();
      MenuExecPointer = ME_Idle;
      break;
      
    case ME_PullSideSteps:
      //setPullSideSteps();
      MenuExecPointer = ME_Idle;
      break;
      
    case ME_PullNumber:
      //setPullNumber();
      MenuExecPointer = ME_Idle;
      break;
      
    case ME_RewSteps:
      //setRewSteps();
      MenuExecPointer = ME_Idle;
      break;

    case ME_Contrast:
      break;
      
    default:
      break;
  }
}


/** [duke] 
  * @brief      Всегда возвращает "1"
  * @param      None
  * @retval     1
  */
int noFunc(void) 
{
  return 1;
}

