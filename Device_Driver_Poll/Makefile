obj-m := poll_lkm.o


host:
	make -C /lib/modules/6.2.0-34-generic/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/6.2.0-34-generic/build/ M=$(PWD) clean

help:
	make -C /lib/modules/6.2.0-34-generic/build/ M=$(PWD) help
