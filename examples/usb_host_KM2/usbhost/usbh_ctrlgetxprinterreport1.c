#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : CtrlGetXPrinterReport1
* Description    : 打印机类命令
* Input          : None
* Output         : None
* Return         : ERR_USB_BUF_OVER 描述符长度错误
                   ERR_SUCCESS      成功
                   其他
*******************************************************************************/
uint8_t   CtrlGetXPrinterReport1( void )
{
    uint8_t   s;
    uint16_t   len;
    CopySetupReqPkg( XPrinterReport );
    s = HostCtrlTransfer( Com_Buffer, (uint8_t *)&len );                         // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
//     if ( len < ( (XPrinterReport[7]<<8)|(XPrinterReport[6]) ))
//     {
//         return( ERR_USB_BUF_OVER );                                         // 描述符长度错误
//     }
    return( ERR_SUCCESS );
}
