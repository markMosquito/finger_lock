#include <stm32f2xx.h>

#define CH_SENS(a,b)  0x##a##b

typedef enum
{
	SENS1 = 0x02,
	SENS2 = 0x03,
	SENS3 = 0x04,
	SENS4 = 0x05,
	SENS5 = 0x06,
	SENS6 = 0x07,
	CTRL1 = 0x08,
	CTRL2 = 0x09,
	REF_RST1 = 0x0A,
	REF_RST2 = 0x0B,
	CH_HOLD1 = 0x0C,
	CH_HOLD2 = 0x0D,
	CAL_HOLD1 = 0x0E,
	CAL_HOLD2 = 0x0F,
	OUTPUT1 = 0x10,
	OUTPUT2 = 0x11,
	OUTPUT3 = 0x12,
} TSM_RegMapAddr_TypeDef;
