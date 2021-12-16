objects = button.o buzzer.o colorled.o fnd.o led.o temperature.o textlcd.o accelMagGyro.o touch.o libbmp.o libfbdev.o

all: main.elf
	scp main.elf ecube@192.168.0.29:/home/ecube/kootest

main.elf: main.c libMyPeri.a
	arm-linux-gnueabi-gcc main.c -l MyPeri -lpthread -L. -o main.elf

libMyPeri.a: $(objects)
	arm-linux-gnueabi-ar rc libMyPeri.a *.o

$(objects): %.o: %.c
	arm-linux-gnueabi-gcc -o ./$@ -c $<
	
clean:
	-rm *.o *.a *.elf