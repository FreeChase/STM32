#ifndef __USERTASK_H__
#define __USERTASK_H__

#include "../component/circlebuf/circlebuf_x.h"
#include "../component/statemach/xstatemach.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "timers.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_USE_UART_NUM (MAX_TEST_UART_NUM+1)
#define MAX_TEST_UART_NUM 5

enum eHandleState{
  eMainState_None = 0,
  eMainState_Test1Start,  
  eMainState_Test1Waiting,  
  eMainState_Test1End,  
  eMainState_Test2Start,  
  eMainState_Test2Waiting,  
  eMainState_Test2End,  
  eMainState_Test3Start,  
  eMainState_Test3Waiting,  
  eMainState_Test3End,  
  eMainState_Test4Start,  
  eMainState_Test4Waiting,  
  eMainState_Test4End,  
  eMainState_Test5Start,  
  eMainState_Test5Waiting,  
  eMainState_Test5End,  
  eMainState_Test6Start,  
  eMainState_Test6Waiting,  
  eMainState_Test6End,  
  eMainState_Test7Start,  
  eMainState_Test7Waiting,  
  eMainState_Test7End,  
  eMainState_Test8Start,  
  eMainState_Test8Waiting,  
  eMainState_Test8End,  
  eMainState_ReadStart,
  eMainState_ReadWaiting,
  eMainState_ReadEnd,
};

enum eUartHandleState{
  eTestBD21State_None = 0,
  eTestBD21State_Start,  
  eTestBD21State_Waiting,  
  eTestBD21State_Over,  
};

enum eResultBD21State{
  eResultBD21State_None = 0,
  eResultBD21State_Outputing,  
};

enum{
  eSoftTimer_ADC = 0,
  eSoftTimer_Key,
  eSoftTimer_Max,
};

enum eKeyType{
  eKeyType_Read= 0,
  eKeyType_Clear,
  eKeyType_Test,
  eKeyType_Max,
};

enum{
  eKeyEvent_Read_Pressed= 0,
  eKeyEvent_Read_LongPressed,
  eKeyEvent_Read_Released,

  eKeyEvent_Clear_Pressed,//3
  eKeyEvent_Clear_LongPressed,
  eKeyEvent_Clear_Released,

  eKeyEvent_Test_Pressed,//6
  eKeyEvent_Test_LongPressed,
  eKeyEvent_Test_Released,

  eKeyEvent_Max,
};


typedef int (* pf_int)(char);
typedef int (* pf_intx)(char *);

typedef struct 
{
  CircularBuffer cirbuf;  //循环buf
  pf_int putItem;         //写一个item，循环buf
  pf_intx getItem;        //获取一个item
}T_UartBufCtrl;

typedef struct 
{
  //接受循环buf
  T_UartBufCtrl rx;
  //行为函数，接受数据，发送数据
  pf_int pfSendByte;
  pf_intx pfRecvByte;
}T_UartCtrl;


extern T_UartCtrl gUartPc;
extern T_UartCtrl gUartDbg;
extern T_UartCtrl gUartLoopBack;
extern T_UartCtrl gUartCtrl[6];

typedef void (* pf_adcCompute)(void);
typedef double (* pf_adcGet)(void);

typedef struct{
  uint32_t raw[10];
  uint32_t average;
  uint32_t cnt;
}T_ADCValue;

typedef struct{
  T_ADCValue adcData;
  pf_adcCompute pfcompute;
  pf_adcGet pfGetAdcValue;
}T_ADCCtrl;





typedef struct{
  T_StrSearchSM strSM;
  char matchStr[32]; //匹配字符串
  int result; //0-匹配失败  1-匹配成功 2-超时 
  int timeoutFlag;//0-未超时 1-超时
  TimerHandle_t xtimer;
  T_UartCtrl * puart;
}T_UartComplexCtrl;



typedef struct 
{
    int state;      //状态
    int oldstate;   //老状态
    T_UartComplexCtrl* plv2UartComplex; //用户自定义数据
}T_STATEMACHAttr_Lv1TestBD21;

typedef struct 
{
    int (*  init)(void *);//进入状态，前置初始化
    int (*  handlers)(void *,int );//
    int (*  exit)(void *);
}T_STATEMACHFuncUnit_Lv1TestBD21;



typedef struct 
{
    T_STATEMACHAttr_Lv1TestBD21 attr;           //状态
    T_STATEMACHFuncUnit_Lv1TestBD21 func;     //状态条目   
}T_STATEMACHCtrl_Lv1TestBD21;

//*********************
//? 结果输出状态机 START
//*********************

typedef struct{
  int ledType;  //配置  0-常灭  1-常亮  2-周期闪烁
  int timeHigh; //亮时间
  int curValue; //当前值，1High 0Low
  int curTime;  //当前time计数
  TimerHandle_t xtimer;
}T_TestResultInfo;

typedef struct 
{
    int state;      //状态
    int oldstate;   //老状态
    T_TestResultInfo* plv2TestResult; //用户自定义数据
}T_STATEMACHAttr_Lv1ResultOutput;

typedef struct 
{
    int (*  init)(void *);//进入状态，前置初始化
    int (*  handlers)(void *,int );//
    int (*  exit)(void *);
}T_STATEMACHFuncUnit_Lv1ResultOutput;



typedef struct 
{
    T_STATEMACHAttr_Lv1ResultOutput attr;           //状态
    T_STATEMACHFuncUnit_Lv1ResultOutput func;     //状态条目   
}T_STATEMACHCtrl_Lv1ResultOutput;

//*********************
//? 结果输出状态机 END
//*********************

typedef struct{
  T_STATEMACHCtrl_Lv1TestBD21 lv1TestBD21[MAX_TEST_UART_NUM];  //5个子状态机
  T_STATEMACHCtrl_Lv1ResultOutput lv1BD21ResultOutput;        //结果输出
  char readFlashResult[20];    //测试中间过程记录
  char testUltimateResult[20];  //测试最终结果 'Y' 'N'用于记录
}T_MainCtrlInfo;

typedef struct 
{
    int state;      //状态
    int oldstate;   //老状态
    T_MainCtrlInfo* pinfo; //用户自定义数据
}T_STATEMACHAttr_Main;

typedef struct 
{
    int (*  init)(void *);//进入状态，前置初始化
    int (*  handlers)(void *,int );//
    int (*  exit)(void *);
}T_STATEMACHFuncUnit_Main;



typedef struct 
{
    T_STATEMACHAttr_Main attr;           //状态
    T_STATEMACHFuncUnit_Main func;     //状态条目   
}T_STATEMACHCtrl_Main;



typedef struct{
  GPIO_TypeDef * pport;
  uint32_t    pinIndex;
}T_LEDIndexTable;


//任务优先级
#define PCTALK_TASK_PRIO		4
//任务堆栈大小	
#define PCTALK_STK_SIZE 		2048  
//任务句柄
extern TaskHandle_t PCTalk_Handler;
//任务函数
void PermierTask(void * argument);

//任务优先级
#define SHELL_TASK_PRIO		1
//任务堆栈大小	
#define SHELL_STK_SIZE 	4096  
//任务句柄
extern TaskHandle_t Shell_Handler;
//任务函数
void ShellTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif 

