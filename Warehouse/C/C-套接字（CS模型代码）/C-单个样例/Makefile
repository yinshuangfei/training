OBJS=server client
server: server.o 
server.o: server.c 
	gcc -c server.c 

	
client: client.o
client.o: client.c
	gcc -c client.c 

clean:
	rm *.o
	rm $(OBJS)
	
	
