
all: a1monitor a1shell 

a1monitor: 
	gcc a1monitor.c -o a1monitor.o 

a1shell: 
	gcc a1shell.c -o a1shell.o

clean:
	rm -rf *.o

cmput379Assign1.tar.gz: $(FILELIST)
	tar uzf $@ $^

tar:
	tar -cvf submit.tar *
# tar: a1monitor.c a1shell.c 
# 	tar uzf 
