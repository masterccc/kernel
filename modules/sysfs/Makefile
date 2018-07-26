obj-m += sysfs.o

all:
	make -C /lib/modules/4.17.8-1-ARCH/build M=$(PWD) modules

clean:
	make -C /lib/modules/4.17.8-1-ARCH/build M=$(PWD) clean
