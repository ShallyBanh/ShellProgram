all: a1monitor a1shell 

a1monitor: a1shell 
	gcc a1monitor.c -o a1monitor.o 

a1shell: 
	gcc a1shell.c -o a1shell

clean:
	rm -rf *.o $(OBJECTS) a1shell 

tar:
	tar -cvf submit.tar *
