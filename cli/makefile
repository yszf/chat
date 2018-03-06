cli:main.o cli_fun.o sem.o
	g++ -o cli main.o cli_fun.o sem.o -ljson
main.o:main.cpp
	g++ -c main.cpp -g
cli_fun.o:cli_fun.cpp
	g++ -c cli_fun.cpp -g
sem.o:sem.c
	g++ -c sem.c -g
clean:
	rm *.o cli
