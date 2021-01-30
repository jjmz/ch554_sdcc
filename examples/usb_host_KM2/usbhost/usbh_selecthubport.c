#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : SelectHubPort( uint8_t HubPortIndex )
* Description    : 选定需要操作的HUB口
* Input          : uint8_t HubPortIndex 选择操作指定的ROOT-HUB端口的外部HUB的指定端口
* Output         : None
* Return         : None
*******************************************************************************/
void    SelectHubPort( uint8_t HubPortIndex )  
{ 
    if( HubPortIndex )                                                         // 选择操作指定的ROOT-HUB端口的外部HUB的指定端口
    {
        SetHostUsbAddr( DevOnHubPort[HubPortIndex-1].DeviceAddress );          // 设置USB主机当前操作的USB设备地址
        SetUsbSpeed( DevOnHubPort[HubPortIndex-1].DeviceSpeed );               // 设置当前USB速度
		if ( DevOnHubPort[HubPortIndex-1].DeviceSpeed == 0 )                   // 通过外部HUB与低速USB设备通讯需要前置ID
        {
            UH_SETUP |= bUH_PRE_PID_EN;                                        // 启用PRE PID
            HubLowSpeed = 1;
			mDelayuS(100);
        }
    }
    else                                                                       
    {
        HubLowSpeed = 0;        			
        SetHostUsbAddr( ThisUsbDev.DeviceAddress );                            // 设置USB主机当前操作的USB设备地址
        SetUsbSpeed( ThisUsbDev.DeviceSpeed );                                 // 设置USB设备的速度
    }
}

