#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>


/*******************************************************************************
* Function Name  : AnalyzeBulkEndp
* Description    : 分析出批量端点,GpVar[0]、GpVar[1]存放上传端点。GpVar[2]、GpVar[3]存放下传端点
* Input          : buf：待分析数据缓冲区地址   HubPortIndex：0表示根HUB，非0表示外部HUB下的端口号
* Output         : None
* Return         : 0
*******************************************************************************/
uint8_t   AnalyzeBulkEndp( __xdata uint8_t *buf, uint8_t HubPortIndex ) 
{
    uint8_t   i, s1,s2, l;
    s1 = 0;s2 = 2;

	if(HubPortIndex)
		memset( DevOnHubPort[HubPortIndex-1].GpVar,0,sizeof(DevOnHubPort[HubPortIndex-1].GpVar) ); //清空数组
	else
		memset( ThisUsbDev.GpVar,0,sizeof(ThisUsbDev.GpVar) );                     //清空数组

    for ( i = 0; i < ( (PXUSB_CFG_DESCR)buf ) -> wTotalLengthL; i += l )       // 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if ( (( (PXUSB_ENDP_DESCR)(buf+i) ) -> bDescriptorType == USB_DESCR_TYP_ENDP)     // 是端点描述符
                && ((( (PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_BULK))  // 是中断端点

        {
            if(HubPortIndex)
			{
				if(( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK )
					DevOnHubPort[HubPortIndex-1].GpVar[s1++] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
				else
					DevOnHubPort[HubPortIndex-1].GpVar[s2++] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
			}
			else
			{
				if(( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK )
					ThisUsbDev.GpVar[s1++] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
				else
					ThisUsbDev.GpVar[s2++] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
			}

			if(s1 == 2) s1 = 1;
			if(s2 == 4) s2 = 3;			
		}
        l = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bLength;                          // 当前描述符长度,跳过
        if ( l > 16 )
        {
            break;
        }
    }
    return( 0 );
}

