#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : AnalyzeRootHub(void)
* Description    : 分析ROOT-HUB状态,处理ROOT-HUB端口的设备插拔事件
                   如果设备拔出,函数中调用DisableRootHubPort()函数,将端口关闭,插入事件,置相应端口的状态位
* Input          : None
* Output         : None
* Return         : 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
*******************************************************************************/
uint8_t   AnalyzeRootHub( void )
{ 
	uint8_t	s;
	s = ERR_SUCCESS;
	if ( USB_MIS_ST & bUMS_DEV_ATTACH ) {                                        // 设备存在
		if ( ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT                        // 检测到有设备插入
			|| ( UHOST_CTRL & bUH_PORT_EN ) == 0x00 ) {                              // 检测到有设备插入,但尚未允许,说明是刚插入
			DisableRootHubPort( );                                                   // 关闭端口
//		ThisUsbDev.DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
			ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;                            //置连接标志
#if DE_PRINTF
			//printf( "USB dev in\n" );
			printstr( "USB dev in\n" );
#endif
			s = ERR_USB_CONNECT;
		}
	}
	else if ( ThisUsbDev.DeviceStatus >= ROOT_DEV_CONNECTED ) {                  //检测到设备拔出
		DisableRootHubPort( );                                                     // 关闭端口
#if DE_PRINTF		
		//printf( "USB dev out\n" );
		printstr( "USB dev out\n" );
#endif
		if ( s == ERR_SUCCESS ) s = ERR_USB_DISCON;
	}
//	UIF_DETECT = 0;                                                            // 清中断标志
	return( s );
}
