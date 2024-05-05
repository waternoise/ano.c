build: main.o sqlite3.o db.o
	gcc -g -o ./bin/ano -Wall -std=c99 -pedantic -ansi -lpthread -ldl -lm sqlite3.o main.o db.o

main.o: main.c sqlite3.h db.h
	gcc -g -c main.c

sqlite3.o: sqlite3.c sqlite3.h
	gcc -g -c sqlite3.c

db.o: db.c db.h
	gcc -g -c db.c

cleano:
	rm -rf ./bin/ano ./*.o

clean:
	rm -rf ./bin/ano ./*.db

run:
	./bin/ano
