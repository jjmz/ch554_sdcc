#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : HubGetPortStatus
* Description    : 查询HUB端口状态,返回在TxBuffer中
* Input          : uint8_t HubPortIndex 
* Output         : None
* Return         : ERR_SUCCESS 成功
                   ERR_USB_BUF_OVER 长度错误
*******************************************************************************/
uint8_t   HubGetPortStatus( uint8_t HubPortIndex )   
{
    uint8_t   s;
    uint8_t  len;
    pSetupReq -> bRequestType = HUB_GET_PORT_STATUS;
    pSetupReq -> bRequest = HUB_GET_STATUS;
    pSetupReq -> wValueL = 0x00;
    pSetupReq -> wValueH = 0x00;
    pSetupReq -> wIndexL = HubPortIndex;
    pSetupReq -> wIndexH = 0x00;
    pSetupReq -> wLengthL = 0x04;
    pSetupReq -> wLengthH = 0x00;
    s = HostCtrlTransfer( Com_Buffer, (uint8_t *)&len );                           // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < 4 )
    {
        return( ERR_USB_BUF_OVER );                                            // 描述符长度错误
    }
    return( ERR_SUCCESS );
}
