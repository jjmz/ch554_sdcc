#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : InitDevOnHub
* Description    : 初始化枚举外部HUB后的二级USB设备
* Input          : uint8_t HubPortIndex  指定外部HUB
* Output         : None
* Return         : ERR_SUCCESS 成功
                   ERR_USB_UNKNOWN 未知设备                 
*******************************************************************************/
uint8_t InitDevOnHub( uint8_t HubPortIndex ) 
{
    uint8_t   i, s, cfg, dv_cls, if_cls;
    uint8_t   ifc;
#if DE_PRINTF		
    //printf( "Init dev @ExtHub-port_%1d ", (uint16_t)HubPortIndex );
    printstr( "Init dev @ExtHub-port_"); printhex2(HubPortIndex);
#endif
    if ( HubPortIndex == 0 )
    {
        return( ERR_USB_UNKNOWN );
    }
    SelectHubPort( HubPortIndex );                                      // 选择操作指定的ROOT-HUB端口的外部HUB的指定端口,选择速度
#if DE_PRINTF		
    //printf( "GetDevDescr: " );
    printstr( "GetDevDescr: " );
#endif
    s = CtrlGetDeviceDescr( );                                          // 获取设备描述符
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
	DevOnHubPort[HubPortIndex-1].DeviceVID = (((uint16_t)((PXUSB_DEV_DESCR)Com_Buffer)->idVendorH)<<8 ) + ((PXUSB_DEV_DESCR)Com_Buffer)->idVendorL; //保存VID PID信息
	DevOnHubPort[HubPortIndex-1].DevicePID = (((uint16_t)((PXUSB_DEV_DESCR)Com_Buffer)->idProductH)<<8 ) + ((PXUSB_DEV_DESCR)Com_Buffer)->idProductL;

    dv_cls = ( (PXUSB_DEV_DESCR)Com_Buffer ) -> bDeviceClass;             // 设备类代码
    cfg = ( 1<<4 ) + HubPortIndex;                                      // 计算出一个USB地址,避免地址重叠
    s = CtrlSetUsbAddress( cfg );                                       // 设置USB设备地址
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    DevOnHubPort[HubPortIndex-1].DeviceAddress = cfg;                   // 保存分配的USB地址
#if DE_PRINTF			
    //printf( "GetCfgDescr: " );
    printstr( "GetCfgDescr: " );
#endif
    s = CtrlGetConfigDescr( );                                          // 获取配置描述符
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    cfg = ( (PXUSB_CFG_DESCR)Com_Buffer ) -> bConfigurationValue;
#if DE_PRINTF			
    for ( i = 0; i < ( (PXUSB_CFG_DESCR)Com_Buffer ) -> wTotalLengthL; i ++ )
    {
        //printf( "x%02X ", (uint16_t)( Com_Buffer[i] ) );
	putchar('x'); printhex2(Com_Buffer[i]); putchar(' ');
    }
    //printf("\n");
    printlf();
#endif		
    /* 分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等 */
    if_cls = ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceClass; // 接口类代码
    if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE )                  // 是USB存储类设备,基本上确认是U盘
    {
		AnalyzeBulkEndp(Com_Buffer , HubPortIndex );
#if DE_PRINTF
		for(i=0;i!=4;i++)
		{
			//printf("%02x ",(uint16_t)DevOnHubPort[HubPortIndex-1].GpVar[i] );
			printhex2(DevOnHubPort[HubPortIndex-1].GpVar[i]); putchar(' ');
		}
		//printf("\n");
		printlf();
#endif
        s = CtrlSetUsbConfig( cfg );                                          // 设置USB设备配置
        if ( s == ERR_SUCCESS )
        {
            DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_STORAGE;
#if DE_PRINTF						
            //printf( "USB-Disk Ready\n" );
            printstr( "USB-Disk Ready\n" );
#endif					
            SetUsbSpeed( 1 );                                                 // 默认为全速
            return( ERR_SUCCESS );
        }
    }
    else if ( (dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && (( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceSubClass <= 0x01) )    // 是HID类设备,键盘/鼠标等
    {
        ifc = ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> cfg_descr.bNumInterfaces;			
        s = AnalyzeHidIntEndp( Com_Buffer, HubPortIndex);                                     // 从描述符中分析出HID中断端点的地址
#if DE_PRINTF														 
        //printf( "AnalyzeHidIntEndp %02x\n",(uint16_t)s );
        printstr( "AnalyzeHidIntEndp "); printhex2(s); printlf();
#endif	
        if_cls = ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceProtocol;
        s = CtrlSetUsbConfig( cfg );                                           // 设置USB设备配置
        if ( s == ERR_SUCCESS )
        {
			for(dv_cls=0;dv_cls<ifc;dv_cls++)
			{											
				s = CtrlGetHIDDeviceReport(dv_cls);                    //获取报表描述符
				if(s == ERR_SUCCESS)
				{
#if DE_PRINTF														
					for ( i = 0; i < 64; i++ )
					{
						//printf( "x%02X ", (uint16_t)( Com_Buffer[i] ) );
						putchar('x'); printhex2( Com_Buffer[i] ); putchar(' ');
					}
					//printf("\n");
					printlf();
#endif														
				}
			}					
            //需保存端点信息以便主程序进行USB传输
            DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
			if ( if_cls == 1 )
			{
				DevOnHubPort[HubPortIndex-1].DeviceType = DEV_TYPE_KEYBOARD;
															 //进一步初始化,例如设备键盘指示灯LED等
				if(ifc > 1)
				{
#if DE_PRINTF														
					//printf( "USB_DEV_CLASS_HID Ready\n" );
					printstr( "USB_DEV_CLASS_HID Ready\n" );
#endif																
					DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HID;//复合HID设备															
				}																												
#if DE_PRINTF														
				//printf( "USB-Keyboard Ready\n" );
				printstr( "USB-Keyboard Ready\n" );
#endif													
				SetUsbSpeed( 1 );                            // 默认为全速

				return( ERR_SUCCESS );
			}
			else if ( if_cls == 2 )
			{
				DevOnHubPort[HubPortIndex-1].DeviceType = DEV_TYPE_MOUSE;
															 //为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
				if(ifc > 1)
				{
#if DE_PRINTF														
					//printf( "USB_DEV_CLASS_HID Ready\n" );
					printstr( "USB_DEV_CLASS_HID Ready\n" );
#endif																
					DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HID;//复合HID设备															
				}															
#if DE_PRINTF													
				//printf( "USB-Mouse Ready\n" );
				printstr( "USB-Mouse Ready\n" );
#endif													
				SetUsbSpeed( 1 );                            // 默认为全速

				return( ERR_SUCCESS );
			}
			s = ERR_USB_UNSUPPORT;			
        }
    }
    else if ( dv_cls == USB_DEV_CLASS_HUB )                                     // 是HUB类设备,集线器等
    {
        DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HUB;
#if DE_PRINTF				
        //printf( "This program don't support Level 2 HUB\n");                    // 需要支持多级HUB级联请参考本程序进行扩展
        printstr( "This program don't support Level 2 HUB\n");                    // 需要支持多级HUB级联请参考本程序进行扩展
#endif		
        s = HubClearPortFeature( i, HUB_PORT_ENABLE );                          // 禁止HUB端口
        if ( s != ERR_SUCCESS )
        {
            return( s );
        }
        s = ERR_USB_UNSUPPORT;
    }
	else                                                                //其他设备
	{
		AnalyzeBulkEndp(Com_Buffer , HubPortIndex );                      //分析出批量端点
#if DE_PRINTF
		for(i=0;i!=4;i++)
		{
			//printf("%02x ",(uint16_t)DevOnHubPort[HubPortIndex-1].GpVar[i] );
			printhex2(DevOnHubPort[HubPortIndex-1].GpVar[i]);putchar(' ');
		}
		//printf("\n");
		printlf();
#endif	
		s = CtrlSetUsbConfig( cfg );                                     // 设置USB设备配置
		if ( s == ERR_SUCCESS ) 
		{
			//需保存端点信息以便主程序进行USB传输
			DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
			DevOnHubPort[HubPortIndex-1].DeviceType = dv_cls ? dv_cls : if_cls;
            SetUsbSpeed( 1 );                                                    // 默认为全速
            return( ERR_SUCCESS );                                               //未知设备初始化成功
		}		
	}
#if DE_PRINTF			
    //printf( "InitDevOnHub Err = %02X\n", (uint16_t)s );
    printstr( "InitDevOnHub Err = "); printhex2(s);printlf();
#endif		
    DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_FAILED;
    SetUsbSpeed( 1 );                                                            // 默认为全速
    return( s );
}

