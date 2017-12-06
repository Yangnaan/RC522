#define GPD0CON (*(volatile unsigned int *)0xE02000A0)
#define GPD0DAT (*(volatile unsigned int *)0xE02000A4)

void buzzor_init(void)
{
	GPD0CON=0x1;
}

void beep(void)
{
	GPD0DAT=1;
	int n=0x50000;
	while(n--);
	GPD0DAT=0;
}
