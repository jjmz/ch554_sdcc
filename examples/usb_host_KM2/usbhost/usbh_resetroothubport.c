#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : ResetRootHubPort( )
* Description    : 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
* Input          : None   
* Output         : None
* Return         : None
*******************************************************************************/
void  ResetRootHubPort( )
{
    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;                                      //USB设备的端点0的最大包尺寸
    memset( &ThisUsbDev,0,sizeof(ThisUsbDev));                                 //清空结构体
	SetHostUsbAddr( 0x00 );
    UHOST_CTRL &= ~bUH_PORT_EN;                                                // 关掉端口
	SetUsbSpeed( 1 );                                                          // 默认为全速
	UHOST_CTRL = UHOST_CTRL & ~ bUH_LOW_SPEED | bUH_BUS_RESET;                 // 默认为全速,开始复位
    mDelaymS( 20 );                                                            // 复位时间10mS到20mS
    UHOST_CTRL = UHOST_CTRL & ~ bUH_BUS_RESET;                                 // 结束复位
    mDelayuS( 250 );
    UIF_DETECT = 0;                                                            // 清中断标志
}
