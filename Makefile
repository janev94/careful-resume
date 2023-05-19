obj-m += tcp_cubic_cr.o

IDIR= /lib/modules/$(shell uname -r)/kernel/net/ipv4/
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

install_cubic_cr:
	install -v -m 644 tcp_cubic_cr.ko $(IDIR)
	depmod
	modprobe tcp_cubic_cr
	sysctl -w net.ipv4.tcp_allowed_congestion_control="$(shell sysctl net.ipv4.tcp_allowed_congestion_control -n) cubic_cr"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf Module.markers modules.order Module.symvers tcp_cubic_cr.o tcp_cubic_cr.mod.o tcp_cubic_cr.mod.c tcp_cubic_cr.ko

