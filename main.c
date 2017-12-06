#include <stdio.h>
#include "rc522.h"

unsigned char UID[5];
unsigned char RF_Buffer[18];
unsigned char Password_Buffer[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};// Mifare One 缺省密码

char          MBRX[30];
char          MBKeyTP[30];
char          Event;
unsigned char DISP_MODE,i; // 编辑控件显示模式
unsigned char des_on       = 0; // DES加密标志

void delay(unsigned int n);
unsigned char RC522_ReadRawRC(unsigned char address);

//寻卡
void Find_Card(void)
{
  unsigned char Temp[4]={0};
    if(PcdRequest(0x52,Temp)!=MI_OK)
    {
      if(PcdRequest(0x52,Temp)!=MI_OK)//再寻一次
      {
        printf("Failed!"); 
        return;
      }
    }    
    if(Temp[0]==0x04&&Temp[1]==0x00)  
      printf("MFOne-S50");
    else if(Temp[0]==0x02&&Temp[1]==0x00)
      printf("MFOne-S70");
    else if(Temp[0]==0x44&&Temp[1]==0x00)
      printf("MF-UltraLight");
    else if(Temp[0]==0x08&&Temp[1]==0x00)
      printf("MF-Pro");
    else if(Temp[0]==0x44&&Temp[1]==0x03)
      printf("MF Desire");
    else
      printf("Unknown");
    
    /*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
    if ( PcdAnticoll (Temp ) == MI_OK )  
    {
      beep();
      printf ("\r\nThe Card ID is: %02X%02X%02X%02X\r\n", Temp[0],Temp[1], Temp[2],Temp [3]);						
                
    }        
}

int main(void)
{
    clock_setup();//时钟初始化
    uart_init();//串口初始化
    rc522_init();//spi初始化
    char x = PcdReset();//复位
    //PcdAntennaOff();
   // delay(0x10000);
    //PcdAntennaOn();
    PcdConfigISOType('A');
    //DisplayConfigMenu();
    printf("rfc test\r\n");

    while(1)
    {
        printf("read result: 0x%x\r\n", ReadRawRC(VersionReg));
        delay(0x1000);
        Find_Card();//寻卡
        

        
        delay(0x380000);
    }
}
