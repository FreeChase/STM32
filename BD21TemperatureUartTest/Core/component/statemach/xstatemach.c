#include "xstatemach.h"


// 初始化状态机控制结构
void SM_Init(T_STATEMACHCtrl *pCtrl) {
    if (pCtrl) {
        pCtrl->attr.state = 0;
        pCtrl->attr.oldstate = 0;
        // pCtrl->attr.usrData = NULL;
    }
}

// 设置新状态，自动处理init和exit回调
int SM_SetState(T_STATEMACHCtrl *pCtrl, int newState) {
    // 参数检查
    if (!pCtrl) {
        return SM_ERROR_NULL_POINTER;
    }
    
    // 如果状态没有改变，直接返回
    if (pCtrl->attr.state == newState) {
        return SM_NO_STATE_CHANGE;
    }
    
    // 保存旧状态
    pCtrl->attr.oldstate = pCtrl->attr.state;
    
    // 如果存在exit函数，调用旧状态的exit
    if (pCtrl->func.exit) {
        pCtrl->func.exit(pCtrl);
    }
    
    // 更新为新状态
    pCtrl->attr.state = newState;
    
    // 如果存在init函数，调用新状态的init
    if (pCtrl->func.init) {
        pCtrl->func.init(pCtrl);
    }
    
    return SM_SUCCESS;
}

// 获取当前状态
int SM_GetState(T_STATEMACHCtrl *pCtrl) {
    if (!pCtrl) {
        return SM_ERROR_NULL_POINTER;
    }
    return pCtrl->attr.state;
}

// 获取上一个状态
int SM_GetOldState(T_STATEMACHCtrl *pCtrl) {
    if (!pCtrl) {
        return SM_ERROR_NULL_POINTER;
    }
    return pCtrl->attr.oldstate;
}

// 检查状态是否发生改变
int SM_IsStateChanged(T_STATEMACHCtrl *pCtrl) {
    if (!pCtrl) {
        return 0;
    }
    return (pCtrl->attr.state != pCtrl->attr.oldstate);
}




//***********************************/
//? 单个字符输入，匹配指定字符串 KMP算法
//***********************************/
// 构建next数组
static void build_next(T_StrSearchSM* sm) {
    sm->next[0] = -1;
    
    int i = 0;
    int j = -1;
    
    while (i < sm->pattern_len - 1) {
        if (j == -1 || sm->pattern[i] == sm->pattern[j]) {
            i++;
            j++;
            sm->next[i] = j;
        } else {
            j = sm->next[j];
        }
    }
}

//* 初始化字符串匹配状态机
void StrSearch_Init(T_StrSearchSM* sm, const char* pattern) {
    sm->pattern = pattern;
    sm->pattern_len = strlen(pattern);
    if (sm->pattern_len >= MAX_PATTERN_LENGTH) {
        printf("错误：模式串长度不能超过%d\n", MAX_PATTERN_LENGTH);
        return;
    }
    sm->current_state = 0;
    build_next(sm);
}

// 处理单个字符，返回：
// -1: 不匹配
//  0: 部分匹配
//  1: 完全匹配
int StrSearch_ProcessChar(T_StrSearchSM* sm, char c) {
    // 当字符不匹配时，回退状态
    while (sm->current_state != -1 && c != sm->pattern[sm->current_state]) {
        sm->current_state = sm->next[sm->current_state];
    }
    
    // 状态前进
    sm->current_state++;
    
    // 判断匹配情况
    if (sm->current_state == sm->pattern_len) {
        // 完全匹配，重置状态以便继续搜索
        sm->current_state = sm->next[sm->current_state - 1];
        return 1;  // 完全匹配
    }
    
    return 0;  // 部分匹配
}

// 重置状态机
void StrSearch_Reset(T_StrSearchSM* sm) {
    sm->current_state = 0;
}


/*
StrSearch_Init(&puartComplex->strSM,puartComplex->matchStr);

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
*/

//***********************************/
//? 单个字符输入，匹配指定字符串 KMP算法 END
//***********************************/