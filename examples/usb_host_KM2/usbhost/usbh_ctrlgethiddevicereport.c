#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : CtrlGetHIDDeviceReport
* Description    : 获取HID设备报表描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        错误
*******************************************************************************/
uint8_t   CtrlGetHIDDeviceReport( uint8_t infc )
{
    uint8_t   s;
    uint8_t   len;

        CopySetupReqPkg( SetupSetHIDIdle );
    if(HubLowSpeed)                                                               //HUB下低速设备
    {
                TxBuffer[5] = infc;
    }
    else
    {
                TxBuffer[4] = infc;
    }
    s = HostCtrlTransfer( Com_Buffer, (uint8_t *)&len );                                    // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }

        CopySetupReqPkg( SetupGetHIDDevReport );
    if(HubLowSpeed)                                                               //HUB下低速设备
    {
                TxBuffer[5] = infc;
    }
    else
    {
                TxBuffer[4] = infc;
    }
    s = HostCtrlTransfer( Com_Buffer, (uint8_t *)&len );                                    // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }

    return( ERR_SUCCESS );
}

