#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : EnableRootHubPort( )
* Description    : 使能ROOT-HUB端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
* Input          : None
* Output         : None
* Return         : 返回ERR_SUCCESS为检测到新连接,返回ERR_USB_DISCON为无连接
*******************************************************************************/
uint8_t   EnableRootHubPort( )
{
	if ( ThisUsbDev.DeviceStatus < ROOT_DEV_CONNECTED ) ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;
	if ( USB_MIS_ST & bUMS_DEV_ATTACH ) {                                        // 有设备
		if ( ( UHOST_CTRL & bUH_PORT_EN ) == 0x00 ) {                              // 尚未使能
			ThisUsbDev.DeviceSpeed = USB_MIS_ST & bUMS_DM_LEVEL ? 0 : 1;
			if ( ThisUsbDev.DeviceSpeed == 0 ) UHOST_CTRL |= bUH_LOW_SPEED;          // 低速
		}
		USB_CTRL |= bUC_DMA_EN;                                                    // 启动USB主机及DMA,在中断标志未清除前自动暂停
		UH_SETUP = bUH_SOF_EN;		
		UHOST_CTRL |= bUH_PORT_EN;                                                 //使能HUB端口
		return( ERR_SUCCESS );
	}
	return( ERR_USB_DISCON );
}
