#include <stdint.h>

#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

#include "ssd1306_conf.h"

typedef struct {
	const uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef;

typedef struct 
{
	unsigned int Index;		//汉字编码UTF-8
	unsigned char Msk[32]; 	//字模
}typFNT_GB16;

#ifdef SSD1306_INCLUDE_FONT_6x8
extern FontDef Font_6x8;
#endif
#ifdef SSD1306_INCLUDE_FONT_7x10
extern FontDef Font_7x10;
#endif
#ifdef SSD1306_INCLUDE_FONT_11x18
extern FontDef Font_11x18;
#endif
#ifdef SSD1306_INCLUDE_FONT_16x26
extern FontDef Font_16x26;
#endif

#ifdef SSD1306_INCLUDE_FONT_CN16
extern typFNT_GB16 CN16_Msk[4];
#endif

#ifdef SSD1306_INCLUDE_FONT_8x16
extern const uint8_t Font8x16[];
#endif

#endif // __SSD1306_FONTS_H__
