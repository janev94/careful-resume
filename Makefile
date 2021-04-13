obj-m += tcp_reno_verbose.o tcp_cubic_verbose.o

IDIR= /lib/modules/$(shell uname -r)/kernel/net/ipv4/
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

install:
	install -v -m 644 tcp_reno_verbose.ko $(IDIR)
	depmod
	modprobe tcp_reno_verbose
	sysctl -w net.ipv4.tcp_allowed_congestion_control="$(shell sysctl net.ipv4.tcp_allowed_congestion_control -n) reno_verbose"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf Module.markers modules.order Module.symvers tcp_reno_verbose.ko tcp_reno_verbose.mod.c tcp_reno_verbose.mod.o tcp_reno_verbose.o

