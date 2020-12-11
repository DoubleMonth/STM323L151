#include "display.h"
#include "delay/delay.h"
#include "usart.h"
#include "ePaper_2in13/Example/EPD_Test.h"
#include "ePaper_2in13/Config/EPD_2in13.h"
#include "pcf8563/pcf8563.h"
#include "SHT31/sht31_iic.h"

int  welcomeDisplay(void)
{
	printf("display welcome Image!\r\n");
	DEV_Module_Init();

    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN13_Init(EPD_2IN13_FULL);
    EPD_2IN13_Clear();
    DEV_Delay_ms(500);
	
	//Create a new image cache
	UBYTE *BlackImage;
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_2IN13_WIDTH % 8 == 0)? (EPD_2IN13_WIDTH / 8 ): (EPD_2IN13_WIDTH / 8 + 1)) * EPD_2IN13_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT, 270, WHITE);
	
#if 1   //show image for array    
    printf("show image for array\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(gImage_2in13);

    EPD_2IN13_Display(BlackImage);
    DEV_Delay_ms(2000);
#endif
	printf("Drawing\r\n");
    //1.Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);	
	EPD_2IN13_Display(BlackImage);
    DEV_Delay_ms(1000);
	
	
	Paint_DrawNum(0, 0, 1, &Font80, BLACK, WHITE);  //时-十位
	Paint_DrawNum(40, 0, 2, &Font80, BLACK, WHITE);  //时-个位
	Paint_DrawString_EN(70, 0, ":", &Font80, WHITE, BLACK);  //:
	Paint_DrawNum(100, 0, 3, &Font80, BLACK, WHITE);  //分-十位
	Paint_DrawNum(140, 0, 4, &Font80, BLACK, WHITE);  //分-个位
	
	Paint_DrawString_CN (187, 2, "温", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 2, "度", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 25, 2, &Font20, BLACK, WHITE);  			//温度-十位
	Paint_DrawNum		(200, 25, 1, &Font20, BLACK, WHITE);  			//温度-十位
	Paint_DrawString_EN (210, 25, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 25, 3, &Font20, BLACK, WHITE);  			//温度-十位
	Paint_DrawString_EN	(234, 25, "C", &Font20, WHITE, BLACK);  //℃
	
	Paint_DrawString_CN (187, 41, "湿", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 41, "度", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 62, 2, &Font20, BLACK, WHITE);  			//温度-十位
	Paint_DrawNum		(200, 62, 1, &Font20, BLACK, WHITE);  			//温度-十位
	Paint_DrawString_EN (210, 62, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 62, 3, &Font20, BLACK, WHITE);  			//温度-十位
	Paint_DrawString_EN (234, 62, "%", &Font20, WHITE, BLACK);  //%
	
	Paint_DrawRectangle (0, 84, 250, 88, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);   //画两个矩形显示成黑色条
	
	
	
	
	Paint_DrawNum		(0, 97, 2, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(9, 97, 0, &Font16, BLACK, WHITE);  			//0
	Paint_DrawNum		(18, 97, 2, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(27, 97, 0, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (37, 92, "年", &Font21CN, BLACK, WHITE);   //年
	Paint_DrawNum		(52, 97, 5, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(61, 97, 6, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (70, 92, "月", &Font21CN, BLACK, WHITE);   //月
	Paint_DrawNum		(86, 97, 7, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(95, 97, 0, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (104, 92, "日", &Font21CN, BLACK, WHITE);   //日
	
	Paint_DrawString_CN (128, 92, "十", &Font21CN, BLACK, WHITE);   //十
	Paint_DrawString_CN (144, 92, "月", &Font21CN, BLACK, WHITE);   //月
	Paint_DrawString_CN (160, 92, "初", &Font21CN, BLACK, WHITE);   //初
	Paint_DrawString_CN (176, 92, "十", &Font21CN, BLACK, WHITE);   //十
	
	Paint_DrawString_CN (200, 92, "星", &Font21CN, BLACK, WHITE);   //星
	Paint_DrawString_CN (216, 92, "期", &Font21CN, BLACK, WHITE);   //期
	Paint_DrawString_CN (232, 92, "日", &Font21CN, BLACK, WHITE);   //日
	EPD_2IN13_Display(BlackImage);
	
	
#if 0   //Partial refresh, example shows time    		
    printf("Partial refresh\r\n");
    EPD_2IN13_Init(EPD_2IN13_PART);
    Paint_SelectImage(BlackImage);
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    sPaint_time.Sec = 56;
    UBYTE num = 20;
    for (;;) {
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60) {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60) {
                sPaint_time.Hour =  sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24) {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        Paint_ClearWindows(140, 90, 140 + Font20.Width * 7, 90 + Font20.Height, WHITE);
        Paint_DrawTime(140, 90, &sPaint_time, &Font20, WHITE, BLACK);

        num = num - 1;
        if(num == 0) {
            break;
        }
        EPD_2IN13_Display(BlackImage);
        DEV_Delay_ms(500);//Analog clock 1s
    }

#endif
	return 0;
	
}
