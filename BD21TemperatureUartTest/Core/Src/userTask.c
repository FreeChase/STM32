
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "usart.h"
#include "userTask.h"
#include "key.h"
#include "flash_inner.h"
#include "../component/xprintf/xprintf.h"
#include "../component/statemach/xstatemach.h"
#include "../../Middlewares/Third_Party/letter-shell/src/shell.h"
#include "zsversion.h"

TaskHandle_t PCTalk_Handler;

TaskHandle_t Shell_Handler;
T_UartCtrl gUartPc;
T_UartCtrl gUartDbg;
T_UartCtrl gUartLoopBack;

T_UartCtrl gUartCtrl[6];

void ShellInit(void);
void shellParamInit(void);


int _putchar(char a);
int _getchar(char *a);
int _sendbyte(char a);


int _putchar_0(char a);
int _getchar_0(char *a);
int _sendbyte_0(char a);

int _putchar_1(char a);
int _getchar_1(char *a);
int _sendbyte_1(char a);

int _putchar_2(char a);
int _getchar_2(char *a);
int _sendbyte_2(char a);

int _putchar_3(char a);
int _getchar_3(char *a);
int _sendbyte_3(char a);

int _putchar_4(char a);
int _getchar_4(char *a);
int _sendbyte_4(char a);

int _putchar_5(char a);
int _getchar_5(char *a);
int _sendbyte_5(char a);


int _dbg_putchar(char a);
int _dbg_getchar(char *a);
int _dbg_sendbyte(char a);

int _loopback_putchar(char a);
int _loopback_getchar(char *a);
int _loopback_sendbyte(char a);

int ResultOutput( void );
void LoopProcess(void);
int MainStateInit(void * p );
int MainStateHandle(void * p ,int keyEvent);
int TestBD21StateInit(void *p);
int TestBD21StateHandle(void * p ,int keyEvent);

static char uartbuf[512] = {0};
static char uartbuf_dbg[512] = {0};
static char uartbuf_loopback[512] = {0};
static char uartbuf_all[6][1024] = {0};
char pinGetValue[7];

TimerHandle_t gTimers[ eSoftTimer_Max ]={0};
T_ADCCtrl gAdcCtrl;



//* 对应关系
T_LEDIndexTable gledTable[20] = {
    { LED_1_GPIO_Port, LED_1_Pin},
    { LED_2_GPIO_Port, LED_2_Pin},
    { LED_3_GPIO_Port, LED_3_Pin},
    { LED_4_GPIO_Port, LED_4_Pin},
    { LED_5_GPIO_Port, LED_5_Pin},
    { LED_6_GPIO_Port, LED_6_Pin},
    { LED_7_GPIO_Port, LED_7_Pin},
    { LED_8_GPIO_Port, LED_8_Pin},
    { LED_9_GPIO_Port, LED_9_Pin},
    { LED_10_GPIO_Port, LED_10_Pin},
    { LED_11_GPIO_Port, LED_11_Pin},
    { LED_12_GPIO_Port, LED_12_Pin},
    { LED_13_GPIO_Port, LED_13_Pin},
    { LED_14_GPIO_Port, LED_14_Pin},
    { LED_15_GPIO_Port, LED_15_Pin},
    { LED_16_GPIO_Port, LED_16_Pin},
    { LED_17_GPIO_Port, LED_17_Pin},
    { LED_18_GPIO_Port, LED_18_Pin},
    { LED_19_GPIO_Port, LED_19_Pin},
    { LED_20_GPIO_Port, LED_20_Pin},
};

//* event事件，
KeyControl gkey[eKeyType_Max];
KeyState gKeylastState[eKeyType_Max];
xQueueHandle xQueue_KeyEvent;
int gu32handleState = eMainState_None;

T_TestResultInfo gBD21TestResult;
T_UartComplexCtrl gUartComplexCtrl[MAX_TEST_UART_NUM];
T_MainCtrlInfo gMainCtrl_info;
T_STATEMACHCtrl_Main gMainCtrl;

// T_STATEMACHCtrl gMainCtrlInfo.lv1TestBD21[MAX_TEST_UART_NUM];
// T_MainCtrlInfo  gMainCtrlInfo;

// T_UartComplexCtrl gLv2UartComplexCtrl[MAX_TEST_UART_NUM];
// TimerHandle_t gUartTimeoutTimer[ MAX_TEST_UART_NUM ]={0};

static void ParamInit(void);



/* USER CODE END Header_StartDefaultTask */
void PermierTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    //int i;
    //uint32_t value = 0;
    char ch;
    int keyEvent;
    int handleState = eMainState_None;
    // init
    ParamInit();
    
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    // HAL_ADC_Start_DMA(&hadc1, adcValue, 1);
    // xprintf("HelloWorld %d\r\n", 123);
    /* Infinite loop */
    for (;;)
    {
        // while (gUartCtrl[0].pfRecvByte(&ch))
        // {
        //     xprintf("%c",ch);
        // }
        //* 周期性read key_clear，判断是否清除flash
            LoopProcess();
        osDelay(10);
    }
    /* USER CODE END StartDefaultTask */
}
void shellTask(void *param);

extern Shell shell;
void ShellTask(void *argument)
{
    shellParamInit();
    ShellInit();
    while(1)
    {
        shellTask(&shell);
    }
}


void shellParamInit(void)
{
     //调试串口
    initializeBuffer(&gUartCtrl[5].rx.cirbuf, uartbuf_all[5], sizeof(uartbuf_all[5]));
    gUartCtrl[5].rx.putItem = _putchar_5;
    gUartCtrl[5].rx.getItem = _getchar_5;
    gUartCtrl[5].pfSendByte = _sendbyte_5;
    gUartCtrl[5].pfRecvByte = _getchar_5; //跟getchar使用同一函数

    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
}

//定时器回调函数
static void vTimerADCCallback( TimerHandle_t xTimer )
{
//    gAdcCtrl.adcData.raw[gAdcCtrl.adcData.cnt%10] = HAL_ADC_GetValue(&hadc1);
//    gAdcCtrl.adcData.cnt++;
//    gAdcCtrl.pfcompute();
}
//定时器回调函数
static void vTimerUartTimeoutCallback( TimerHandle_t xTimer )
{
    int i = 0;
    for (i = 0; i < sizeof(gMainCtrl.attr.pinfo->lv1TestBD21)/sizeof(gMainCtrl.attr.pinfo->lv1TestBD21[0]); i++)
    {
        if( xTimer == gMainCtrl.attr.pinfo->lv1TestBD21[i].attr.plv2UartComplex->xtimer )
        {
            gMainCtrl.attr.pinfo->lv1TestBD21[i].attr.plv2UartComplex->timeoutFlag = 1;
        }

    }
}
//定时器回调函数
static void vTimerBD21ResultCallback( TimerHandle_t xTimer )
{
		int i=0;
    gMainCtrl.attr.pinfo->lv1BD21ResultOutput.attr.plv2TestResult->curTime+=10;
    gMainCtrl.attr.pinfo->lv1BD21ResultOutput.attr.plv2TestResult->curTime =\
    gMainCtrl.attr.pinfo->lv1BD21ResultOutput.attr.plv2TestResult->curTime%1000;
    if( gMainCtrl.attr.pinfo->lv1BD21ResultOutput.attr.plv2TestResult->curTime >\
        gMainCtrl.attr.pinfo->lv1BD21ResultOutput.attr.plv2TestResult->timeHigh )
    {
        //* high state still 


            // xprintf("All Low!!!\r\n");
            for (i = 0; i < 20; i++)
            {
                //TODO 完善灯灭逻辑
                HAL_GPIO_WritePin(gledTable[i].pport,gledTable[i].pinIndex,0);
            }

    }
    else
    {
            // xprintf("All High!!!\r\n");s
            for (i = 0; i < 20; i++)
            {
                //TODO 完善灯亮逻辑
                HAL_GPIO_WritePin(gledTable[i].pport,gledTable[i].pinIndex,1);
            }

    }
}

// 定时器回调函数
static void vTimerKeyCallback(TimerHandle_t xTimer)
{
    // 模拟按键输入
    bool rawKeyState = false; // 这里应该是实际的硬件读取
    int keyEvent = 0;
    rawKeyState = HAL_GPIO_ReadPin(KEY_CLEAR_GPIO_Port, KEY_CLEAR_Pin);
    // keyclear 状态机(每10ms调用一次)
    Key_StateMachine(&gkey[eKeyType_Clear], rawKeyState);
    // 检测状态变化并处理
    if (gkey[eKeyType_Clear].state != gKeylastState[eKeyType_Clear]) {
        switch (gkey[eKeyType_Clear].state) {
            case KEY_PRESSED:
                // xprintf("keyClear press\r\n");
                keyEvent = eKeyEvent_Clear_Pressed;
                xQueueSend( xQueue_KeyEvent, &keyEvent, 0 );
                break;
            case KEY_LONG_PRESSED:
                // xprintf("keyClear long press\r\n");
                keyEvent = eKeyEvent_Clear_LongPressed;
                xQueueSend( xQueue_KeyEvent, &keyEvent, 0 );
                break;
            case KEY_RELEASED:
                // xprintf("keyClear released\r\n");
                break;
            default:
                break;
        }
        gKeylastState[eKeyType_Clear] = gkey[eKeyType_Clear].state;
    }
    
    rawKeyState = HAL_GPIO_ReadPin(KEY_READ_GPIO_Port, KEY_READ_Pin);
    // keyRead 状态机(每10ms调用一次)
    Key_StateMachine(&gkey[eKeyType_Read], rawKeyState);
    // 检测状态变化并处理
    if (gkey[eKeyType_Read].state != gKeylastState[eKeyType_Read]) {
        switch (gkey[eKeyType_Read].state) {
            case KEY_PRESSED:
                // xprintf("keyRead press\r\n");
                break;
            case KEY_LONG_PRESSED:
                keyEvent = eKeyEvent_Read_LongPressed;
                xQueueSend( xQueue_KeyEvent, &keyEvent, 0 );
                break;
            case KEY_RELEASED:
                // xprintf("keyRead released\r\n");
                break;
            default:
                break;
        }
        gKeylastState[eKeyType_Read] = gkey[eKeyType_Read].state;
    }

    rawKeyState = HAL_GPIO_ReadPin(KEY_TEST_GPIO_Port, KEY_TEST_Pin);
    // keyTest 状态机(每10ms调用一次)
    Key_StateMachine(&gkey[eKeyType_Test], rawKeyState);
    // 检测状态变化并处理
    if (gkey[eKeyType_Test].state != gKeylastState[eKeyType_Test]) {
        switch (gkey[eKeyType_Test].state) {
            case KEY_PRESSED:
                // xprintf("keyTest press\r\n");
                break;
            case KEY_LONG_PRESSED:
                // xprintf("keyTest long press\r\n");
                keyEvent = eKeyEvent_Test_LongPressed;
                xQueueSend( xQueue_KeyEvent, &keyEvent, 0 );
                break;
            case KEY_RELEASED:
                // xprintf("keyTest released\r\n");
                break;
            default:
                break;
        }
        gKeylastState[eKeyType_Test] = gkey[eKeyType_Test].state;
    }
}




static void ParamInit(void)
{
    int i;
    //* gMainCtrl init 
    memset(&gMainCtrl,0,sizeof(gMainCtrl));
    memset(&gMainCtrl_info,0,sizeof(gMainCtrl_info));
    memset(&gBD21TestResult,0,sizeof(gBD21TestResult));

    gMainCtrl.attr.state = eMainState_None;
    // gMainCtrl.attr.usrData = &gMainCtrlInfo;
    gMainCtrl.func.init =       MainStateInit;
    gMainCtrl.func.handlers =   MainStateHandle;
    gMainCtrl.attr.pinfo = &gMainCtrl_info;
    //* gMainCtrlInfo.lv1TestBD21 init 

    //* 1.初始化UartStrHandler
    // memset(&gMainCtrl.attr.pinfo->lv1TestBD21[i].attr.lv2UartComplex,0,sizeof(gMainCtrl.attr.pinfo->lv1TestBD21[i].attr.lv2UartComplex));
    for (i = 0; i < sizeof(gUartComplexCtrl)/sizeof(gUartComplexCtrl[0]); i++)
    {
        strcpy(gUartComplexCtrl[i].matchStr, "ZS_BDSDK_V");
       gUartComplexCtrl[i].puart = &gUartCtrl[i];

        gUartComplexCtrl[i].xtimer = xTimerCreate(/* Just a text name, not used by the RTOS kernel. */
                                           "timerout",
                                           /* The timer period in ticks, must be greater than 0. */
                                           1000,
                                           /* The timers will auto-reload themselves when they
                                           expire. */
                                           pdFALSE,
                                           /* The ID is used to store a count of the number of
                                           times the timer has expired, which is initialized to 0. */
                                           (void *)i,
                                           /* Each timer calls the same callback when it expires. */
                                           vTimerUartTimeoutCallback);
        gMainCtrl.attr.pinfo->lv1TestBD21[i].attr.plv2UartComplex = &gUartComplexCtrl[i];
    }
    
    // memset(&gMainCtrl.attr.info ,0,sizeof(gMainCtrl.attr.info));
    for (i = 0; i < sizeof(gMainCtrl.attr.pinfo->lv1TestBD21)/sizeof(gMainCtrl.attr.pinfo->lv1TestBD21[0]); i++)
    { 
        //* 赋值 usrdata
        // gMainCtrl.attr.pinfo->lv1TestBD21[i].attr.lv2UartComplex = &gLv2UartComplexCtrl[i];
        gMainCtrl.attr.pinfo->lv1TestBD21[i].attr.state = eTestBD21State_None;
        gMainCtrl.attr.pinfo->lv1TestBD21[i].func.init =        TestBD21StateInit;
        gMainCtrl.attr.pinfo->lv1TestBD21[i].func.handlers =    TestBD21StateHandle;
    }
    
    //* 测试结果状态机 初始化
    gMainCtrl.attr.pinfo->lv1BD21ResultOutput.attr.plv2TestResult = &gBD21TestResult;
    gMainCtrl.attr.pinfo->lv1BD21ResultOutput.attr.plv2TestResult->timeHigh = 500;
    //* 周期闪烁 10ms间隔
    gMainCtrl.attr.pinfo->lv1BD21ResultOutput.attr.plv2TestResult->xtimer = xTimerCreate(/* Just a text name, not used by the RTOS kernel. */
                                           "timerout",
                                           /* The timer period in ticks, must be greater than 0. */
                                           10,
                                           /* The timers will auto-reload themselves when they
                                           expire. */
                                           pdTRUE,
                                           /* The ID is used to store a count of the number of
                                           times the timer has expired, which is initialized to 0. */
                                           (void *)i,
                                           /* Each timer calls the same callback when it expires. */
                                           vTimerBD21ResultCallback);
    gMainCtrl.attr.pinfo->lv1BD21ResultOutput.func.init = 0;
    gMainCtrl.attr.pinfo->lv1BD21ResultOutput.func.handlers = 0;

    // gMainCtrl.attr.event = eMainState_None;

    memset(&gkey[0],0,sizeof(gkey));
    memset(&gKeylastState[0],0,sizeof(gKeylastState));

    // 初始化按键: 20ms消抖时间, 1000ms长按时间, 低电平有效
    Key_Init(&gkey[eKeyType_Read], 20, 1000, false);
    Key_Init(&gkey[eKeyType_Clear], 20, 1000, false);
    Key_Init(&gkey[eKeyType_Test], 20, 1000, false);
    
    xQueue_KeyEvent  = xQueueCreate( 5, sizeof(int)); 
    //* key event timer
    gTimers[eSoftTimer_Key] = xTimerCreate(/* Just a text name, not used by the RTOS kernel. */
                                           "TimerKey",
                                           /* The timer period in ticks, must be greater than 0. */
                                           10,
                                           /* The timers will auto-reload themselves when they
                                           expire. */
                                           pdTRUE,
                                           /* The ID is used to store a count of the number of
                                           times the timer has expired, which is initialized to 0. */
                                           (void *)0,
                                           /* Each timer calls the same callback when it expires. */
                                           vTimerKeyCallback);

    // start timer
    if (gTimers[eSoftTimer_Key] == NULL)
    {
        /* The timer was not created. */
        xprintf("error :Key Timer Create Fail!\r\n");
    }
    else
    {
        /* Start the timer. No block time is specified, and even if one was it would be
        ignored because the RTOS scheduler has not yet been started. */
        if (xTimerStart(gTimers[eSoftTimer_Key], 0) != pdPASS)
        {
            /* The timer could not be set into the Active state. */
            xprintf("error :Key Timer Start Fail!\r\n");
        }
    }

    initializeBuffer(&gUartPc.rx.cirbuf, uartbuf, sizeof(uartbuf));
    gUartPc.rx.putItem = _putchar;
    gUartPc.rx.getItem = _getchar;
    gUartPc.pfSendByte = _sendbyte;
    gUartPc.pfRecvByte = _getchar;//跟getchar使用同一函数
   
    //回环串口
    initializeBuffer(&gUartLoopBack.rx.cirbuf, uartbuf_loopback, sizeof(uartbuf_loopback));
    gUartLoopBack.rx.putItem = _loopback_putchar;
    gUartLoopBack.rx.getItem = _loopback_getchar;
    gUartLoopBack.pfSendByte = _loopback_sendbyte;
    gUartLoopBack.pfRecvByte = _loopback_getchar;//跟getchar使用同一函数

    for (i = 0; i < MAX_USE_UART_NUM; i++)
    {
        initializeBuffer(&gUartCtrl[i].rx.cirbuf, &uartbuf_all[i][0], sizeof(uartbuf_all[i]));
    }
    
    gUartCtrl[0].rx.putItem = _putchar_0;
    gUartCtrl[0].rx.getItem = _getchar_0;
    gUartCtrl[0].pfSendByte = _sendbyte_0;
    gUartCtrl[0].pfRecvByte = _getchar_0;//跟getchar使用同一函数

    gUartCtrl[1].rx.putItem = _putchar_1;
    gUartCtrl[1].rx.getItem = _getchar_1;
    gUartCtrl[1].pfSendByte = _sendbyte_1;
    gUartCtrl[1].pfRecvByte = _getchar_1;//跟getchar使用同一函数

    gUartCtrl[2].rx.putItem = _putchar_2;
    gUartCtrl[2].rx.getItem = _getchar_2;
    gUartCtrl[2].pfSendByte = _sendbyte_2;
    gUartCtrl[2].pfRecvByte = _getchar_2;//跟getchar使用同一函数

    gUartCtrl[3].rx.putItem = _putchar_3;
    gUartCtrl[3].rx.getItem = _getchar_3;
    gUartCtrl[3].pfSendByte = _sendbyte_3;
    gUartCtrl[3].pfRecvByte = _getchar_3;//跟getchar使用同一函数

    gUartCtrl[4].rx.putItem = _putchar_4;
    gUartCtrl[4].rx.getItem = _getchar_4;
    gUartCtrl[4].pfSendByte = _sendbyte_4;
    gUartCtrl[4].pfRecvByte = _getchar_4;//跟getchar使用同一函数

    gUartCtrl[5].rx.putItem = _putchar_5;
    gUartCtrl[5].rx.getItem = _getchar_5;
    gUartCtrl[5].pfSendByte = _sendbyte_5;
    gUartCtrl[5].pfRecvByte = _getchar_5;//跟getchar使用同一函数

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);

    //* 
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
}

int _putchar(char a)
{
    return writeBufferNoMutex(&gUartPc.rx.cirbuf, a);
}

int _getchar(char *a)
{
    return readBufferNoMutex(&gUartPc.rx.cirbuf, a);
}

int _sendbyte(char a)
{
    
    huart3.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC));
    return 0;
}


int _dbg_putchar(char a)
{
    return writeBufferNoMutex(&gUartDbg.rx.cirbuf, a);
}

int _dbg_getchar(char *a)
{
    return readBufferNoMutex(&gUartDbg.rx.cirbuf, a);
}

int _dbg_sendbyte(char a)
{
    
    huart1.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC));
    return 0;
}

int _loopback_putchar(char a)
{
    return writeBufferNoMutex(&gUartLoopBack.rx.cirbuf, a);
}

int _loopback_getchar(char *a)
{
    return readBufferNoMutex(&gUartLoopBack.rx.cirbuf, a);
}

int _loopback_sendbyte(char a)
{
    
    huart4.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TC));
    return 0;
}

int _putchar_0(char a)
{
    return writeBufferNoMutex(&gUartCtrl[0].rx.cirbuf, a);
}

int _getchar_0(char *a)
{
    return readBufferNoMutex(&gUartCtrl[0].rx.cirbuf, a);
}

int _sendbyte_0(char a)
{
    
    huart1.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC));
    return 0;
}
int _putchar_1(char a)
{
    return writeBufferNoMutex(&gUartCtrl[1].rx.cirbuf, a);
}

int _getchar_1(char *a)
{
    return readBufferNoMutex(&gUartCtrl[1].rx.cirbuf, a);
}

int _sendbyte_1(char a)
{
    
    huart2.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TC));
    return 0;
}
int _putchar_2(char a)
{
    return writeBufferNoMutex(&gUartCtrl[2].rx.cirbuf, a);
}

int _getchar_2(char *a)
{
    return readBufferNoMutex(&gUartCtrl[2].rx.cirbuf, a);
}

int _sendbyte_2(char a)
{
    
    huart3.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC));
    return 0;
}
int _putchar_3(char a)
{
    return writeBufferNoMutex(&gUartCtrl[3].rx.cirbuf, a);
}

int _getchar_3(char *a)
{
    return readBufferNoMutex(&gUartCtrl[3].rx.cirbuf, a);
}

int _sendbyte_3(char a)
{
    
    huart4.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TC));
    return 0;
}
int _putchar_4(char a)
{
    return writeBufferNoMutex(&gUartCtrl[4].rx.cirbuf, a);
}

int _getchar_4(char *a)
{
    return readBufferNoMutex(&gUartCtrl[4].rx.cirbuf, a);
}

int _sendbyte_4(char a)
{
    
    huart5.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart5,UART_FLAG_TC));
    return 0;
}
int _putchar_5(char a)
{
    return writeBufferNoMutex(&gUartCtrl[5].rx.cirbuf, a);
}

int _getchar_5(char *a)
{
    return readBufferNoMutex(&gUartCtrl[5].rx.cirbuf, a);
}

int _sendbyte_5(char a)
{
    
    huart6.Instance->DR = a;
    while(!__HAL_UART_GET_FLAG(&huart6,UART_FLAG_TC));
    return 0;
}

void LoopProcess( void )
{
    if(gMainCtrl.func.handlers)
        gMainCtrl.func.handlers((char*)&gMainCtrl, 0);
}

#define TEST_INDEX_START 0
#define TEST_INDEX_MAX  5

int MainStateInit(void * p)
{
    int i=0;
    T_STATEMACHCtrl_Main * pstatCtrl = (T_STATEMACHCtrl_Main *)p;
    T_MainCtrlInfo * pMainInfo   = pstatCtrl->attr.pinfo;
    T_STATEMACHCtrl* plv1TestBD21 = (T_STATEMACHCtrl*)pMainInfo->lv1TestBD21;
    switch (pstatCtrl->attr.state)
    {
        case eMainState_Test1Waiting:
            //* 值初始化，PC1, PC0 PA15 0 0 0
            for (i = 0; i < sizeof(pMainInfo->testUltimateResult); i++)
            {
                pMainInfo->testUltimateResult[i] = 0;
            }

            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
            vTaskDelay(10);
            for (i = 0; i < sizeof(pMainInfo->lv1TestBD21)/sizeof(pMainInfo->lv1TestBD21[0]); i++)
            {
                SM_SetState((T_STATEMACHCtrl*)&pMainInfo->lv1TestBD21[i],eTestBD21State_None);
            }
            xprintf("Init test1\r\n");
            break;
        case eMainState_Test2Waiting:
            //* 值初始化，PC1, PC0 PA15  0 0 1
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
            vTaskDelay(10);
            for (i = 0; i < sizeof(pMainInfo->lv1TestBD21)/sizeof(pMainInfo->lv1TestBD21[0]); i++)
            {
                SM_SetState((T_STATEMACHCtrl*)&pMainInfo->lv1TestBD21[i],eTestBD21State_None);
            }
            xprintf("Init test2\r\n");
            break;
        case eMainState_Test3Waiting:
            //* 值初始化，PC1, PC0 PA15  0 1 0
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
            vTaskDelay(10);
            for (i = 0; i < sizeof(pMainInfo->lv1TestBD21)/sizeof(pMainInfo->lv1TestBD21[0]); i++)
            {
                SM_SetState((T_STATEMACHCtrl*)&pMainInfo->lv1TestBD21[i],eTestBD21State_None);
            }
            xprintf("Init test3\r\n");
            break;
        case eMainState_Test4Waiting:
            //* 值初始化，PC1, PC0 PA15  0 1 1
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
            vTaskDelay(10);
            for (i = 0; i < sizeof(pMainInfo->lv1TestBD21)/sizeof(pMainInfo->lv1TestBD21[0]); i++)
            {
                SM_SetState((T_STATEMACHCtrl*)&pMainInfo->lv1TestBD21[i],eTestBD21State_None);
            }
            xprintf("Init test4\r\n");
            break;
        case eMainState_Test5Waiting:
            //* 值初始化，PC1, PC0 PA15  0 1 1
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
            vTaskDelay(10);
            for (i = 0; i < sizeof(pMainInfo->lv1TestBD21)/sizeof(pMainInfo->lv1TestBD21[0]); i++)
            {
                SM_SetState((T_STATEMACHCtrl*)&pMainInfo->lv1TestBD21[i],eTestBD21State_None);
            }
            xprintf("Init test5\r\n");
            break;
        case eMainState_Test6Waiting:
            //* 值初始化，PC1, PC0 PA15  0 1 1
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
            vTaskDelay(10);
            for (i = 0; i < sizeof(pMainInfo->lv1TestBD21)/sizeof(pMainInfo->lv1TestBD21[0]); i++)
            {
                SM_SetState((T_STATEMACHCtrl*)&pMainInfo->lv1TestBD21[i],eTestBD21State_None);
            }
            xprintf("Init test6\r\n");
            break;
        case eMainState_Test7Waiting:
            //* 值初始化，PC1, PC0 PA15  0 1 1
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
            vTaskDelay(10);
            for (i = 0; i < sizeof(pMainInfo->lv1TestBD21)/sizeof(pMainInfo->lv1TestBD21[0]); i++)
            {
                SM_SetState((T_STATEMACHCtrl*)&pMainInfo->lv1TestBD21[i],eTestBD21State_None);
            }
            xprintf("Init test7\r\n");
            break;
        case eMainState_Test8Waiting:
            //* 值初始化，PC1, PC0 PA15  1 1 1
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
            vTaskDelay(10);
            for (i = 0; i < sizeof(pMainInfo->lv1TestBD21)/sizeof(pMainInfo->lv1TestBD21[0]); i++)
            {
                SM_SetState((T_STATEMACHCtrl*)&pMainInfo->lv1TestBD21[i],eTestBD21State_None);
            }
            xprintf("Init test8\r\n");
            break;
        case eMainState_Test8End:
            //* 测试完成灯亮1s
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_SET);
            vTaskDelay(1000);
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_RESET);
            //* 忽视之前接受到的key event
            xQueueReset( xQueue_KeyEvent );
            break;
        case eMainState_ReadEnd:
            ResultOutput();
            //* 测试完成灯亮1s
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_SET);
            vTaskDelay(1000);
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_RESET);
            //* 忽视之前接受到的key event
            xQueueReset( xQueue_KeyEvent );
        break;

    }
		return 0;
}

int MainResultAnalyse(T_UartComplexCtrl * puartComplex,char* ptestUltimateResult)
{
        //* 0 默认状态-未匹配到 2 超时
        if (puartComplex->result == 0 ||\
            puartComplex->result == 2)
        {
            *ptestUltimateResult = 'N';
        }
        else //* 1 成功
        {
            //* 若为0时，赋值Y，其余情况有N为N
            if(*ptestUltimateResult == 0)
            {
                //* 若为初值则赋值Y
                *ptestUltimateResult = 'Y';
            }
        }
				return 0;
}
int MainStateHandle(void * p ,int nouse)
{
    BaseType_t xret;
    int s32ledflash=0;
    int keyEvent=0;
    int i = 0,s32tmp=0;
    char * pflash = 0;
    T_UartComplexCtrl * puartComplex=0;
    T_STATEMACHCtrl_Main * pstatCtrl = (T_STATEMACHCtrl_Main *)p;
    T_MainCtrlInfo * pMainInfo   = pstatCtrl->attr.pinfo;
    T_STATEMACHCtrl_Lv1TestBD21 * plv1TestBD21SM   = (T_STATEMACHCtrl_Lv1TestBD21 *)&pstatCtrl->attr.pinfo->lv1TestBD21[0];


    switch (pstatCtrl->attr.state)
    {

    case eMainState_None:
        xret = xQueueReceive(xQueue_KeyEvent, &keyEvent, 0);
        if (xret)
        {

            if (keyEvent == eKeyEvent_Read_LongPressed)
            {
                // for (i = 0; i < count; i++)
                // {
                //     /* code */
                //     plv1TestBD21SM->func.handlers(&plv1TestBD21SM[i],keyEvent);
                // }
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl,eMainState_ReadWaiting);
            }
            else if (keyEvent == eKeyEvent_Test_LongPressed)
            {
                //* 串口子状态机置0初始化
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    SM_SetState( (T_STATEMACHCtrl *)&plv1TestBD21SM[i],eTestBD21State_None);
                }
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl,eMainState_Test1Waiting);
            }
            else if (keyEvent == eKeyEvent_Clear_Pressed)
            {
                xprintf("Handle Clear Event\r\n");
                InnerFlashErase(FLASH_PARAMS_START, FLASH_PARAMS_END);
                //* 测试完成灯亮1s
                HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_SET);
                vTaskDelay(1000);
                HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_RESET);
            }
        }

        break;
    case eMainState_Test1Waiting:
    case eMainState_Test2Waiting:
    case eMainState_Test3Waiting:
    case eMainState_Test4Waiting:
    case eMainState_Test5Waiting:
    case eMainState_Test6Waiting:
    case eMainState_Test7Waiting:
    case eMainState_Test8Waiting:
        // xret = xQueueReceive(xQueue_KeyEvent, &keyEvent, 0);
        //* 串口通讯子测试程序待运行
        for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
        {
            /* code */
            plv1TestBD21SM[i].func.handlers((T_STATEMACHCtrl *)&plv1TestBD21SM[i], keyEvent);
        }

        //* 判断统计结果
        //* 1.是否都判断完成
        s32tmp = 1;
        for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
        {
            puartComplex = plv1TestBD21SM[i].attr.plv2UartComplex;
            if (puartComplex->result == 0)
            {
                //* 有未测试成功，则将s32tmp置0
                s32tmp = 0;
            }
        }
        //* 2.若完成则统计并切换状态
        if (s32tmp)
        {
            if (pstatCtrl->attr.state == eMainState_Test1Waiting)
            {
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    MainResultAnalyse(plv1TestBD21SM[i].attr.plv2UartComplex, &pMainInfo->testUltimateResult[0 + i]);
                }
                //* 跳转至测试第二组
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_Test2Waiting);
            }
            else if (pstatCtrl->attr.state == eMainState_Test2Waiting)
            {
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    MainResultAnalyse(plv1TestBD21SM[i].attr.plv2UartComplex, &pMainInfo->testUltimateResult[10 + i]);
                }
                //* 跳转至测试第二组
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_Test3Waiting);
            }
            else if (pstatCtrl->attr.state == eMainState_Test3Waiting)
            {
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    MainResultAnalyse(plv1TestBD21SM[i].attr.plv2UartComplex, &pMainInfo->testUltimateResult[5 + i]);
                }
                //* 跳转至测试第二组
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_Test4Waiting);
            }
            else if (pstatCtrl->attr.state == eMainState_Test4Waiting)
            {
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    MainResultAnalyse(plv1TestBD21SM[i].attr.plv2UartComplex, &pMainInfo->testUltimateResult[15 + i]);
                }
                //* 跳转至测试第二组
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_Test5Waiting);
            }
            else if (pstatCtrl->attr.state == eMainState_Test5Waiting)
            {
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    MainResultAnalyse(plv1TestBD21SM[i].attr.plv2UartComplex, &pMainInfo->testUltimateResult[0 + i]);
                }
                //* 跳转至测试第二组
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_Test6Waiting);
            }
            else if (pstatCtrl->attr.state == eMainState_Test6Waiting)
            {
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    MainResultAnalyse(plv1TestBD21SM[i].attr.plv2UartComplex, &pMainInfo->testUltimateResult[10 + i]);
                }
                //* 跳转至测试第二组
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_Test7Waiting);
            }
            else if (pstatCtrl->attr.state == eMainState_Test7Waiting)
            {
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    MainResultAnalyse(plv1TestBD21SM[i].attr.plv2UartComplex, &pMainInfo->testUltimateResult[5 + i]);
                }
                //* 跳转至测试第二组
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_Test8Waiting);
            }
            else if (pstatCtrl->attr.state == eMainState_Test8Waiting)
            {
                for (i = TEST_INDEX_START; i < TEST_INDEX_MAX; i++)
                {
                    MainResultAnalyse(plv1TestBD21SM[i].attr.plv2UartComplex, &pMainInfo->testUltimateResult[15 + i]);
                }
                //* 递增写入flash
                InnerFlashSelfIncWrite((char*)&pMainInfo->testUltimateResult[ 0 ], 20);
                //* 跳转至测试 8 end
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_Test8End);
            }
        }

        break;
    case eMainState_Test8End:
        xret = xQueueReceive(xQueue_KeyEvent, &keyEvent, 0);
        if (xret)
        {
            //* 测试终状态，只支持擦除
            if (keyEvent == eKeyEvent_Clear_Pressed)
            {
                xprintf("Handle Clear Event\r\n");
                InnerFlashErase(FLASH_PARAMS_START, FLASH_PARAMS_END);
                //* 测试完成灯亮1s
                HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_SET);
                vTaskDelay(1000);
                HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_RESET);
            }
        }
        break;
    case eMainState_ReadWaiting:
        //*1.读取flash,并判断均值结果
        pflash = (char *)FLASH_PARAMS_START;
        if(pflash[0] == 0xff)
        {
            pstatCtrl->attr.pinfo->readFlashResult[0] = 'E';
        }
        while (pflash[0] != 0xff)
        {
            for (i = 0; i < 20; i++)
            {
                if (pflash[i] == 'Y')
                {
                    if (pstatCtrl->attr.pinfo->readFlashResult[i] == 0)
                        pstatCtrl->attr.pinfo->readFlashResult[i] = 'Y';
                }
                else if (pflash[i] == 'N')
                {
                    if (pstatCtrl->attr.pinfo->readFlashResult[i] != 'E')
                        pstatCtrl->attr.pinfo->readFlashResult[i] = 'N';
                }
                else
                {
                    s32ledflash = 1;
                    pstatCtrl->attr.pinfo->readFlashResult[i] = 'E';
                }
            }
            //* 32字节偏移
            pflash += 32;
        }


        //*3 切换状态
        SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eMainState_ReadEnd);
        break;
    case eMainState_ReadEnd:
    default:
        xret = xQueueReceive(xQueue_KeyEvent, &keyEvent, 0);
        if (xret)
        {
            //* only handle erease event
            if (keyEvent == eKeyEvent_Clear_Pressed)
            {
                xprintf("Handle Clear Event\r\n");
                InnerFlashErase(FLASH_PARAMS_START, FLASH_PARAMS_END);
                //* 测试完成灯亮1s
                HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_SET);
                vTaskDelay(1000);
                HAL_GPIO_WritePin(LED_STATUS_GPIO_Port,LED_STATUS_Pin,GPIO_PIN_RESET);
            }
        }
        break;
    }
		return 0;
}

int TestBD21StateInit(void *p)
{
    int i = 0;
    T_STATEMACHCtrl_Lv1TestBD21 *pstatCtrl = (T_STATEMACHCtrl_Lv1TestBD21 *)p;
    // T_UartComplexCtrl *puartComplex = (T_UartComplexCtrl *)pstatCtrl->attr.usrData;
    switch (pstatCtrl->attr.state)
    {
    case eTestBD21State_Start:
    case eTestBD21State_Waiting:
        pstatCtrl->attr.plv2UartComplex->result = 0;
        pstatCtrl->attr.plv2UartComplex->timeoutFlag = 0;
        StrSearch_Reset(&pstatCtrl->attr.plv2UartComplex->strSM);
        //XXX 此处添加清除UART操作
        clearBufferNoMutex(&pstatCtrl->attr.plv2UartComplex->puart->rx.cirbuf);
        
        break;
    case eTestBD21State_Over:
        break;
    default:
        break;
    }
    return 0;
}

int TestBD21StateHandle(void * p ,int keyEvent)
{
    int ret=0,cnt=0;
    char sendStr[]="$ZSCSM,VER,2,1*FC\r\n";
    char *poffset=0;
    char ch;
    T_STATEMACHCtrl_Lv1TestBD21 * pstatCtrl = (T_STATEMACHCtrl_Lv1TestBD21 *)p;
    T_UartComplexCtrl * puartComplex = pstatCtrl->attr.plv2UartComplex;
    switch (pstatCtrl->attr.state)
    {

    case eTestBD21State_None:
    case eTestBD21State_Start:
        // //* 0.清除已经缓冲数据 
        // //XXX 已经清除缓冲
        // clearBufferNoMutex(&puartComplex->puart->rx.cirbuf);
        //* 1.发送zsver数据
        poffset = sendStr;
        while (*poffset)
        {
            if(puartComplex->puart->pfSendByte)
                puartComplex->puart->pfSendByte(*poffset);
            poffset++;
        }
        
        //* 2.触发 soft timer，等待应答
        xTimerStart( puartComplex->xtimer, 0 );
        //* 3.切换为next状态
        StrSearch_Init(&puartComplex->strSM,puartComplex->matchStr);
        puartComplex->timeoutFlag = 0;
        SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eTestBD21State_Waiting);
        break;
    case eTestBD21State_Waiting:
        //* 持续解析
        // char teststr[] = "$BDZTI,0,1,0,0,0,*70$BDZDA,3,000329.00,01,01,2006,-08,00,00,00,00,N*0F\r\n[I/DBG] 000.000209.553 Ptl_in_proc(new) len1=19 \r\n$ZSCSM,VER,2,1*FC$ZSCUSTM,VER,VERSION: ZS_BDSDK_V1.0.0.0_svn8429 DATE:2024-09-18 TIME:17:35:11 ICDESTORY:open HWVERSION: C_V5.0*10";
        // char teststr[] = "$B";
        // char * poffset = teststr;
        // while (*poffset)
        cnt = 0;
        while (puartComplex->puart->pfRecvByte(&ch))
        {
            // xprintf("0x%x %c\r\n",ch,ch);
            ret = StrSearch_ProcessChar(&puartComplex->strSM,ch);
            if(ret == 1)
            {
                //* 匹配成功，跳转置解析结束状态
                puartComplex->result = 1;
                //* 停止计时
                xTimerStop( puartComplex->xtimer, 0 );
                
                SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eTestBD21State_Over);
				break;
            }
            //* 单词匹配不超过2048个
            if(cnt++>2048)
                break;
        }
        
        //* 等待时间足够，跳转至解析结束状态
        if(puartComplex->timeoutFlag)
        {
            xprintf("wait timerout !!!(0x%x)\r\n",puartComplex->xtimer);
            SM_SetState((T_STATEMACHCtrl *)pstatCtrl, eTestBD21State_Over);
        }
        break;
    case eTestBD21State_Over:
        //* 分析判断处理结果
        if(puartComplex->timeoutFlag)
        {
            puartComplex->timeoutFlag=0;
            puartComplex->result = 2;
        }
        // xprintf("current handle result (0x%x)\r\n",puartComplex->result);
        break;
    default:
        break;
    }
	return 0;
}

int ResultOutput(void)
{
    int i = 0;
    int s32ledflash = 0;

    for (i = 0; i < 20; i++)
    {
        if (gMainCtrl_info.readFlashResult[i] == 'E' || gMainCtrl_info.readFlashResult[i] == 0)
            s32ledflash = 1;
    }

    if (s32ledflash)
    {
        //*结果异常 启动定时器周期闪烁
        xTimerStart( gMainCtrl_info.lv1BD21ResultOutput.attr.plv2TestResult->xtimer , 0 );
    }
    else
    {
        //* 结果有效，直接常亮or长灭
        for (i = 0; i < 20; i++)
        {
            if (gMainCtrl_info.readFlashResult[i] == 'Y')
            {
                //* Y 灯灭
                // TODO 写对应GPIO
                HAL_GPIO_WritePin(gledTable[i].pport,gledTable[i].pinIndex,RESET);
            }
            else
            {
                //* N 灯亮
                // TODO 写对应GPIO
                HAL_GPIO_WritePin(gledTable[i].pport,gledTable[i].pinIndex,SET);
            }
            xprintf("%d:%c\r\n", i, gMainCtrl_info.readFlashResult[i]);
        }
    }

    return 0;
}


int ResultBD21StateInit(void *p)
{
    int i = 0;
    T_STATEMACHCtrl_Lv1ResultOutput *pstatCtrl = (T_STATEMACHCtrl_Lv1ResultOutput *)p;
    // T_UartComplexCtrl *puartComplex = (T_UartComplexCtrl *)pstatCtrl->attr.usrData;
    switch (pstatCtrl->attr.state)
    {
    case eResultBD21State_Outputing:
        
        break;
    default:
        break;
    }
    return 0;
}

int ResultBD21StateHandle(void * p ,int keyEvent)
{
    int i = 0;
    T_STATEMACHCtrl_Lv1ResultOutput *pstatCtrl = (T_STATEMACHCtrl_Lv1ResultOutput *)p;
    switch (pstatCtrl->attr.state)
    {
    case eResultBD21State_None:
        break;
    case eResultBD21State_Outputing:

        
        break;
    default:
        break;
    }
}

static char testStr[]="HelloWorld123ABCDEFGH";

/**
 * 函数：回环测试函数
 * 返回值：
 *      -1 失败
 *      0  成功
*/
int LoopbackUartTest(void)
{
    int i;
    char * poffset = 0;
    char buf[64]={0};
    char ch;
    poffset = testStr;

    //清空buf
    while (gUartLoopBack.pfRecvByte(&ch));
    
    while(*poffset)
    {
        gUartLoopBack.pfSendByte(*poffset++);
    }
    //wait 500ms
    vTaskDelay(500);

    poffset = buf;
    while (gUartLoopBack.pfRecvByte(&ch))
    {
        *poffset++=ch;
    }
    
    for ( i = 0; i < strlen(testStr); i++)
    {
        if(buf[i] != testStr[i])
            return -1;
    }
    
	return 0;
}

int UartTestPut(int uart_index,int loop)
{
    int i;
    char str[64]="$ZSRSION:ZS_BDSDK_V1.0.0.0_sv4234fdg";
    char * poffset = 0;
    int str_index = 0;
    if(loop>10)
    {
        xprintf("check params :loop <= 10 !\r\n");
        return -1;
    }
    if(uart_index>6)
    {
        xprintf("check params :uart_index <= 6 !\r\n");
        return -1;
    }
    
    
    for (i = 0; i < loop; i++)
    {

        str_index = 0;
        while ( str[str_index])
        {
            gUartCtrl[uart_index].rx.putItem(str[str_index]);
            str_index++;
        }
    }
    
    
    
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), UartTestPut, UartTestPut, int index int loop);

int UartPutStr(int index,char *str)
{
    int cnt = 0;
    if(index > 5)
    {
        xprintf("error check params!!\r\n");
        return -1;
    }
    
    while (str[cnt]!= 0 && cnt<1024 )
    {
        /* code */
        gUartCtrl[index].rx.putItem(str[cnt]);
        cnt++;
    }
    
    
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), UartPutStr, UartPutStr, int index char* str);

int UartGetDat(int index)
{
    int cnt = 0;
    char ch;
    if(index > 5)
    {
        xprintf("error check params!!\r\n");
        return -1;
    }
  
    while ( gUartCtrl[index].rx.getItem( &ch ) )
    {
        /* code */
        xprintf("%c",ch);
    }
    xprintf("\r\n");
    
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), UartGetDat, UartGetDat, int index);

int GetTestResult(void)
{
    xprintf("Result:\r\n");
    for (size_t i = 0; i < 20; i++)
    {
        xprintf("%d %c\r\n",i,gMainCtrl.attr.pinfo->testUltimateResult[i]);
    }
    
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), GetTestResult, GetTestResult, void);

int UartSendVer(int index)
{
    char str[]="$ZSCSM,VER,2,1*FC\r\n";
    int cnt = 0;
    if(index > 5 )
    {
        xprintf("error : check params!\r\n");
    }

    xprintf("sendver index %d \r\n",index);
    while (str[cnt]!= 0)
    {
        /* code */
        gUartCtrl[index].pfSendByte(str[cnt]);
        cnt++;
    }
    

	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), UartSendVer, UartSendVer, index 0-5);


int SendKeyEvent(int keyEvent)
{
    xQueueSend( xQueue_KeyEvent, &keyEvent, 0 );
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), SendKeyEvent, SendKeyEvent, int keyEvent);

int func(int i, char ch, char *str)
{
    xprintf("input int: %d, char: %c, string: %s\r\n", i, ch, str);
		return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), func, func, test);


void ver(void)
{
    xprintf("ver: %s\r\n", BSP_VERSION);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ver, ver, ver);

void verAll(void)
{
    xprintf("verAll: %s\r\n", BSP_VERSION_ALL);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), verAll, verAll, verAll);

void GetBuild(void)
{
    xprintf("date: %s\r\n", __DATE__);
    xprintf("time: %s\r\n", __TIME__);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), GetBuild, GetBuild, GetBuild);

void GetRuntime(void)
{
//    float usage;
//    TickType_t xLastWakeTime = xTaskGetTickCount();
//    usage = 1- ((float)ulTaskRunTime / (xLastWakeTime - ulStartRunTime) );
//    usage = usage * 100;
//    xprintf("usage: %f %\r\n", usage);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), GetRuntime, GetRuntime, GetRuntime);

void memdisplay(void *start_addr, size_t length, size_t unit_size) {
    if (unit_size != 1 && unit_size != 2 && unit_size != 4 && unit_size != 8) {
        xprintf("Invalid unit_size. Please use 1, 2, 4, or 8.\r\n");
        return;
    }

    size_t bytes_per_line = 16;
    uint8_t *mem = (uint8_t *)start_addr;

    // 打印列头
    xprintf("%-10s", "Address");
    for (size_t i = 0; i < bytes_per_line; ++i) {
        xprintf("0x%X ", i);
    }
    xprintf("  ASCII\r\n");

    // 打印横线
    xprintf("------------------");
    for (size_t i = 0; i < bytes_per_line; ++i) {
        xprintf("-----");
    }
    xprintf("\r\n");

    size_t addr = 0;
    while (addr < length) {
        // 打印行头地址
        xprintf("%p  ", (void *)(mem + addr));

        // 打印每行的内容
        for (size_t i = 0; i < bytes_per_line && addr < length; i += unit_size) {
            switch (unit_size) {
                case 1:
                    xprintf("%02X  ", mem[addr + i]);
                    break;
                case 2:
                    xprintf("%04X    ", *((uint16_t *)(mem + addr + i)));
                    break;
                case 4:
                    xprintf("%08X        ", *((uint32_t *)(mem + addr + i)));
                    break;
                case 8:
                    xprintf("%016llX   ", *((uint64_t *)(mem + addr + i)));
                    break;
                default:
                    // 不太可能到达这里，因为上面已经检查了 unit_size 的合法性
                    break;
            }
        }

        // 打印ASCII码
        xprintf("  ");
        for (size_t i = 0; i < bytes_per_line && addr < length; ++i) {
            char c = (mem[addr + i] >= 32 && mem[addr + i] <= 126) ? mem[addr + i] : '.';
            xprintf("%c", c);
        }

        xprintf("\r\n");
        addr += bytes_per_line;
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), md, memdisplay, addr len unit);
