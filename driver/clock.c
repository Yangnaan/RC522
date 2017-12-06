#define APLL_CON0 *(volatile unsigned int *)0xE0100100
#define MPLL_CON  *(volatile unsigned int *)0xE0100108
#define CLK_DIV0  *(volatile unsigned int *)0xE0100300
#define CLK_SRC0  *(volatile unsigned int *)0xE0100200

//时钟初始化
void clock_setup(void)
{
    //先关闭所有的路径开关
    CLK_SRC0=0;
    //分频配置
    CLK_DIV0=1<<28|4<<24|1<<20|3<<16|1<<12|4<<8|4<<4|0<<0;
    //pll的配置
    APLL_CON0=1<<31|125<<16|3<<8|1<<0;
    MPLL_CON=1<<31|667<<16|12<<8|1<<0;

    //路径开关配置
    CLK_SRC0=1<<28|1<<4|1<<0;
}
