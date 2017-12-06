.global _start //声明为全局变量
.global main
_start:
	bl main
//	ldr r0, =0xE0200280//赋值伪指令
//	mov r1, #0x1 //r1=0001
//	str r1, [r0]  //1写到内存中

//	add r0,r0,#4 //数据寄存器的地址
//	mov r1, #0x0
//	str r1, [r0]
	
over:
	b over
