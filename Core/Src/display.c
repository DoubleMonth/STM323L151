#include "display.h"
#include "delay/delay.h"
#include "usart.h"
#include "ePaper_2in13/Example/EPD_Test.h"
#include "ePaper_2in13/Config/EPD_2in13_V2.h"
#include "pcf8563/pcf8563.h"
#include "SHT31/sht31_iic.h"
#include "key/key.h"

SHT31_DATA Read_SHT31_DATA;//存储的湿度数据
uint8_t clockBuffer[8]={0};
extern uint8_t Interrupt_1min;  //1分钟中断标志
extern uint16_t USART_RX_STA;       //接收状态标记
extern UART_HandleTypeDef huart1;
extern uint8_t USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
uint8_t setTimeBuff[8]={0};
//extern uint8_t Interrupt_1min;  //1分钟中断标志
char *weekString[]={"日","一","二","三","四","五","六"};
//Create a new image cache
UBYTE *BlackImage;
uint32_t u32_temp,u32_humi;  //温湿度转换成整型。
extern uint8_t set_key;
extern uint8_t add_key;
extern uint8_t dec_key;
int  updateDisplayFull(void)
{
//	uint32_t u32_temp,u32_humi;
	printf("display welcome Image!\r\n");
	DEV_Module_Init();

    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
    EPD_2IN13_V2_Clear();
    DEV_Delay_ms(500);
	
	
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_2IN13_V2_WIDTH % 8 == 0)? (EPD_2IN13_V2_WIDTH / 8 ): (EPD_2IN13_V2_WIDTH / 8 + 1)) * EPD_2IN13_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, 270, WHITE);
	
#if 0   //show image for array    
    printf("show image for array\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(gImage_2in13);

    EPD_2IN13_Display(BlackImage);
    DEV_Delay_ms(2000);
#endif
#if 1
	printf("Drawing\r\n");
    //1.Select Image
    Paint_SelectImage(BlackImage);
	Paint_SetMirroring(MIRROR_HORIZONTAL); //
    Paint_Clear(WHITE);	
	EPD_2IN13_V2_Display(BlackImage);
    DEV_Delay_ms(1000);
#endif
	
	PCF8563_ReadTime(clockBuffer);	
//	SHT31_ReadTH(&Read_SHT31_DATA);
	u32_temp=(uint32_t)(Read_SHT31_DATA.temperature*10.0);
	u32_humi=(uint32_t)(Read_SHT31_DATA.humidity*10.0);
	
	Paint_DrawNum(0, 0, clockBuffer[4]/10, &Font80, BLACK, WHITE);  //时-十位
	Paint_DrawNum(40, 0, clockBuffer[4]%10, &Font80, BLACK, WHITE);  //时-个位
	Paint_DrawString_EN(70, 0, ":", &Font80, WHITE, BLACK);  //:
	Paint_DrawNum(100, 0, clockBuffer[6]/10, &Font80, BLACK, WHITE);  //分-十位
	Paint_DrawNum(140, 0, clockBuffer[6]%10, &Font80, BLACK, WHITE);  //分-个位
	
	Paint_DrawString_CN (187, 2, "温", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 2, "度", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 25, u32_temp / 100, &Font20, BLACK, WHITE);  			//温度-十位  Read_SHT31_DATA.temperature
	Paint_DrawNum		(200, 25, u32_temp / 100 % 10, &Font20, BLACK, WHITE);  			//温度-个位
	Paint_DrawString_EN (210, 25, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 25, u32_temp  % 10, &Font20, BLACK, WHITE);  			//温度-小数位
	Paint_DrawString_EN	(234, 25, "C", &Font20, WHITE, BLACK);  //℃
	
	Paint_DrawString_CN (187, 41, "湿", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 41, "度", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 62, u32_humi / 100 , &Font20, BLACK, WHITE);  			//温度-十位
	Paint_DrawNum		(200, 62, u32_humi / 100 % 10, &Font20, BLACK, WHITE);  			//温度-个位
	Paint_DrawString_EN (210, 62, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 62, u32_humi % 10, &Font20, BLACK, WHITE);  			//温度-小数位
	Paint_DrawString_EN (234, 62, "%", &Font20, WHITE, BLACK);  //%
	
	Paint_DrawRectangle (0, 84, 250, 88, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);   //画两个矩形显示成黑色条
	
		
	Paint_DrawNum		(0, 97, 2, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(9, 97, 0, &Font16, BLACK, WHITE);  			//0
	Paint_DrawNum		(18, 97, clockBuffer[1]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(27, 97, clockBuffer[1]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (37, 92, "年", &Font21CN, BLACK, WHITE);   //年
	Paint_DrawNum		(52, 97, clockBuffer[2]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(61, 97, clockBuffer[2]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (70, 92, "月", &Font21CN, BLACK, WHITE);   //月
	Paint_DrawNum		(86, 97, clockBuffer[3]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(95, 97, clockBuffer[3]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (104, 92, "日", &Font21CN, BLACK, WHITE);   //日
	
	Paint_DrawString_CN (128, 92, "十", &Font21CN, BLACK, WHITE);   //十
	Paint_DrawString_CN (144, 92, "月", &Font21CN, BLACK, WHITE);   //月
	Paint_DrawString_CN (160, 92, "初", &Font21CN, BLACK, WHITE);   //初
	Paint_DrawString_CN (176, 92, "十", &Font21CN, BLACK, WHITE);   //十
	
	Paint_DrawString_CN (200, 92, "星", &Font21CN, BLACK, WHITE);   //星
	Paint_DrawString_CN (216, 92, "期", &Font21CN, BLACK, WHITE);   //期
	Paint_DrawString_CN (232, 92, weekString[clockBuffer[7]%10], &Font21CN, BLACK, WHITE);   //几
	EPD_2IN13_V2_Display(BlackImage);
	
	
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
//墨水屏局部刷新

int  updateDisplayPart(void)
{
	
	
	printf("Partial refresh\r\n");
    EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);
    Paint_SelectImage(BlackImage);
    
	
	PCF8563_ReadTime(clockBuffer);	
	if (Interrupt_1min>=10)   //每十分钟更新一次温湿度
	{
//		SHT31_ReadTH(&Read_SHT31_DATA);
	}
	Paint_ClearWindows(0, 0, 180 , 80, WHITE);
	u32_temp=(uint32_t)(Read_SHT31_DATA.temperature*10.0);
	u32_humi=(uint32_t)(Read_SHT31_DATA.humidity*10.0);
	
	Paint_DrawNum(0, 0, clockBuffer[4]/10, &Font80, BLACK, WHITE);  //时-十位
	Paint_DrawNum(40, 0, clockBuffer[4]%10, &Font80, BLACK, WHITE);  //时-个位
	Paint_DrawString_EN(70, 0, ":", &Font80, WHITE, BLACK);  //:
	Paint_DrawNum(100, 0, clockBuffer[6]/10, &Font80, BLACK, WHITE);  //分-十位
	Paint_DrawNum(140, 0, clockBuffer[6]%10, &Font80, BLACK, WHITE);  //分-个位
	
	Paint_DrawString_CN (187, 2, "温", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 2, "度", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 25, u32_temp / 100, &Font20, BLACK, WHITE);  			//温度-十位  Read_SHT31_DATA.temperature
	Paint_DrawNum		(200, 25, u32_temp / 100 % 10, &Font20, BLACK, WHITE);  			//温度-个位
	Paint_DrawString_EN (210, 25, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 25, u32_temp  % 10, &Font20, BLACK, WHITE);  			//温度-小数位
	Paint_DrawString_EN	(234, 25, "C", &Font20, WHITE, BLACK);  //℃
	
	Paint_DrawString_CN (187, 41, "湿", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 41, "度", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 62, u32_humi / 100 , &Font20, BLACK, WHITE);  			//温度-十位
	Paint_DrawNum		(200, 62, u32_humi / 100 % 10, &Font20, BLACK, WHITE);  			//温度-个位
	Paint_DrawString_EN (210, 62, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 62, u32_humi % 10, &Font20, BLACK, WHITE);  			//温度-小数位
	Paint_DrawString_EN (234, 62, "%", &Font20, WHITE, BLACK);  //%
	
	Paint_DrawRectangle (0, 84, 250, 88, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);   //画两个矩形显示成黑色条
	
		
	Paint_DrawNum		(0, 97, 2, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(9, 97, 0, &Font16, BLACK, WHITE);  			//0
	Paint_DrawNum		(18, 97, clockBuffer[1]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(27, 97, clockBuffer[1]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (37, 92, "年", &Font21CN, BLACK, WHITE);   //年
	Paint_DrawNum		(52, 97, clockBuffer[2]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(61, 97, clockBuffer[2]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (70, 92, "月", &Font21CN, BLACK, WHITE);   //月
	Paint_DrawNum		(86, 97, clockBuffer[3]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(95, 97, clockBuffer[3]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (104, 92, "日", &Font21CN, BLACK, WHITE);   //日
	
	Paint_DrawString_CN (128, 92, "十", &Font21CN, BLACK, WHITE);   //十
	Paint_DrawString_CN (144, 92, "月", &Font21CN, BLACK, WHITE);   //月
	Paint_DrawString_CN (160, 92, "初", &Font21CN, BLACK, WHITE);   //初
	Paint_DrawString_CN (176, 92, "十", &Font21CN, BLACK, WHITE);   //十
	
	Paint_DrawString_CN (200, 92, "星", &Font21CN, BLACK, WHITE);   //星
	Paint_DrawString_CN (216, 92, "期", &Font21CN, BLACK, WHITE);   //期
	Paint_DrawString_CN (232, 92, weekString[clockBuffer[7]%10], &Font21CN, BLACK, WHITE);   //几
	
	EPD_2IN13_V2_Display(BlackImage);
	return 0;
} 
void setTime(void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t len=0;
	if (set_key)
	{
		if(USART_RX_STA&0x8000)   //通过串口更新时间
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			
			for (i=0;i<15;i+=2)
			{
				if (i==14)
					setTimeBuff[j]=(USART_RX_BUF[i]-'0');
				else
					setTimeBuff[j]=(USART_RX_BUF[i]-'0')*10+(USART_RX_BUF[i+1]-'0');
				j++;
			}
			j=0;
			PCF8563_WriteTime(setTimeBuff);   //PCF8563写时间
			printf("\r\n您的时间更新为:\r\n");
			HAL_UART_Transmit(&huart1,(uint8_t*)USART_RX_BUF,len,1000);	//发送接收到的数据
			while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC)!=SET);		//等待发送结束
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
			set_key=0;  //清除按键标志
		}
		
	}
}
