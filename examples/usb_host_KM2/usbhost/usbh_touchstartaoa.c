#include <ch554.h>
#include <debug.h>
#include "string.h"
#include "usbhost.h"

#include "../print/small_print.h"

#include <ch554_usb.h>

uint8_t TouchStartAOA(void)
{
	uint8_t len,s,i,Num;
	uint16_t cp_len;
    //获取协议版本号
    CopySetupReqPkg( GetProtocol );
    s = HostCtrlTransfer( Com_Buffer, &len );  // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
	if(Com_Buffer[0]<2) return  ERR_AOA_PROTOCOL;

    //输出字符串
    for(i=0; i<6; i++)
    {
        Num=Sendlen[i];
        CopySetupReqPkg(&SetStringID[8*i]);
		cp_len = (pSetupReq -> wLengthH << 8)|( pSetupReq -> wLengthL);
		memcpy(Com_Buffer,&StringID[Num],cp_len);
        s = HostCtrlTransfer( Com_Buffer, &len );  // 执行控制传输
        if ( s != ERR_SUCCESS )
        {
            return( s );
        }
    }	

    CopySetupReqPkg(TouchAOAMode);
    s = HostCtrlTransfer( Com_Buffer, &len );  // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    return ERR_SUCCESS;	
}

