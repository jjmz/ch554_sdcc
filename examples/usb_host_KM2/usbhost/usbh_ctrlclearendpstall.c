#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : CtrlClearEndpStall
* Description    : 清除端点STALL
* Input          : uint8_t endp       端点地址
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
uint8_t   CtrlClearEndpStall( uint8_t endp )  
{
    CopySetupReqPkg( SetupClrEndpStall );                                      // 清除端点的错误
    if(HubLowSpeed)                                                               //HUB下低速设备
    {		
      pSetupReq -> wIndexH = endp;                                               // 端点地址
    }
    if(HubLowSpeed == 0)                                                          
    {		
      pSetupReq -> wIndexL = endp;                                               // 端点地址
    }	
    return( HostCtrlTransfer( NULL, NULL ) );                                  // 执行控制传输
}
