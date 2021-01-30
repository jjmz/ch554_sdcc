#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : USBHostTransact
* Description    : CH554传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
                   本子程序着重于易理解,而在实际应用中,为了提供运行速度,应该对本子程序代码进行优化
* Input          : uint8_t endp_pid 令牌和地址  endp_pid: 高4位是token_pid令牌, 低4位是端点地址
                   uint8_t tog      同步标志
                   uint16_t timeout 超时时间
* Output         : None
* Return         : ERR_USB_UNKNOWN 超时，可能硬件异常
                   ERR_USB_DISCON  设备断开
                   ERR_USB_CONNECT 设备连接
                   ERR_SUCCESS     传输完成
*******************************************************************************/
uint8_t   USBHostTransact( uint8_t endp_pid, uint8_t tog, uint16_t timeout )
{
//	uint8_t	TransRetry;
#define	TransRetry	UEP0_T_LEN	                                               // 节约内存
	uint8_t	s, r;
	uint16_t	i;
	UH_RX_CTRL = UH_TX_CTRL = tog;
	TransRetry = 0;

	do {
		UH_EP_PID = endp_pid;                                                      // 指定令牌PID和目的端点号
		UIF_TRANSFER = 0;                                                          // 允许传输
//  s = WaitUSB_Interrupt( );
		for ( i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- );
		UH_EP_PID = 0x00;                                                          // 停止USB传输
//	if ( s != ERR_SUCCESS ) return( s );  // 中断超时,可能是硬件异常
		if ( UIF_TRANSFER == 0 ) return( ERR_USB_UNKNOWN );
		if ( UIF_DETECT ) {                                                        // USB设备插拔事件
//			mDelayuS( 200 );                                                       // 等待传输完成
			UIF_DETECT = 0;                                                          // 清中断标志
			s = AnalyzeRootHub( );                                                   // 分析ROOT-HUB状态
			if ( s == ERR_USB_CONNECT ) FoundNewDev = 1;
			if ( ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT ) return( ERR_USB_DISCON );// USB设备断开事件
			if ( ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED ) return( ERR_USB_CONNECT );// USB设备连接事件
			mDelayuS( 200 );  // 等待传输完成
		}
		if ( UIF_TRANSFER ) {  // 传输完成
			if ( U_TOG_OK ) return( ERR_SUCCESS );
			r = USB_INT_ST & MASK_UIS_H_RES;  // USB设备应答状态
			if ( r == USB_PID_STALL ) return( r | ERR_USB_TRANSFER );
			if ( r == USB_PID_NAK ) {
				if ( timeout == 0 ) return( r | ERR_USB_TRANSFER );
				if ( timeout < 0xFFFF ) timeout --;
				-- TransRetry;
			}
			else switch ( endp_pid >> 4 ) {
				case USB_PID_SETUP:
				case USB_PID_OUT:
//					if ( U_TOG_OK ) return( ERR_SUCCESS );
//					if ( r == USB_PID_ACK ) return( ERR_SUCCESS );
//					if ( r == USB_PID_STALL || r == USB_PID_NAK ) return( r | ERR_USB_TRANSFER );
					if ( r ) return( r | ERR_USB_TRANSFER );  // 不是超时/出错,意外应答
					break;  // 超时重试
				case USB_PID_IN:
//					if ( U_TOG_OK ) return( ERR_SUCCESS );
//					if ( tog ? r == USB_PID_DATA1 : r == USB_PID_DATA0 ) return( ERR_SUCCESS );
//					if ( r == USB_PID_STALL || r == USB_PID_NAK ) return( r | ERR_USB_TRANSFER );
					if ( r == USB_PID_DATA0 && r == USB_PID_DATA1 ) {  // 不同步则需丢弃后重试
					}  // 不同步重试
					else if ( r ) return( r | ERR_USB_TRANSFER );  // 不是超时/出错,意外应答
					break;  // 超时重试
				default:
					return( ERR_USB_UNKNOWN );  // 不可能的情况
					break;
			}
		}
		else {  // 其它中断,不应该发生的情况
			USB_INT_FG = 0xFF;  /* 清中断标志 */
		}
		mDelayuS( 15 );	
	} while ( ++ TransRetry < 3 );
	return( ERR_USB_TRANSFER );  // 应答超时
}

