#define forwardSM       1
#define backwardSM      0
#define forwardA       0
#define backwardA      1
//��������� �������� ������ ������
#define InitSM          0      //�������� ��������� �������� �������
#define Wait_strip      1      //�������� ������ �� ����� 
#define Sup_roller      2      //������ ������ �� ������� ������
#define Main_pull       3      //�������� ������ ������
#define ErrorSM         4      // ��������� ������
#define CompleteSM      5      //������� ���������
//---------------------------������ ������ SM------------------------------------------------
//��������� ������ ������ ���� MessegSM
#define NoNewTaskSM       0       //��� ������ �������
#define NewTaskSM         1       //�������� ����� �������
#define ReInitSM          2       //�����������������
//��������� �� ������� ������ ������ CodeErrorSM
#define bahd_exit                0       //������ �� ������� �� ������ ������ 
#define NoStrip                  1       //������ ��� �� ����� � ������� ������� Timer_inputSM
#define not_tightened_strip      2      //�� �������� ������ ��� ����� - ��������� ���-�� ������ � ������ ���
#define ErrRotSM                 3      //�������� ����� ������ ��� ������� ������
#define not_tightened_strip_timer 4     //�� �������� ������ ��� ����� - �������� ������ 
//--------------------------������ ������� CM--------------------------------------------------
//��������� ������ ������� ���� MessegCM
#define NoNewTaskCM              0         //��� ������ �������
#define NewTaskCM                1         //�������� ����� �������
#define ResetCM                  2         //���������� � ��������
#define ReInitCM                 3         //����������������� ������ 
//��������� �������� ������ �������
#define InitCM                   0         //�������� ��������� �������� �������
#define CuttingSM                1         //��������� ������� ������
#define CompleteCM               2         //������� ���������
#define ErrorCM                  3         // ��������� ������
//��������� �� ������� ������ ������ CodeErrorCM
#define not_initCM                0       //������� �� � �������� ���������
#define Exceeded_TimeCM           1       //��������� ����� �������
// ---------------------------��� X--------------------------------------
#define m_rightX                  0       //�������� ������
#define m_leftX                   1       //�������� �����
//��������� �������� ��� X
#define NoNewTaskX                0       //��� ������ �������
#define motion_rightX             1       //�������� ������ X
#define motion_leftX            2       //�������� ����� X
#define ReInitX                   3         //����������������� ������ 
//��������� �������� ��� X
#define InitX                     0       //�������� ��������� � ������������ �������
#define MotionX                   1       //����������� �� ��� �
#define CompleteX                 2       //������� ���������
#define ErrorX                    3       //��������� ������
//��������� �� ������� �������� ��� X
#define reach_limitX              0       //��������� LimitX ��� ������� ����� ������
#define reach_HomeX               1      //��������� HomeX ��� ������� ����� �����
#define ErrRotX                   2      // ������ �������� ������ X
#define reached_limitX            3      //��������� LimitX � �������� ������

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

