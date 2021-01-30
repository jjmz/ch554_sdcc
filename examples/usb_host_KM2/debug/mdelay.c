#include <stdint.h>

#include "ch554.h"
#include "debug.h"

/*******************************************************************************
* Function Name  : mDelayms(UNIT16 n)
* Description    : ms delay function
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void	mDelaymS( uint16_t n )                                                  // Delay in mS
{
	while ( n ) {
#ifdef	DELAY_MS_HW
		while ( ( TKEY_CTRL & bTKC_IF ) == 0 );
		while ( TKEY_CTRL & bTKC_IF );
#else
		mDelayuS( 1000 );
#endif
		-- n;
	}
}                                         
