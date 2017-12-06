
name=rc522

#转换成.bin文件
$(name)new.bin: $(name).elf  
	arm-linux-objcopy -O binary $^ $(name).bin

#将.o文件链接成elf文件
$(name).elf: cpu/start.o main.o rc522.o libc.a driver/driver.a
	arm-linux-ld -Ttext=0x20000000 $^ -o $@ 

#编译cpu文件夹下的文件
cpu/start.o:
	cd cpu; make; cd ../

#编译driver文件夹下的文件，并打包成一个静态库
driver/driver.a:
	cd driver; make; cd ../


.c.o:
	arm-linux-gcc $< -c -fno-builtin #将所有.c文件编译成.o文件

clean:
	rm *.o  *.elf  *.bin -f
	cd cpu; make clean; cd ../
	cd driver; make clean; cd ../
