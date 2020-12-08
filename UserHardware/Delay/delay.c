#include "Delay/delay.h"

uint8_t fac_us;
void delay_init(uint8_t SYSCLK)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    //SysTick  HCLK
    fac_us=SYSCLK; //OS,fac_us 
}
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told,tnow,tcnt=0;
    uint32_t reload=SysTick->LOAD; //LOAD 
    ticks=nus*fac_us; //
    told=SysTick->VAL; //
    while(1)
    {
        tnow=SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow<told)tcnt+=told-tnow;// SYSTICK 
            else tcnt+=reload-tnow+told;
            told=tnow;
            if(tcnt>=ticks)break; //
        }
    };
}