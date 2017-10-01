a1monitor: a1shell 
	gcc -o a1monitor a1monitor.c 

a1shell: 
	gcc -o a1shell a1shell.c 

clean:
	rm -rf a1shell a1monitor 

tar:
	tar -cvf submit.tar *
