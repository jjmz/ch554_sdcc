#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : CopySetupReqPkg
* Description    : 复制控制传输的请求包
* Input          : P__code uint8_t pReqPkt 控制请求包地址
* Output         : None
* Return         : None
*******************************************************************************/
void CopySetupReqPkg( __code uint8_t *pReqPkt )                                        // 复制控制传输的请求包
{
    uint8_t   i;
    if(HubLowSpeed)                                                               //HUB下低速设备
    {
		((__xdata uint8_t *)pSetupReq)[ 0 ] = *pReqPkt;			
		for ( i = 1; i != sizeof( USB_SETUP_REQ )+1; i ++ )
		{
			((__xdata uint8_t *)pSetupReq)[ i ] = *pReqPkt;
			pReqPkt++;
		}
	}
    if(HubLowSpeed == 0)
    {
		for ( i = 0; i != sizeof( USB_SETUP_REQ ); i ++ )
		{
			((__xdata uint8_t *)pSetupReq)[ i ] = *pReqPkt;
			pReqPkt++;
		}			
    }
}
