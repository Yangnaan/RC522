CCFLAGS=-fno-builtin -I$(shell pwd)/../include/

driver.a: spi.o clock.o uart.o buzzor.o
	arm-linux-ar -r -o $@ $^ 

.c.o:
	arm-linux-gcc $< -c $(CCFLAGS)

clean:
	rm -f *.o *.a
