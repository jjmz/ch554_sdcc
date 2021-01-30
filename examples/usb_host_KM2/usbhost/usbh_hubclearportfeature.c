#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

/*******************************************************************************
* Function Name  : HubClearPortFeature
* Description    : 清除HUB端口特性
* Input          : uint8_t HubPortIndex                                         //HUB端口
                   uint8_t FeatureSelt                                          //HUB端口特性
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        错误
*******************************************************************************/
uint8_t   HubClearPortFeature( uint8_t HubPortIndex, uint8_t FeatureSelt ) 
{
    pSetupReq -> bRequestType = HUB_CLEAR_PORT_FEATURE;
    pSetupReq -> bRequest = HUB_CLEAR_FEATURE;
    pSetupReq -> wValueL = FeatureSelt;
    pSetupReq -> wValueH = 0x00;
    pSetupReq -> wIndexL = HubPortIndex;
    pSetupReq -> wIndexH = 0x00;
    pSetupReq -> wLengthL = 0x00;
    pSetupReq -> wLengthH = 0x00;
    return( HostCtrlTransfer( NULL, NULL ) );                                // 执行控制传输
}
