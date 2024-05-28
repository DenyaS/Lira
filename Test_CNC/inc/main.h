#define forwardSM       1
#define backwardSM      0
#define forwardA       0
#define backwardA      1
//Состояние автомата модуля подачи
#define InitSM          0      //исходное состояние ожидание задания
#define Wait_strip      1      //ожидание полосы по входу 
#define Sup_roller      2      //подача полосы до мерного ролика
#define Main_pull       3      //основная подача полосы
#define ErrorSM         4      // состояние ошибки
#define CompleteSM      5      //Задание выполнено
//---------------------------Модуль подачи SM------------------------------------------------
//Сообщения модулю подачи Лира MessegSM
#define NoNewTaskSM       0       //Нет нового задания
#define NewTaskSM         1       //Получено новое задание
#define ReInitSM          2       //Переинициализация
//Сообщение об ошибках модуля подачи CodeErrorSM
#define bahd_exit                0       //Полоса не забрана на выходе модуля 
#define NoStrip                  1       //Полосы нет по входу в течении таймера Timer_inputSM
#define not_tightened_strip      2      //Не затянули полосу под ролик - превышено кол-во ошибок а полосы нет
#define ErrRotSM                 3      //Превышен порог ошибок при отмерке полосы
#define not_tightened_strip_timer 4     //Не затянули полосу под ролик - превышен таймер 
//--------------------------Модуль обрезки CM--------------------------------------------------
//Сообщения модулю обрезки Лира MessegCM
#define NoNewTaskCM              0         //Нет нового задания
#define NewTaskCM                1         //Получено новое задание
#define ResetCM                  2         //Приведение в исходное
#define ReInitCM                 3         //Переинициализация модуля 
//Состояние автомата модуля обрезки
#define InitCM                   0         //исходное состояние ожидание задания
#define CuttingSM                1         //Состояние обрезки полосы
#define CompleteCM               2         //Обрезка выполнена
#define ErrorCM                  3         // состояние ошибки
//Сообщение об ошибках модуля подачи CodeErrorCM
#define not_initCM                0       //Обрезка не в исходном состоянии
#define Exceeded_TimeCM           1       //Превышено время обрезки
// ---------------------------Ось X--------------------------------------
#define m_rightX                  0       //Движение вправо
#define m_leftX                   1       //Движение влево
//Сообщения автомату оси X
#define NoNewTaskX                0       //Нет нового задания
#define motion_rightX             1       //Движение вправо X
#define motion_leftX            2       //Движение влево X
#define ReInitX                   3         //Переинициализация модуля 
//Состояние автомата оси X
#define InitX                     0       //Исходное состояние и приполучении задания
#define MotionX                   1       //Перемещение по оси Х
#define CompleteX                 2       //Задание выполнено
#define ErrorX                    3       //Состояние ошибки
//Сообщения об ошибках автомата оси X
#define reach_limitX              0       //Достигнут LimitX при попытке пойти вправо
#define reach_HomeX               1      //Достигнут HomeX при попытке пойти влево
#define ErrRotX                   2      // Ошибка вращение мотора X
#define reached_limitX            3      //Достигнут LimitX в процессе работы

typedef enum 
{
  init_A,
  init_X,
  init_Y
} in_axis;

enum 
{
  Cyl_init_Man,
  Cyl_Behind_Man,
  Cyl_Ahead_Man,
  Cyl_Comes_Out_Man, 
  Cyl_Comes_Man,      
};
enum 
{
  Cyl_init_Zah,
  Close_Zah,
  Open_Zah,
  Cyl_Comes_Out_Zah, 
  Cyl_Comes_Zah,      
};
enum 
{
  Cyl_init_Fin,
  Cyl_Behind_Fin,
  Cyl_Ahead_Fin,
  Cyl_Comes_Out_Fin, 
  Cyl_Comes_Fin,      
};

