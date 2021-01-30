#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : SetUsbSpeed
* Description    : 设置当前USB速度
* Input          : uint8_t FullSpeed
* Output         : None
* Return         : None
*******************************************************************************/
void    SetUsbSpeed( uint8_t FullSpeed )  
{
    if ( FullSpeed )                                                           // 全速
    {
        USB_CTRL &= ~ bUC_LOW_SPEED;                                           // 全速
        UH_SETUP &= ~ bUH_PRE_PID_EN;                                          // 禁止PRE PID
    }
    else
    {
        USB_CTRL |= bUC_LOW_SPEED;                                             // 低速		
    }
}
