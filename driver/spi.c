#include "../include/s5pv210.h"
#include <stdio.h>
  
void spi0_enable(void)  
{  
    CS_REG0 &= ~(1 << 0);  
      
}  
  
void spi0_disable(void)  
{  
    CS_REG0 |= (1 << 0);  
}  

void delay(unsigned int n)
{
    while(n--);
}

void spi_init(void)
{
     //一、设置GPIO口使用GPB0~GPB3  
     GPBCON &= ~(0xFFFF << 0);  
     GPBCON |= (0x2222 << 0);  
   
     //二、设置SPI控制器相关  
     //软重置一下--可有可无  
     CH_CFG0 |= (1<<5);  
     delay(100);  
     CH_CFG0 &= ~(1<<5);  
     delay(100);  
   
     //1. Set Transfer Type. (CPOL & CPHA set)  
     CH_CFG0 &= ~(0x3<<2); //cpol=0,cpha=0  
     CH_CFG0 &= ~(1<<4); //主机模式  
       
     //3. Set Clock configuration register.这边设置采用PCLK 667M / (2* (111+1))，并且使能时钟  
     //千万注意下面两句，不可以写在一起，必须先使能再设定时钟否则不好使  
     CLK_CFG0 |= (1<<8);  
     CLK_CFG0 |= (99);  //66M/200=330000
     //4. Set SPI MODE configuration register.  
     /*******************************  
     * 设置FIFO缓冲大小0~63字节，发送缓冲是当FIFO小于设定发生中断，接收是当数据满了发生中断  
     * 总线宽度保持1字节、  
     *********************************/  
     MODE_CFG0 =0;  
       
     //7. Set Tx or Rx Channel on.  
     //打开发送与接收  
     //CH_CFG0 |= (0x3<<0);  
}
char TX_level(void)
{
    return SPI_STATUS0>>6;
}
char RX_level(void)
{
    return SPI_STATUS0>>15;
}

void SPI0_WriteByte(unsigned char TxData)  
{  
    int i =0;  
    //关闭读打开写  
    CH_CFG0 &= ~(0x3<<0); 
    CH_CFG0|=0x1;
    delay(1);  
    SPI_TX_DATA0 = TxData;  
    while(!(SPI_STATUS0 & (1<<25)));  
   // printf("send TX level=%d\r\n",TX_level());
    //delay(20);//可能要延时长点  
}  

unsigned char SPI0_ReadByte(void)  
{  
   // spi0_enable();
    unsigned long temp = 0;    
    CH_CFG0 |= (0b11);  
    SPI_TX_DATA0 = 0x0;  
    //這個等待必須加上，否則讀取會出錯  
    delay(200);  
    temp = (SPI_RX_DATA0 & 0xFF);   
    return (temp & 0xff);        
}  
