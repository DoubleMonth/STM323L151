#include "display.h"
#include "delay/delay.h"
#include "usart.h"
#include "ePaper_2in13/Example/EPD_Test.h"
#include "ePaper_2in13/Config/EPD_2in13_V2.h"
#include "pcf8563/pcf8563.h"
#include "si7020/si7020_iic.h"
#include "key/key.h"
#include "pwr_stop.h"

uint8_t clockBuffer[8]={0};
extern uint8_t Interrupt_1min;  //1�����жϱ�־
extern uint16_t USART_RX_STA;       //����״̬���
extern UART_HandleTypeDef huart1;
extern uint8_t USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
uint8_t setTimeBuff[8]={0};
//extern uint8_t Interrupt_1min;  //1�����жϱ�־
char *weekString[]={"��","һ","��","��","��","��","��"};
//Create a new image cache
UBYTE *BlackImage;
SI7020_DATA read_si7020_data;   //�洢��ʪ������
uint32_t u32_temp,u32_humi;  //��ʪ��ת�������͡�
extern uint8_t set_key;
extern uint8_t add_key;
extern uint8_t dec_key;
int  updateDisplayFull(void)
{
//	uint32_t u32_temp,u32_humi;
	EPDPowerOn();   //�ȸ�EPD����
	PCF8563_ReadTime(clockBuffer);	
	si7020Measure(&read_si7020_data);
	u32_temp=(uint32_t)(read_si7020_data.temperature*10.0);
	u32_humi=(uint32_t)(read_si7020_data.humidity*10.0);
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
	Paint_SelectImage(BlackImage);
    Paint_SetMirroring(MIRROR_HORIZONTAL); //
    Paint_Clear(WHITE);
	
#if 0   //show image for array    
    printf("show image for array\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(gImage_2in13);

    EPD_2IN13_Display(BlackImage);
    DEV_Delay_ms(2000);
#endif
#if 0
	printf("Drawing\r\n");
    //1.Select Image
    Paint_SelectImage(BlackImage);
	Paint_SetMirroring(MIRROR_HORIZONTAL); //
    Paint_Clear(WHITE);	
	EPD_2IN13_V2_Display(BlackImage);
    DEV_Delay_ms(1000);
#endif
	Paint_DrawNum(0, 0, clockBuffer[4]/10, &Font80, BLACK, WHITE);  //ʱ-ʮλ
	Paint_DrawNum(40, 0, clockBuffer[4]%10, &Font80, BLACK, WHITE);  //ʱ-��λ
	Paint_DrawString_EN(70, 0, ":", &Font80, WHITE, BLACK);  //:
	Paint_DrawNum(100, 0, clockBuffer[5]/10, &Font80, BLACK, WHITE);  //��-ʮλ
	Paint_DrawNum(140, 0, clockBuffer[5]%10, &Font80, BLACK, WHITE);  //��-��λ
	
	Paint_DrawString_CN (187, 2, "��", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 2, "��", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 25, u32_temp / 100, &Font20, BLACK, WHITE);  			//�¶�-ʮλ  Read_SHT31_DATA.temperature
	Paint_DrawNum		(200, 25, u32_temp / 100 % 10, &Font20, BLACK, WHITE);  			//�¶�-��λ
	Paint_DrawString_EN (210, 25, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 25, u32_temp  % 10, &Font20, BLACK, WHITE);  			//�¶�-С��λ
	Paint_DrawString_EN	(234, 25, "C", &Font20, WHITE, BLACK);  //��
	
	Paint_DrawString_CN (187, 41, "ʪ", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 41, "��", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 62, u32_humi / 100 , &Font20, BLACK, WHITE);  			//�¶�-ʮλ
	Paint_DrawNum		(200, 62, u32_humi / 100 % 10, &Font20, BLACK, WHITE);  			//�¶�-��λ
	Paint_DrawString_EN (210, 62, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 62, u32_humi % 10, &Font20, BLACK, WHITE);  			//�¶�-С��λ
	Paint_DrawString_EN (234, 62, "%", &Font20, WHITE, BLACK);  //%
	
	Paint_DrawRectangle (0, 84, 250, 88, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);   //������������ʾ�ɺ�ɫ��
	
		
	Paint_DrawNum		(0, 97, 2, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(9, 97, 0, &Font16, BLACK, WHITE);  			//0
	Paint_DrawNum		(18, 97, clockBuffer[1]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(27, 97, clockBuffer[1]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (37, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawNum		(52, 97, clockBuffer[2]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(61, 97, clockBuffer[2]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (70, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawNum		(86, 97, clockBuffer[3]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(95, 97, clockBuffer[3]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (104, 92, "��", &Font21CN, BLACK, WHITE);   //��
	
	Paint_DrawString_CN (128, 92, "ʮ", &Font21CN, BLACK, WHITE);   //ʮ
	Paint_DrawString_CN (144, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawString_CN (160, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawString_CN (176, 92, "ʮ", &Font21CN, BLACK, WHITE);   //ʮ
	
	Paint_DrawString_CN (200, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawString_CN (216, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawString_CN (232, 92, weekString[clockBuffer[7]%10], &Font21CN, BLACK, WHITE);   //��
	EPD_2IN13_V2_Display(BlackImage);
	
	
	return 0;
	
}
//īˮ���ֲ�ˢ��

int  updateDisplayPart(void)
{
	static uint8_t i=1;
//	EPDPowerOn();   //�ȸ�EPD����
	PCF8563_ReadTime(clockBuffer);	
	si7020Measure(&read_si7020_data);
	u32_temp=(uint32_t)(read_si7020_data.temperature*10.0);
	u32_humi=(uint32_t)(read_si7020_data.humidity*10.0);
	printf("Partial refresh\r\n");
	DEV_Module_Init();
	 //Create a new image cache
    UBYTE *BlackImage;
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_2IN13_V2_WIDTH % 8 == 0)? (EPD_2IN13_V2_WIDTH / 8 ): (EPD_2IN13_V2_WIDTH / 8 + 1)) * EPD_2IN13_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
//    printf("Paint_NewImage\r\n");
//    Paint_NewImage(BlackImage, EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, 270, WHITE);
//    Paint_SelectImage(BlackImage);
//    Paint_SetMirroring(MIRROR_HORIZONTAL); //
//    Paint_Clear(WHITE);
	
//	EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
	EPD_2IN13_V2_DisplayPartBaseImage(BlackImage);
	
	EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);
    Paint_SelectImage(BlackImage);
	
	
	Paint_ClearWindows(0, 0, 250 , 122, WHITE);
	Paint_DrawNum(0, 0, clockBuffer[4]/10, &Font80, BLACK, WHITE);  //ʱ-ʮλ
	Paint_DrawNum(40, 0, clockBuffer[4]%10, &Font80, BLACK, WHITE);  //ʱ-��λ
	Paint_DrawString_EN(70, 0, ":", &Font80, WHITE, BLACK);  //:
	Paint_DrawNum(100, 0, clockBuffer[5]/10, &Font80, BLACK, WHITE);  //��-ʮλ
	Paint_DrawNum(140, 0, clockBuffer[5]%10, &Font80, BLACK, WHITE);  //��-��λ
	
	Paint_DrawString_CN (187, 2, "��", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 2, "��", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 25, u32_temp / 100, &Font20, BLACK, WHITE);  			//�¶�-ʮλ  Read_SHT31_DATA.temperature
	Paint_DrawNum		(200, 25, u32_temp / 100 % 10, &Font20, BLACK, WHITE);  			//�¶�-��λ
	Paint_DrawString_EN (210, 25, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 25, u32_temp  % 10, &Font20, BLACK, WHITE);  			//�¶�-С��λ
	Paint_DrawString_EN	(234, 25, "C", &Font20, WHITE, BLACK);  //��
	
	Paint_DrawString_CN (187, 41, "ʪ", &Font21CN, BLACK, WHITE);
	Paint_DrawString_CN (205, 41, "��", &Font21CN, BLACK, WHITE);
	Paint_DrawNum		(187, 62, u32_humi / 100 , &Font20, BLACK, WHITE);  			//�¶�-ʮλ
	Paint_DrawNum		(200, 62, u32_humi / 100 % 10, &Font20, BLACK, WHITE);  			//�¶�-��λ
	Paint_DrawString_EN (210, 62, ".", &Font20, WHITE, BLACK);  //.
	Paint_DrawNum		(220, 62, u32_humi % 10, &Font20, BLACK, WHITE);  			//�¶�-С��λ
	Paint_DrawString_EN (234, 62, "%", &Font20, WHITE, BLACK);  //%
	
	Paint_DrawRectangle (0, 84, 250, 88, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);   //������������ʾ�ɺ�ɫ��
	
		
	Paint_DrawNum		(0, 97, 2, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(9, 97, 0, &Font16, BLACK, WHITE);  			//0
	Paint_DrawNum		(18, 97, clockBuffer[1]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(27, 97, clockBuffer[1]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (37, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawNum		(52, 97, clockBuffer[2]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(61, 97, clockBuffer[2]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (70, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawNum		(86, 97, clockBuffer[3]/10, &Font16, BLACK, WHITE);  			//2
	Paint_DrawNum		(95, 97, clockBuffer[3]%10, &Font16, BLACK, WHITE);  			//0
	Paint_DrawString_CN (104, 92, "��", &Font21CN, BLACK, WHITE);   //��
	
	Paint_DrawString_CN (128, 92, "ʮ", &Font21CN, BLACK, WHITE);   //ʮ
	Paint_DrawString_CN (144, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawString_CN (160, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawString_CN (176, 92, "ʮ", &Font21CN, BLACK, WHITE);   //ʮ
	
	Paint_DrawString_CN (200, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawString_CN (216, 92, "��", &Font21CN, BLACK, WHITE);   //��
	Paint_DrawString_CN (232, 92, weekString[clockBuffer[7]%10], &Font21CN, BLACK, WHITE);   //��
	
	EPD_2IN13_V2_DisplayPart(BlackImage);   //�ֲ�ˢ��
	HAL_Delay(1000);
	EPD_2IN13_V2_Sleep();
    free(BlackImage);
    BlackImage = NULL;
//	DEV_Delay_ms(500);
//}
//	DEV_Delay_ms(500);//Analog clock 1s
	
//	printf("Goto Sleep...\r\n");
//    EPD_2IN13_V2_Sleep();
//    free(BlackImage);
//    BlackImage = NULL;
//    DEV_Delay_ms(1000);//Analog clock 1s

//    // close 5V
//    printf("close 5V, Module enters 0 power consumption ...\r\n");
//    DEV_Module_Exit();
	
//	EPDPowerOff();   //��EPD�ϵ�
//	return 0;
} 
void setTime(void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t len=0;
	if (set_key)
	{
		if(USART_RX_STA&0x8000)   //ͨ�����ڸ���ʱ��
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			
			for (i=0;i<15;i+=2)
			{
				if (i==14)
					setTimeBuff[j]=(USART_RX_BUF[i]-'0');
				else
					setTimeBuff[j]=(USART_RX_BUF[i]-'0')*10+(USART_RX_BUF[i+1]-'0');
				j++;
			}
			j=0;
			PCF8563_WriteTime(setTimeBuff);   //PCF8563дʱ��
			printf("\r\n����ʱ�����Ϊ:\r\n");
			HAL_UART_Transmit(&huart1,(uint8_t*)USART_RX_BUF,len,1000);	//���ͽ��յ�������
			while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;
			set_key=0;  //���������־
		}
		
	}
}
