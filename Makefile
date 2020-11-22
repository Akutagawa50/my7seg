
obj-m := my7seg.o

my7seg.ko: my7seg.c
	make -C /usr/src/linux-headers-`uname -r` M=`pwd` V=1 modules

clean: 
	make -C /usr/src/linux-headers-`uname -r` M=`pwd` V=1 clean
