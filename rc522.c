#include "include/s5pv210.h"
#include "rc522.h"
void spi0_enable(void);
void spi0_disable(void);
void SPI0_WriteByte(unsigned char TxData);
unsigned char SPI0_ReadByte(void);



    /////////////////////////////////////////////////////////////////////  
    //功    能：讀RC632寄存器  
    //參數說明：Address[IN]:寄存器地址  
    //返    回：讀出的值  
    /////////////////////////////////////////////////////////////////////  
unsigned char ReadRawRC(unsigned char address)  
    {  
        
        unsigned char dat;          
        //片选信号低电平选中  
        spi0_enable();  
        SPI0_WriteByte(((address<<1)&0x7E)|0x80);  
        delay(0x100);
        dat = SPI0_ReadByte();  
        //片选信号高电平取消选中  
        spi0_disable();   
        return dat;  
    }  
      
    /////////////////////////////////////////////////////////////////////  
    //功    能：寫RC632寄存器  
    //參數說明：Address[IN]:寄存器地址  
    //          value[IN]:寫入的值  
    /////////////////////////////////////////////////////////////////////  
void WriteRawRC(unsigned char address, unsigned char value)  
{            
    //片选信号低电平选中  
    spi0_enable();          
    SPI0_WriteByte(((address<<1)&0x7E));  
    SPI0_WriteByte(value);            
    //片选信号高电平取消选中  
    delay(200);//这个地方延时很重要
    spi0_disable();                   
}  

//******************************************************************/
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
//******************************************************************/
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = ReadRawRC(reg)&(~mask);
  delay(10);
  WriteRawRC(reg, tmp) ;  // clear bit mask
} 

//******************************************************************/
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
//******************************************************************/
void SetBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = ReadRawRC(reg)| mask;
   delay(10);
  WriteRawRC(reg,tmp);  // set bit mask
}
#define PREG(x) delay(0x100);printf(#x"=%x\n",ReadRawRC(x)) 
char PcdReset(void)//测试ok
{
    GPH2DAT|=0x1;
    delay(0x20);
    GPH2DAT&=~0x1;
    delay(0x20);
    GPH2DAT|=0x1;
    delay(0x2200);
    WriteRawRC(CommandReg,PCD_RESETPHASE);

    while ( ReadRawRC ( CommandReg ) & 0x10 );

    delay(100);
    WriteRawRC(ModeReg,0x3D);
    WriteRawRC(TReloadRegL,30);
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
   WriteRawRC(TxAutoReg, 0x40 );				   //调制发送信号为100%ASK	
    return MI_OK;
}


void rc522_init(void)
{
    spi_init();
    GPH2CON=0x1;
   // PcdReset();
}


//******************************************************************/
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
//******************************************************************/
char PcdComMF522(unsigned char Command  ,unsigned char *pInData , 
                 unsigned char InLenByte,unsigned char *pOutData, 
                 unsigned int  *pOutLenBit                       )
{
  char status = MI_ERR;
  unsigned char irqEn   = 0x00;
  unsigned char waitFor = 0x00;
  unsigned char lastBits;
  unsigned char n;
  unsigned int  i;
  switch (Command)
  {
    case PCD_AUTHENT:
      irqEn   = 0x12;
      waitFor = 0x10;
      break;
    case PCD_TRANSCEIVE:
      irqEn   = 0x77;
      waitFor = 0x30;
      break;
    default:
      break;
  }
  WriteRawRC(ComIEnReg,irqEn|0x80); //
  ClearBitMask(ComIrqReg,0x80);
  WriteRawRC(CommandReg,PCD_IDLE);
  SetBitMask(FIFOLevelReg,0x80); // 清空FIFO 
  for(i=0; i<InLenByte; i++)
    WriteRawRC(FIFODataReg,pInData[i]); // 数据写入FIFO 
  WriteRawRC(CommandReg, Command); // 命令写入命令寄存器  
  if(Command == PCD_TRANSCEIVE)
  {
    SetBitMask(BitFramingReg,0x80); // 开始发送 
  }  
  i = 3000; //根据时钟频率调整，操作M1卡最大等待时间25ms
  do 
  {
    n = ReadRawRC(ComIrqReg);
    i--;
  }  while((i!=0)&&!(n&0x01)&&!(n&waitFor)); 
  ClearBitMask(BitFramingReg,0x80);
  if(i!=0)
  {
    if(!(ReadRawRC(ErrorReg)&0x1B))
    {
      status = MI_OK;
      if (n&irqEn&0x01)
        status = MI_NOTAGERR;
      if(Command==PCD_TRANSCEIVE)
      {
        n = ReadRawRC(FIFOLevelReg);
        lastBits = ReadRawRC(ControlReg)&0x07;
        if(lastBits)
          *pOutLenBit = (n-1)*8 + lastBits;
        else
          *pOutLenBit = n*8;
        if(n==0)
          n = 1;
        if(n>MAXRLEN)
          n = MAXRLEN;
        for (i=0; i<n; i++)
          pOutData[i] = ReadRawRC(FIFODataReg); 
      }
    }
    else
      status = MI_ERR;        
  }
  SetBitMask(ControlReg,0x80);// stop timer now
  WriteRawRC(CommandReg,PCD_IDLE); 
  return status;
}

//打开天线
/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    PREG(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
        PREG(TxControlReg);
    }
}


/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

void PcdConfigISOType (unsigned char ucType)
{
	if ( ucType == 'A')                     //ISO14443_A
  {
		ClearBitMask ( Status2Reg, 0x08 );		
    WriteRawRC ( ModeReg, 0x3D );//3F		
		WriteRawRC ( RxSelReg, 0x86 );//84		
		WriteRawRC( RFCfgReg, 0x7F );   //4F		
		WriteRawRC( TReloadRegL, 30 );//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
		WriteRawRC ( TReloadRegH, 0 );		
		WriteRawRC ( TModeReg, 0x8D );		
		WriteRawRC ( TPrescalerReg, 0x3E );		
		delay (2000);		
		PcdAntennaOn ();//开天线		
   } 
}


char PcdRequest(unsigned char req_code, unsigned char *pTagType)
{
    char status;  
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ClearBitMask(Status2Reg,0x08);    
    WriteRawRC(BitFramingReg,0x07);
    //PREG(BitFramingReg);
    SetBitMask(TxControlReg,0x03);
    //PREG(TxControlReg);
 
    ucComMF522Buf[0] = req_code;

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
  if ((status == MI_OK) && (unLen == 0x10))
  {    
    *pTagType     = ucComMF522Buf[0];
    *(pTagType+1) = ucComMF522Buf[1];
  }
  else
    status = MI_ERR;
  return status;
}

/*
 * 函数名：PcdAnticoll
 * 描述  ：防冲撞
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdAnticoll ( unsigned char * pSnr )
{
    char cStatus;
    unsigned char uc, ucSnr_check = 0;
    unsigned char ucComMF522Buf[MAXRLEN]; 
	  unsigned int ulLen;    

    ClearBitMask ( Status2Reg, 0x08 );		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    WriteRawRC ( BitFramingReg, 0x00);		//清理寄存器 停止收发
    ClearBitMask ( CollReg, 0x80 );			//清ValuesAfterColl所有接收的位在冲突后被清除
   
    ucComMF522Buf [ 0 ] = 0x93;	//卡片防冲突命令
    ucComMF522Buf [ 1 ] = 0x20;
   
    cStatus = PcdComMF522 (PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//与卡片通信
	
    if ( cStatus == MI_OK)		//通信成功
    {
			for ( uc = 0; uc < 4; uc ++ )
			{
         * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//读出UID
         ucSnr_check ^= ucComMF522Buf [ uc ];
      }
			
      if ( ucSnr_check != ucComMF522Buf [ uc ] )
				cStatus = MI_ERR;    
				 
    }    
    SetBitMask ( CollReg, 0x80 );				
    return cStatus;			
}




