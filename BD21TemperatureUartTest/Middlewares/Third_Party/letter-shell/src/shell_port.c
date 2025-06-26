#include "shell.h"
#include "usart.h"
#include "stdint.h"
#include "userTask.h"

char shellBuffer[512];

Shell shell;

static short sshellRead(char * buf, unsigned short len )
{
    int ret;
    char ch;
    ret = gUartCtrl[5].pfRecvByte(&ch);
    //注意，只实现单字节访问
    if(ret == 1)
    {
        *buf=ch;
        return 1;
    }

    return 0;
}

static short sshellWrite(char * buf, unsigned short len )
{
    uint16_t send  = 0;
    while (send < len)
    {
        // FUartPs_write(&g_UART, buf[send++]);
        //         while((FUartPs_getLineStatus(&g_UART) & Uart_line_thre) != Uart_line_thre);
        gUartCtrl[5].pfSendByte(buf[send++]);   
    }

    return len;
}



void ShellInit(void)
{
    shell.read = sshellRead;
    shell.write = sshellWrite;
    shellInit(&shell, shellBuffer, 512);
}
