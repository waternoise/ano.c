build: main.o sqlite3.o
	gcc -o ./bin/ano -Wall -std=c99 -pedantic -ansi -lpthread -ldl -lm sqlite3.o main.o

main.o: main.c sqlite3.h
	gcc -c main.c

sqlite3.o: sqlite3.c sqlite3.h
	gcc -c sqlite3.c

cleano:
	rm -rf ./bin/ano ./*.o

clean:
	rm -rf ./bin/ano

run:
	./bin/ano
