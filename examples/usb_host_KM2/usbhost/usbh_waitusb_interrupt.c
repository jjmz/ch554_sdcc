#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"
#include "../print/small_print.h"
#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : WaitUSB_Interrupt
* Description    : 等待USB中断
* Input          : None
* Output         : None
* Return         : 返回ERR_SUCCESS 数据接收或者发送成功
                   ERR_USB_UNKNOWN 数据接收或者发送失败
*******************************************************************************/
uint8_t WaitUSB_Interrupt( void )
{
    uint16_t  i;
    for ( i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- ){;}
    return( UIF_TRANSFER ? ERR_SUCCESS : ERR_USB_UNKNOWN );
}

