#ifndef __KEY_H__
#define __KEY_H__

#include "../component/circlebuf/circlebuf_x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif


// 合并状态和事件为一个枚举
typedef enum {
    KEY_RELEASED = 0,    // 按键释放状态
    KEY_PRESSING,        // 按键按下但未消抖
    KEY_PRESSED,         // 按键确认按下(短按)
    KEY_LONG_PRESSED,    // 按键长按状态
    KEY_RELEASING       // 按键释放但未消抖
} KeyState;

// 按键配置结构体
typedef struct {
    uint16_t debounceTime;    // 消抖时间(ms)
    uint16_t longPressTime;   // 长按时间阈值(ms)
    bool isActiveLow;         // 是否低电平有效
} KeyConfig;

// 按键控制结构体
typedef struct {
    KeyState state;          // 当前状态(包含事件信息)
    uint32_t pressTime;      // 按下时间计数
    uint32_t releaseTime;    // 释放时间计数
    KeyConfig config;        // 按键配置
} KeyControl;


void Key_StateMachine(KeyControl* key, bool rawState);
void Key_Init(KeyControl* key, uint16_t debounceTime, uint16_t longPressTime, bool isActiveLow);


#ifdef __cplusplus
}
#endif

#endif 

