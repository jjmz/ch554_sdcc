#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : DisableRootHubPort( )
* Description    : 关闭HUB端口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  DisableRootHubPort( )          
{
	ThisUsbDev.DeviceStatus = ROOT_DEV_DISCONNECT;
    ThisUsbDev.DeviceAddress = 0x00;
}

