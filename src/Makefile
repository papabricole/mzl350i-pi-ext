CC=gcc -Wall -O3

all: mzl350i

mzl350i: mzl350i.c ili9481.o bcm2835.o
	$(CC) -o mzl350i -l rt mzl350i.c ili9481.o bcm2835.o

ili9481.o: ili9481.c ili9481.h
	$(CC) -c ili9481.c -o ili9481.o

bcm2835.o: bcm2835.c bcm2835.h
	$(CC) -c bcm2835.c -o bcm2835.o

install: all
	@sudo killall mzl350i || echo "No process, no problem."
	@sudo cp mzl350i /usr/bin/
	@sudo cp lcd /etc/init.d/
	@sudo chmod 755 /etc/init.d/lcd
	@sudo update-rc.d lcd defaults

uninstall:
	@sudo update-rc.d -f lcd remove

clean:
	rm -f mzl350i *.o
