#include "delay.h"
#include "usart.h"
#include "decode_nmea.h"
 
/************************************************
 ALIENTEK战舰STM32开发板实验
 串口实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

/************************************************
 北京超核电子
 惯性导航模块： HI600
 串口接收数据例程
 本例程只供学习使用，观察数据输出，不做其他用途
 串口2接收来自HI600的数据
 串口1将串口2成功接收到的数据打印到终端上
 这里的终端一般指的是PC机上串口调试助手
 官网：http://www.hipnuc.com
************************************************/

static uint8_t ch;
static nmea_raw_t nmea_raw;                                    /* HI600 stram read/control struct */
static int ret;                               /* 0: no new frame arrived, 1: new frame arrived */
static int flag;

static void dump_data(nmea_raw_t *nmea_raw)
{
    printf("%s:solq:%d lat:%.8f(rad) lon:%.8f(rad) alt:%.2f(m)\r\n", nmea_raw->nmea_hdr, nmea_raw->solq, nmea_raw->lat, nmea_raw->lon, nmea_raw->alt);
}

int main(void)
{		
    delay_init();	    	                               /* delay function */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);        /* config NVIC priorith group */
    uart_init(115200);	                                   /* uart baud 115200 */

    printf("uart ok\r\n");
    while(1)
    {
        if(flag)
        {
            flag = 0;
            
            /* decode each byte */
            ret = decode_nmea(&nmea_raw, ch);
            
            if(ret)
                dump_data(&nmea_raw);

        }
	}	 
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        flag = 1;
        ch = USART_ReceiveData(USART2);	
    }
} 
