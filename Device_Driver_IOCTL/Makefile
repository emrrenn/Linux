obj-m := emre_CDD.o


host:
	make -C /lib/modules/6.2.0-26-generic/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/6.2.0-26-generic/build/ M=$(PWD) clean

help:
	make -C /lib/modules/6.2.0-26-generic/build/ M=$(PWD) help
