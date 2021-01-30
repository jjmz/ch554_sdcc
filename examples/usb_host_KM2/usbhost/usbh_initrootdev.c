#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>


/*******************************************************************************
* Function Name  : InitRootDevice
* Description    : 初始化指定ROOT-HUB端口的USB设备
* Input          : uint8_t RootHubIndex 指定端口，内置HUB端口号0/1
* Output         : None
* Return         :
*******************************************************************************/
uint8_t InitRootDevice( void ) 
{
    uint8_t   t,i, s, cfg, dv_cls, if_cls,ifc;
	uint8_t touchaoatm = 0;
    t = 0;
#if DE_PRINTF	
    //printf( "Reset USB Port\n");
    printstr( "Reset USB Port\n");
#endif
USBDevEnum:
    for(i=0;i<t;i++)
    {
        mDelaymS( 100 );	
        if(t>10) return( s );			
    }
    ResetRootHubPort( );                                                    // 检测到设备后,复位相应端口的USB总线
    for ( i = 0, s = 0; i < 100; i ++ )                                     // 等待USB设备复位后重新连接,100mS超时
    {
        mDelaymS( 1 );
        if ( EnableRootHubPort( ) == ERR_SUCCESS )                          // 使能ROOT-HUB端口
        {
            i = 0;
            s ++;                                                           // 计时等待USB设备连接后稳定
            if ( s > (20+t) )
            {
                break;                                                      // 已经稳定连接15mS
            }
        }
    }	
    if ( i )                                                                 // 复位后设备没有连接
    {
        DisableRootHubPort( );
#if DE_PRINTF
        //printf( "Disable usb port because of disconnect\n" );
        printstr( "Disable usb port because of disconnect\n" );
#endif	
//         return( ERR_USB_DISCON );
    }
    SelectHubPort( 0 );
#if DE_PRINTF		
    //printf( "GetDevDescr: " );
    printstr( "GetDevDescr: " );
#endif
    s = CtrlGetDeviceDescr( );                                               // 获取设备描述符
    if ( s == ERR_SUCCESS )
    {
#if DE_PRINTF	
        for ( i = 0; i < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; i ++ )
        {
            //printf( "x%02X ", (uint16_t)( Com_Buffer[i] ) );				
	    putchar('x'); printhex2(Com_Buffer[i]); putchar(' ');
        }
        //printf( "\n" );                                                       // 显示出描述符
	printlf();
#endif	
		ThisUsbDev.DeviceVID = (((uint16_t)((PXUSB_DEV_DESCR)Com_Buffer)->idVendorH)<<8 ) + ((PXUSB_DEV_DESCR)Com_Buffer)->idVendorL; //保存VID PID信息
		ThisUsbDev.DevicePID = (((uint16_t)((PXUSB_DEV_DESCR)Com_Buffer)->idProductH)<<8 ) + ((PXUSB_DEV_DESCR)Com_Buffer)->idProductL;
        dv_cls = ( (PXUSB_DEV_DESCR)Com_Buffer ) -> bDeviceClass;               // 设备类代码			
        s = CtrlSetUsbAddress( ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL );// 设置USB设备地址,加上RootHubIndex可以保证2个HUB端口分配不同的地址
        if ( s == ERR_SUCCESS )
        {
            ThisUsbDev.DeviceAddress = ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL;  // 保存USB地址
#if DE_PRINTF						
            //printf( "GetCfgDescr: " );
            printstr( "GetCfgDescr: " );
#endif					
            s = CtrlGetConfigDescr( );                                        // 获取配置描述符
            if ( s == ERR_SUCCESS )
            {
                cfg = ( (PXUSB_CFG_DESCR)Com_Buffer ) -> bConfigurationValue;
                ifc = ( (PXUSB_CFG_DESCR)Com_Buffer ) -> bNumInterfaces;					
#if DE_PRINTF							
                for ( i = 0; i < ( (PXUSB_CFG_DESCR)Com_Buffer ) -> wTotalLengthL; i ++ )
                {
                    //printf( "x%02X ", (uint16_t)( Com_Buffer[i] ) );
		    putchar('x'); printhex2(Com_Buffer[i]); putchar(' ');
                }
                //printf("\n");
		printlf();
#endif								
                                                                              //分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等
                if_cls = ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceClass;  // 接口类代码								
                if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE )      // 是USB存储类设备,基本上确认是U盘
                {
					AnalyzeBulkEndp(Com_Buffer , 0 );
#if DE_PRINTF
					for(i=0;i!=4;i++)
					{
						//printf("%02x ",(uint16_t)ThisUsbDev.GpVar[i] );
						printhex2(ThisUsbDev.GpVar[i]); putchar(' ');
					}
					//printf("\n");
					printlf();
#endif					
                    s = CtrlSetUsbConfig( cfg );                              // 设置USB设备配置
                    if ( s == ERR_SUCCESS )
                    {
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = USB_DEV_CLASS_STORAGE;
#if DE_PRINTF												
                        //printf( "USB-Disk Ready\n" );
                        printstr( "USB-Disk Ready\n" );
#endif											
                        SetUsbSpeed( 1 );                                     // 默认为全速
                        return( ERR_SUCCESS );
                    }
                }
                else if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_PRINTER && ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceSubClass == 0x01 )// 是打印机类设备
                {
#if DE_PRINTF										
                    //printf( "USB-Print OK\n" );
                    printstr( "USB-Print OK\n" );
#endif									
                    if((Com_Buffer[19] == 5)&&(Com_Buffer[20]&&0x80)){
                       ThisUsbDev.GpVar[0] = Com_Buffer[20];                     //IN 端点											
                    }
                    else if((Com_Buffer[19] == 5)&&((Com_Buffer[20]&&0x80) == 0)){
                       ThisUsbDev.GpVar[1] = Com_Buffer[20];                    //OUT 端点												
                    }		
                    if((Com_Buffer[26] == 5)&&(Com_Buffer[20]&&0x80)){
                       ThisUsbDev.GpVar[0] = Com_Buffer[27];                     //IN 端点											
                    }
                    else if((Com_Buffer[26] == 5)&&((Com_Buffer[20]&&0x80) == 0)){
                       ThisUsbDev.GpVar[1] = Com_Buffer[27];                    //OUT 端点												
                    }										
//                  ThisUsbDev.GpVar = ( (PUSB_CFG_DESCR_LONG)Com_Buffer ) -> endp_descr[0].bEndpointAddress;// 保存批量传输端点
                    s = CtrlSetUsbConfig( cfg );                            // 设置USB设备配置
                    if ( s == ERR_SUCCESS )
                    {									
                        s = CtrlSetUsbIntercace(cfg);
//                         if(s == ERR_SUCCESS){
                                                                           //需保存端点信息以便主程序进行USB传输
                         s = CtrlGetXPrinterReport1( );                    //打印机类命令
                         if(s == ERR_SUCCESS){													 
                           ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                           ThisUsbDev.DeviceType = USB_DEV_CLASS_PRINTER;
#if DE_PRINTF														 
                           //printf( "USB-Print Ready\n" );
                           printstr( "USB-Print Ready\n" );
#endif													 
                           SetUsbSpeed( 1 );                               // 默认为全速
                           return( ERR_SUCCESS );
						 }													 
//                         }
                    }
                }
                else if ( (dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && (( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceSubClass <= 0x01) )// 是HID类设备,键盘/鼠标等
                { 									
                    s = AnalyzeHidIntEndp( Com_Buffer,0 );                    // 从描述符中分析出HID中断端点的地址								
#if DE_PRINTF														 
                    //printf( "AnalyzeHidIntEndp %02x\n",(uint16_t)s );
                    printstr( "AnalyzeHidIntEndp "); printhex2(s); printlf();
#endif		                    
					if_cls = ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceProtocol;
#if DE_PRINTF														 
                    //printf( "CtrlSetUsbConfig %02x\n",(uint16_t)cfg );
                    printstr( "CtrlSetUsbConfig "); printhex2(s); printlf();
#endif		
                    s = CtrlSetUsbConfig( cfg );                          // 设置USB设备配置								
                    if ( s == ERR_SUCCESS )
                    {
#if DE_PRINTF												
                        //printf( "GetHIDReport: " );
                        printstr( "GetHIDReport: " );
#endif			
                        for(dv_cls=0;dv_cls<ifc;dv_cls++)
                        {											
							s = CtrlGetHIDDeviceReport(dv_cls);                    //获取报表描述符
							if(s == ERR_SUCCESS)
							{
#if DE_PRINTF														
								for ( i = 0; i < 64; i++ )
								{
									//printf( "x%02X ", (uint16_t)( Com_Buffer[i] ) );
									putchar('x'); printhex2(Com_Buffer[i]); putchar(' ');
								}
								//printf("\n");
								printlf();
#endif														
							}
						}
                        //Set_Idle( );
                                                                         //需保存端点信息以便主程序进行USB传输
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        if ( if_cls == 1 )
                        {
                            ThisUsbDev.DeviceType = DEV_TYPE_KEYBOARD;
                                                                         //进一步初始化,例如设备键盘指示灯LED等
                            if(ifc > 1)
                            {
#if DE_PRINTF														
								//printf( "USB_DEV_CLASS_HID Ready\n" );
								printstr( "USB_DEV_CLASS_HID Ready\n" );
#endif																
								ThisUsbDev.DeviceType = USB_DEV_CLASS_HID;//复合HID设备															
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
                            ThisUsbDev.DeviceType = DEV_TYPE_MOUSE;
                                                                         //为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
                            if(ifc > 1)
                            {
#if DE_PRINTF														
								//printf( "USB_DEV_CLASS_HID Ready\n" );
								printstr( "USB_DEV_CLASS_HID Ready\n" );
#endif																
								ThisUsbDev.DeviceType = USB_DEV_CLASS_HID;//复合HID设备															
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
                else if ( dv_cls == USB_DEV_CLASS_HUB )                   // 是HUB类设备,集线器等
                {
                    s = AnalyzeHidIntEndp( Com_Buffer,0 );                    // 从描述符中分析出HID中断端点的地址
#if DE_PRINTF		
                    //printf( "AnalyzeHidIntEndp %02x\n",(uint16_t)s );
                    printstr( "AnalyzeHidIntEndp "); printhex2(s); printlf();
#endif	                   			
#if DE_PRINTF										
                    //printf( "GetHubDescr: ");
                    printstr( "GetHubDescr: ");
#endif									
                    s = CtrlGetHubDescr( );
                    if ( s == ERR_SUCCESS )
                    {
#if DE_PRINTF												
                        for( i = 0; i < Com_Buffer[0]; i++ )
                        {
                            //printf( "x%02X ",(uint16_t)(Com_Buffer[i]) );
			    putchar('x'); printhex2(Com_Buffer[i]); putchar(' ');
                        }                       
			//printf("\n");
			printlf();
#endif												
                        ThisUsbDev.GpHUBPortNum = ( (PXUSB_HUB_DESCR)Com_Buffer ) -> bNbrPorts;// 保存HUB的端口数量
                        if ( ThisUsbDev.GpHUBPortNum > HUB_MAX_PORTS )
                        {
                            ThisUsbDev.GpHUBPortNum = HUB_MAX_PORTS;// 因为定义结构DevOnHubPort时人为假定每个HUB不超过HUB_MAX_PORTS个端口
                        }
                        //if ( ( (PXUSB_HUB_DESCR)Com_Buffer ) -> wHubCharacteristics[0] & 0x04 ) printf("带有集线器的复合设备\n");
                        //else printf("单一的集线器产品\n");
                        s = CtrlSetUsbConfig( cfg );                     // 设置USB设备配置
                        if ( s == ERR_SUCCESS )
                        {
                            ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                            ThisUsbDev.DeviceType = USB_DEV_CLASS_HUB;
                            //需保存端点信息以便主程序进行USB传输,本来中断端点可用于HUB事件通知,但本程序使用查询状态控制传输代替
                            //给HUB各端口上电,查询各端口状态,初始化有设备连接的HUB端口,初始化设备
                            for ( i = 1; i <= ThisUsbDev.GpHUBPortNum; i ++ )// 给HUB各端口都上电
                            {
                                DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT;  // 清外部HUB端口上设备的状态
                                s = HubSetPortFeature( i, HUB_PORT_POWER );
                                if ( s != ERR_SUCCESS )
                                {
#if DE_PRINTF																		
                                    //printf( "Ext-HUB Port_%1d# power on error\n",(uint16_t)i );// 端口上电失败
                                    printstr( "Ext-HUB Port_"); printhex2(i); printstr(" power on error\n");
#endif																	
                                }
                            }
//							for ( i = 1; i <= ThisUsbDev.GpVar[0]; i ++ )            // 查询HUB各端口连接状态
//							{
//								s = HubGetPortStatus( i );                           // 获取端口状态
//								if ( s != ERR_SUCCESS ) 
//#if DE_PRINTF																	
//								printf( "Ext-HUB Port_%1d#	clear connection error\n",(uint16_t)i );	// 端口连接状态清除失败
//#endif															
//							}														
                            SetUsbSpeed( 1 );                                        // 默认为全速
                            return( ERR_SUCCESS );
                        }
                    }
                }
                else                                                                 //其他设备
                {			
#if DE_PRINTF														 
                    //printf( "dv_cls %02x\n",(uint16_t)dv_cls );
                    printstr( "dv_cls "); printhex2(dv_cls );printlf();
                    //printf( "if_cls %02x\n",(uint16_t)if_cls );
                    printstr( "if_cls "); printhex2(if_cls );printlf();
                    //printf( "if_subcls %02x\n",(uint16_t)( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceSubClass );
                    printstr( "if_subcls ");printhex2( ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceSubClass );printlf();	
#endif				
					AnalyzeBulkEndp(Com_Buffer , 0 );                                  //分析出批量端点
#if DE_PRINTF
					for(i=0;i!=4;i++)
					{
						//printf("%02x ",(uint16_t)ThisUsbDev.GpVar[i] );
						printhex2(ThisUsbDev.GpVar[i]); putchar(' ');
					}
					//printf("\n");
					printlf();
#endif
                    s = CtrlSetUsbConfig( cfg );                                     // 设置USB设备配置
                    if ( s == ERR_SUCCESS ) 
                    {
#if DE_PRINTF						
						//printf("%02x %02x\n",(uint16_t)ThisUsbDev.DeviceVID,(uint16_t)ThisUsbDev.DevicePID);
						printhex2(ThisUsbDev.DeviceVID);putchar(' ');printhex2(ThisUsbDev.DevicePID);printlf();
#endif						
						if((ThisUsbDev.DeviceVID==0x18D1)&&(ThisUsbDev.DevicePID&0xff00)==0x2D00)   //如果是AOA配件
						{
#if DE_PRINTF						
							//printf("AOA Mode\n");
							printstr("AOA Mode\n");
#endif
							ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
							ThisUsbDev.DeviceType = DEF_AOA_DEVICE;                      //这只是自定义的变量类，不属于USB协议类
							SetUsbSpeed( 1 );                                            // 默认为全速
							return( ERR_SUCCESS );
						}
						else   //如果不是AOA 配件模式，尝试启动配件模式.
						{
							s = TouchStartAOA();
							if( s == ERR_SUCCESS ) 
							{
								if(touchaoatm<3)         //尝试AOA启动次数限制
								{
									touchaoatm++;
									mDelaymS(500);      //部分安卓设备自动断开重连，所以此处最好有延时
									goto USBDevEnum;    //其实这里可以不用跳转，AOA协议规定，设备会自动重新接入总线。
								}
								//执行到这，说明可能不支持AOA，或是其他设备
								ThisUsbDev.DeviceType = dv_cls ? dv_cls : if_cls;
								ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
								SetUsbSpeed( 1 );                                            // 默认为全速
								return( ERR_SUCCESS );                                       // 未知设备初始化成功									
							}							
						}
                    }
                }
            }
        }
    }
#if DE_PRINTF			
    //printf( "InitRootDev Err = %02X\n", (uint16_t)s );
    printstr( "InitRootDev Err = "); printhex2(s); printlf();
#endif		
    ThisUsbDev.DeviceStatus = ROOT_DEV_FAILED;
    SetUsbSpeed( 1 );                                                                 // 默认为全速
    t++;
    goto USBDevEnum;		
}

