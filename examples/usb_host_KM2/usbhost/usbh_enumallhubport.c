#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"
#include "../print/small_print.h"
#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : EnumAllHubPort
* Description    : 枚举所有ROOT-HUB端口下外部HUB后的二级USB设备
* Input          : None
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        失败
*******************************************************************************/
uint8_t   EnumAllHubPort( void ) 
{
    uint8_t   s;

	if ( (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS) && (ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) )// HUB枚举成功
	{ 
		SelectHubPort( 0 );                                                        // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
		//做点什么?  给HUB各端口上电,查询各端口状态,初始化有设备连接的HUB端口,初始化设备
//             for ( i = 1; i <= ThisUsbDev.GpVar; i ++ ){                             // 初始化HUB各端口
//               s = HubSetPortFeature( i, HUB_PORT_POWER );                           // 给HUB各端口上电
//               if ( s != ERR_SUCCESS )
//               {
//                 return( s );                                                        // 可能是该HUB断开了
//               }             							
//             }				
		s = EnumHubPort( );                                                        // 枚举指定ROOT-HUB端口上的外部HUB集线器的各个端口,检查各端口有无连接或移除事件
		if ( s != ERR_SUCCESS )                                                    // 可能是HUB断开了
		{
#if DE_PRINTF								
			//printf( "EnumAllHubPort err = %02X\n", (uint16_t)s );
			printstr( "EnumAllHubPort err = "); printhex2(s); printlf();
#endif							
		}
		SetUsbSpeed( 1 );                                                          // 默认为全速
	}
    return( ERR_SUCCESS );
}

