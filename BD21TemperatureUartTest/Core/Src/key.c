#include <stdint.h>
#include <stdbool.h>
#include "key.h"


// 初始化按键控制结构体
void Key_Init(KeyControl* key, uint16_t debounceTime, uint16_t longPressTime, bool isActiveLow) {
    key->state = KEY_RELEASED;
    key->pressTime = 0;
    key->releaseTime = 0;
    key->config.debounceTime = debounceTime;
    key->config.longPressTime = longPressTime;
    key->config.isActiveLow = isActiveLow;
}

// 状态机处理函数
void Key_StateMachine(KeyControl* key, bool rawState) {
    // 根据配置转换原始状态
    bool keyPressed = key->config.isActiveLow ? !rawState : rawState;
    
    switch (key->state) {
        case KEY_RELEASED:
            if (keyPressed) {
                key->state = KEY_PRESSING;
                key->pressTime = 0;
            }
            break;
            
        case KEY_PRESSING:
            if (keyPressed) {
                key->pressTime+=10;
                if (key->pressTime >= key->config.debounceTime) {
                    key->state = KEY_PRESSED;
                }
            } else {
                key->state = KEY_RELEASED;
            }
            break;
            
        case KEY_PRESSED:
            if (keyPressed) {
                key->pressTime+=10;
                if (key->pressTime >= key->config.longPressTime) {
                    key->state = KEY_LONG_PRESSED;
                }
            } else {
                key->state = KEY_RELEASING;
                key->releaseTime = 0;
            }
            break;
            
        case KEY_LONG_PRESSED:
            if (!keyPressed) {
                key->state = KEY_RELEASING;
                key->releaseTime = 0;
            }
            break;
            
        case KEY_RELEASING:
            if (!keyPressed) {
                key->releaseTime+=10;
                if (key->releaseTime >= key->config.debounceTime) {
                    key->state = KEY_RELEASED;
                }
            } else {
                key->state = KEY_PRESSED;
            }
            break;
            
        default:
            key->state = KEY_RELEASED;
            break;
    }
}