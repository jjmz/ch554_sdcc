#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>


/*******************************************************************************
* Function Name  : AnalyzeHidIntEndp
* Description    : 从描述符中分析出HID中断端点的地址,如果HubPortIndex是0保存到ROOTHUB，如果是非零值则保存到HUB下结构体
* Input          : P__xdata uint8_t buf ： 待分析数据缓冲区地址 HubPortIndex：0表示根HUB，非0表示外部HUB下的端口号
* Output         : None
* Return         : 端点数
*******************************************************************************/
uint8_t   AnalyzeHidIntEndp( __xdata uint8_t *buf, uint8_t HubPortIndex ) 
{
    uint8_t   i, s, l;
    s = 0;

	if(HubPortIndex)
		memset( DevOnHubPort[HubPortIndex-1].GpVar,0,sizeof(DevOnHubPort[HubPortIndex-1].GpVar) ); //清空数组
	else
		memset( ThisUsbDev.GpVar,0,sizeof(ThisUsbDev.GpVar) );                     //清空数组

    for ( i = 0; i < ( (PXUSB_CFG_DESCR)buf ) -> wTotalLengthL; i += l )       // 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bDescriptorType == USB_DESCR_TYP_ENDP  // 是端点描述符
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_INTER// 是中断端点
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK ) )// 是IN端点
        {           // 保存中断端点的地址,位7用于同步标志位,清0
            if(HubPortIndex)
				DevOnHubPort[HubPortIndex-1].GpVar[s] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
			else
				ThisUsbDev.GpVar[s] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;// 中断端点的地址，可以根据需要保存wMaxPacketSize和bInterval                                                          
#if DE_PRINTF			
			//printf("%02x ",(uint16_t)ThisUsbDev.GpVar[s]);
			printhex2(ThisUsbDev.GpVar[s]); putchar(' ');
#endif
			s++;
			if(s >= 4) break;	//只分析4个端点

		}
        l = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bLength;                          // 当前描述符长度,跳过
        if ( l > 16 )
        {
            break;
        }
    }
#if DE_PRINTF
	//printf("\n");
	printlf();
#endif	
    return( s );
}

