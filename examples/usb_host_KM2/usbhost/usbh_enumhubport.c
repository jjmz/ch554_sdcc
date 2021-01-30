#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : EnumHubPort
* Description    : 枚举指定ROOT-HUB端口上的外部HUB集线器的各个端口,检查各端口有无连接或移除事件并初始化二级USB设备
* Input          : uint8_t RootHubIndex ROOT_HUB0和ROOT_HUB1
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        失败
*******************************************************************************/
uint8_t EnumHubPort( ) 
{
    uint8_t   i, s;

    for ( i = 1; i <= ThisUsbDev.GpHUBPortNum; i ++ )                                       // 查询集线器的端口是否有变化
    {
        SelectHubPort( 0 );                                                          // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
        s = HubGetPortStatus( i );                                                   // 获取端口状态
        if ( s != ERR_SUCCESS )
        {
            return( s );                                                              // 可能是该HUB断开了
        }
        if ( (( Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07)) ) && ( Com_Buffer[2]&(1<<(HUB_C_PORT_CONNECTION&0x07)) ))||(Com_Buffer[2] == 0x10) ) 
        {                                                                            // 发现有设备连接
			DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_CONNECTED;                     // 有设备连接
            DevOnHubPort[i-1].DeviceAddress = 0x00;
            s = HubGetPortStatus( i );                                               // 获取端口状态
            if ( s != ERR_SUCCESS )
            {
                return( s );                                                         // 可能是该HUB断开了
            }
            DevOnHubPort[i-1].DeviceSpeed = Com_Buffer[1] & (1<<(HUB_PORT_LOW_SPEED&0x07)) ? 0 : 1;// 低速还是全速
            if ( DevOnHubPort[i-1].DeviceSpeed )
            {
#if DE_PRINTF								
                //printf( "Found full speed device on port %1d\n", (uint16_t)i );
		printstr( "Found full speed device on port "); printhex2(i); printlf();
#endif							
            }
            else
            {
#if DE_PRINTF								
                //printf( "Found low speed device on port %1d\n", (uint16_t)i );
		printstr( "Found low speed device on port "); printhex2(i); printlf();
#endif							
            }
            mDelaymS( 200 );                                                          // 等待设备上电稳定
            s = HubSetPortFeature( i, HUB_PORT_RESET );                               // 对有设备连接的端口复位
            if ( s != ERR_SUCCESS )
            {
                return( s );                                                          // 可能是该HUB断开了
            }
#if DE_PRINTF							
            //printf( "Reset port and then wait in\n" );
            printstr( "Reset port and then wait in\n" );
#endif						
            do                                                                        // 查询复位端口,直到复位完成,把完成后的状态显示出来
            {
                mDelaymS( 1 );
                s = HubGetPortStatus( i );
                if ( s != ERR_SUCCESS )
                {
                    return( s );                                                      // 可能是该HUB断开了
                }
            }
            while ( Com_Buffer[0] & (1<<(HUB_PORT_RESET&0x07)) );                       // 端口正在复位则等待
            mDelaymS( 100 );
            s = HubClearPortFeature( i, HUB_C_PORT_RESET );                           // 清除复位完成标志
//             s = HubSetPortFeature( i, HUB_PORT_ENABLE );                              // 启用HUB端口
            s = HubClearPortFeature( i, HUB_C_PORT_CONNECTION );                      // 清除连接或移除变化标志
            if ( s != ERR_SUCCESS )
            {
                return( s );
            }
            s = HubGetPortStatus( i );                                                // 再读取状态,复查设备是否还在
            if ( s != ERR_SUCCESS )
            {
                return( s );
            }
            if ( ( Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07)) ) == 0 )
            {
                DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT;                 // 设备不在了
            }
            s = InitDevOnHub( i );                                                    // 初始化二级USB设备
            if ( s != ERR_SUCCESS )
            {
                return( s );
            }
            SetUsbSpeed( 1 );                                                         // 默认为全速
        }
		else if (Com_Buffer[2]&(1<<(HUB_C_PORT_ENABLE&0x07))  )                         // 设备连接出错
		{
			HubClearPortFeature( i, HUB_C_PORT_ENABLE );                              // 清除连接错误标志
#if DE_PRINTF						
			//printf( "Device on port error\n" );		
			printstr( "Device on port error\n" );		
#endif					
			s = HubSetPortFeature( i, HUB_PORT_RESET );                               // 对有设备连接的端口复位
			if ( s != ERR_SUCCESS ) 
			return( s );                                                              // 可能是该HUB断开了
			do                                                                        // 查询复位端口,直到复位完成,把完成后的状态显示出来
			{
				mDelaymS( 1 );
				s = HubGetPortStatus( i );
				if ( s != ERR_SUCCESS ) return( s );                                    // 可能是该HUB断开了
			} while ( Com_Buffer[0] & (1<<(HUB_PORT_RESET&0x07)) );                     // 端口正在复位则等待
		}
        else if ( ( Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07)) ) == 0 )              // 设备已经断开
        {
            if ( DevOnHubPort[i-1].DeviceStatus >= ROOT_DEV_CONNECTED )
            {
#if DE_PRINTF								
                //printf( "Device on port %1d removed\n", (uint16_t)i );
                printstr( "Device on port ");printhex2(i);printstr("removed\n");
#endif							
            }
            DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT;                     // 有设备连接
            if ( Com_Buffer[2]&(1<<(HUB_C_PORT_CONNECTION&0x07)) )
            {
                HubClearPortFeature( i, HUB_C_PORT_CONNECTION );                      // 清除移除变化标志
            }
        }
    }
    return( ERR_SUCCESS );                                                            // 返回操作成功
}

