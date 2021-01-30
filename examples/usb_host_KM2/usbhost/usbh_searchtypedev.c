#include <ch554.h>
#include <debug.h>
#include "usbhost.h"
#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : SearchTypeDevice
* Description    : 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到
* Input          : uint8_t type 搜索的设备类型
* Output         : None
* Return         : 输出高8位为ROOT-HUB端口号,低8位为外部HUB的端口号,低8位为0则设备直接在ROOT-HUB端口上
                   当然也可以根据USB的厂商VID产品PID进行搜索(事先要记录各设备的VID和PID),以及指定搜索序号
*******************************************************************************/
uint16_t  SearchTypeDevice( uint8_t type )   
{
	uint8_t  RootHubIndex;                                                          //CH554只有一个USB口,RootHubIndex = 0,只需看返回值的低八位即可
    uint8_t  HubPortIndex;

	RootHubIndex = 0;
	if ( (ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS) )// 外部集线器HUB且枚举成功
	{
		for ( HubPortIndex = 1; HubPortIndex <= ThisUsbDev.GpHUBPortNum; HubPortIndex ++ )// 搜索外部HUB的各个端口
		{
			if ( DevOnHubPort[HubPortIndex-1].DeviceType == type && DevOnHubPort[HubPortIndex-1].DeviceStatus >= ROOT_DEV_SUCCESS )
			{
				return( ( (uint16_t)RootHubIndex << 8 ) | HubPortIndex );           // 类型匹配且枚举成功
			}
		}
	}
	if ( (ThisUsbDev.DeviceType == type) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS) )
	{
		return( (uint16_t)RootHubIndex << 8 );                                      // 类型匹配且枚举成功,在ROOT-HUB端口上
	} 

    return( 0xFFFF );
}

