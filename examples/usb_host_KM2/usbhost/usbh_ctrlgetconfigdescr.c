#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : CtrlGetConfigDescr
* Description    : 获取配置描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : ERR_USB_BUF_OVER 描述符长度错误
                   ERR_SUCCESS      成功
                   其他
*******************************************************************************/
uint8_t CtrlGetConfigDescr( void )
{
    uint8_t   s,len;
    CopySetupReqPkg( SetupGetCfgDescr );
    s = HostCtrlTransfer( Com_Buffer, (uint8_t *)&len );                                      // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }

    len = ( (PXUSB_CFG_DESCR)Com_Buffer ) -> wTotalLengthL;
    CopySetupReqPkg( SetupGetCfgDescr );
    if(HubLowSpeed)                                                                //HUB下低速设备
    {
      pSetupReq -> wLengthH = len;                                                 // 完整配置描述符的总长度
    }
    if(HubLowSpeed == 0)                                                          
    {
      pSetupReq -> wLengthL = len;                                                 // 完整配置描述符的总长度
    }
    s = HostCtrlTransfer( Com_Buffer, (uint8_t *)&len );                                // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    return( ERR_SUCCESS );
}
