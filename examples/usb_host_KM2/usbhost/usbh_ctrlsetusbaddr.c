#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : CtrlSetUsbAddress
* Description    : 设置USB设备地址
* Input          : uint8_t addr 设备地址
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
uint8_t CtrlSetUsbAddress( uint8_t addr ) 
{
    uint8_t   s;
    CopySetupReqPkg( SetupSetUsbAddr );
    if(HubLowSpeed)                                                               //HUB下低速设备
    {		
      pSetupReq -> wValueH = addr;                                                // USB设备地址
    }
    if(HubLowSpeed == 0)                                                          
    {		
      pSetupReq -> wValueL = addr;                                                // USB设备地址
    }		
    s = HostCtrlTransfer( NULL, NULL );                                         // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    SetHostUsbAddr( addr );                                                     // 设置USB主机当前操作的USB设备地址
    mDelaymS( 10 );                                                             // 等待USB设备完成操作
    return( ERR_SUCCESS );
}
