#include "s5pv210.h"

//串口初始化
void uart_init(void)
{
    GPA0CON=0x22;//引脚配置为串口功能
    ULCON0=0x3;//8个数据位，1个停止位，无校验
    UCON0=0x5; //使用轮询模式， 时钟源使用pclk
    UBRDIV0=35; //115200的波特率，整数部分
    UDIVSLOT0=0x0888; //小数部分
}

//发送一个字节
void uart0_putchar(char data)
{
    while(~UTRSTAT0&0x2);//如果发送寄存器不为空则等待
    UTXH0=data;
}

//接收一个字节
char uart0_getchar(void)
{
    while(~UTRSTAT0&0x1);//如果接收寄存器为空则等待
    return URXH0;
}
void putc(unsigned char c)
{
	uart0_putchar(c);
}
unsigned char getc(void)
{
	return uart0_getchar();

}

