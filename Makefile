# Normally if you simple C program, running gcc MyOverlay.c -o myOverlay works.
# But a Kernel Module is not a normal program. It is a piece of code that must be "plugged into" a giant, running machine (the linux Kernel).
# To plug safely code must be compiled with exact same settings as the Kernel itself (same opt,same memory rules).The Makefile ensures this happens.



obj-m += MyOverlay.o # tells kernel i want to create a module named MyOverlay.o

all: # when typing make in the terminal, it looks for all the section and runs the commands under it
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules  
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


# "-C /lib/modules/$(shell uname -r)/build" tells make to jump  into the official Linux Kernel source folder 
# "M=$(PWD)" the code to compile is in current folder
# "modules" tells kernek build the modules don't recompile the whole operating System !

# .c Human-readable -> .o Object file machine code -> .ko Kerenl Object final plug inserted into the Kernel