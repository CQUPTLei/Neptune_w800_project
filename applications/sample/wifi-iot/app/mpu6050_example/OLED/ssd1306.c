#include "ssd1306.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>  // For memcpy

#include <stdio.h>
#include <unistd.h>
#include "cmsis_os2.h"
#include "iot_i2c.h"
#include "iot_errno.h"
#include "ssd1306_conf.h"

#define BITS_NAMBER  8

#define SSD1306_I2C_IDX 0
#define SSD1306_CTRL_CMD 0x00
#define SSD1306_CTRL_DATA 0x40
#define SSD1306_MASK_CONT (0x1<<7)

// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];
// Screen object
static SSD1306_t SSD1306;

void HAL_Delay(uint32_t ms)
{
    uint32_t msPerTick = 1000 / osKernelGetTickFreq(); // 10ms
    if (ms >= msPerTick) {
        osDelay(ms / msPerTick);
    }

    uint32_t restMs = ms % msPerTick;
    if (restMs > 0) {
        //usleep(restMs * 1000);
        tls_os_time_delay(restMs/2);
    }
}

uint32_t HAL_GetTick(void)
{
    uint32_t msPerTick = 1000 / osKernelGetTickFreq(); // 10ms
    uint32_t tickMs = osKernelGetTickCount() * msPerTick;

    uint32_t csPerMs = osKernelGetSysTimerFreq() / 1000; // 160K cycle/ms
    uint32_t csPerTick = csPerMs * msPerTick; // 1600K cycles/tick
    uint32_t restMs = osKernelGetSysTimerCount() % csPerTick / csPerMs;

    return tickMs + restMs;
}

void HAL_Delayus(void)//40us
{
    uint32_t tick;
    // Calculating 100us timeout in system timer ticks
    const uint32_t timeout = 30U * osKernelGetSysTimerFreq() / 1000000u;

    tick = osKernelGetSysTimerCount(); // get start value of the Kernel system tick
    do {
        ;
    } while(osKernelGetSysTimerCount() - tick < timeout);
}

static uint32_t ssd1306_SendData(uint8_t* data, size_t size)
{
    IoTI2cWrite(SSD1306_I2C_IDX, SSD1306_I2C_ADDR, data, size);
    HAL_Delayus();
}

static uint32_t ssd1306_WiteByte(uint8_t regAddr, uint8_t byte)
{
    uint8_t buffer[] = {regAddr, byte};
    ssd1306_SendData(buffer, sizeof(buffer));
}

// Send a byte to the command register
void ssd1306_WriteCommand(uint8_t byte) {
    ssd1306_WiteByte(SSD1306_CTRL_CMD, byte);
}

// Send data
void ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
    uint8_t data[SSD1306_WIDTH * 2] = {0};
    for (size_t i = 0; i < buff_size; i++) {
        data[i*2] = SSD1306_CTRL_DATA | SSD1306_MASK_CONT;
        data[i*2+1] = buffer[i];
    }
    data[(buff_size - 1) * 2] = SSD1306_CTRL_DATA;
    ssd1306_SendData(data, sizeof(data));
}

/**
 * @brief Set Display ON/OFF.
 * @param[in] on 0 for OFF, any for ON.
 */
void ssd1306_SetDisplayOn(const uint8_t on)
{
    uint8_t value;
    if (on) {
        value = 0xAF;   // Display on
        SSD1306.DisplayOn = 1;
    } else {
        value = 0xAE;   // Display off
        SSD1306.DisplayOn = 0;
    }
    ssd1306_WriteCommand(value);
}

// Initialize the oled screen
void ssd1306_Init(void) {
    // Wait for the screen to boot
    HAL_Delay(100);

    // Init OLED
    ssd1306_SetDisplayOn(0); // display off
    ssd1306_WriteCommand(0x20); // Set Memory Addressing Mode
    ssd1306_WriteCommand(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode
    ssd1306_WriteCommand(0xB0); // Set Page Start Address for Page Addressing Mode,0-7
    ssd1306_WriteCommand(0xC8); // Set COM Output Scan Direction
    ssd1306_WriteCommand(0x00); // ---set low column address
    ssd1306_WriteCommand(0x10); // ---set high column address
    ssd1306_WriteCommand(0x40); // --set start line address - CHECK
    ssd1306_SetContrast(0xFF);
    ssd1306_WriteCommand(0xA1); // --set segment re-map 0 to 127 - CHECK
    ssd1306_WriteCommand(0xA6); // --set normal color
    ssd1306_WriteCommand(0xA8); // --set multiplex ratio(1 to 64) - CHECK
    ssd1306_WriteCommand(0x3F); //
    ssd1306_WriteCommand(0xA4); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    ssd1306_WriteCommand(0xD3); // -set display offset - CHECK
    ssd1306_WriteCommand(0x00); // -not offset
    ssd1306_WriteCommand(0xD5); // --set display clock divide ratio/oscillator frequency
    ssd1306_WriteCommand(0xF0); // --set divide ratio
    ssd1306_WriteCommand(0xD9); // --set pre-charge period
    ssd1306_WriteCommand(0x11); // 0x22 by default
    ssd1306_WriteCommand(0xDA); // --set com pins hardware configuration - CHECK
    ssd1306_WriteCommand(0x12);
    ssd1306_WriteCommand(0xDB); // --set vcomh
    ssd1306_WriteCommand(0x30); // 0x20,0.77xVcc, 0x30,0.83xVcc
    ssd1306_WriteCommand(0x8D); // --set DC-DC enable
    ssd1306_WriteCommand(0x14);
    ssd1306_SetDisplayOn(1); // --turn on SSD1306 panel

    
    ssd1306_Fill(Black);// Clear screen
    ssd1306_UpdateScreen();// Flush buffer to screen

    SSD1306.CurrentX = 0;// Set default values for screen object
    SSD1306.CurrentY = 0;
    SSD1306.Initialized = 1;
}

// Fill the whole screen with the given color
void ssd1306_Fill(SSD1306_COLOR color)
{
    /* Set memory */
    uint32_t i;

    for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

void ssd1306_Fill2(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2, SSD1306_COLOR color)
{
	uint8_t x, y;
	for (x = x1; x < x2; x++)
	{
		for (y = y1; y < y2; y++)
		{
			ssd1306_DrawPixel(x, y,(SSD1306_COLOR) color);
		}
	}
}

// Position the cursor
void ssd1306_SetCursor(uint8_t x, uint8_t y)
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}


// Write the screenbuffer with changed to the screen
void ssd1306_UpdateScreen(void) {
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages

    uint8_t cmd[] = {
        0X21,   // 设置列起始和结束地址
        0X00,   // 列起始地址 0
        0X7F,   // 列终止地址 127
        0X22,   // 设置页起始和结束地址
        0X00,   // 页起始地址 0
        0X07,   // 页终止地址 7
    };
    uint32_t count = 0;
    uint8_t data[sizeof(cmd)*2 + SSD1306_BUFFER_SIZE + 1] = {};

    // copy cmd
    for (uint32_t i = 0; i < sizeof(cmd)/sizeof(cmd[0]); i++) {
        data[count++] = SSD1306_CTRL_CMD | SSD1306_MASK_CONT;
        data[count++] = cmd[i];
    }

    // copy frame data
    data[count++] = SSD1306_CTRL_DATA;
    memcpy(&data[count], SSD1306_Buffer, sizeof(SSD1306_Buffer));
    count += sizeof(SSD1306_Buffer);

    // send to i2c bus
    uint32_t retval = ssd1306_SendData(data, count);
    if (retval != IOT_SUCCESS) {
        printf("ssd1306_UpdateScreen send frame data filed: %d!\r\n", retval);
    }
}
//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
    if ((x >= SSD1306_WIDTH) || (y >= SSD1306_HEIGHT)) {
        // Don't write outside the buffer
        return;
    }

    // Check if pixel should be inverted
    if (SSD1306.Inverted) {
        color = (SSD1306_COLOR)!color;
    }

    // Draw in the right color
    if (color == White) {
        SSD1306_Buffer[x + (y / BITS_NAMBER) * SSD1306_WIDTH] |= 1 << (y % BITS_NAMBER);
    } else {
        SSD1306_Buffer[x + (y / BITS_NAMBER) * SSD1306_WIDTH] &= ~(1 << (y % BITS_NAMBER));
    }
}


// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char ssd1306_DrawChar(char ch, FontDef Font, SSD1306_COLOR color)
{
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126) 
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (SSD1306.CurrentX + Font.FontWidth) ||
        SSD1306_HEIGHT < (SSD1306.CurrentY + Font.FontHeight)) {
        return 0;
    }
    // Use the font to write
    for (i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for (j = 0; j < Font.FontWidth; j++) {
            if ((b << j) & 0x8000) {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
            } else {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
            }
        }
    }
    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;
    return ch;
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char ssd1306_DrawChar1(char ch, FontDef Font, SSD1306_COLOR color)
{
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126) 
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (SSD1306.CurrentX + Font.FontWidth) ||
        SSD1306_HEIGHT < (SSD1306.CurrentY + Font.FontHeight)) {
        return 0;
    }
    // Use the font to write
    for (i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for (j = 0; j < Font.FontWidth; j++) {
            if ((b << j) & 0x80) {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
            } else {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
            }
        }
    }
    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;
    return ch;
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char ssd1306_DrawChar_u8(char ch, SSD1306_COLOR color) {
    uint32_t i, j;
    uint8_t b;
    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (SSD1306.CurrentX + 8) ||
        SSD1306_HEIGHT < (SSD1306.CurrentY + 16)){
        return 0;
    }
    // Use the font to write
    for(i = 0; i < 16; i++) {    //
        b = Font8x16[(ch - 32) * 16 + i]; 
        for(j = 0; j < 8; j++) {
            
            if((b << j) & 0x80)  {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
            } else {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
            }            
        }
    }
    // The current space is now taken
    SSD1306.CurrentX += 8;
    // Return written char for validation
    return ch;
}

// Write full string to screenbuffer
char ssd1306_DrawString(char* str, FontDef Font, SSD1306_COLOR color)
{
    // Write until null-byte
    while (*str) {
        if (ssd1306_DrawChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }
        // Next char
        str++;
    }

    return *str;
}

void ssd1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
{
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;

    ssd1306_DrawPixel(x2, y2, color);
    while ((x1 != x2) || (y1 != y2)) {
        ssd1306_DrawPixel(x1, y1, color);
        int32_t error2 = error * 2;
        if (error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }
        if (error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
    return;
}

//Draw polyline
void ssd1306_DrawPolyline(const SSD1306_VERTEX *par_vertex, uint16_t par_size, SSD1306_COLOR color) {
  uint16_t i;
  if(par_vertex != 0){
    for(i = 1; i < par_size; i++){
      ssd1306_DrawLine(par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y, color);
    }
  }
  return;
}

//Draw rectangle
void ssd1306_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
  ssd1306_DrawLine(x1,y1,x2,y1,color);
  ssd1306_DrawLine(x2,y1,x2,y2,color);
  ssd1306_DrawLine(x2,y2,x1,y2,color);
  ssd1306_DrawLine(x1,y2,x1,y1,color);
  return;
}

/*Convert Degrees to Radians*/
static float ssd1306_DegToRad(float par_deg) {
    return par_deg * 3.14 / 180.0;
}
/*Normalize degree to [0;360]*/
static uint16_t ssd1306_NormalizeTo0_360(uint16_t par_deg) {
  uint16_t loc_angle;
  if(par_deg <= 360)
  {
    loc_angle = par_deg;
  }
  else
  {
    loc_angle = par_deg % 360;
    loc_angle = ((par_deg != 0)?par_deg:360);
  }
  return loc_angle;
}

/*DrawArc. Draw angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle in degree
 * sweep in degree
 */
void ssd1306_DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color) {
    #define CIRCLE_APPROXIMATION_SEGMENTS 36
    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1,xp2;
    uint8_t yp1,yp2;
    uint32_t count = 0;
    uint32_t loc_sweep = 0;
    float rad;

    loc_sweep = ssd1306_NormalizeTo0_360(sweep);

    count = (ssd1306_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;
    while(count < approx_segments)
    {
        rad = ssd1306_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sin(rad)*radius);
        yp1 = y + (int8_t)(cos(rad)*radius);
        count++;
        if(count != approx_segments)
        {
            rad = ssd1306_DegToRad(count*approx_degree);
        }
        else
        {
            rad = ssd1306_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sin(rad)*radius);
        yp2 = y + (int8_t)(cos(rad)*radius);
        ssd1306_DrawLine(xp1,yp1,xp2,yp2,color);
    }

    return;
}

//Draw circle by Bresenhem's algorithm
void ssd1306_DrawCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,SSD1306_COLOR par_color) {
  int32_t x = -par_r;
  int32_t y = 0;
  int32_t err = 2 - 2 * par_r;
  int32_t e2;

  if (par_x >= SSD1306_WIDTH || par_y >= SSD1306_HEIGHT) {
    return;
  }

    do {
      ssd1306_DrawPixel(par_x - x, par_y + y, par_color);
      ssd1306_DrawPixel(par_x + x, par_y + y, par_color);
      ssd1306_DrawPixel(par_x + x, par_y - y, par_color);
      ssd1306_DrawPixel(par_x - x, par_y - y, par_color);
        e2 = err;
        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x) {
              e2 = 0;
            }
            else
            {
              /*nothing to do*/
            }
        }
        else
        {
          /*nothing to do*/
        }
        if(e2 > x) {
          x++;
          err = err + (x * 2 + 1);
        }
        else
        {
          /*nothing to do*/
        }
    } while(x <= 0);

    return;
}


void ssd1306_DrawBitmap(const uint8_t* bitmap, uint32_t size)
{
    uint8_t rows = size * 8 / SSD1306_WIDTH;
    if (rows > SSD1306_HEIGHT) {
        rows = SSD1306_HEIGHT;
    }
    for (uint8_t y = 0; y < rows; y++) {
        for (uint8_t x = 0; x < SSD1306_WIDTH; x++) {
            uint8_t byte = bitmap[(y * SSD1306_WIDTH / 8) + (x / 8)];
            uint8_t bit = byte & (0x80 >> (x % 8));
            ssd1306_DrawPixel(x, y, bit ? White : Black);
        }
    }
}

/**
 * @brief Sets the contrast of the display.
 * @param[in] value contrast to set.
 * @note Contrast increases as the value increases.
 * @note RESET = 7Fh.
 */
void ssd1306_SetContrast(const uint8_t  value)
{
    const uint8_t kSetContrastControlRegister = 0x81;
    ssd1306_WriteCommand(kSetContrastControlRegister);
    ssd1306_WriteCommand(value);
}

/**
	x:横坐标，0-127
	y:0-64
    *s:要显示的字符（中英文均可，前提是数组中包含的汉字）
    pctolcd软件生成字符设置：
    阴码，顺向，行列式，16进制
    英文字符适配 8*16的
*/
void ssd1306_DrawChinese(uint8_t x, uint8_t y, char *s, SSD1306_COLOR color)
{
	unsigned char i,k,length;
	uint32_t Index = 0;
    uint8_t b;
	length = strlen(s);//取字符串总长
	for(k=0; k<length; k++)
	{
		if((uint8_t)(*(s+k)) <= 127){//小于128是ASCII符号
            ssd1306_SetCursor(x,y);
            ssd1306_DrawChar_u8(*(s+k), color);
			x += 8;//x坐标右移8
		}
        else if((uint8_t)(*(s+k)) > 127){//大于127，为汉字，UTF-8是3个字节    
            Index = ((uint8_t)(*(s+k)) << 16) | ((uint8_t)(*(s+k+1)) << 8) | (uint8_t)(*(s+k+2));//取汉字的编码
			//printf("byte is %x  \r\n", Index );	
            for(i=0;i<sizeof(CN16_Msk)/34;i++){//查数组
				if(Index == CN16_Msk[i].Index){
					//查询到这个字
				//	printf("Pindex is %d, %d\r\n",i,color);
                    for (uint8_t t = 0; t < 2; t++)
                    {                                                                 
                        for(uint8_t k = (0 + 16 * t); k < (16 + 16 * t); k++) {    //
                            b = CN16_Msk[i].Msk[k];
                            for(uint8_t j = 0; j < 8; j++) {
                                if((b << j) & 0x80)  {                  
                                    ssd1306_DrawPixel(x + j, (y + k % 16), color);
                                } else {                              
                                    ssd1306_DrawPixel(x + j, (y + k % 16), !color);
                                }
                            }
                           // printf("\n");
                        }
                        x += 8;
                    }
					k += 2; //汉字占3B,跳过2个	
				}
			}
		}
	}
}
