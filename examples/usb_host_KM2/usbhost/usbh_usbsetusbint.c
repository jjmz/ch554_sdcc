#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : CtrlSetUsbIntercace
* Description    : 设置USB设备接口
* Input          : uint8_t cfg       配置值
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
uint8_t   CtrlSetUsbIntercace( uint8_t cfg )                   
{
    CopySetupReqPkg( SetupSetUsbInterface );
    if(HubLowSpeed)                                                               //HUB下低速设备
    {		
      pSetupReq -> wValueH = cfg;                                                 // USB设备配置
    }
    if(HubLowSpeed == 0)                                                          
    {		
      pSetupReq -> wValueL = cfg;                                                 // USB设备配置
    }		
    return( HostCtrlTransfer( NULL, NULL ) );                             // 执行控制传输
}
