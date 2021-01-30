#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : CtrlGetHubDescr
* Description    : 获取HUB描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : ERR_SUCCESS 成功
                   ERR_USB_BUF_OVER 长度错误
*******************************************************************************/
uint8_t   CtrlGetHubDescr( void )  
{
    uint8_t   s;
    uint8_t  len;
    CopySetupReqPkg( SetupGetHubDescr );
    s = HostCtrlTransfer( Com_Buffer, (uint8_t *)&len );                                    // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetHubDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                                            // 描述符长度错误
    }
//  if ( len < 4 ) return( ERR_USB_BUF_OVER );                                 // 描述符长度错误
    return( ERR_SUCCESS );
}
