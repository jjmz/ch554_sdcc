#include <ch554.h>
#include <debug.h>
#include "usbhost.h"
#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : SETorOFFNumLock
* Description    : NumLock的点灯判断
* Input          : Puint8_t buf 点灯键值
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t SETorOFFNumLock(uint8_t *buf)
{
    uint8_t tmp[]= {0x21,0x09,0x00,0x02,0x00,0x00,0x01,0x00};
    uint8_t len,s;
    if((buf[2]==0x53)&(buf[0]|buf[1]|buf[3]|buf[4]|buf[5]|buf[6]|buf[7]==0))
    {			
        if(HubLowSpeed)                                                               //HUB下低速设备
        {	
			((__xdata uint8_t *)pSetupReq)[ 0 ] = 0X21;					
			for ( s = 1; s != sizeof( tmp )+1; s ++ )
			{
				((__xdata uint8_t *)pSetupReq)[ s ] = tmp[s];
			}
        }
        if(HubLowSpeed == 0)                                                          
        {		
			for ( s = 0; s != sizeof( tmp ); s ++ )
			{
				((__xdata uint8_t *)pSetupReq)[ s ] = tmp[s];
			}
        }	
        s = HostCtrlTransfer( Com_Buffer, &len );                                     // 执行控制传输
        if ( s != ERR_SUCCESS )
        {
            return( s );
        }
    }
    return( ERR_SUCCESS );
}

