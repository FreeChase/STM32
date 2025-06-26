#ifndef _XSTATEMACH_H_
#define _XSTATEMACH_H_

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

//***********************************/
//? 标准状态机相关定义 START
//***********************************/

typedef struct 
{
    int state;      //状态
    int oldstate;   //老状态
    void * usrData; //用户自定义数据
}T_STATEMACHAttr;

typedef struct 
{
    int (*  init)(void *);//进入状态，前置初始化
    int (*  handlers)(void *,int );//
    int (*  exit)(void *);
}T_STATEMACHFuncUnit;



typedef struct 
{
    T_STATEMACHAttr attr;           //状态
    T_STATEMACHFuncUnit func;     //状态条目   
}T_STATEMACHCtrl;


// 状态机错误码定义
typedef enum {
    SM_SUCCESS = 0,
    SM_ERROR_NULL_POINTER = -1,
    SM_ERROR_INVALID_STATE = -2,
    SM_NO_STATE_CHANGE = 1
} E_SMError;

// API函数声明
int SM_SetState(T_STATEMACHCtrl *pCtrl, int newState);
int SM_GetState(T_STATEMACHCtrl *pCtrl);
int SM_GetOldState(T_STATEMACHCtrl *pCtrl);
int SM_IsStateChanged(T_STATEMACHCtrl *pCtrl);
void SM_Init(T_STATEMACHCtrl *pCtrl);

//***********************************/
//? 标准状态机相关定义 END
//***********************************/



//***********************************/
//? 单个字符输入，匹配指定字符串 KMP算法  START
//***********************************/

#define MAX_PATTERN_LENGTH 64


// 状态机结构
typedef struct {
    const char* pattern;    // 模式串
    int pattern_len;       // 模式串长度
    int next[MAX_PATTERN_LENGTH];  // next数组，固定大小
    int current_state;    // 当前状态
} T_StrSearchSM;

void StrSearch_Reset(T_StrSearchSM* sm);
void StrSearch_Init(T_StrSearchSM* sm, const char* pattern);
int StrSearch_ProcessChar(T_StrSearchSM* sm, char c);
//***********************************/
//? 单个字符输入，匹配指定字符串 KMP算法 END
//***********************************/




#ifdef __cplusplus
}
#endif


#endif
