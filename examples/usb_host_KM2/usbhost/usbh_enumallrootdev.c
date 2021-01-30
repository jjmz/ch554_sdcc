#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : EnumAllRootDevice
* Description    : 枚举所有ROOT-HUB端口的USB设备
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t   EnumAllRootDevice( void )   
{
    __idata uint8_t   s;
#if DE_PRINTF	
    //printf( "EnumUSBDev\n" );
    printstr( "EnumUSBDev\n" );
#endif
	if ( ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED )                        // 刚插入设备尚未初始化
	{
		s = InitRootDevice( );                                      // 初始化/枚举指定HUB端口的USB设备
		if ( s != ERR_SUCCESS )
		{
			return( s );
		}
	}
    return( ERR_SUCCESS );
}

