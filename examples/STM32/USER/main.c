#include "delay.h"
#include "usart.h"
#include "decode_nmea.h"
 
/************************************************
 ALIENTEKս��STM32������ʵ��
 ����ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

/************************************************
 �������˵���
 ���Ե���ģ�飺 HI600
 ���ڽ�����������
 ������ֻ��ѧϰʹ�ã��۲��������������������;
 ����2��������HI600������
 ����1������2�ɹ����յ������ݴ�ӡ���ն���
 ������ն�һ��ָ����PC���ϴ��ڵ�������
 ������http://www.hipnuc.com
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
