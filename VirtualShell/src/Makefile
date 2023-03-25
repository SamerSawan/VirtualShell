mysh: shell.c interpreter.c shellmemory.c
	gcc -D XVAL=$(framesize) -D YVAL=$(varmemsize) -c -g shell.c interpreter.c shellmemory.c scheduler.c
	gcc -o mysh shell.o interpreter.o shellmemory.o scheduler.o

clean: 
	rm mysh; rm *.o
