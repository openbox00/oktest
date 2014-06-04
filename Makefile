#versatile_uboot
MACHINE=		versatile_uboot
PATH := 	    /home/asic/opt/nicta/tools/gcc-3.4.4-glibc-2.3.5/arm-linux/bin/:$(PATH) 
COMMAND=		./tools/build.py
OPTIONS=		MACHINE=${MACHINE} PYFREEZE=False 

all: multicell
.PHONY: multicell
install:
	export PATH
	echo $(PATH)
hello:
	${COMMAND} ${OPTIONS} PROJECT=examples EXAMPLE=hello
clean:
	rm -vfr build/
	rm -vf *.pyc



