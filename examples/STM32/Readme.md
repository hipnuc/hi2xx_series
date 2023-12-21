# STM32例程

​    本例程提供了C 语言例程代码

​	测试环境：Windows10_x64 操作系统

​	编译器：keil_V5.28

​	开发板：正点原子-战舰V3 STM32F103ZET6

​	测试设备：Hi600

### 硬件连接

1. 将Hi600 正确插入到模块评估板上。

| 超核调试板 | 正点原子开发板 |
| ---------- | -------------- |
| RXD        | PA2(TXD)       |
| TXD        | PA3(RXD)       |
| 5V         | 5V             |
| GND        | GND            |

2. 用杜邦线将上表相对应的引脚连接起来。*
3. 用USB线插到开发板的 __USB_232__ 插口上，另一端插到电脑上。

### 观察输出

​	打开串口调试助手，打开开发板对应的串口号，观察数据输出

```
uart ok
GGA:solq:5 lat:0.70170613(rad) lon:2.02853400(rad) alt:69.66(m)
GGA:solq:5 lat:0.70170609(rad) lon:2.02853401(rad) alt:69.91(m)
GGA:solq:5 lat:0.70170604(rad) lon:2.02853401(rad) alt:70.21(m)
GGA:solq:5 lat:0.70170600(rad) lon:2.02853401(rad) alt:70.44(m)
.......
```

### 代码结构

解码程序分为两部分: 

* decode_nmea.c/h:    解析NMEA报文，这两个文件与硬件无关，一般情况下不需要用户改动，可以移植到所有支持C开发的平台上。

使用说明:

1. 先确硬件连接正确，并且有基本的MCU串口操作知识，使用过串口中断接收

2. 串口终端接收Hi600的消息

   ```
   void USART2_IRQHandler(void)
   {
   	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
       {
           flag = 1;
           ch = USART_ReceiveData(USART2);	
       }
   }
   ```

3. 在主线程中(while(1)) 中调用解析函数

    ```
    ret = decode_nmea(&nmea_raw, ch);
    ```
    
4. 在主线程中(while(1)) 中调用打印函数，打印接收到的数据

   ```
   dump_data(&nmea_raw);
   ```
   
   

