CC=cc -Iinclude

obj= return_numbers.o tell_time.o helloworld.o 

do_stuff: $(obj)
	$(CC) -o do_stuff $(obj)

return_numbers.o:src/return_numbers.c
	$(CC) -c src/return_numbers.c

tell_time.o:src/tell_time.c
	$(CC) -c src/tell_time.c

helloworld.o:src/helloworld.c
	$(CC) -c src/helloworld.c

clean:
	rm do_stuff $(obj)
