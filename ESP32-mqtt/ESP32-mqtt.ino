/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include "srvr.h"
/* Entry point ----------------------------------------------------------------*/
void setup()
{
  //初始化
	printf("EPD_2in13_V3_test Demo\r\n");
	DEV_Module_Init();
	printf("e-Paper Init and Clear...\r\n");
	EPD_2in13_V3_Init();
	EPD_2in13_V3_Clear();
	//创建一个新图片缓存
	UBYTE *BlackImage;
	UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) 
	{
		printf("Failed to apply for black memory...\r\n");
		while (1);
	}
	printf("Paint_NewImage\r\n");
// Drawing on the image
	Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90, WHITE);  	
	Debug("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	Paint_Clear(WHITE);
  //打印初始数据
	Paint_DrawString_EN(10, 10, "Name: ", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(10, 50, "Price: ", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(10, 90, "Stock: ", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(120, 10, "null", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(120, 50, "null", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(120, 90, "null", &Font24, WHITE, BLACK);
  EPD_2in13_V3_Display_Base(BlackImage);
  setup_mqtt();
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
  loop_mqtt();
}
