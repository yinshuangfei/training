PROGRAM = test

m1: 01_reverse_list.c comm.c
	gcc -DM1 -g $^ -o $(PROGRAM);./$(PROGRAM)

m2: 01_reverse_list.c comm.c
	gcc -DM2 -g $^ -o $(PROGRAM);./$(PROGRAM)

m3: 01_reverse_list.c comm.c
	gcc -DM3 -g $^ -o $(PROGRAM);./$(PROGRAM)

clean:
	rm -fr $(PROGRAM)
